#include "window.hpp"
#include <GLFW/glfw3.h>


void Window::create()
{
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, "flight", nullptr, nullptr);
    glfwSetCursorPos(window, posx, posy);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::getCursorMove(double *mousex, double *mousey)
{
    double newMouseX, newMouseY;
    glfwGetCursorPos(window, &newMouseX, &newMouseY);
    *mousex = newMouseX - posx;
    *mousey = newMouseY - posy;
    posx = newMouseX;
    posy = newMouseY;
}
