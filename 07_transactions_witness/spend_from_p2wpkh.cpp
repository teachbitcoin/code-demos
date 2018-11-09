#include <bitcoin/bitcoin.hpp>

int main()
{
    //**************************************************************************
    // 1. Restore wallet and generate spending key pairs
    //**************************************************************************

    std::string my_sentence = "amount right cheese defy click eight slight strategy replace earn simple labor";
    const auto my_word_list = bc::split(my_sentence, " ", true);

    const auto hd_seed = bc::wallet::decode_mnemonic(my_word_list);

    bc::wallet::hd_private hd_master_private(bc::to_chunk(hd_seed),
        bc::wallet::hd_private::testnet);
    const auto hd_master_public = hd_master_private.to_public();


    // We will be spending a p2wpkh output script controlled by
    // the following key: m/44'/1'/2'/0/8
    //--------------------------------------------------------------------------

    const auto hd_m_44h_1h_2h_0_8 = hd_master_private
        .derive_private(44 + bc::wallet::hd_first_hardened_key)
        .derive_private(1 + bc::wallet::hd_first_hardened_key)
        .derive_private(2 + bc::wallet::hd_first_hardened_key)
        .derive_private(0)
        .derive_private(8);
    const auto secret_44h_1h_2h_0_8 = hd_m_44h_1h_2h_0_8.secret();
    const auto pubkey_44h_1h_2h_0_8 =
        hd_m_44h_1h_2h_0_8.to_public().point();

    //**************************************************************************
    // 2. Fetch previous P2PKH UTXO which we will spend.
    //**************************************************************************

    // Previous TX hash.
    const auto prev_tx_literal =
        "9568c5a759f53c3db476362bdb4e36db31f4986fc2189cab36e9880ef2b3424b";
    bc::hash_digest prev_tx_hash;
    bc::decode_hash(prev_tx_hash, prev_tx_literal);

    // Previous UXTO prev_index.
    uint32_t prev_index = 0;
    bc::chain::output_point uxto_to_spend(prev_tx_hash, prev_index);

    // Previous output script not required for signing witness transactions.

    // Previous output amount.
    uint64_t previous_output_amount(498951);

    //**************************************************************************
    // 3. Build, Sign & Complete Transaction.
    //**************************************************************************

    // A. Encode P2PKH output.
    //--------------------------------------------------------------------------

    // We will send our funds to n2MBcctgzBt1h8Nvfu3XAEPJLrmWET7emw
    // ...pubkeyhash: e48199d47742b245464b1366d95ef26aa4c8bb2c

    const auto receiving_pubkeyhash =
        bc::base16_literal("e48199d47742b245464b1366d95ef26aa4c8bb2c");
    const auto p2pkh_operations =
        bc::chain::script::to_pay_key_hash_pattern(receiving_pubkeyhash);

    // Compute fees.
    //--------------------------------------------------------------------------

    // tx bytes: 4 + 1 + 1*(32+4+0+4) + 1 + 1*(8+33) + 4 = 91

    // Witness tx size: (91) + 72 + 33 = 196

    // # of sigops: one in script_code, one in output.
    // Output amount = 498951 - 1 * 100 - 1 * 400 - 196 * 1 = 498254

    uint64_t output_amount(498254);

    // Build P2WPKH output.
    bc::chain::output p2wpkh_output(output_amount, p2pkh_operations);

    // B. Build & Sign Transaction.
    //--------------------------------------------------------------------------

    // Build input0 object.
    bc::chain::input input;
    input.set_previous_output(uxto_to_spend);
    input.set_sequence(bc::max_input_sequence);

    // Build Transaction.
    bc::chain::transaction tx;
    tx.set_version(1u);
    tx.inputs().push_back(input);
    tx.outputs().push_back(p2wpkh_output);

    // Script code.
    const auto p2wpkh_script_code = bc::chain::script::to_pay_key_hash_pattern(
        bc::bitcoin_short_hash(pubkey_44h_1h_2h_0_8));

    // Previous input index.
    uint8_t input_index(0);

    // Sign transaction.
    bc::endorsement signature;
    bc::chain::script::create_endorsement(signature, secret_44h_1h_2h_0_8,
        p2wpkh_script_code, tx, input_index, bc::machine::sighash_algorithm::all,
        bc::machine::script_version::zero, previous_output_amount);

    // C. Finalise Transaction.
    //--------------------------------------------------------------------------

    // Build witness (...instead of input script)
    // [signature] [publicKey]
    bc::data_stack witness_stack;
    witness_stack.push_back(signature);
    witness_stack.push_back(bc::to_chunk(pubkey_44h_1h_2h_0_8));
    bc::chain::witness p2wpkh_witness(witness_stack);
    tx.inputs()[0].set_witness(p2wpkh_witness);

    // Serialisation to include witness: wire=true/witness=true
    std::cout << bc::encode_base16(tx.to_data(true, true)) << std::endl;
}
// g++ -std=c++11 -o spend_from_p2wpkh spend_from_p2wpkh.cpp $(pkg-config --cflags libbitcoin --libs libbitcoin)
