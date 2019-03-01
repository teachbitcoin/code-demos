#ifndef WALLET_HPP
#define WALLET_HPP

#include <string>
#include <vector>

namespace wallet
{

enum class commands
{
    new_wallet,
    new_address,
    list_addresses,
    balance,
    none
};

extern std::string db_file;
extern std::string acct_key_table_name;
extern std::string acct_key_column_name;
extern std::string rec_addr_table_name;
extern std::string rec_addr_column_name;


// Command functions.
// -----------------------------------------------------------------------------

// Create Wallet (Wallet class)
bool create_new_wallet(const std::string& account_key_str);

// Create New Address.
bool generate_address(std::string& new_address);

// List all addresses.
bool list_all(std::vector<std::string>& address_list);

// Wallet balance (per address).
bool get_balance(std::vector<std::pair<std::string, uint64_t>>& address_amount_list);

void resolve_input(commands& command, std::vector<std::string>& arguments,
    const int& argc, char* argv[]);

commands resolve_string(std::string& input);

} // Namespace

#endif
