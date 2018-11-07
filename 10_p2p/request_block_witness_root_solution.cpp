// Compile with:
// g++ -std=c++11 `pkg-config --cflags libbitcoin libbitcoin-network --libs libbitcoin libbitcoin-network` -o [filename] [filename.cpp]

#include <bitcoin/network.hpp>

BC_USE_LIBBITCOIN_MAIN

int bc::main(int argc, char* argv[])
{
    // Create P2P object.
    // -------------------------------------------------------------------------

    // Single Outbound Channel.
    bc::network::settings settings(bc::config::settings::mainnet);
    settings.seeds.clear();
    settings.seeds.push_back({"mainnet1.libbitcoin.net",8333});
    settings.seeds.push_back({"mainnet2.libbitcoin.net",8333});
    settings.seeds.push_back({"mainnet3.libbitcoin.net",8333});
    settings.inbound_connections = 0; // default setting = 0.
    settings.outbound_connections = 1;  // default setting = 8.
    settings.host_pool_capacity = 100; // default setting = 0.
    settings.threads = 3; // default setting = 0.
    settings.services = bc::message::version::service::none; // default none.

    // Create P2P object.
    bc::network::p2p my_p2p(settings);

    // Promise which signals completion to main loop (below).
    std::promise<bc::code> promise;

    // Handlers
    // -------------------------------------------------------------------------

    const auto send_handler = [](const bc::code& ec)
        {
            bc::cout << std::string("[My App] GetData message for block sent: ")
                .append(ec.message()) << std::endl;
        };

    const auto block_subscription_handler = [&promise](const bc::code& ec,
        bc::message::block::const_ptr block)
    {
        if (ec)
            return true;

        const auto coinbase = block->transactions()[0];
        auto coinbase_hash = coinbase.hash();
        std::reverse(std::begin(coinbase_hash), std::end(coinbase_hash));

        bc::cout << std::string("[My App] Coinbase TXID: ")
            .append(bc::encode_base16(bc::to_chunk(coinbase_hash))) << std::endl;

        bc::cout << std::string("[My App] Coinbase witness root: ")
            .append(bc::encode_base16(
                bc::to_chunk(block->generate_merkle_root(true)))) << std::endl;

        bc::cout << std::string("[My App] Coinbase witness data: ")
            .append(coinbase.inputs()[0].witness().to_string())
            << std::endl;

        promise.set_value(ec);

        return false;
        // Note: It is not possible to a message to request further headers
        // from the peer from this subscription_handler, since no channel
        // pointer is accessible.
    };

    const auto connect_handler = [&block_subscription_handler, &send_handler](
        const bc::code& ec, bc::network::channel::ptr channel)
    {
        // Resubscribe if connect error occurs.
        if (ec)
            return true;

        // Get negotiated version.
        bc::cout << std::string("[My App] New channel with version: ")
            .append(std::to_string(channel->peer_version()->value()))
            << std::endl;

        // Depending on version, get_blocks or get_headers.
        if (channel->peer_version()->value() >=
            bc::message::version::level::headers)
        {
            // Subscribe to block message.
            channel->subscribe<bc::message::block>(
                  block_subscription_handler);

            // Send get_data(inv(block_type, block_hash)).
            const auto type =
                bc::message::inventory_vector::type_id::witness_block;
            const auto block_hash = bc::hash_literal(
                "000000000000000000018b63b42f45010822ed77ba6000a5d193283ff812c274");
            bc::message::inventory_vector inv_vector(type, block_hash);
            bc::message::get_data get_data_msg({inv_vector});

            channel->send(get_data_msg, send_handler);
        }

        else
        {
            // We reject peer that do not support headers first sync.
            channel->stop(bc::error::channel_stopped);
        }

        // Resubscribe
        return true;
    };

    const auto run_handler = [&my_p2p](const bc::code& ec)
    {
        // Omitted.
    };

    const auto start_handler = [&my_p2p, &promise, &connect_handler,
        &run_handler](const bc::code& ec)
    {
        if (ec && ec != bc::error::peer_throttling)
        {
            promise.set_value(ec);
        }

        // Network run.
        my_p2p.subscribe_connection(connect_handler);
        my_p2p.run(run_handler);
    };

    // Start P2P object.
    // -------------------------------------------------------------------------

    my_p2p.start(start_handler);

    std::cout << promise.get_future().get().message() << std::endl;

    return 0;
}
