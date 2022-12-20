#pragma one

#include <eigen3/Eigen/Geometry>

#include <ostream>

struct Body
{
    float mass;
    Eigen::Vector3f position;
    Eigen::Vector3f speed;
    Eigen::Quaternionf orientation;
    Eigen::Vector3f rotation;

    Eigen::Vector3f force(const Body& other) const;
    void accelerate(Eigen::Vector3f force, float time);

    friend std::ostream& operator<<(std::ostream& os, const Body& body);
};
