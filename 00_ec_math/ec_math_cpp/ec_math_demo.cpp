#include <iostream>
#include <bitcoin/bitcoin.hpp>

//g++ -std=c++11 ec_math_demo.cpp -o ec_math_demo $(pkg-config --cflags libbitcoin --libs libbitcoin)

int main(int argc, char* argv[])

{

    // 1a) Create a random 256bit value, verify it is of field order.
    bc::data_chunk my_chunk(32);
    bc::pseudo_random_fill(my_chunk);

    bc::ec_secret my_secret = bc::to_array<32>(my_chunk);

    if(bc::verify(my_secret))
        std::cout << "Secret is valid" << std::endl;

    // 1b) Convert secret to point.
    bc::ec_compressed my_publickey;
    bc::secret_to_public(my_publickey, my_secret);

    std::cout << bc::encode_base16(my_publickey) << std::endl;

    // 2) Pick a random x-coordinate, verify it is on curve.
    bc::ec_compressed my_compressed_point;
    int attempts;

    while (!bc::verify(my_compressed_point))
    {
        bc::data_chunk random_chunk(32);
        bc::pseudo_random_fill(random_chunk);

        // Assign even/odd prefix to random byte array.
        std::vector<uint8_t> compressed_point;
        compressed_point.push_back(0x02);
        compressed_point.insert(compressed_point.end(),
            random_chunk.begin(), random_chunk.end());

        my_compressed_point = bc::to_array<33>(compressed_point);
        attempts++;
    }

    std::cout << "Random EC point found: "
              << bc::encode_base16(bc::to_chunk(my_compressed_point))
              << std::endl;

    std::cout << "Random EC point Attempts: " << attempts << std::endl;

    return 0;

}
