#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../include/wallet.hpp"

// Account key derivation path M/44h/0h/0h.
std::string account_key_literal = "xpub6DysRJdhBem4fM4QSEtYxwL8Jyme8QaC72sm8YNu"
  "yhDY44sknK8NqGWpHKWbaB2aj5iA7DQpMSUxrxyopwraTXs1RSvYQe5Jjo5ZNYeWCaJ";

std::string account_key_literal_testnet = "tpubDCeiyDoS1b2QPUDyC4FFrdYL3k7Wu5v2"
  "i8m61L68HTL68dM9NLZdddCjfSUc6UCo7gN6JWTVmDpbTTJHy7CvRrMEotb5G6gwVGQiThFAEW2";

// Address of M/44h/0h/0h/0/0.
std::string address_literal = "1HUAB7k5JBC6nproyhKjXwpgqD1ebW8sGR";
std::string address_literal_testnet = "mxk1YwZygZwNpf9DmTxsgazHduVd123bFZ";

// Test Fixture.
struct wallet_test_fixture {
    wallet_test_fixture() { std::remove(wallet::db_file.c_str()); }
    ~wallet_test_fixture() { std::remove(wallet::db_file.c_str()); }
};

BOOST_FIXTURE_TEST_SUITE( wallets_test_suite , wallet_test_fixture )

BOOST_AUTO_TEST_CASE( create_new_address )
{
    BOOST_REQUIRE(wallet::create_new_wallet(account_key_literal));
    std::string address;
    BOOST_REQUIRE(wallet::generate_address(address));
    BOOST_CHECK_EQUAL(address, address_literal);
}

BOOST_AUTO_TEST_CASE( create_new_address_testnet )
{
    BOOST_REQUIRE(wallet::create_new_wallet(account_key_literal_testnet));
    std::string address;
    BOOST_REQUIRE(wallet::generate_address(address));
    BOOST_CHECK_EQUAL(address, address_literal_testnet);
}

BOOST_AUTO_TEST_SUITE_END()
