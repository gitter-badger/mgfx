#include "common.h"
#include "device/GL/deviceGL.h"
#include "camera/camera.h"
#include "scene/scene.h"
#include "scene/mesh.h"
#include "fileutils.h"

#include <iostream>

void APIENTRY DebugCB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *param)
{
    DeviceGL* pDevice = (DeviceGL*)param;
    if (type != GL_DEBUG_TYPE_OTHER)
    {
        if (type == GL_DEBUG_TYPE_PERFORMANCE)
        {
            std::cout << "GLPerfError: " << message;
        }
        else if (type == GL_DEBUG_TYPE_ERROR)
        {
            std::cout << "GLError: " << message;
            //DebugBreak();
        }
        else
        {
            std::cout << "GLError: " << message;
        }
    }
}

DeviceGL::DeviceGL()
{

}

bool DeviceGL::Init(std::shared_ptr<Scene>& pScene)
{
    m_spScene = pScene;

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    pWindow = SDL_CreateWindow("GLShell", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    glContext = SDL_GL_CreateContext(pWindow);

    gl3wInit();

    // Get useful debug messages
    CHECK_GL(glDebugMessageCallback((GLDEBUGPROC)&DebugCB, this));
    CHECK_GL(glEnable(GL_DEBUG_OUTPUT));

    //    SDL_GL_SetSwapInterval(0);

    // Enable depth test
    CHECK_GL(glEnable(GL_DEPTH_TEST));

    // Accept fragment if it closer to the camera than the former one
    CHECK_GL(glDepthFunc(GL_LESS));

    // Cull triangles which normal is not towards the camera
    CHECK_GL(glEnable(GL_CULL_FACE));

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders(GetMediaPath("shaders/GL/StandardShading.vertexshader").c_str(), GetMediaPath("shaders/GL/StandardShading.fragmentshader").c_str());

    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");

    // Load the texture
    Texture = loadDDS(GetMediaPath("textures/uvmap.DDS").c_str());
    if (!Texture)
    {
        Cleanup();
        return false;
    }

    // Get a handle for our "myTextureSampler" uniform
    TextureID = glGetUniformLocation(programID, "myTextureSampler");

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Setup ImGui binding
    ImGui_ImplSdlGL3_Init(pWindow);


    return true;
}

bool DeviceGL::PreRender()
{
    int x, y;
    SDL_GetWindowSize(pWindow, &x, &y);

    auto pCamera = m_spScene->GetCurrentCamera();
    if (pCamera)
    {
        pCamera->SetFilmSize(glm::uvec2(x, y));
        pCamera->PreRender();
    }

    return true;
}

void DeviceGL::DestroyDeviceMeshes()
{
    for (auto& spMesh : m_mapDeviceMeshes)
    {
        DestroyDeviceMesh(spMesh.second.get());
    }
    m_mapDeviceMeshes.clear();
}

void DeviceGL::DestroyDeviceMesh(GLMesh* pDeviceMesh)
{
    for (auto& indexPart : pDeviceMesh->m_glMeshParts)
    {
        auto& spGLPart = indexPart.second;
        glDeleteBuffers(1, &spGLPart->normalID);
        glDeleteBuffers(1, &spGLPart->positionID);
        glDeleteBuffers(1, &spGLPart->uvID);
    }
}

std::shared_ptr<GLMesh> DeviceGL::BuildDeviceMesh(Mesh* pMesh)
{
    auto spDeviceMesh = std::make_shared<GLMesh>();


    for (auto& spPart : pMesh->GetMeshParts())
    {
        auto spGLPart = std::make_shared<GLMeshPart>();
        spGLPart->numVertices = uint32_t(spPart->Positions.size());

        CHECK_GL(glGenBuffers(1, &spGLPart->positionID));
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, spGLPart->positionID));
        CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*spPart->Positions.size(), spPart->Positions.data(), GL_STATIC_DRAW));

        CHECK_GL(glGenBuffers(1, &spGLPart->normalID));
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, spGLPart->normalID));
        CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*spPart->Normals.size(), spPart->Normals.data(), GL_STATIC_DRAW));

        CHECK_GL(glGenBuffers(1, &spGLPart->uvID));
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, spGLPart->uvID));
        CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*spPart->UVs.size(), spPart->UVs.data(), GL_STATIC_DRAW));

        spDeviceMesh->m_glMeshParts[spPart.get()] = spGLPart;
    }

    return spDeviceMesh;
}

void DeviceGL::Draw(Mesh* pMesh)
{
    GLMesh* pDeviceMesh = nullptr;
    auto itrFound = m_mapDeviceMeshes.find(pMesh);
    if (itrFound == m_mapDeviceMeshes.end())
    {
        auto spDeviceMesh = BuildDeviceMesh(pMesh);
        m_mapDeviceMeshes[pMesh] = spDeviceMesh;
        pDeviceMesh = spDeviceMesh.get();
    }
    else
    {
        pDeviceMesh = itrFound->second.get();
    }

 //   glBindVertexArray(pDeviceMesh->VertexArrayID);

    // 1rst attribute buffer : vertices
    /*
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, pDeviceMesh->vertexbuffer);
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
    glBindBuffer(GL_ARRAY_BUFFER, pDeviceMesh->uvbuffer);
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
    glBindBuffer(GL_ARRAY_BUFFER, pDeviceMesh->normalbuffer);
    glVertexAttribPointer(
        2,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    */

    for (auto& indexPart : pDeviceMesh->m_glMeshParts)
    {
        auto spGLPart = indexPart.second;

        CHECK_GL(glEnableVertexAttribArray(0));
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, spGLPart->positionID));
        CHECK_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));

        CHECK_GL(glEnableVertexAttribArray(1));
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, spGLPart->normalID));
        CHECK_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));

        CHECK_GL(glEnableVertexAttribArray(2)); 
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, spGLPart->uvID));
        CHECK_GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
        glDrawArrays(GL_TRIANGLES, 0, spGLPart->numVertices);
    }

    // enable texture.
    /*
    Material mat = materials[mesh->matId];
    if (mat.diffuseTexFile != "") {
        GL_C(glBindTexture(GL_TEXTURE_2D, mat.diffuseTex));
        GL_C(glActiveTexture(GL_TEXTURE0));
        GL_C(glUniform1i(glGetUniformLocation(outputGeoShader, "uDiffTex"), 0));
    }
    */


    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

bool DeviceGL::Render()
{
    if (!m_spScene)
    {
        return true;
    }

    auto clear_color = m_spScene->GetClearColor();
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto& pCamera = m_spScene->GetCurrentCamera();
    if (pCamera)
    {
        glViewport(0, 0, pCamera->GetFilmSize().x, pCamera->GetFilmSize().y);
    }

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

    glm::vec3 lightPos = glm::vec3(4, 5000, 4);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(TextureID, 0);

    m_spScene->Render(this);

    return true;
}

void DeviceGL::Cleanup()
{
    ImGui_ImplSdlGL3_Shutdown();

    DestroyDeviceMeshes();

    glDeleteVertexArrays(1, &VertexArrayID);

    glDeleteTextures(1, &Texture);

    glDeleteProgram(programID);

    // Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(pWindow);
}

void DeviceGL::Prepare2D()
{
    ImGui_ImplSdlGL3_NewFrame(pWindow);

    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
}

void DeviceGL::ProcessEvent(SDL_Event& event)
{
    ImGui_ImplSdlGL3_ProcessEvent(&event);
}

void DeviceGL::Swap()
{
    SDL_GL_SwapWindow(pWindow);
}

#if false 
inline char* GetShaderLogInfo(GLuint shader) {
    GLint len;
    GLsizei actualLen;
    GL_C(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len));
    char* infoLog = new char[len];
    GL_C(glGetShaderInfoLog(shader, len, &actualLen, infoLog));
    return infoLog;
}

GLuint LoadTexture(const char* file) {
    std::vector<unsigned char> buffer;
    lodepng::load_file(buffer, file);

    lodepng::State state;
    unsigned int width, height;
    std::vector<unsigned char> imageData;
    unsigned error = lodepng::decode(imageData, width, height, state, buffer);

    if (error != 0) {
        printf("Could not load texture %s: %s\n", file, lodepng_error_text(error));
    }

    GLuint tex;
    GL_C(glGenTextures(1, &tex));

    GL_C(glBindTexture(GL_TEXTURE_2D, tex));

    GL_C(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data()));

    GL_C(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_C(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL_C(glGenerateMipmap(GL_TEXTURE_2D));
    GL_C(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_C(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GL_C(glBindTexture(GL_TEXTURE_2D, 0));

    return tex;
}

        Material mat = materials[mesh->matId];
        if (mat.diffuseTexFile != "") {
            GL_C(glBindTexture(GL_TEXTURE_2D, mat.diffuseTex));
            GL_C(glActiveTexture(GL_TEXTURE0));
            GL_C(glUniform1i(glGetUniformLocation(outputGeoShader, "uDiffTex"), 0));
        }

        glDrawArrays(GL_TRIANGLES, 0, mesh->positions.size());
    }


#endif
