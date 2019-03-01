#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../include/database.hpp"

#define DB_FILE "database.db"
#define TABLE_NAME "test_table"

// Test Fixture.
struct database_test_fixture {
    database_test_fixture()
        : column_names({ {"column1"}, {"column2"}, {"column3"} }),
          column_entry_names({ {"column1"}, {"column3"} }),
          column_entry_values({ {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80}, {0xff,0xff,0xff} }),
          column_return_values({ {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80}, {}, {0xff,0xff,0xff} })
    {
    }

    ~database_test_fixture()
    {
        std::remove(DB_FILE);
    }

    database db;
    database::column_strings column_names;
    database::column_strings column_entry_names;
    database::data_row column_entry_values;
    database::data_row column_return_values;
};

BOOST_FIXTURE_TEST_SUITE( database_test_suite , database_test_fixture )

BOOST_AUTO_TEST_CASE( construct_open__read_table_expected )
{
    BOOST_REQUIRE(db.open(DB_FILE) == NULL);
    BOOST_REQUIRE(db.create_table(TABLE_NAME, column_names) == NULL);
    BOOST_REQUIRE(db.insert_row(TABLE_NAME, column_entry_names,
        column_entry_values) == NULL);

    database::data_rows data_rows;
    BOOST_REQUIRE( db.read_table(data_rows, TABLE_NAME) == NULL);
    BOOST_REQUIRE( data_rows.back() == column_return_values);

}

BOOST_AUTO_TEST_SUITE_END()
