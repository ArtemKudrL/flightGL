#include "game.hpp"

#include <cpptoml.h>
#include <string>
#include <cmath>

#include <iostream>

namespace
{
Eigen::Vector3f getVec(const std::shared_ptr<cpptoml::table> config, const std::string& str)
{
    auto vec = *config->get_qualified_array_of<double>(str);
    return {static_cast<float>(vec[0]),
            static_cast<float>(vec[1]),
            static_cast<float>(vec[2])};
}
Eigen::Quaternionf getYprQuat(const std::shared_ptr<cpptoml::table> config, const std::string& str)
{
    Eigen::Vector3f ypr = getVec(config, str);
    ypr *= M_PI / 180;
    double cr = cos(ypr.z() * 0.5);
    double sr = sin(ypr.z() * 0.5);
    double cp = cos(ypr.y() * 0.5);
    double sp = sin(ypr.y() * 0.5);
    double cy = cos(ypr.x() * 0.5);
    double sy = sin(ypr.x() * 0.5);

    Eigen::Quaternionf q;
    q.w() = cr * cp * cy + sr * sp * sy;
    q.x() = sr * cp * cy - cr * sp * sy;
    q.y() = cr * sp * cy + sr * cp * sy;
    q.z() = cr * cp * sy - sr * sp * cy;

    return q;
}
std::string getStr(const std::shared_ptr<cpptoml::table> config, const std::string& str)
{
    return *config->get_qualified_as<std::string>(str);
}
}

void Game::start(const std::string& filename, const std::map<std::string, std::size_t>& models, const std::map<std::string, std::size_t>& textures)
{
    time = std::chrono::high_resolution_clock::now();
    auto config = cpptoml::parse_file("../" + filename);

    eye.position = getVec(config, "eye.position");
    eye.speed = getVec(config, "eye.speed");
    eye.direction = getVec(config, "eye.direction");
    eye.upDirection = getVec(config, "eye.upDirection");
    eye.boosting = false;
    eye.slowing = false;

    plane.body.mass = 1;
    plane.body.position = getVec(config, "plane.position");
    plane.body.speed = {0, 0 ,0};
    plane.body.orientation = getYprQuat(config, "plane.yprOrientation");
    plane.body.rotation = {0, 0, 0};
    plane.model = models.at(getStr(config, "plane.model"));
    plane.texture = textures.at(getStr(config, "plane.texture"));

    auto bodyNames = config->get_array_of<std::string>("bodies");

    for (const auto& name : *bodyNames)
    {
        Object obj {
            static_cast<float>(*config->get_qualified_as<double>(name + ".mass")),
            getVec(config, name + ".position"),
            getVec(config, name + ".speed"),
            getYprQuat(config, name + ".yprOrientation"),
            getVec(config, name + ".rotation"),
            textures.at(*config->get_qualified_as<std::string>(name + ".texture")),
            models.at(*config->get_qualified_as<std::string>(name + ".model"))
        };
        objects.push_back(obj);
    }
}

void Game::process()
{
    std::chrono::duration<float> elapsedTime = std::chrono::high_resolution_clock::now() - time;
    time = std::chrono::high_resolution_clock::now();

    moveEye(elapsedTime.count());

    std::vector<Eigen::Vector3f> forces(objects.size(), {0, 0, 0});
    for (std::size_t i = 0; i < objects.size(); ++i)
        for (std::size_t j = 0; j < objects.size(); ++j)
        {
            if (i == j)
                continue;

            forces[i] += objects[i].body.force(objects[j].body);
        }

    for (std::size_t i = 0; i < objects.size(); ++i)
        objects[i].body.accelerate(forces[i], elapsedTime.count());
}

void Game::rotateEye(Eigen::Vector2d mouseMove)
{
    eye.direction += mouseMove.x() * eye.direction.cross(eye.upDirection);
    eye.direction -= mouseMove.y() * eye.upDirection;
    eye.direction.normalize();
}

void Game::moveEye(double time)
{
    const double A = 0.01;
    const double S = 1000.0;
    eye.position += eye.speed;
    if (eye.boosting && !eye.slowing)
        eye.speed += A * eye.direction * time;
    if (!eye.boosting && eye.slowing)
        eye.speed -= S * eye.speed * time;
}
