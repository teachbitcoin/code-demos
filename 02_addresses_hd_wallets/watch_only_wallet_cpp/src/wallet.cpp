#include "../include/database.hpp"
#include "../include/wallet.hpp"
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>

namespace wallet
{

std::string db_file = "wallet.db";
std::string acct_key_table_name = "account_keys";
std::string acct_key_column_name = "xpub";
std::string rec_addr_table_name = "receiving_addresses";
std::string rec_addr_column_name = "address";

std::string testnet_endpoint_literal = "tcp://testnet1.libbitcoin.net:19091";
std::string mainnet_endpoint_literal = "tcp://mainnet2.libbitcoin.net:9081";


// Command Functions.
// -----------------------------------------------------------------------------

// TODO: Only one account xpub argument.
bool create_new_wallet(const std::string& account_key_str)
{
    if (account_key_str.size() == 0)
        return false;

    // Delete previous database.
    std::remove(db_file.c_str());

    database db;

    if (db.open(db_file))
        return false;

    // TODO: Acct key literal integrity check.
    bc::wallet::hd_public account_key(account_key_str);

    // Create account key table, insert key.
    if (db.create_table(acct_key_table_name, {{ acct_key_column_name }}))
        return false;

    database::data_row row;
    row.push_back(bc::to_chunk(account_key.to_hd_key()));

    if (db.insert_row(acct_key_table_name, {{ acct_key_column_name }}, row ))
        return false;

    // Create P2PKH address table ("addresses").
    if (db.create_table(rec_addr_table_name, {{ rec_addr_column_name }} ))
        return false;

    return true;
};


bool generate_address(std::string& address)
{
    database db;

    if (db.open(db_file))
        return false;

    // Read and reconstruct account key.
    database::data_rows key_rows;
    if (db.read_table(key_rows, acct_key_table_name))
        return false;

    auto key_chunk = key_rows.back().back();
    bc::wallet::hd_public account_key(
        bc::to_array<bc::wallet::hd_key_size>(key_chunk));

    // Derive current index from receiving address table.
    database::data_rows address_rows;
    if (db.read_table(address_rows, rec_addr_table_name))
        return false;

    // Generate new receiving address.
    auto index = address_rows.size();
    auto hdkey = account_key.derive_public(0).derive_public(index);

    auto point = hdkey.point();
    auto hd_prefix = bc::wallet::hd_public::to_prefix(hdkey.lineage().prefixes);

    uint32_t address_prefix;

    if (hd_prefix == bc::wallet::hd_public::mainnet)
        address_prefix = bc::wallet::ec_private::mainnet_p2kh;
    else if (hd_prefix == bc::wallet::hd_public::testnet)
        address_prefix = bc::wallet::ec_private::testnet_p2kh;
    else
    {
        // Unknown network.
        return false;
    }

    bc::wallet::ec_public public_key(point);
    auto payment_address = public_key.to_payment_address(address_prefix);
    address = payment_address.encoded();

    // Write new address to table.
    database::data_row row;
    row.push_back(bc::to_chunk(payment_address.to_payment()));

    if (db.insert_row(rec_addr_table_name, {{ rec_addr_column_name }}, row ))
        return false;

    return true;
};


bool list_all(std::vector<std::string>& address_list)
{
    database db;

    if (db.open(db_file))
        return false;

    // Read address table (single column rows).
    database::data_rows address_rows;
    if (db.read_table(address_rows, rec_addr_table_name))
        return false;

    // Convert to string.
    for (auto address_row : address_rows)
    {
        auto payment = bc::to_array<bc::wallet::payment_size>(
            address_row.back());

        bc::wallet::payment_address payment_address(payment);

        address_list.push_back(payment_address.encoded());
    }
    return true;
}


bool get_balance(std::vector<std::pair<std::string, uint64_t>>&
    address_amount_list)
{

    // TODO: Move to input argument.
    uint64_t wallet_balance;

    // Get address list.
    std::vector<std::string> address_list;
    if(!list_all(address_list))
        return false;

    // ZMQ socket setup.
    bc::protocol::zmq::context context;
    bc::protocol::zmq::socket socket(context,
        bc::protocol::zmq::socket::role::dealer);

    // Endpoint.
    // TODO: switch endpoint based on address prefix(es).
    bc::config::endpoint testnet_endpoint(testnet_endpoint_literal);

    if (socket.connect(testnet_endpoint) != bc::error::success)
        return false;

    bc::chain::history::list address_history_list;
    bc::code error;

    auto address_list_unqueried = address_list;

    const auto completion_handler =
        [&address_amount_list, &address_list_unqueried]
        (const bc::chain::history::list& history_list)
    {
        uint64_t address_unspent(0u);

        for (const auto& history : history_list)
        {
            // Unspent: Non-existent spending input point.
            bc::chain::input_point null_point({}, 0xFFFFFFFF);

            if (history.spend == null_point)
            {
                address_unspent += history.value;
            }
        }

        if (address_unspent > 0)
        {
            std::pair<std::string, uint64_t> address_amount_pair;
            address_amount_pair.first = address_list_unqueried.front();
            address_amount_pair.second = address_unspent;
            address_amount_list.push_back(address_amount_pair);
        }
        address_list_unqueried.erase(address_list_unqueried.begin());
    };

    const auto error_handler = [&error](const bc::code& code)
    {
        error = code;
        // TODO: Return false.
    };

    const auto unknown_handler = [](const std::string& command)
    {
        // TODO Handle unknown command.
    };

    bc::client::socket_stream stream(socket);
    bc::client::proxy proxy(stream, unknown_handler, 1000, 0); // Wait/retries.

    bc::protocol::zmq::poller poller;
    poller.add(socket);

    for (const auto& address : address_list)
    {
        // Address object from string.
        bc::wallet::payment_address payment_address(address);

        proxy.blockchain_fetch_history3(error_handler, completion_handler,
              payment_address, 0);

        if (poller.wait(1000).contains(socket.id()))
            stream.read(proxy);
    }

    return true;
}


// Command-line argument parsers.
// -----------------------------------------------------------------------------

void resolve_input(commands& command, std::vector<std::string>& arguments,
    const int& argc, char* argv[])
{
    // Command input.
    if (argc > 1)
    {
        std::string command_string(argv[1]);
        command=resolve_string(command_string);

        // Argument String Vector.
        if (argc > 2)
        {
            for (int i=2; i<argc; i++)
            {
                std::string argument(argv[i]);
                arguments.push_back(argument);
            }
        }
    }
    // No command arg supplied.
    else
        command=commands::none;
};


commands resolve_string(std::string& input)
{
	 if( input == "new-wallet" )
	 		return commands::new_wallet;

	 if( input == "new-address" )
	 		return commands::new_address;

	 if( input == "list" )
	 		return commands::list_addresses;

  if( input == "balance" )
	 		return commands::balance;

   else
      return commands::none;
};


} // Namespace.
