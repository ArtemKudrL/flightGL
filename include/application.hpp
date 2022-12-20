#pragma once

#include "renderer.hpp"
#include "game.hpp"
#include "window.hpp"

#include <GLFW/glfw3.h>
#include <atomic>
#include <mutex>

class Application
{
    std::atomic_bool running;
    std::mutex m;

    double fps = 60.0;
    double tps = 2000.0;

    Window window;
    Game game;
    Renderer renderer;

    std::string appConfig;
    std::string gameConfig;

public:
    Application(const std::string& filename = "../app.toml");
    void run();

private:
    void gameLoop();
    void renderLoop();
    void logicLoop();

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int modes)
    {
        Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
        if (action == GLFW_PRESS && key == GLFW_KEY_W)
            app->game.eye.boosting = true;
        if (action == GLFW_PRESS && key == GLFW_KEY_S)
            app->game.eye.slowing = true;
        if (action == GLFW_RELEASE && key == GLFW_KEY_W)
            app->game.eye.boosting = false;
        if (action == GLFW_RELEASE && key == GLFW_KEY_S)
            app->game.eye.slowing = false;
    }
    void processInput();
};
