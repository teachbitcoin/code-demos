#include <iostream>
#include <bitcoin/bitcoin.hpp>
#include "include/wallet.hpp"

using namespace wallet;

int main(const int argc, char* argv[])
{
		// resolve input
		commands command;
		std::vector<std::string> arguments;
		resolve_input(command, arguments, argc, argv);

		switch(command)
		{
				case commands::new_wallet :
				{
						// Create new wallet with account key.
						// M/44h/0h/0h, M/44h/0h/1h, M/44h/0h/2h ...
						if(create_new_wallet(arguments.front()))
								std::cout << "New wallet created." << std::endl;
						else
								std::cerr << "New wallet: Error." << std::endl;
								return 1;
						break;
				}

				case commands::new_address :
				{
						std::string new_address;
						if(generate_address(new_address))
								std::cout << new_address << std::endl;
						else
								std::cerr << "New address: Error." << std::endl;
								return 1;
						break;
				}

				case commands::list_addresses :
				{
						std::vector<std::string> address_list;
						if(list_all(address_list))
						{
								for (auto address : address_list)
										std::cout << address << std::endl;
						}
						else
								std::cerr << "List address: Error." << std::endl;
								return 1;
						break;
				}

				case commands::balance :
				{
						std::vector<std::pair<std::string, uint64_t>> address_amount_list;
						if(get_balance(address_amount_list))
						{
								for (const auto& address_amount_pair : address_amount_list)
								{
										std::cout << address_amount_pair.first << ": "
															<< address_amount_pair.second << std::endl;
								}
						}
						else
								std::cerr << "Get balance: Error." << std::endl;
						break;
				}

				case commands::none :
				{
						std::cout << "main [command]" << std::endl;
				}

		}

  	return 0;
}
