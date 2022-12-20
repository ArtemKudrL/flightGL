#pragma once

#include <GLFW/glfw3.h>

class Window
{
    GLFWwindow *window;
    int width;
    int height;
    double posx;
    double posy;
public:
    Window() :
        window(nullptr),
        width(600),
        height(600),
        posx(0.5*width),
        posy(0.5*height)
        { glfwInit(); }

    ~Window()
    {
        if (window)
            close();
        glfwTerminate();
    }

    void create();
    void close()
        { glfwDestroyWindow(window); }

    void setUserPointer(void *pointer) const
        { glfwSetWindowUserPointer(window, pointer); }
    void setKeyCallback(GLFWkeyfun callback) const
        { glfwSetKeyCallback(window, callback); }

    void getCursorMove(double *mousex, double *mousey);

    float w() const
        { return width; }
    float h() const
        { return height; }

    void makeContextCurrent() const
        { glfwMakeContextCurrent(window); }
    void swapBuffers() const
        { glfwSwapBuffers(window); }
    bool shouldClose() const
        { return glfwWindowShouldClose(window); }
};
