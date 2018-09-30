#include <bitcoin/bitcoin.hpp>

bc::code run_script_on_program(bc::machine::program& current_program,
    const bc::chain::script& current_script) {

    bc::code ec;

    //Check if script is valid.
    //    Trailing invalid op due to push op size mismatch
    //    Script is unspendable (op_return / > max_script_size of 10kbytes)
    if (!current_program.is_valid()) {
        ec = bc::error::invalid_script;
        return ec;
    }

    // Script is valid:
    else {

        // Run individual operations of script.
        int script_index(0);

        // Loop through all script operations.
        for (const auto& op: current_program) {

            // Max script element size (520 bytes)
            if (op.is_oversized()) {
                return bc::error::invalid_push_data_size;
            }

            // Disallowed operations which can cause script vulnerabilities.
            if (op.is_disabled()) {
                return bc::error::op_disabled;
            }

            // Increment operation count for (op >= op_97),
            //    Maximimum count of 201 permitted.
            if (!current_program.increment_operation_count(op)) {
                return bc::error::invalid_operation_count;
            }

            // If operation is unconditional, conditional state must be positive
            //    for subsequent operation.
            if (current_program.if_(op))
            {
                // Check that stack < 1000 elements (overflow).
                if (!current_program.is_stack_overflow())
                {
                    // Execute operation. Changes state of stack.
                    if ((ec = current_program.evaluate(op))) // if error
                    {
                        return ec;
                    }

                    // Print out operator that has been executed.
                    std::cout << std::endl;
                    std::cout << std::string(script_index + 1, '>')
                              << " Operation: " << script_index << std::endl;
                    std::cout << op.to_string(bc::machine::rule_fork::all_rules)
                              << std::endl;
                    std::cout << std::endl;

                    // Print stack state after execution of operation.
                    bc::machine::program program_copy(current_script,
                        current_program);
                    std::cout << std::string(script_index + 1, '>')
                              << " Stack after operation: "
                              << script_index << std::endl;
                    while (!program_copy.empty())
                    {
                        std::cout << "[" << bc::encode_base16(program_copy.pop())
                                  << "]" << std::endl;
                    }

                    // Increment script_index.
                    script_index += 1;
                }
            }
        }
    }

    // Checks for no outstanding flow control operations.
    // e.g. missing ENDIF
    current_program.closed() ?
        ec = bc::error::success : ec = bc::error::invalid_stack_scope;

    return ec;

}


bc::code evaluate_in_out_p2sh_witness_runs(const bc::chain::script
    previous_output_script, uint64_t previous_output_amount,
    const bc::chain::transaction& transaction, uint32_t input_index,
    uint32_t forks)

    {

    auto input_script = transaction.inputs()[input_index].script();
    auto witness = transaction.inputs()[input_index].witness();

    bc::code ec;

    // 1) Evaluate input script.
    //--------------------------------------------------------------------------

    bc::machine::program input_program(input_script, transaction,
        input_index, forks);
    std::cout << "=========== Input script evaluation ==========="
              << std::endl;
    if ((ec = run_script_on_program(input_program, input_script)))
        return ec;

    // 2) Evaluate output script.
    //--------------------------------------------------------------------------

    bc::machine::program output_program(previous_output_script, input_program);
    std::cout << "\n"
              << "=========== Output script evaluation =========="
              << std::endl;
    if ((ec = run_script_on_program(output_program, previous_output_script)))
        return ec;

    // 3) Evaluate stack after input/output run.
    //--------------------------------------------------------------------------

    if (!output_program.stack_result(false))
    {
        return bc::error::stack_false;
    }

    // 4) Check for p2w pattern.
    //--------------------------------------------------------------------------

    bool witnessed(false);

    if ((forks & bc::machine::rule_fork::bip141_rule) ==
        bc::machine::rule_fork::bip141_rule)
    {
        if ((witnessed = bc::chain::script::is_witness_program_pattern(
            previous_output_script.operations())))
        {
            //Omitted: Evaluate witness program
        }
    }

    // 5) Detect P2SH pattern in output script (BIP16)
    //--------------------------------------------------------------------------

    if (!((forks & bc::machine::rule_fork::bip16_rule) ==
        bc::machine::rule_fork::bip16_rule))
    {
        return bc::error::success;
    }

    if (previous_output_script.output_pattern() ==
        bc::machine::script_pattern::pay_script_hash)
    {

        std::cout << "\n"
                  << "----------- P2SH pattern detected -------------"
                  << std::endl;

        // Valid embedded script push in input script.
        if (!bc::chain::script::is_relaxed_push(input_script.operations()))
            return bc::error::invalid_script_embed;

        // Extract embedded script at the top of the stack.
        bc::chain::script embedded_script(input_program.pop(), false);
        bc::machine::program embedded_program(embedded_script,
            std::move(input_program), true);

        std::cout << "\n"
                  << "======= P2SH Embedded script evaluation ======="
                  << std::endl;

        if ((ec = run_script_on_program(embedded_program, embedded_script)))
            return ec;

        if (!embedded_program.stack_result(false))
            return bc::error::stack_false;

        // 6) Detect witness program pattern in P2SH embedded script (bip141)
        //----------------------------------------------------------------------

        if (!((forks & bc::machine::rule_fork::bip141_rule) ==
            bc::machine::rule_fork::bip141_rule))
        {
            return bc::error::success;
        }

        if ((witnessed = bc::chain::script::is_witness_program_pattern(
            embedded_script.operations())))
        {

            std::cout << "\n"
                      << "---------- Witness program detected -----------"
                      << std::endl;

            // 7) Extract and run extracted witness program script.
            //--------------------------------------------------------------

            // The input script must be a push of the embedded_script (bip141).
            if (input_script.size() != 1)
            {
                return bc::error::dirty_witness;
            }

            const auto version = embedded_script.version();

            // Detect version 0 of witness program.
            if (version == bc::machine::script_version::zero)
            {
                bc::chain::script script;
                bc::data_stack stack;

                if (!witness.extract_embedded_script(
                    script, stack, embedded_script))//
                {
                    return bc::error::invalid_witness;
                }

                std::cout
                    << "\n"
                    << "============== Witness evaluation ============="
                    << std::endl;

                bc::machine::program witness(script, transaction, input_index,
                    forks, std::move(stack), previous_output_amount,
                    version);

                ec = run_script_on_program(witness, script);

                if (!witness.stack_result(false))
                    return bc::error::stack_false;
            }
        }
    } // End p2sh run.

    std::cout << "\n"
            << "--------- Script evaluation complete ----------"
            << std::endl;

    // Witness must be empty
    if (!witnessed && !witness.empty())
        return bc::error::unexpected_witness;

    return bc::error::success;
}
