#include "../include/app.hpp"
#include "../include/shader.hpp"
#include "../include/mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MyApp : public App {
    private:
        const GLfloat clearColor[4] = {0.2f, 0.3f, 0.3f, 1.0f};
        Plane plane;
        Sphere sphere;
        Torus torus;

    public:
        void init(){
            
            ShaderInfo shaders[] = {
                {GL_VERTEX_SHADER, "simpleShader.vert"},
                {GL_FRAGMENT_SHADER, "simpleShader.frag"},
                {GL_NONE, ""}
            };
            ShaderProgram simpleShader(shaders);

            ShaderInfo textureAndLightingShaders[] = {
                {GL_VERTEX_SHADER, "textureAndLightingShader.vert"},
                {GL_FRAGMENT_SHADER, "textureAndLightingShader.frag"},
                {GL_NONE, ""}
            };

            ShaderProgram textureAndLightingShader(textureAndLightingShaders);

            plane.generateMesh(20);
            plane.setup();
            plane.setShaderProgram(textureAndLightingShader);
            plane.setMapKd(plane.loadTextureFromFile("stone.jpg"));
            plane.setNs(99.0f);

            sphere.generateMesh(60);
            sphere.setup();
            sphere.setShaderProgram(textureAndLightingShader);
            sphere.setMapKd(sphere.loadTextureFromFile("wood.jpg"));
            sphere.setNs(99.0f);

            torus.generateMesh(60);
            torus.setup();
            torus.setShaderProgram(textureAndLightingShader);
            torus.setMapKd(torus.loadTextureFromFile("fur.jpg"));
            torus.setNs(99.0f);
            
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

            glm::mat4 plane_model_matrix = glm::translate(I, glm::vec3(0.0f, -1.0f, 0.0f)) 
                                        * glm::rotate(I, glm::radians(-90.0f), X)
                                        * glm::scale(I, glm::vec3(50.0f, 50.0f, 50.0f));
            
            plane.getShaderProgram().setModelMatrix(plane_model_matrix);
            plane.getShaderProgram().setViewMatrix(view_matrix);
            plane.getShaderProgram().setProjectionMatrix(projection_matrix);
            plane.getShaderProgram().setVec3("sun_position", positionOfLight1);
            plane.getShaderProgram().setVec3("eye_position", cameraPosition);
            plane.render();

            glm::mat4 sphere_model_matrix = glm::translate(I, glm::vec3(1.0f, 0.3f, 0.0f))
                                                * glm::scale(I, glm::vec3(0.8f, 0.8f, 0.8f));
            sphere.getShaderProgram().setModelMatrix(sphere_model_matrix);
            sphere.getShaderProgram().setViewMatrix(view_matrix);
            sphere.getShaderProgram().setProjectionMatrix(projection_matrix);
            sphere.getShaderProgram().setVec3("sun_position", positionOfLight1);
            sphere.getShaderProgram().setVec3("eye_position", cameraPosition);
            sphere.render();

            glm::mat4 torus_model_matrix = glm::translate(I, glm::vec3(-1.0f, 0.3f, 0.0f))
                                                * glm::rotate(I, glm::radians(90.0f), Y)
                                                * glm::scale(I, glm::vec3(1.3f, 1.3f, 1.3f));
            torus.getShaderProgram().setModelMatrix(torus_model_matrix);
            torus.getShaderProgram().setViewMatrix(view_matrix);
            torus.getShaderProgram().setProjectionMatrix(projection_matrix);
            torus.getShaderProgram().setVec3("sun_position", positionOfLight1);
            torus.getShaderProgram().setVec3("eye_position", cameraPosition);
            torus.render();
        }

        ~MyApp(){
        }

};


DECLARE_MAIN(MyApp)