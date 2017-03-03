#pragma once

class Scene;
class Mesh;
class Window;
struct WindowData;

// An abstracted device, called to do the final rendering of the scene
struct IDevice
{
    // Create the device, tell it the scene
    virtual bool Init(const std::shared_ptr<Scene>& spScene) = 0;

    // Render the scene with 3D
    virtual bool Begin3D() = 0;
    virtual void End3D() = 0;

    // Render the scene with 2D
    virtual bool Begin2D() = 0;
    virtual void End2D() = 0;

    // Quads
    virtual uint32_t CreateQuad() = 0; 
    virtual void DestroyQuad(uint32_t id) = 0;
    virtual void UpdateQuad(uint32_t id, const std::vector<glm::u8vec4>& quadData, const glm::uvec2& size) = 0;
    virtual void DrawQuad(uint32_t id, const glm::vec4& target) = 0;

    // For displaying the overlay GUI 
    virtual void BeginGUI() = 0;
    virtual void EndGUI() = 0;

    // Cleanup the device before shutting down
    virtual void Cleanup() = 0;

    // Process relevent events
    virtual void ProcessEvent(SDL_Event& event) = 0;

    // Copy the back buffer to the screen window
    virtual void Swap() = 0;

    // Draw a mesh
    virtual void Draw(Mesh* pMesh) = 0;

    // Get the window associated with the device
    virtual SDL_Window* GetSDLWindow() const = 0;
};