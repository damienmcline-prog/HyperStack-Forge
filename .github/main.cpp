#include <asio.hpp>
#include <iostream>
#include <vector>
#include <cstring>

using asio::ip::tcp;

int main() {
    try {
        asio::io_context io;

        unsigned short port = 4000;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), port));

        std::cout << "[SERVER] Running on port " << port << "\n";

        while (true) {
            tcp::socket socket(io);
            acceptor.accept(socket);

            std::cout << "[SERVER] Client connected\n";

            while (true) {
                uint32_t msg_length = 0;

                // Read message length
                asio::error_code ec;
                asio::read(socket, asio::buffer(&msg_length, sizeof(msg_length)), ec);
                if (ec) break;

                // Read message body
                std::vector<char> buffer(msg_length);
                asio::read(socket, asio::buffer(buffer.data(), msg_length), ec);
                if (ec) break;

                std::string msg(buffer.begin(), buffer.end());
                std::cout << "[SERVER] Received: " << msg << "\n";

                // Basic command handling
                std::string response;
                if (msg == "PING") response = "PONG";
                else if (msg == "JOIN") response = "WELCOME_PLAYER";
                else if (msg.rfind("MOVE", 0) == 0) response = "ACK_MOVE";
                else response = "UNKNOWN_CMD";

                // Send response
                uint32_t resp_len = response.size();
                std::vector<char> out(sizeof(resp_len) + response.size());
                std::memcpy(out.data(), &resp_len, sizeof(resp_len));
                std::memcpy(out.data() + sizeof(resp_len), response.data(), response.size());

                asio::write(socket, asio::buffer(out), ec);
                if (ec) break;
            }

            std::cout << "[SERVER] Client disconnected\n";
        }

        io.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
