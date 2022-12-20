#include "application.hpp"

#include <glm/fwd.hpp>
#include <iterator>
#include <thread>
#include <chrono>

#include <cpptoml.h>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>

const auto clk = std::chrono::high_resolution_clock::now;
typedef std::chrono::duration<double> duration;

namespace
{
const glm::vec3& cast(const Eigen::Vector3f& vec)
{
    return *reinterpret_cast<const glm::vec3*>(vec.data());
}
glm::mat4 cast(const Eigen::Quaternionf& q)
{
    return *reinterpret_cast<glm::mat3*>(q.toRotationMatrix().data());
}
}

Application::Application(const std::string& filename)
{
    appConfig = filename;
    auto config = cpptoml::parse_file(filename);

    gameConfig = *config->get_as<std::string>("gameConfig");
    auto clearColor = config->get_array_of<double>("clearColor");
    for (int i = 0; i < clearColor->size(); ++i)
        renderer.clearColor[i] = clearColor->at(i);
    auto files = config->get_array_of<std::string>("modelFiles");
    std::size_t index = 0;
    for (const auto &filename : *files)
        renderer.modelNames[filename] = index++;

    files = config->get_array_of<std::string>("textureFiles");
    index = 0;
    for (const auto &filename : *files)
        renderer.textureNames[filename] = index++;
}

void Application::run()
{
    window.create();

    window.setUserPointer(this);
    window.setKeyCallback(keyCallback);

    running = true;

    auto renderThread = std::thread([this]() { this->renderLoop(); });
    auto gameThread = std::thread([this]() { this->gameLoop(); });

    logicLoop();
    gameThread.join();
    renderThread.join();
}

void Application::gameLoop()
{
    m.lock();
        game.start(gameConfig, renderer.modelNames, renderer.textureNames);
    m.unlock();

    auto tickTime = duration(1/tps);
    auto sleepTime = tickTime;

    while (running)
    {
        auto tickStart = clk();

        m.lock();
        game.process();
        m.unlock();

        sleepTime = 0.5*(tickTime + sleepTime - (clk() - tickStart));
        std::this_thread::sleep_for(sleepTime);
    }
}

void Application::renderLoop()
{
    m.lock();
        window.makeContextCurrent();
        renderer.init();
    m.unlock();

    auto frameTime = duration(1/fps);
    auto sleepTime = frameTime;

    while (running)
    {
        auto frameStart = clk();

        renderer.clear();

        m.lock();
            UniformData data {
                glm::perspective(
                        glm::radians(45.0f),
                        window.w()/window.h(),
                        0.01f, 1.0e3f),
                glm::lookAt(
                        cast(game.eye.position),
                        cast(game.eye.position + game.eye.direction),
                        cast(game.eye.upDirection)),
                cast(game.plane.body.orientation),
                glm::vec3(0, 0, 1.0e3f),
                static_cast<glm::int32>(game.plane.texture),
                glm::vec3(1.0e6f)
            };
        m.unlock();
        renderer.setTexture(game.plane.texture);
        renderer.setUniforms(data);
        renderer.setBuffer(game.plane.model);
        renderer.render();

        for (const auto& obj : game.objects)
        {
            m.lock();
                data.m = glm::translate(
                            cast(obj.body.orientation),
                            cast(obj.body.position)),
            m.unlock();
            renderer.setTexture(obj.texture);
            renderer.setUniforms(data);
            renderer.setBuffer(obj.model);
            renderer.render();
        }

        renderer.end();

        window.swapBuffers();

        sleepTime = 0.5*(frameTime + sleepTime - (clk() - frameStart));
        std::this_thread::sleep_for(sleepTime);
    }
}

void Application::logicLoop()
{
    while (running)
    {
        processInput();
        glfwWaitEvents();

        if (window.shouldClose())
            running = false;
    }
}

void Application::processInput()
{
    const double C = 3.0;

    Eigen::Vector2d mouseMove;
    window.getCursorMove(&mouseMove.x(), &mouseMove.y());
    mouseMove.x() *= C/window.w();
    mouseMove.y() *= C/window.h();
    game.rotateEye(mouseMove);
}
