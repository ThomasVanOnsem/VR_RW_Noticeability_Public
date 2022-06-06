#include <iostream>
#include "APF.h"

std::pair<bool, double> APF::predictRotation(Game *game, std::string user) {
    std::pair<bool, double> empty = std::make_pair(false, 0);
    if(!game->setupCompleted()){
        std::cout << "ERROR: Game not properly setup!" << std::endl;
        return empty;
    }
    std::map<std::string, Player*> users = game->getUsers();
    //Improvement 1, ignore very small position updates
    if(config::use_improvements && equalError(users[user]->getLocation(), users[user]->getPreviousLocation(), config::standing_still_error) ){
        return empty;
    }
    if(users[user]->getHistory().size() < config::startup_counter){
        return empty;
    }
    //Improvement 2, stop recommending x steps after reset (avoid wall bouncing)
    if(config::use_improvements && reset && resetCounter < 20){
        resetCounter += 1;
        return empty;
    }
    else{
        reset = false;
        resetCounter = 0;
    }

    Coordinate force_vector = calculateForceVectors(game, user);
    Rates* rates = calculateMaxRotations(game, user);

    double user_dir = getAngle(users[user]->getLocation() - users[user]->getPreviousLocation());
    double desired_dir = getAngle(force_vector);

    user_dir = std::fmod(user_dir, (2 * M_PI));
    desired_dir = std::fmod(desired_dir, (2 * M_PI));
    double desired_rotation = desired_dir - user_dir;
    desired_rotation = std::fmod((desired_rotation + M_PI), (2 * M_PI) - M_PI);

    //Equation 10
    rates->scale((norm(force_vector)/t_a_norm) * scaleMultiplier);
    //Equation 11
    rates->movingRate = std::min(rates->movingRate, maxMoveRate);
    //Equation 12
    double best_rate = rates->getBestRate() * delta_t;

    double rec_rot = 0.25;
    if(desired_rotation < 0){
        rec_rot = -0.25;
    }
    if (0.25 >= std::abs(desired_rotation)){
        std::cout << "Recommending rotation: " << desired_rotation << std::endl;
        rec_rot = desired_rotation;
    }
    /*else {
        double possible_rotation;
        if (desired_rotation < 0) {
            possible_rotation = user_dir - best_rate;
        }
        else{
            possible_rotation = user_dir + best_rate;
        }

        std::cout << "Recommending rotation 2: " << possible_rotation << std::endl;

        rec_rot = possible_rotation;
    }*/

    std::pair<bool, Coordinate> resetNeed = resetNeeded(game, user, 5);
    if(resetNeed.first){
        reset = true;
        resetCounter = 0;
        return std::make_pair(true, 110); //TODO
    }
    return std::make_pair(false, rec_rot);
}

Rates* APF::calculateMaxRotations(Game* game, const std::string& user) const {
    auto* rates = new Rates();
    std::map<std::string, Player*> users = game->getUsers();

    double linear_velocity;
    if (users[user]->getHistory().size() > 1){
        linear_velocity = norm(users[user]->getLocation() - users[user]->getPreviousLocation()) / delta_t;
    }
    else{
        linear_velocity = users[user]->calculateSpeed();
    }

    rates->baseRate = baseRate * delta_t;
    if (users[user]->getHistory().size() > 1){
        rates->headRateActual = getAngleBetween(users[user]->getLocation(), users[user]->getPreviousLocation()) / delta_t;
    }
    else{
        rates->headRateActual = 0;
    }


    if (linear_velocity >= v){
        std::cout << "Reducing velocity: " << linear_velocity << std::endl;
        rates->movingRate = linear_velocity / r;
    }
    else{
        rates->headRotateAmplify = rates->headRateActual * angRateScaleDilate;
        rates->headRateCompress = rates->headRateActual * angRateScaleCompress;
    }

    return rates;
}

Coordinate APF::calculateForceVectors(Game* game, const std::string& user) {
    double sum_d = calculateSumD(game, user);
    std::vector<Coordinate> w_j = calculateEnvVectors(game, user, sum_d);
    std::vector<Coordinate> u_j = calculateOtherUsersVectors(game, user, sum_d);
    return sum(w_j) + sum(u_j);
}

double APF::calculateSumD(Game* game, const std::string& user) {
    double distance = 0.0;

    std::vector<Coordinate> border = game->getBorder();
    std::map<std::string, Player*> users = game->getUsers();
    for(int i=0; i<border.size(); i += 1){
        int start_index = i % border.size();
        int end_index = (i+1) % border.size();
        Coordinate start = border[start_index];
        Coordinate end = border[end_index];
        Coordinate user_loc = users[user]->getLocation();

        Coordinate d_i = getVectorFromSegment(user_loc, start, end);
        distance += norm(d_i);
    }

    std::map<std::string, Player*>::iterator it;
    for(it = users.begin(); it != users.end(); it++){
        if(it->first != user){
            Coordinate h_j = (users[user]->getLocation() - it->second->getLocation()); //TODO kappa?
            distance += norm(h_j);
        }
    }
    return distance;
}

std::vector<Coordinate> APF::calculateEnvVectors(Game *game, const std::string &user, double sum) {
    std::vector<Coordinate> env_vectors;
    std::vector<Coordinate> border = game->getBorder();
    std::map<std::string, Player*> users = game->getUsers();
    for(int i=0; i<border.size(); i += 1){  //TODO code duplication with calculateSumD
        //Equation 2 from Bachmann et al.
        int start_index = i % border.size();
        int end_index = (i+1) % border.size();
        Coordinate start = border[start_index];
        Coordinate end = border[end_index];
        Coordinate user_loc = users[user]->getLocation();
        Coordinate d_i = getVectorFromSegment(user_loc, start, end);

        //Equation 6 from Bachmann et al.
        Coordinate w_i = (d_i / norm(d_i)) * (sum / norm(d_i));

        env_vectors.push_back(w_i);
    }
    return env_vectors;
}

std::vector<Coordinate> APF::calculateOtherUsersVectors(Game *game, const std::string &user, double sum) const {
    std::vector<Coordinate> player_vct;
    std::map<std::string, Player*> users = game->getUsers();
    std::map<std::string, Player*>::iterator it;
    for(it = users.begin(); it != users.end(); it++){ //TODO code duplication with calculateSumD
        if(it->first != user){
            //Equation 4 from Bachmann et al.
            Coordinate h_j = (users[user]->getLocation() - it->second->getLocation());  //TODO add kappa?

            //Equation 7 from Bachmann et al.
            Coordinate u_j = (h_j / norm(h_j)) * (sum / pow(norm(h_j), gamma)) * calculateKappa(game, user, it->first);

            player_vct.push_back(u_j);
        }
    }
    return player_vct;
}


double APF::calculateKappa(Game *game, const std::string &user, const std::string &other_user) {
    std::map<std::string, Player*> users = game->getUsers();
    Coordinate vector_to_other = users[other_user]->getLocation() - users[user]->getLocation();
    Coordinate vector_from_other = users[user]->getLocation() - users[other_user]->getLocation();
    Coordinate user_dir = users[user]->getLocation() - users[user]->getPreviousLocation();
    Coordinate other_user_dir = users[other_user]->getLocation() - users[other_user]->getPreviousLocation();

    double theta_user = getAngleBetween(vector_to_other, user_dir);
    double theta_other = getAngleBetween(vector_from_other, other_user_dir);

    return clip((cos(theta_user) + cos(theta_other)) / 2, 0, 1);
}


Coordinate APF::getVectorFromSegment(Coordinate point, Coordinate s1, Coordinate s2) {
    return point - nearestPointOnSegment(point, s1, s2);
}

Coordinate APF::nearestPointOnSegment(Coordinate point, Coordinate s1, Coordinate s2) {
    double t_closest = dot(point - s1, s2 - s1) / pow(norm(s2-s1),2);
    double t_on_segment = clip(t_closest, 0, 1);
    return s1 + ((s2 -s1) * t_on_segment);
}

std::pair<bool, Coordinate> APF::resetNeeded(Game* game, const std::string& user, double threshold) const { //TODO more efficient
    double sum_d = calculateSumD(game, user);
    std::vector<Coordinate> w_j = calculateEnvVectors(game, user, sum_d);
    std::vector<Coordinate> u_j = calculateOtherUsersVectors(game, user, sum_d);
    Coordinate force_vct = sum(w_j) + sum(u_j);
    std::vector<Coordinate> all_vct;
    all_vct.reserve(w_j.size() + u_j.size());
    all_vct.insert(all_vct.end(), w_j.begin(), w_j.end());
    all_vct.insert(all_vct.end(), u_j.begin(), u_j.end());
    for(Coordinate c1: all_vct){
        if(norm(c1) > threshold){
            return std::make_pair<bool, Coordinate>(true, force_vct * delta_t / norm(force_vct));
        }
    }
    return std::make_pair(false, Coordinate());
}



void Rates::scale(double scale) {
    movingRate = movingRate * scale;
}

double Rates::getBestRate() const {
    return std::max(movingRate, baseRate);
}