#include <bitcoin/bitcoin.hpp>

int main()
{
    //**************************************************************
    // 1. Restore wallet and generate spending key pairs
    //**************************************************************

    std::string my_sentence = "amount right cheese defy click eight slight strategy replace earn simple labor";
    const auto my_word_list = bc::split(my_sentence, " ", true);

    const auto hd_seed = bc::wallet::decode_mnemonic(my_word_list);

    bc::wallet::hd_private hd_master_private(bc::to_chunk(hd_seed),
        bc::wallet::hd_private::testnet);
    const auto hd_master_public = hd_master_private.to_public();

    // Three Receiving public keys in 3rd wallet account.
    //-------------------------------------------------------------

    const auto hd_m_44h_1h_2h_0 = hd_master_private
        .derive_private(44 + bc::wallet::hd_first_hardened_key)
        .derive_private(1 + bc::wallet::hd_first_hardened_key)
        .derive_private(2 + bc::wallet::hd_first_hardened_key)
        .derive_private(0);

    const auto hd_m_44h_1h_2h_0_9 = hd_m_44h_1h_2h_0.derive_private(9);
    const auto secret_44h_1h_2h_0_9 = hd_m_44h_1h_2h_0_9.secret();
    const auto pubkey_44h_1h_2h_0_9 = hd_m_44h_1h_2h_0_9.to_public().point();

    const auto hd_m_44h_1h_2h_0_10 = hd_m_44h_1h_2h_0.derive_private(10);
    const auto secret_44h_1h_2h_0_10 = hd_m_44h_1h_2h_0_10.secret();
    const auto pubkey_44h_1h_2h_0_10 = hd_m_44h_1h_2h_0_10.to_public().point();

    const auto hd_m_44h_1h_2h_0_11 = hd_m_44h_1h_2h_0.derive_private(11);
    const auto secret_44h_1h_2h_0_11 = hd_m_44h_1h_2h_0_11.secret();
    const auto pubkey_44h_1h_2h_0_11 = hd_m_44h_1h_2h_0_11.to_public().point();

    //**************************************************************
    // 2. Fetch previous P2PKH UTXO which we will spend.
    //**************************************************************

    // Previous TX hash.
    const auto prev_tx_literal =
        "2fc6c2abe0352b2cf1c4e1f48c10757e7a69b6b4f1a8252448a23221abbe00fa";
    bc::hash_digest prev_tx_hash;
    bc::decode_hash(prev_tx_hash, prev_tx_literal);

    // Previous UXTO prev_index.
    uint32_t prev_index = 0;
    bc::chain::output_point uxto_to_spend(prev_tx_hash, prev_index);

    // Previous output script not required for signing witness transactions.

    // Previous output amount.
    uint64_t previous_output_amount(498254);

    //**************************************************************
    // 3. Build, Sign & Complete Transaction.
    //**************************************************************

    // A. Encode P2PKH output.
    //-------------------------------------------------------------

    // We will send our funds to n2MBcctgzBt1h8Nvfu3XAEPJLrmWET7emw
    // ...pubkeyhash: e48199d47742b245464b1366d95ef26aa4c8bb2c

    const auto receiving_pubkeyhash =
        bc::base16_literal("e48199d47742b245464b1366d95ef26aa4c8bb2c");
    const auto p2pkh_operations =
        bc::chain::script::to_pay_key_hash_pattern(receiving_pubkeyhash);

    // Compute fees.
    //-------------------------------------------------------------

    // tx bytes: 4 + 1 + 1*(32+4+0+4) + 1 + 1*(8+33) + 4 = 91

    // Witness tx size: (91) + 1 + 72 + 72 + 1 + 1 + 33 + 33 + 33 + 1 = 338

    // # of sigops: one multisig in script_code, one in output.
    // Output amount = 498951 - 1 * 20 * 100 - 1 * 400 - 338 * 1 = 496216

    uint64_t output_amount(496216);

    // Build P2WPKH output.
    bc::chain::output p2wpkh_output(output_amount, p2pkh_operations);

    // B. Build & Sign Transaction.
    //-------------------------------------------------------------

    // Build input0 object.
    bc::chain::input input;
    input.set_previous_output(uxto_to_spend);
    input.set_sequence(bc::max_input_sequence);

    // Build Transaction.
    bc::chain::transaction tx;
    tx.set_version(1);
    tx.inputs().push_back(input);
    tx.outputs().push_back(p2wpkh_output);

    // Script code: witness script = e.g. multisig
    uint8_t signatures(2); //2 of 3
    bc::point_list points;
    points.push_back(pubkey_44h_1h_2h_0_9);
    points.push_back(pubkey_44h_1h_2h_0_10);
    points.push_back(pubkey_44h_1h_2h_0_11);
    bc::chain::script witness_script =
        bc::chain::script::to_pay_multisig_pattern(signatures, points);

    // Previous input index.
    uint8_t input0_index(0);

    // Create signatures for witness.
    bc::endorsement sig0;
    bc::endorsement sig1;
    bc::chain::script::create_endorsement(sig0, secret_44h_1h_2h_0_9,
        witness_script, tx, input0_index, bc::machine::sighash_algorithm::all,
        bc::machine::script_version::zero, previous_output_amount);
    bc::chain::script::create_endorsement(sig1, secret_44h_1h_2h_0_10,
        witness_script, tx, input0_index, bc::machine::sighash_algorithm::all,
        bc::machine::script_version::zero, previous_output_amount);

    // Create witness.
    bc::data_stack witness_stack;
    bc::data_chunk empty_chunk;
    witness_stack.push_back(empty_chunk);
    witness_stack.push_back(sig0);
    witness_stack.push_back(sig1);
    witness_stack.push_back(witness_script.to_data(false));
    bc::chain::witness p2wsh_witness(witness_stack);
    tx.inputs()[0].set_witness(p2wsh_witness);

    // Serialisation to include witness: wire=true/witness=true
    std::cout << bc::encode_base16(tx.to_data(true, true)) << std::endl;
}
// g++ -std=c++11 -o spend_from_p2wsh spend_from_p2wsh.cpp $(pkg-config --cflags libbitcoin --libs libbitcoin)
