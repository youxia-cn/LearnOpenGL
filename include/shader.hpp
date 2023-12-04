#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ShaderInfo{
        GLenum type;
        std::string filename;
        GLuint shader_id;
};

class ShaderProgram{
    private:
        GLuint program_id;
    public:
        ShaderProgram(){
            program_id = 0;
        }
        
        ShaderProgram(ShaderInfo* shaders){
            if(shaders == nullptr){
                program_id = 0;
                return;
            }

            program_id = glCreateProgram();

            ShaderInfo* entry = shaders;
            //加载并编译Shader
            while(entry->type != GL_NONE){
                GLuint shader_id = glCreateShader(entry->type);
                entry->shader_id = shader_id;

                std::ifstream fs(entry->filename);
                std::string content((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
                               
                if(content.empty()){ //只要有一个Shader文件打不开，就删掉之前创建的所有Shader
                    std::cerr << "Unable to open file '" << entry->filename << "'" << std::endl;
                    for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
                        glDeleteShader( entry->shader_id );
                        entry->shader_id = 0;
                    }
                    return;
                }
                const GLchar* source = content.c_str();
                glShaderSource( shader_id, 1, &source, NULL );

                glCompileShader( shader_id );

                GLint compiled;
                glGetShaderiv( shader_id, GL_COMPILE_STATUS, &compiled );
                if ( !compiled ) {//如果Shader编译失败，输出失败原因，便于调试
                    GLsizei len;
                    glGetShaderiv( shader_id, GL_INFO_LOG_LENGTH, &len );

                    GLchar* log = new GLchar[len+1];
                    glGetShaderInfoLog( shader_id, len, &len, log );
                    std::cerr << entry->filename << "," << "Shader compilation failed: " << log << std::endl;
                    delete [] log;

                    //编译失败，也要删除前面创建的所有Shader
                    for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
                        glDeleteShader( entry->shader_id );
                        entry->shader_id = 0;
                    }

                    return ;
                }

                glAttachShader( program_id, shader_id );
        
                ++entry;
                
            }
            
            //进入连接阶段
            glLinkProgram( program_id );

            GLint linked;
            glGetProgramiv( program_id, GL_LINK_STATUS, &linked );
            if ( !linked ) {//如果连接失败，则输出调试信息，同样删除之前创建的所有Shader
                GLsizei len;
                glGetProgramiv( program_id, GL_INFO_LOG_LENGTH, &len );

                GLchar* log = new GLchar[len+1];
                glGetProgramInfoLog( program_id, len, &len, log );
                std::cerr << "Shader linking failed: " << log << std::endl;
                delete [] log;

                for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
                    glDeleteShader( entry->shader_id );
                    entry->shader_id = 0;
                }
        
                return;
            }
            
        }

        GLuint getId(){
            return program_id;
        }
        
        void setCurrent(){
            glUseProgram(program_id);
        }

        void setModelMatrix(glm::mat4 model_matrix){
            glProgramUniformMatrix4fv(program_id, glGetUniformLocation(program_id, "model_matrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));
        }

        void setViewMatrix(glm::mat4 view_matrix){
            glProgramUniformMatrix4fv(program_id, glGetUniformLocation(program_id, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
        }

        void setProjectionMatrix(glm::mat4 projection_matrix){
            glProgramUniformMatrix4fv(program_id, glGetUniformLocation(program_id, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
        }

        void setVec3(const char* name, glm::vec3 value){
            glProgramUniform3fv(program_id, glGetUniformLocation(program_id, name), 1, glm::value_ptr(value));
        }

        void setVec4(const char* name, glm::vec4 value){
            glProgramUniform4fv(program_id, glGetUniformLocation(program_id, name), 1, glm::value_ptr(value));
        }

        void setInt(const char* name, GLuint value){
            glProgramUniform1i(program_id, glGetUniformLocation(program_id, name), value);
        }

        void setFloat(const char* name, GLuint value){
            glProgramUniform1f(program_id, glGetUniformLocation(program_id, name), value);
        }



};

#endif