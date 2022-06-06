#ifndef RW_SERVER_PLAYER_H
#define RW_SERVER_PLAYER_H
#include <string>
#include "utils/utils.h"
#include "algorithms/Extrapolation.h"
#include "algorithms/LinearExtrapolation.h"
#include "config.h"

class Player {
public:
    explicit Player(std::string);
    ~Player() = default;
    void setLocation(Coordinate);
    void setRotation(Coordinate);
    void setOffset(Coordinate);
    void invertPos();
    double calculateSpeed();
    Coordinate getLocation();
    Coordinate getRotation();
    Coordinate getPreviousLocation();
    std::vector<Coordinate> getHistory();
private:
    std::string id;
    Coordinate location;
    Coordinate rotation;
    std::vector<Coordinate> history;
    /**
     * Offset the position of the user. Because the HMD will always send virtual coordinates, we sometimes need to
     * offset those to physical coordinates. (eg.: When starting 2 players in the same space, they can't both start
     * from (0,0,0).
     */
    Coordinate offset;
    /**
     * Offset the direction of the user.
     * 1 == Same direction (normal) as HMD
     * -1 == Opposite direction to HMD
     */
    int invert_pos = 1;
    Extrapolation* extrapolator;
};


#endif //RW_SERVER_PLAYER_H
