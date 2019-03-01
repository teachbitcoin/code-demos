#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <sqlite3.h>

class database
{
public:

    typedef std::vector<std::string> column_strings;
    typedef std::vector<uint8_t> data_chunk;
    typedef std::vector<data_chunk> data_row;
    typedef std::vector<data_row> data_rows;
    typedef const char* code;

    // Constructor.
    database();
    // Close database upon destruction.
    ~database();

    code open(const std::string& file_name);

    code create_table(const std::string& table_name,
        const column_strings& column_names);

    code insert_row(const std::string& table_name,
        const column_strings& column_names,
        const data_row& column_values);

    code read_table(data_rows& data_rows, const std::string& table_name);

    code drop_table(const std::string& table_name);

private:
    // Insert new row to table (string values).
    code insert_row(const std::string& table_name,
        const column_strings& column_names,
        const column_strings& column_values);

    // SQLite callback.
    static int sqlite_callback(void* row_vector, int argc, char **argv,
      char **column_name);
    // SQLite query (Synchronous).
    code sqlite_query(const char* query_string, void* data_rows);
    // Database context.
    sqlite3* context;
    // Database file.
    std::string file_name;

    // Hex / bytes conversions.
    static std::string bytes_to_hex(const std::vector<uint8_t>& bytes);
    static bool hex_to_bytes(data_chunk& out, const std::string& hex);
    static uint8_t hex_to_char(const char c);
};

#endif
