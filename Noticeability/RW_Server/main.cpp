#include <iostream>
#include <filesystem>
#include "Connection.h"
#include "Game.h"
#include "algorithms/APF.h"
#include "utils/Logger.h"
#include "utils/utils.h"

int main() {
    auto game = new Game();
    auto rw = new APF();
    game->setRWAlgorithm(rw);

    std::filesystem::path cwd = std::filesystem::current_path();
    auto* env_log = new Logger("environment", cwd.parent_path().string() + "\\logs\\");
    auto* user_log = new Logger("user", cwd.parent_path().string() + "\\logs\\");
    if(config::logging){
        env_log->log("Guardian: " + std::to_string(config::use_guardian));
        env_log->log("Room size: " + std::to_string(config::room_size));
        env_log->log("Use Delay: " + std::to_string(config::use_improvements));
        env_log->log("Delay: " + std::to_string(config::message_delay));
        env_log->log("# Players: " + std::to_string(config::player_amount));
        if(!config::use_guardian){
            for(Coordinate coordinate: game->getBorder()){
                env_log->log("Border: " + coordinate.asString());
            }
            for(Coordinate coordinate: game->getOffsets()){
                env_log->log("Offsets: " + coordinate.asString());
            }
        }
    }

    auto connection = new Connection(game, env_log, user_log);
    connection->bindSocket();
    connection->listen();

    delete connection;
    delete game;
    delete rw;
    delete user_log;
    delete env_log;
    return 0;
}
