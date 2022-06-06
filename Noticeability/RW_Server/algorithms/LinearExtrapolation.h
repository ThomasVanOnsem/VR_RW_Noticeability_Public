#ifndef RW_SERVER_LINEAREXTRAPOLATION_H
#define RW_SERVER_LINEAREXTRAPOLATION_H


#include "Extrapolation.h"

class LinearExtrapolation: public Extrapolation{
public:
    LinearExtrapolation() = default;
    ~LinearExtrapolation() = default;
    Coordinate extrapolate(Coordinate, Coordinate, double, double) override;
};


#endif //RW_SERVER_LINEAREXTRAPOLATION_H
