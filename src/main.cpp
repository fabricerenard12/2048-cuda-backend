#include "App.h"
#include "game_state.pb.h"
#include "Game.hpp"
#include "MonteCarlo.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

int main() {
    struct AuthData {};
	uWS::WebSocket<false, true, AuthData> *activeConnection = nullptr;

    uWS::App().ws<AuthData>("/*", {
        .open = [&activeConnection](auto *ws) {
            if (activeConnection) {
                std::cout << "Another client attempted to connect. Closing connection..." << std::endl;
                ws->close();
                return;
            }

            activeConnection = ws;
            std::cout << "Client connected!" << std::endl;
        },
        .message = [](auto *ws, std::string_view msg, uWS::OpCode opCode) {
            if (opCode == uWS::OpCode::BINARY) {
                GameStateMessage receivedMessage;
                GameStateMessage response;

                if (receivedMessage.ParseFromArray(msg.data(), msg.size())) {
                    std::cout << "Received Game State: " << receivedMessage.game_state() << std::endl;
					std::cout << "Received Score: " << receivedMessage.score() << std::endl;
					
					if (receivedMessage.game_state().size() != 64) {
        				throw std::invalid_argument("The string must contain exactly 64 characters.");
    				}
    
    				std::bitset<64> gameState = std::bitset<64>(receivedMessage.game_state());
                    std::cout << gameState << std::endl;

					Game game = Game();
					game.setGrid(gameState.to_ullong());
                    game.setScore(receivedMessage.score());

					Move bestMove = performMC(
						game,
						400,
						20
					);

                    std::cout << static_cast<int>(bestMove) << std::endl;

					game.makeMove(bestMove);

                    std::bitset<64> newGameState = std::bitset<64>(game.getGrid());
                    std::string serializedResponse;

			        std::cout << newGameState.to_string() << std::endl;
                    response.set_game_state(newGameState.to_string());
			        response.set_score(game.getScore());

                    response.SerializeToString(&serializedResponse);

                    ws->send(serializedResponse, uWS::OpCode::BINARY);
                } else {
                    std::cerr << "Failed to parse message" << std::endl;
                }
            }
        },
        .close = [&activeConnection](auto *ws, int code, std::string_view message) {
            if (ws == activeConnection) {
                std::cout << "Client disconnected with code " << code << std::endl;
                activeConnection = nullptr;
            }
        }
    }).listen(8000, [](auto *listen_socket) {
        if (listen_socket) {
            std::cout << "Listening on port " << 8000 << std::endl;
        } else {
            std::cout << "Failed to listen on port 8000" << std::endl;
        }
    }).run();

    std::cout << "Server shut down" << std::endl;
}
