#ifndef __APP_HPP__
#define __APP_HPP__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class App
{
private:
    const int SCR_WIDTH = 1920;
    const int SCR_HEIGHT = 1080;

public:
    static App *the_app;

    float aspect;
    glm::vec3 cameraPosition;
    glm::vec3 cameraUp;
    glm::vec3 positionOfLight1;
    glm::vec3 positionOfLight2;

    App()
    {
        aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
        cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        positionOfLight1 = glm::vec3(10.0f, 0.0f, -10.0f);
        positionOfLight2 = glm::vec3(-0.5f, 5.0f, -0.5f);
    }

    virtual void init()
    {
    }

    virtual void display()
    {
    }

    static void onWindowSize(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
        the_app->aspect = (float)width / (float)height;
    }

    static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            switch (key)
            {
            case GLFW_KEY_M: // 切换线框模式和填充模式
            {
                static GLenum mode = GL_FILL;
                mode = (mode == GL_FILL ? GL_LINE : GL_FILL);
                glPolygonMode(GL_FRONT_AND_BACK, mode);
                return;
            }
            }
        }
    }

    virtual void run(App *app)
    {
        if (the_app != NULL)
        { // 同一时刻，只能有一个App运行
            std::cerr << "The the_app is already run." << std::endl;
            return;
        }
        the_app = app;

        glfwInit();
        GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "StudyOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);
        glfwSetWindowSizeCallback(window, onWindowSize);
        glfwSetKeyCallback(window, onKey);
        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Failed to initalize GLEW" << std::endl;
            return;
        }

        init(); // 用来准备各种数据

        while (!glfwWindowShouldClose(window))
        {

            display(); // 这里才是渲染图形的主战场

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        glfwDestroyWindow(window);

        glfwTerminate();
        return;
    }
};

App *App::the_app = NULL;

#define DECLARE_MAIN(a)                   \
    int main(int argc, const char **argv) \
    {                                     \
        a *app = new a;                   \
        app->run(app);                    \
        delete app;                       \
        return 0;                         \
    }

#endif