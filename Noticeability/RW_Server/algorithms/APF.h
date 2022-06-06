#ifndef RW_SERVER_APF_H
#define RW_SERVER_APF_H
#include "RWAlgorithm.h"
#include "../Game.h"
#include "../config.h"


class Rates{
public:
    Rates() = default;
    ~Rates() = default;
    void scale(double);
    double getBestRate() const;
    double baseRate = 0;
    double movingRate = 0;
    double headRateActual = 0;
    double headRateCompress = 0;
    double headRotateAmplify = 0;
};


class APF: public RWAlgorithm{
public:
    std::pair<bool, double> predictRotation(Game*, std::string) override;

private:
    /**
     * Equation 1 from Bachmann et al.
     * @return The total force vector.
     */
    Coordinate calculateForceVectors(Game*, const std::string&);
    /**
     * Equation 5 from Bachmann et al.
     * @return The sum of the distances to all boundary line segments and all other users.
     */
    static double calculateSumD(Game*, const std::string&);
    /**
     * Equation 6 from Bachmann et al.
     * @return Vector of coordinates from all the force vectors associated with boundary line segments w_j.
     */
    static std::vector<Coordinate> calculateEnvVectors(Game*, const std::string&, double);
    /**
     * Equation 7 from Bachmann et al.
     * @return Vector of coordinates from individual force vectors associated with other users u_j.
     */
    std::vector<Coordinate> calculateOtherUsersVectors(Game*, const std::string&, double) const;
    /**
     * Equation 3 from Bachmann et al.
     * @return Double value representing kappa.
     */
    static double calculateKappa(Game*, const std::string&, const std::string&);
    Rates* calculateMaxRotations(Game*, const std::string&) const;
    std::pair<bool, Coordinate> resetNeeded(Game*, const std::string&, double) const;

    static Coordinate getVectorFromSegment(Coordinate, Coordinate, Coordinate);
    static Coordinate nearestPointOnSegment(Coordinate, Coordinate, Coordinate);

    double baseRate = rad(0.1);
    double v = 0.1;
    double scaleMultiplier = 2.5;
    double angRateScaleDilate = 1.3;
    double angRateScaleCompress = 0.85;
    double maxHeadRate = rad(30);
    double gamma = 1.4;
    double r = 7.5;
    double maxMoveRate = rad(0.3);
    double delta_t = 0.1;
    double t_a_norm = 15.0;
    bool reset = false;
    int resetCounter = 0;
};


#endif //RW_SERVER_APF_H
