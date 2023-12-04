#ifndef __MESH_H__
#define __MESH_H__
#define STB_IMAGE_IMPLEMENTATION
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <GL/glew.h>
#include "shader.hpp"
#include "stb_image.h"

struct Vertex
{
    glm::vec4 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Texture
{
    unsigned int id;
    std::string path;
};

class Mesh
{
protected:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;
    GLfloat ns;
    int hasMapKd = 0;
    GLuint mapKd;
    int hasMapKs = 0;
    GLuint mapKs;

    GLuint VAO, VBO, EBO;
    ShaderProgram shaderProgram;

public:
    void setShaderProgram(ShaderProgram shaderProgram)
    {
        this->shaderProgram = shaderProgram;
    }

    ShaderProgram getShaderProgram()
    {
        return shaderProgram;
    }

    void generateMesh(int iSlices);

    void setVertices(std::vector<Vertex> &&vertices)
    {
        this->vertices = vertices;
    }

    void setIndices(std::vector<GLuint> &&indices)
    {
        this->indices = indices;
    }

    void setKa(glm::vec3 ka)
    {
        this->ka = ka;
    }

    void setKd(glm::vec3 kd)
    {
        this->kd = kd;
    }

    void setKs(glm::vec3 ks)
    {
        this->ks = ks;
    }

    void setNs(GLfloat ns)
    {
        this->ns = ns;
    }

    void setMapKd(GLuint mapKd)
    {
        this->mapKd = mapKd;
        hasMapKd = 1;
    }

    void setMapKs(GLuint mapKs)
    {
        this->mapKs = mapKs;
        hasMapKs = 1;
    }

    void setup()
    {
        glCreateVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glCreateBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glNamedBufferStorage(VBO, sizeof(Vertex) * vertices.size(), &vertices[0], 0);
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * indices.size(), &indices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(2);
    }

    void render()
    {
        glBindVertexArray(VAO);
        shaderProgram.setCurrent();
        shaderProgram.setVec3("ka", ka);
        shaderProgram.setVec3("kd", kd);
        shaderProgram.setVec3("ks", ks);
        shaderProgram.setFloat("ns", ns);
        if (hasMapKd)
        {
            shaderProgram.setInt("hasMapKd", 1);
            shaderProgram.setInt("mapKd", mapKd);
        }
        else
        {
            shaderProgram.setInt("hasMapKd", 0);
        }
        if (hasMapKs)
        {
            shaderProgram.setInt("hasMapKs", 1);
            shaderProgram.setInt("mapKs", mapKs);
        }
        else
        {
            shaderProgram.setInt("hasMapKs", 0);
        }
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    unsigned int loadTextureFromFile(const char *path)
    {
        
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
            {
                format = GL_RED;
            }
            else if (nrComponents == 3)
            {
                format = GL_RGB;
            }
            else if (nrComponents == 4)
            {
                format = GL_RGBA;
            }
            glActiveTexture(GL_TEXTURE0 + textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            textureID = 0;
        }
        return textureID;
    }
};

class Plane : public Mesh
{
public:
    void generateMesh(int iSlices)
    {
        int n = iSlices + 1;
        float s = 2.0f / (float)iSlices;
        Vertex temp_vertex;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                temp_vertex.position = glm::vec4(s * j - 1.0f, s * i - 1.0f, 0.0f, 1.0f);
                temp_vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);
                temp_vertex.texCoord = glm::vec2(1.0f / (float)iSlices * j, 1.0f / (float)iSlices * i);
                vertices.push_back(temp_vertex);
            }
        }

        for (int i = 0; i < iSlices; i++)
        {
            for (int j = 0; j < iSlices; j++)
            {
                indices.push_back(i * n + j);
                indices.push_back((i + 1) * n + j + 1);
                indices.push_back((i + 1) * n + j);

                indices.push_back(i * n + j);
                indices.push_back(i * n + j + 1);
                indices.push_back((i + 1) * n + j + 1);
            }
        }
    }
};

class Sphere : public Mesh
{
public:
    void generateMesh(int iSlices)
    {
        int m = iSlices / 2 + 1;
        int n = iSlices + 1;
        float s = 360.0f / (float)iSlices;
        glm::vec4 up(0.0f, 1.0f, 0.0f, 1.0f);
        glm::mat4 I(1.0f);
        glm::vec3 X(1.0f, 0.0f, 0.0f);
        glm::vec3 Y(0.0f, 1.0f, 0.0f);
        glm::vec3 Z(0.0f, 0.0f, 1.0f);
        Vertex temp_vertex;
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                temp_vertex.position = glm::rotate(I, glm::radians(s * j), Y) * glm::rotate(I, glm::radians(s * i), Z) * up;
                temp_vertex.normal = temp_vertex.position;
                temp_vertex.texCoord = glm::vec2(1.0f / (float)(n - 1) * j, 1.0f / (float)(m - 1) * i);
                vertices.push_back(temp_vertex);
            }
        }

        for (int i = 0; i < m - 1; i++)
        {
            for (int j = 0; j < n - 1; j++)
            {
                indices.push_back(i * n + j);
                indices.push_back((i + 1) * n + j);
                indices.push_back((i + 1) * n + j + 1);

                indices.push_back(i * n + j);
                indices.push_back((i + 1) * n + j + 1);
                indices.push_back(i * n + j + 1);
            }
        }
    }
};

class Torus : public Mesh
{
public:
    void generateMesh(int iSlices)
    {
        int n = iSlices + 1;
        float s = -360.0f / (float)iSlices;
        glm::vec4 top(0.0f, 0.2f, 0.0f, 1.0f);
        glm::vec4 normal_up(0.0f, 1.0f, 0.0f, 1.0f);
        glm::mat4 I(1.0f);
        glm::vec3 X(1.0f, 0.0f, 0.0f);
        glm::vec3 Y(0.0f, 1.0f, 0.0f);
        glm::vec3 Z(0.0f, 0.0f, 1.0f);
        Vertex temp_vertex;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                temp_vertex.position = glm::rotate(I, glm::radians(s * j), Z) * glm::translate(I, glm::vec3(0.0f, 0.8f, 0.0f)) * glm::rotate(I, glm::radians(s * i), X) * top;
                temp_vertex.normal = glm::rotate(I, glm::radians(s * j), Z) * glm::rotate(I, glm::radians(s * i), X) * normal_up;
                temp_vertex.texCoord = glm::vec2(1.0f / (float)iSlices * j * 4, 1.0f / (float)iSlices * i);
                vertices.push_back(temp_vertex);
            }
        }
        for (int i = 0; i < iSlices; i++)
        {
            for (int j = 0; j < iSlices; j++)
            {
                indices.push_back(i * n + j);
                indices.push_back((i + 1) * n + j + 1);
                indices.push_back((i + 1) * n + j);

                indices.push_back(i * n + j);
                indices.push_back(i * n + j + 1);
                indices.push_back((i + 1) * n + j + 1);
            }
        }
    }
};

#endif