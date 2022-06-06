#ifndef RW_SERVER_UTILS_H
#define RW_SERVER_UTILS_H

#include <vector>
#include <string>
#include <sstream>
#include <cmath>

class Coordinate{
public:
    double x = 0;
    double y = 0;
    double z = 0;
    Coordinate operator+(const Coordinate&) const;
    Coordinate operator+(double) const;
    Coordinate operator-(const Coordinate&) const;
    Coordinate operator/(double) const;
    Coordinate operator*(const Coordinate&) const;
    Coordinate operator*(double) const;
    bool operator==(const Coordinate&) const;
    bool operator!=(const Coordinate&) const;
    std::ostream& operator<<(std::ostream& os) const;
    std::vector<double> asVector();
    std::string asString() const;
};

/**
 * Splits a string based on a given character.
 * @return A vector of all substrings without split character.
 */
std::vector<std::string> custom_split(const std::string&, char);

/**
 * Parse a string containing coordinates to a coordinate object.
 * @return Coordinate object with corresponding x,y,z attributes.
 */
Coordinate getCoordinateFromString(std::string);

Coordinate sum(const std::vector<Coordinate>&);
double getAngleBetween(Coordinate, Coordinate);
double norm(Coordinate);
double dot(Coordinate, Coordinate);
double clip(double, int, int);
double getAngle(Coordinate);
double rad(double);
bool equalError(Coordinate, Coordinate, double);


#endif //RW_SERVER_UTILS_H
