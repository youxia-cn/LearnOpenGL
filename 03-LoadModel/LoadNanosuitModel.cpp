#include "../include/app.hpp"
#include "../include/shader.hpp"
#include "../include/mesh.hpp"
#include "../include/model.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MyApp : public App {
    private:
        const GLfloat clearColor[4] = {0.2f, 0.3f, 0.3f, 1.0f};
        Model saber;

    public:
        void init(){            

            ShaderInfo textureAndLightingShaders[] = {
                {GL_VERTEX_SHADER, "textureAndLightingShader.vert"},
                {GL_FRAGMENT_SHADER, "textureAndLightingShader.frag"},
                {GL_NONE, ""}
            };

            ShaderProgram textureAndLightingShader(textureAndLightingShaders);

            saber.loadModel("nanosuit/nanosuit.obj");
            saber.setShaderProgram(textureAndLightingShader);
            
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);

            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }

        void display(){
            glClearBufferfv(GL_COLOR, 0, clearColor);
            glClear(GL_DEPTH_BUFFER_BIT);

            glm::mat4 I(1.0f);
            glm::vec3 X(1.0f, 0.0f, 0.0f);
            glm::vec3 Y(0.0f, 1.0f, 0.0f);
            glm::vec3 Z(0.0f, 0.0f, 1.0f);
            float t = (float)glfwGetTime();

            glm::vec3 newCameraPosition = glm::vec3(glm::vec4(cameraPosition, 1.0f) * glm::rotate(I, t, Y));
            glm::mat4 view_matrix = glm::lookAt(newCameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);

            glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), aspect, 1.0f, 100.0f);

            glm::mat4 saber_model_matrix = glm::translate(I, glm::vec3(0.0f, -1.2f, 0.0f)) 
                                        * glm::rotate(I, glm::radians(180.0f), Y)
                                        * glm::scale(I, glm::vec3(0.15f, 0.15f, 0.15f));
            
            saber.setModelMatrix(saber_model_matrix);
            saber.setViewMatrix(view_matrix);
            saber.setProjectionMatrix(projection_matrix);
            saber.setPositionOfLight1(positionOfLight1);
            saber.setPositionOfLight2(positionOfLight2);
            saber.setCameraPosition(newCameraPosition);
            saber.render();
        }

        ~MyApp(){
        }

};


DECLARE_MAIN(MyApp)