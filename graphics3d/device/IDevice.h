#pragma once

class Scene;
class Mesh;
class Window;
struct WindowData;

// An abstracted device, called to do the final rendering of the scene
struct IDevice
{
    // Create the device, tell it the scene
    virtual bool Init(std::shared_ptr<Scene>& spScene) = 0;

    // Render the scene with 3D
    virtual bool Prepare3D() = 0;

    // Render the scene with 2D
    virtual bool Prepare2D() = 0;

    virtual void Draw2D(const std::vector<glm::u8vec4>& data, const glm::uvec2& size) = 0;

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