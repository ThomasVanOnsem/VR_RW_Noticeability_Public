#ifndef RW_SERVER_RWALGORITHM_H
#define RW_SERVER_RWALGORITHM_H
#include <utility>
#include "../utils/utils.h"

class Game;

class RWAlgorithm {
public:
    RWAlgorithm()=default;
    ~RWAlgorithm()=default;
    /**
     * Predict the next position of a user in a game.
     * @return The indication whether a position reset is needed and the recommended adjustments to be made to the user's position.
     */
    virtual std::pair<bool, double> predictRotation(Game*, std::string) = 0;
};


#endif //RW_SERVER_RWALGORITHM_H
