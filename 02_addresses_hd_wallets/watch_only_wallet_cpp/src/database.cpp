#include "../include/database.hpp"
#include <vector>
#include <iomanip>
#include <sstream>


database::database()
    : context()
{
}

database::~database()
{
    sqlite3_close(context);
}


database::code database::open(const std::string& file_name)
{

    // sqlite3 interface consumes non const filename arg.
    std::string file_name_mutable(file_name);
    char* file_name_cstr = &file_name_mutable[0];

    if( sqlite3_open(file_name_cstr, &context) )
    {
        return sqlite3_errmsg(context);
    }

    if( sqlite3_limit(context, 0, 1000) ) // cant increase beyond hard limit.
    {
        // return sqlite3_errmsg(context);
    }

    return NULL;
}


database::code database::create_table(const std::string& table_name,
    const column_strings& column_names)
{
    // Construct query string.
    std::string create_table_string = "create table ";

    std::string columns;
    for (auto column_name : column_names)
    {
        columns += column_name + " " + "BLOB" + ", "; // Width - 1000B
    }
    columns.pop_back(); //Remove " "
    columns.pop_back(); //Remove ","

    auto query_string = create_table_string + table_name + " ("
        + columns + ")";
    auto query_char_string = query_string.c_str();
    auto return_code = database::sqlite_query(query_char_string, NULL);

    if (return_code)
        return return_code;

    return NULL;
}


database::code database::insert_row(const std::string& table_name,
    const column_strings& column_names,
    const data_row& column_values)
{
    column_strings column_values_strings;
    // Convert data row to column_strings.
    for (auto data_chunk : column_values)
    {
        column_values_strings.push_back(bytes_to_hex(data_chunk));
    }

    return insert_row(table_name, column_names, column_values_strings);
}


database::code database::insert_row(const std::string& table_name,
      const column_strings& column_names,
      const column_strings& column_values)
{
    // Construct query string.
    // "insert into table_name (column_name_0, ...) values (x'8080', ...)";

    std::string insert_into_string = "insert into ";

    // (column_name_0, ...)
    std::stringstream column_names_stream;
    column_names_stream << "(";
    for (int i=0; i<column_names.size(); i++)
    {
        column_names_stream << column_names[i];
        if (i < column_names.size()-1)
            column_names_stream << ", ";
    }
    column_names_stream << ")";

    // ('8080', ...)
    std::stringstream column_values_stream;
    column_values_stream << "(";
    for (int i=0; i<column_values.size(); i++)
    {
        column_values_stream << "'" << column_values[i] << "'";
        if (i < column_values.size()-1)
            column_values_stream << ", ";
    }
    column_values_stream << ")";

    auto column_names_string = column_names_stream.str();
    auto column_values_string = column_values_stream.str();

    auto query_string = insert_into_string + table_name + column_names_string
        + "values " + column_values_string;

    // Submit SQL query.
    auto query_char_string = query_string.c_str();
    return database::sqlite_query(query_char_string, NULL);
}


database::code database::read_table(data_rows& data_rows,
    const std::string& table_name)
{
    std::string query_string = "select * from " + table_name;

    const char* query_char_string = query_string.c_str();

    return database::sqlite_query(query_char_string, &data_rows);
}


database::code database::drop_table(const std::string& table_name)
{
    std::string query_string = "drop table " + table_name;

    const char* query_char_string = query_string.c_str();

    return database::sqlite_query(query_char_string, NULL);

}


int database::sqlite_callback(void* row_list, int argc, char **argv,
  char **column_name)
{
    // Cast void* pointer to row_list pointer to access object members.
    auto row_list_ptr = static_cast<database::data_rows*>(row_list);

    // Iterate through returned row entries.
    data_row data_row;

    for(int i=0; i<argc; i++)
  	{
        // Empty row element.
  			if (!argv[i])
            data_row.push_back({});

  			// Non-empty row element.
  			else
  			{
            std::string hex_string(argv[i]);
            database::data_chunk data_entry;
            database::hex_to_bytes(data_entry, hex_string);
  					data_row.push_back(data_entry);
  			}
  	}

    // Add row to row_list.
    row_list_ptr->push_back(data_row);

    return 0;
}


database::code database::sqlite_query(const char* query, void* row_list)
{
    char* db_error_message=0;

    auto rc = sqlite3_exec(context, query, database::sqlite_callback,
      row_list, &db_error_message);

    if( rc!=SQLITE_OK )
    {
      auto code = sqlite3_errmsg(context);
      sqlite3_free(db_error_message);
      return code;
    }

    return NULL;
}


std::string database::bytes_to_hex(const std::vector<uint8_t>& bytes)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int byte: bytes)
      ss << std::setw(2) << byte;
    return ss.str();
}


bool database::hex_to_bytes(std::vector<uint8_t>& out, const std::string& hex)

{
    // Even non-zero string (no guards for out-of-range chars).
    if ((hex.length() % 2) == 0 && hex.length() !=0)
    {
        unsigned out_size = hex.length()/2;

        for (int i=0; i<hex.length(); i=i+2)
        {
            out.push_back(
                (hex_to_char(hex.at(i)) << 4) + hex_to_char(hex.at(i+1))
            );
        }
        return true;
    }
    else
        return false;
}

uint8_t database::hex_to_char(const char c)
{
    if ('A' <= c && c <= 'F')
        return 10 + c - 'A';
    if ('a' <= c && c <= 'f')
        return 10 + c - 'a';
    return c - '0';
}
