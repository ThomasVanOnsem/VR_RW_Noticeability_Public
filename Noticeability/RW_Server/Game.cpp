#include <iostream>
#include "Game.h"
#include "config.h"

Game::Game() {
    if(!config::use_guardian){
        int size = config::room_size;
        addToBorder(Coordinate{((double)size/2.0),0,((double)size/2.0)});
        addToBorder(Coordinate{((double)size/2.0),0,(-(double)size/2.0)});
        addToBorder(Coordinate{(-(double)size/2.0),0,(-(double)size/2.0)});
        addToBorder(Coordinate{(-(double)size/2.0),0,((double)size/2.0)});
        setEnvAdded();
        if(config::player_amount == 1){
            Coordinate c1{0.0, 0.0, 0.0};
            player_offsets.push_back(c1);
        }
        else if(config::player_amount == 2){
            /*Coordinate c1{(double) -size/4, 0.0, (double) size/4};
            Coordinate c2{(double) size/4, 0.0, (double) -size/4};
            player_offsets.push_back(c1);
            player_offsets.push_back(c2);*/
            Coordinate c1{0.0, 0.0, 0.0};
            player_offsets.push_back(c1);
            player_offsets.push_back(c1);
        }
        //TODO more than 2?
    }
}

void Game::addPlayer(const std::string& id) {
    if(players.size()+1 > config::player_amount){
        std::cout << "ERROR: Could not add more players than config allows!" << std::endl;
        return;
    }
    if(players.count(id) == 0){
        auto player = new Player(id);
        player->setOffset(player_offsets[players.size()]);
        players.insert(std::pair<std::string, Player*>(id, player));
        //Because with 2 players, we start them on the diagonal of the square, one of them needs to have a different
        //direction and be guided to the largest open space of the square.
        if(players.size() % 2 != 0){
            //player->invertPos();
        }
    }
}

void Game::updatePlayerPos(const std::string& id, Coordinate coordinate){
    if(players.count(id) == 0){
        addPlayer(id);
    }
    players[id]->setLocation(coordinate);
}

void Game::updatePlayerRot(const std::string& id, Coordinate coordinate) {
    if(players.count(id) == 0){
        std::cout << "ERROR: Cannot save coordinates. Player does not exist yet!" << std::endl;
    }
    players[id]->setRotation(coordinate);
}

void Game::addToBorder(Coordinate coordinate) {
    // Avoid adding the border points again when second user joins
    if(setupCompleted()){
        return;
    }
    border.push_back(coordinate);
}

std::vector<Coordinate> Game::getBorder() {
    return border;
}

void Game::addObstacle(const std::string& name, Coordinate coordinate) {

}

void Game::setRWAlgorithm(RWAlgorithm* algorithm) {
    rw_algorithm = algorithm;
}

RWAlgorithm *Game::getRWAlgorithm() {
    return rw_algorithm;
}

std::map<std::string, Player*> Game::getUsers() {
    return players;
}

Player *Game::getPlayer(const std::string& id) {
    return players[id];
}

bool Game::setupCompleted() const {
    return env_added && !players.empty();
}

bool Game::getEnvAdded() const {
    return env_added;
}

void Game::setEnvAdded() {
    if(border.size() >= 3){
        env_added = true;
        std::cout << "Environment set!" << std::endl;
        return;
    }
    std::cout << "Environment not set!" << std::endl;
}

std::vector<Coordinate> Game::getOffsets() {
    return player_offsets;
}