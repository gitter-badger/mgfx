#include "glrender.h"
#include "GL/gl3w.h"
#include "GL/shader.h"
#include "GL/texture.h"
#include "geometry/objloader.h"

#include "SDL.h"
#include <filesystem>

namespace fs = std::experimental::filesystem::v1;

GLRender::GLRender()
{

}

std::string GetMediaPath(const char* pszMediaName)
{
    fs::path basePath(SDL_GetBasePath());
    return (basePath / pszMediaName).generic_string();
}

bool GLRender::Init()
{
    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);


    // Create and compile our GLSL program from the shaders
    programID = LoadShaders(GetMediaPath("StandardShading.vertexshader").c_str(), GetMediaPath("StandardShading.fragmentshader").c_str());

    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");

    // Load the texture
    Texture = loadDDS(GetMediaPath("uvmap.DDS").c_str());
    if (!Texture)
    {
        Cleanup();
        return false;
    }

    // Get a handle for our "myTextureSampler" uniform
    TextureID = glGetUniformLocation(programID, "myTextureSampler");

    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ(GetMediaPath("suzanne.obj").c_str(), vertices, uvs, normals);
    if (!res)
    {
        Cleanup();
        return false;
    }
    numVertices = uint32_t(vertices.size());

    // Load it into a VBO
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    return true;
}

bool GLRender::Render(Camera* pCamera)
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, pCamera->GetFilmSize().x, pCamera->GetFilmSize().y);

    // Use our shader
    glUseProgram(programID);

    glm::mat4 projection = pCamera->GetProjection();
    glm::mat4 view = pCamera->GetLookAt();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 MVP = projection * view * model;

    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &view[0][0]);

    glm::vec3 lightPos = glm::vec3(4, 4, 4);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(TextureID, 0);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
        2,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, numVertices);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    return true;
}

void GLRender::Cleanup()
{
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &vertexbuffer);

    glDeleteTextures(1, &Texture);

    glDeleteVertexArrays(1, &VertexArrayID);

    glDeleteProgram(programID);
}
