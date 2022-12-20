#include "renderer.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>


namespace
{
struct VertexData
{
    glm::vec3 xyz;
    glm::vec3 norm;
    glm::vec2 uv;
};
}

Renderer::~Renderer()
{
    glDeleteProgram(program);
    glDeleteTextures(textureBuffers.size(), textureBuffers.data());
    glDeleteBuffers(vertexBuffers.size(), vertexBuffers.data());
    glDeleteVertexArrays(sizeof(vertexArrays)/sizeof(*vertexArrays), vertexArrays);
}

void Renderer::init()
{
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LESS);

    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    glGenVertexArrays(VARRNUM, vertexArrays);
    glBindVertexArray(vertexArrays[0]);

    vertexBuffers.resize(modelNames.size());
    vertexBuffersSizes.resize(modelNames.size());
    textureBuffers.resize(textureNames.size());

    glGenBuffers(vertexBuffers.size(), vertexBuffers.data());
    glGenTextures(textureBuffers.size(), textureBuffers.data());

    for (const auto &model : modelNames)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[model.second]);
        vertexBuffersSizes[model.second] = loadModel("../" + model.first);
    }

    for (const auto &texture : textureNames)
    {
        glBindTexture(GL_TEXTURE_2D, textureBuffers[texture.second]);
        loadTexture("../" + texture.first);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    program = loadShaders();

    const std::string uniformNames[UNINUM] {
        "p",
        "v",
        "m",
        "light",
        "sampler",
        "lightColor"
    };

    for (int i = 0; i < UNINUM; ++i)
        uniformLocations[i] = glGetUniformLocation(program, uniformNames[i].c_str());
}

GLuint Renderer::loadShaders() const
{
    auto readShader = [](const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
            throw std::runtime_error("Cant open shader file " + filename);
        std::stringstream sstream;
        sstream << file.rdbuf();
        return sstream.str();
    };

    auto compileShader = [](GLuint shader, const std::string& code)
    {
        char const* pCode = code.c_str();
        glShaderSource(shader, 1, &pCode, nullptr);
        glCompileShader(shader);
    };

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertexShaderCode = readShader("../shader/shader.vert");
    std::string fragmentShaderCode = readShader("../shader/shader.frag");

    compileShader(vertexShader, vertexShaderCode);
    compileShader(fragmentShader, fragmentShaderCode);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glDetachShader(program, fragmentShader);
    glDetachShader(program, vertexShader);

    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    return program;
}

std::size_t Renderer::loadModel(const std::string& filename) const
{
    std::vector<glm::vec3> xyz{};
    std::vector<glm::vec3> norm{};
    std::vector<glm::vec2> uv{};


    std::vector<VertexData> data{};

    std::ifstream file(filename);

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream sstream(line);
        std::string buf;
        sstream >> buf;

        if (buf == "#")
            continue;

        if (buf == "v")
        {
            glm::vec3 xyzNew;
            sstream >> buf;
            xyzNew.x = std::stof(buf);
            sstream >> buf;
            xyzNew.y = std::stof(buf);
            sstream >> buf;
            xyzNew.z = std::stof(buf);
            xyz.push_back(xyzNew);
        }
        else if (buf == "vn")
        {
            glm::vec3 normNew;
            sstream >> buf;
            normNew.x = std::stof(buf);
            sstream >> buf;
            normNew.y = std::stof(buf);
            sstream >> buf;
            normNew.z = std::stof(buf);
            norm.push_back(normNew);
        }
        else if (buf == "vt")
        {
            glm::vec2 uvNew;
            sstream >> buf;
            uvNew.x = std::stof(buf);
            sstream >> buf;
            uvNew.y = std::stof(buf);
            uv.push_back(uvNew);
        }
        else if (buf == "f")
        {
            for (int i = 0; i < 3; ++i)
            {
                VertexData dataNew{};
                sstream >> buf;
                std::size_t pos = buf.find("/");
                std::string token = buf.substr(0, pos);
                dataNew.xyz = xyz[std::stoi(token) - 1];
                buf.erase(0, pos + 1);
                pos = buf.find("/");
                token = buf.substr(0, pos);
                dataNew.uv = uv[std::stoi(token) - 1];
                buf.erase(0, pos + 1);
                dataNew.norm = norm[std::stoi(buf) - 1];
                data.push_back(dataNew);
            }
        }
    }

    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
    return data.size();
}

void Renderer::loadTexture(const std::string& filename) const
{
    int w, h, comp;
    const unsigned char* image = stbi_load(filename.c_str(), &w, &h, &comp, 3);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
}

void Renderer::clear() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
}

void Renderer::setTexture(std::size_t n)
{
    currentTexture = n;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureBuffers[currentTexture]);
}

void Renderer::setBuffer(std::size_t n)
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    currentModel = n;
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[currentModel]);

    glVertexAttribPointer(0,
        sizeof(glm::vec3)/sizeof(GLfloat),
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        (void*)0
    );
    glVertexAttribPointer(1,
        sizeof(glm::vec3)/sizeof(GLfloat),
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        (void*)sizeof(glm::vec3)
    );
    glVertexAttribPointer(2,
        sizeof(glm::vec2)/sizeof(GLfloat),
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        (void*)(sizeof(glm::vec3) + sizeof(glm::vec3))
    );
}

void Renderer::setUniforms(UniformData &data)
{
    glUniformMatrix4fv(uniformLocations[0], 1, GL_FALSE, &(data.p[0][0]));
    glUniformMatrix4fv(uniformLocations[1], 1, GL_FALSE, &(data.v[0][0]));
    glUniformMatrix4fv(uniformLocations[2], 1, GL_FALSE, &(data.m[0][0]));
    glUniform3f(uniformLocations[3], data.light[0], data.light[1], data.light[2]);
    glUniform1i(uniformLocations[4], 0);
    glUniform3f(uniformLocations[5], data.lightColor[0], data.lightColor[1], data.lightColor[2]);
}

void Renderer::render() const
{
    glDrawArrays(GL_TRIANGLES, 0, vertexBuffersSizes[currentModel]);
}

void Renderer::end() const
{
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

