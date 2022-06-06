#ifndef RW_SERVER_EXTRAPOLATION_H
#define RW_SERVER_EXTRAPOLATION_H

#include "../utils/utils.h"

class Extrapolation {
public:
    Extrapolation() = default;
    ~Extrapolation() = default;
    /**
     * Extrapolate the position of a user based on the current position, previous position, speed (in m/s) and some delay (in ms).
     * @return The new position of a user.
     */
    virtual Coordinate extrapolate(Coordinate, Coordinate, double, double) = 0;
};


#endif //RW_SERVER_EXTRAPOLATION_H
