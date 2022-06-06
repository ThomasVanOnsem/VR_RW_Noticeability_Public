#include <iostream>
#include "utils.h"


std::vector<double> Coordinate::asVector(){
    return {x, y, z};
}

std::string Coordinate::asString() const{
    return
    '(' + std::to_string(x) + ','
    + std::to_string(y) + ','
    + std::to_string(z) + ')';
}

Coordinate Coordinate::operator+(const Coordinate &c1) const {
    Coordinate sum{};
    sum.x = x + c1.x;
    sum.y = y + c1.y;
    sum.z = z + c1.z;
    return sum;
}

Coordinate Coordinate::operator+(double addition) const {
    Coordinate sum{};
    sum.x = x * addition;
    sum.y = y * addition;
    sum.z = z * addition;
    return sum;
}

Coordinate Coordinate::operator-(const Coordinate &c1) const {
    Coordinate sum{};
    sum.x = x - c1.x;
    sum.y = y - c1.y;
    sum.z = z - c1.z;
    return sum;
}

Coordinate Coordinate::operator/(double division) const {
    Coordinate sum{};
    sum.x = x / division;
    sum.y = y / division;
    sum.z = z / division;
    return sum;
}

Coordinate Coordinate::operator*(const Coordinate &c1) const {
    Coordinate sum{};
    sum.x = x * c1.x;
    sum.y = y * c1.y;
    sum.z = z * c1.z;
    return sum;
}

Coordinate Coordinate::operator*(const double multiplier) const {
    Coordinate sum{};
    sum.x = x * multiplier;
    sum.y = y * multiplier;
    sum.z = z * multiplier;
    return sum;
}

bool Coordinate::operator==(const Coordinate& c1) const {
    return (c1.x == x) && (c1.y == y) && (c1.z == z);
}

bool Coordinate::operator!=(const Coordinate& c1) const {
    return !(c1 == *this);
}

std::ostream &Coordinate::operator<<(std::ostream &os) const {
    os << '(' << this->x << ',' << this->y << ',' << this->z << ')';
    return os;
}


std::vector<std::string> custom_split(const std::string& str, char separator){
    std::stringstream input(str);
    std::string segment;
    std::vector<std::string> seglist;

    while(std::getline(input, segment, separator))
    {
        seglist.push_back(segment);
    }
    return seglist;
}

Coordinate getCoordinateFromString(std::string coordinate){
    Coordinate coordinate1{};
    coordinate.erase(0,1);
    coordinate.erase(coordinate.size()-1, 1);
    std::vector<std::string> coordinate_str = custom_split(coordinate, ',');
    try{
        coordinate1.x = stof(coordinate_str[0]);
        coordinate1.y = stof(coordinate_str[1]);
        coordinate1.z = stof(coordinate_str[2]);
    }
    catch (const std::exception& e){
        std::cout << "ERROR: Failed to convert coordinate " << coordinate << std::endl;
    }
    return coordinate1;
}

double getAngleBetween(Coordinate c1, Coordinate c2) {
    try{
        return acos(dot(c1, c2) / (norm(c1) * norm(c2)));
    }
    catch (const std::exception& e){
        std::cout << "COULD NOT GET ANGLE BETWEEN COORDINATES!" << std::endl;
        return 0;
    }
}

double norm(Coordinate c1) {
    double sum = 0.0;
    std::vector<double> data = c1.asVector();
    for(double i : data){
        sum += pow(i,2);
    }
    return sqrt(sum);
}

double dot(Coordinate c1, Coordinate c2) {
    double product = 0;
    std::vector<double> c1v = c1.asVector();
    std::vector<double> c2v = c2.asVector();
    for (int i=0; i<c1v.size(); i++){
        product += c1v[i] * c2v[i];
    }
    return product;
}

double clip(double val, int edge1, int edge2) {
    if(edge1 < val && val < edge2){
        return val;
    }
    else if(edge1 > val){
        return edge1;
    }
    return edge2;
}

Coordinate sum(const std::vector<Coordinate>& coordinates) {
    Coordinate result{};
    for(Coordinate coordinate: coordinates){
        result = result + coordinate;
    }
    return result;
}

double getAngle(Coordinate coordinate) {
    return atan2(coordinate.z, coordinate.x);  //TODO change if Y needs to be used
}

double rad(double angle) {
    return angle * M_PI / 180;
}

bool equalError(Coordinate c1, Coordinate c2, double error){
    if(std::abs(std::abs(c1.x) - std::abs(c2.x)) >= error){
        return false;
    }
    if(std::abs(std::abs(c1.y) - std::abs(c2.y)) >= error){
        return false;
    }
    if(std::abs(std::abs(c1.z) - std::abs(c2.z)) >= error){
        return false;
    }
    return true;
}