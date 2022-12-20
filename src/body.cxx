#include "body.hpp"

#include <cmath>

typedef Eigen::Vector3f vec;
typedef Eigen::Quaternionf quat;

const float G = 100;
const float K = 100;

vec Body::force(const Body& other) const
{
    vec dif(other.position - position);
    vec f = mass * other.mass * dif.normalized() *
        (G / std::pow(dif.norm(), 2) - K / std::pow(dif.norm(), 3));
    return f;
}

void Body::accelerate(vec force, float time)
{
    position += speed * time;
    speed += (force / mass) * time;
}

std::ostream& operator<<(std::ostream& os, const Body& body)
{
    os << "position:\n" << body.position << "\n";
    os << "speed:\n" << body.speed << "\n";
    os << "orientation:\n" << body.orientation << "\n";
    os << "rotation:\n" << body.rotation << "\n";
    return os << std::endl;
}
