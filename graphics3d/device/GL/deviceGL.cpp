#include "common.h"
#include "device/GL/deviceGL.h"
#include "camera/camera.h"
#include "scene/scene.h"
#include "scene/mesh.h"
#include "ui/windowmanager.h"
#include "ui/window.h"
#include "fileutils.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void APIENTRY DebugCB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *param)
{
    DeviceGL* pDevice = (DeviceGL*)param;
    if (type != GL_DEBUG_TYPE_OTHER)
    {
        if (type == GL_DEBUG_TYPE_PERFORMANCE)
        {
            LOG(INFO) << "GLPerf: " << message;
        }
        else if (type == GL_DEBUG_TYPE_ERROR)
        {
            LOG(ERROR) << "GL: " << message;
            DebugBreak();
        }
        else
        {
            LOG(WARNING) << "GL: " << message;
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
    SDL_GL_MakeCurrent(pWindow, glContext);

    static bool initOnce = false;
    if (!initOnce)
    {
        gl3wInit();
        initOnce = true;
    }

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

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);

    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");

    // Get a handle for our "myTextureSampler" uniform
    TextureID = glGetUniformLocation(programID, "albedo_sampler");
    TextureIDBump = glGetUniformLocation(programID, "normal_sampler");

    CameraID = glGetUniformLocation(programID, "camera_pos");
    LightDirID = glGetUniformLocation(programID, "light_dir");

    HasNormalMapID = glGetUniformLocation(programID, "has_normalmap");

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Setup ImGui binding
    ImGui_ImplSdlGL3_Init(pWindow);


    return true;
}

void DeviceGL::DestroyDeviceMeshes()
{
    SDL_GL_MakeCurrent(pWindow, glContext);

    for (auto& spMesh : m_mapDeviceMeshes)
    {
        DestroyDeviceMesh(spMesh.second.get());
    }
    m_mapDeviceMeshes.clear();
    
    for (auto& tex : m_mapTexToID)
    {
        glDeleteTextures(1, &tex.second);
    }
    m_mapTexToID.clear();
}

void DeviceGL::DestroyDeviceMesh(GLMesh* pDeviceMesh)
{
    SDL_GL_MakeCurrent(pWindow, glContext);
    for (auto& indexPart : pDeviceMesh->m_glMeshParts)
    {
        auto& spGLPart = indexPart.second;
        glDeleteBuffers(1, &spGLPart->normalID);
        glDeleteBuffers(1, &spGLPart->positionID);
        glDeleteBuffers(1, &spGLPart->uvID);
    }
}

uint32_t DeviceGL::LoadTexture(std::string path)
{
    SDL_GL_MakeCurrent(pWindow, glContext);
    auto itr = m_mapTexToID.find(path);
    if (itr == m_mapTexToID.end())
    {
        int w;
        int h;
        int comp;
        std::string root("models/sponza");
        std::string matPath = root + "/" + path;
        unsigned char* image = stbi_load(GetMediaPath(matPath.c_str()).c_str(), &w, &h, &comp, STBI_default);

        assert(image != nullptr);
        if (image != nullptr)
        {
            uint32_t textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            if (comp == 3)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            }
            else if (comp == 4)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            }

            CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
            CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
            CHECK_GL(glGenerateMipmap(GL_TEXTURE_2D));
            CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

            glBindTexture(GL_TEXTURE_2D, 0);

            stbi_image_free(image);
            m_mapTexToID[path] = textureID;
            return textureID;
        }
        else
        {
            return 0;
        }
    }
    return itr->second;
}

std::shared_ptr<GLMesh> DeviceGL::BuildDeviceMesh(Mesh* pMesh)
{
    SDL_GL_MakeCurrent(pWindow, glContext);
    auto spDeviceMesh = std::make_shared<GLMesh>();

    glBindVertexArray(VertexArrayID);

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

        if (spPart->MaterialID != -1)
        {
            auto& mat = pMesh->GetMaterials()[spPart->MaterialID];
            if (!mat.diffuse_texname.empty())
            {
                spGLPart->textureID = LoadTexture(mat.diffuse_texname);
            }

            if (!mat.bump_texname.empty())
            {
                spGLPart->textureIDBump = LoadTexture(mat.bump_texname);
            }
        }
        else
        {
            spGLPart->textureID = 0;
            spGLPart->textureIDBump = 0;
        }
        spDeviceMesh->m_glMeshParts[spPart.get()] = spGLPart;
    }

    return spDeviceMesh;
}

void DeviceGL::Draw(Mesh* pMesh)
{
    static std::vector<Mesh*> alphaMeshes;

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

    glBindVertexArray(VertexArrayID);

    for (auto& indexPart : pDeviceMesh->m_glMeshParts)
    {
        auto spGLPart = indexPart.second;

        CHECK_GL(glEnableVertexAttribArray(0));
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, spGLPart->positionID));
        // attrib, size, type, normalized, stride, offset 
        CHECK_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));

        CHECK_GL(glEnableVertexAttribArray(1));
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, spGLPart->normalID));
        CHECK_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));

        CHECK_GL(glEnableVertexAttribArray(2));
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, spGLPart->uvID));
        CHECK_GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));

        CHECK_GL(glActiveTexture(GL_TEXTURE0));
        if (spGLPart->textureID)
        {
            CHECK_GL(glBindTexture(GL_TEXTURE_2D, spGLPart->textureID));
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        CHECK_GL(glActiveTexture(GL_TEXTURE1));
        if (spGLPart->textureIDBump)
        {
            glUniform1i(HasNormalMapID, 1);
            CHECK_GL(glBindTexture(GL_TEXTURE_2D, spGLPart->textureIDBump));
        }
        else
        {
            glUniform1i(HasNormalMapID, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glDrawArrays(GL_TRIANGLES, 0, spGLPart->numVertices);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

bool DeviceGL::Render(Window* pWindow)
{
    SDL_GL_MakeCurrent(pWindow->GetSDLWindow(), glContext);
    if (!m_spScene)
    {
        return true;
    }

    auto clear_color = m_spScene->GetClearColor();
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto& pCamera = pWindow->GetCamera();
    if (pCamera)
    {
        glViewport(0, 0, pCamera->GetFilmSize().x, pCamera->GetFilmSize().y);
    }

    // Use our shader
    CHECK_GL(glUseProgram(programID));

    glm::mat4 projection = pCamera->GetProjection();
    glm::mat4 view = pCamera->GetLookAt();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 MVP = projection * view * model;

    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    CHECK_GL(glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]));
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &view[0][0]);

    glm::vec3 cameraPos = pCamera->GetPosition();
    glm::vec3 cameraLook = pCamera->GetViewDirection();

    glUniform3f(CameraID, cameraPos.x, cameraPos.y, cameraPos.z);
    glUniform3f(LightDirID, cameraLook.x, cameraLook.y, cameraLook.z);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(TextureID, 0);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(TextureIDBump, 1);

    m_spScene->Render(this);

    return true;
}

void DeviceGL::Cleanup()
{
    SDL_GL_MakeCurrent(pWindow, glContext);

    ImGui_ImplSdlGL3_Shutdown();

    DestroyDeviceMeshes();

    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

    // Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(pWindow);
}

void DeviceGL::Prepare2D()
{
    SDL_GL_MakeCurrent(pWindow, glContext);
    ImGui_ImplSdlGL3_NewFrame(pWindow);

    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
}

void DeviceGL::ProcessEvent(SDL_Event& event)
{
    SDL_GL_MakeCurrent(pWindow, glContext);
    ImGui_ImplSdlGL3_ProcessEvent(&event);
}

void DeviceGL::Swap()
{
    SDL_GL_MakeCurrent(pWindow, glContext);
    SDL_GL_SwapWindow(pWindow);
}
