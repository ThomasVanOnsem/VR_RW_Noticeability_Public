#include "LinearExtrapolation.h"

Coordinate LinearExtrapolation::extrapolate(Coordinate current_pos, Coordinate prev_pos, double speed, double delay) {
    double distance_done = speed * (delay/1000);
    Coordinate trajectory = current_pos - prev_pos;
    Coordinate u = trajectory / norm(trajectory);
    return current_pos + (u * distance_done);
}