#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

#include <cstddef>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>

#define VARRNUM 1
#define UNINUM 6

struct UniformData
{
    glm::mat4 p;
    glm::mat4 v;
    glm::mat4 m;
    glm::vec3 light;
    glm::int32 sampler;
    glm::vec3 lightColor;
};

class Renderer
{
    GLuint program;
    GLuint vertexArrays[VARRNUM];
    std::vector<GLuint> vertexBuffers;
    std::vector<GLuint> vertexBuffersSizes;
    std::vector<GLuint> textureBuffers;
    GLuint uniformLocations[UNINUM];

    std::size_t currentModel = 0;
    std::size_t currentTexture = 0;

public:
    GLclampf clearColor[4];
    std::map<std::string, std::size_t> modelNames;
    std::map<std::string, std::size_t> textureNames;

    ~Renderer();

    void init();
    void clear() const;
    void setTexture(std::size_t n);
    void setBuffer(std::size_t n);
    void setUniforms(UniformData &data);
    void render() const;
    void end() const;

private:
    GLuint loadShaders() const;
    std::size_t loadModel(const std::string& filename) const;
    void loadTexture(const std::string& filename) const;
};
