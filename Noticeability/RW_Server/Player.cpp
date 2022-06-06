#include "Player.h"

Player::Player(std::string id1) {
    id = id1;
    if(config::use_improvements){
        extrapolator = new LinearExtrapolation();
    }
}

Coordinate Player::getLocation() {
    if(history.size() == 1){
        return location + offset;
    }
    Coordinate loc = location;
    if(config::use_improvements){
        //Use message delay 2 times because of up and down stream of data
        loc = extrapolator->extrapolate(location, history[history.size()-2], calculateSpeed(), config::message_delay*2);
    }
    return (loc*invert_pos) + offset;
}

Coordinate Player::getPreviousLocation() {
    if(history.size() == 1){
        return history[0] + offset;
    }
    if(history.size() == 2){
        return history[0] + offset;
    }
    return (history[history.size()-2]*invert_pos) + offset; //TODO add extrapolation?
}

void Player::setLocation(Coordinate loc) {
    location = loc;
    if(history.empty() || history[history.size()-1] != loc){ //TODO change ignoring same position?
        history.push_back(loc);
    }
}

void Player::setRotation(Coordinate rot) {
    rotation = rot;
}

Coordinate Player::getRotation() {
    return rotation;
}

std::vector<Coordinate> Player::getHistory() {  //TODO add offset + invert
    return history;
}

void Player::setOffset(Coordinate off) {
    offset = off;
}

void Player::invertPos() {
    if(invert_pos == 1){
        invert_pos = -1;
        return;
    }
    invert_pos = 1;
}

double Player::calculateSpeed() {
    double sec_between_msg = config::msg_frequency/(double)1000;
    return norm(location - history[history.size()-2])/(sec_between_msg); //TODO correct? because of l27, ignoring pos sometimes
}