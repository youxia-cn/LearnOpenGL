#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "mesh.hpp"
#include "shader.hpp"

class Model
{
private:
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;
    ShaderProgram shaderProgram;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 cameraPosition;
    glm::vec3 positionOfLight1;
    glm::vec3 positionOfLight2;

    unsigned int loadTextureFromFile(const char* path, const std::string & directory){
        std::string filename = std::string(path);
        filename = directory + '/' + filename;
        std::cout << "TextureFromFile(path, directory):" << path << "," << directory << std::endl;
        std::cout << "filename:" << filename << std::endl;
        unsigned int textureID;
        glGenTextures(1, &textureID);
        
        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if(data){
            GLenum format;
            if(nrComponents == 1){
                format = GL_RED;
            }else if(nrComponents == 3){
                format = GL_RGB;
            }else if(nrComponents == 4){
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
        }else{
            std::cout << "Texture failed to load at path: " << path << std::endl;
            textureID = 0;
        }
        return textureID;
    }

    GLuint loadMaterialTexture(aiMaterial *mat, aiTextureType type){
        if(mat->GetTextureCount(type) > 0){ //假设每个 Mesh 每种纹理只使用一个
            GLuint textureId = 0;
            aiString texture_path;
            mat->GetTexture(type, 0, &texture_path);
            std::cout << "loadMaterialTextures: texture_path :" << texture_path.C_Str() << std::endl;
            bool skip = false;
            //载入过了的纹理图像就不再重复载入了，直接使用已载入的纹理信息
            for(int j=0; j<textures_loaded.size(); j++){
                if(std::strcmp(textures_loaded[j].path.c_str(), texture_path.C_Str())==0){
                    textureId = textures_loaded[j].id;
                    skip = true;
                    break;                
                }
            }
            //如果不是载入过了的纹理，则加载它
            if(!skip){
                Texture texture;
                texture.id = loadTextureFromFile(texture_path.C_Str(), directory);
                texture.path = texture_path.C_Str();
                textures_loaded.push_back(texture);
                textureId = texture.id;
            }
            return textureId;
        }else{
            return 0;
        }
        
    }

public:
    void loadModel(std::string filename)
    {
        Assimp::Importer importer;
        std::cout << "loadModel(std::string filename):" << filename << std::endl;
        const aiScene *scene = importer.ReadFile(filename,
                                                 aiProcess_Triangulate |
                                                     aiProcess_GenNormals |
                                                     aiProcess_FlipUVs);

        directory = filename.substr(0, filename.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene)
    {
        for (int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            Vertex tempVertex;
            std::vector<Vertex> tempVertices;
            std::vector<GLuint> tempIndices;
            //先读取顶点信息
            for (int j = 0; j < mesh->mNumVertices; j++)
            {
                if (mesh->HasPositions())
                {
                    tempVertex.position.x = mesh->mVertices[j].x;
                    tempVertex.position.y = mesh->mVertices[j].y;
                    tempVertex.position.z = mesh->mVertices[j].z;
                    tempVertex.position.w = 1.0f;
                }
                if (mesh->HasNormals())
                {
                    tempVertex.normal.x = mesh->mNormals[j].x;
                    tempVertex.normal.y = mesh->mNormals[j].y;
                    tempVertex.normal.z = mesh->mNormals[j].z;
                }
                if (mesh->HasTextureCoords(0))
                {
                    tempVertex.texCoord.x = mesh->mTextureCoords[0][j].x;
                    tempVertex.texCoord.y = mesh->mTextureCoords[0][j].y;
                }
               

                tempVertices.push_back(tempVertex);
            }
            //再读取索引信息
            for (int i = 0; i < mesh->mNumFaces; i++)
            {
                for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
                {
                    tempIndices.push_back(mesh->mFaces[i].mIndices[j]);
                }
            }

            Mesh tempMesh;

            tempMesh.setVertices(std::move(tempVertices));
            tempMesh.setIndices(std::move(tempIndices));
            tempMesh.setup();
            //最后读取材质信息
            if(mesh->mMaterialIndex >=0){
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                aiColor3D tempColor;
                if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_AMBIENT, tempColor)){
                    tempMesh.setKa(glm::vec3(1.0f));
                }else{
                    tempMesh.setKa(glm::vec3(tempColor.r, tempColor.g, tempColor.b));
                }
                if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, tempColor)){
                    tempMesh.setKd(glm::vec3(1.0f));
                }else{
                    tempMesh.setKd(glm::vec3(tempColor.r, tempColor.g, tempColor.b));
                }
                if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_SPECULAR, tempColor)){
                    tempMesh.setKs(glm::vec3(1.0f));
                }else{
                    tempMesh.setKs(glm::vec3(tempColor.r, tempColor.g, tempColor.b));
                }
                float tempShininess;
                if(AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, tempShininess)){
                    tempMesh.setNs(1.0f);
                }else{
                    tempMesh.setNs(tempShininess);
                }
                GLuint mapKd = loadMaterialTexture(material, aiTextureType_DIFFUSE);
                if(mapKd){
                    tempMesh.setMapKd(mapKd);
                }
                GLuint mapKs = loadMaterialTexture(material, aiTextureType_SPECULAR);
                if(mapKs){
                    tempMesh.setMapKs(mapKs);
                };
            }
            
            this->meshes.push_back(std::move(tempMesh));
        }

        if (node->mNumChildren != 0)
        {
            for (int k = 0; k < node->mNumChildren; k++)
            {
                processNode(node->mChildren[k], scene);
            }
        }
        return;
    }

    void setShaderProgram(ShaderProgram shaderProgram){
        this->shaderProgram = shaderProgram;
        for(int i=0; i<meshes.size(); i++){
            meshes[i].setShaderProgram(shaderProgram);
        }
    }
    
    void setViewMatrix(glm::mat4 matrix){
        viewMatrix = matrix;
    }

    void setModelMatrix(glm::mat4 matrix){
        modelMatrix = matrix;
    }

    void setProjectionMatrix(glm::mat4 matrix){
        projectionMatrix = matrix;
    }

    void setCameraPosition(glm::vec3 position){
        cameraPosition = position;
    }

    void setPositionOfLight1(glm::vec3 position){
        positionOfLight1 = position;
    }

    void setPositionOfLight2(glm::vec3 position){
        positionOfLight2 = position;
    }

    void render()
    {
        for (auto i = meshes.begin(); i != meshes.end(); i++)
        {
            i->getShaderProgram().setModelMatrix(modelMatrix);
            i->getShaderProgram().setViewMatrix(viewMatrix);
            i->getShaderProgram().setProjectionMatrix(projectionMatrix);
            i->getShaderProgram().setVec3("cameraPosition", cameraPosition);
            i->getShaderProgram().setVec3("positionOfLight1", positionOfLight1);
            i->getShaderProgram().setVec3("positionOfLight2", positionOfLight2);
            i->render();
        }
    }
};

#endif