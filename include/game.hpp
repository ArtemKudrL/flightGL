#pragma once

#include "body.hpp"

#include <chrono>
#include <vector>
#include <map>

struct Eye
{
    Eigen::Vector3f position;
    Eigen::Vector3f speed;
    Eigen::Vector3f direction;
    Eigen::Vector3f upDirection;
    bool boosting;
    bool slowing;
};

struct Object
{
    Body body;
    std::size_t texture;
    std::size_t model;
};

class Game
{
    std::chrono::high_resolution_clock::time_point time;
public:
    Eye eye;
    Object plane;
    std::vector<Object> objects;

    void start(const std::string& filename, const std::map<std::string, std::size_t>& models, const std::map<std::string, std::size_t>& textures);
    void process();

    void rotateEye(Eigen::Vector2d mouseMove);
private:
    void moveEye(double time);
};
