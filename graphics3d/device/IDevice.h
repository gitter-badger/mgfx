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

    // Render the scene
    virtual bool Prepare3D() = 0;

    // Prepare the device state for ImGUI/2D rendering
    virtual void Prepare2D() = 0;

    // Complete the 2D Rendering
    virtual void Finish2D() = 0;

    // Cleanup the device before shutting down
    virtual void Cleanup() = 0;

    // Process relevent events
    virtual void ProcessEvent(SDL_Event& event) = 0;

    // Copy the back buffer to the screen window
    virtual void Swap() = 0;

    // Draw a mesh
    virtual void Draw(Mesh* pMesh) = 0;

    // Get the window associated with the device
    virtual SDL_Window* GetWindow() const = 0;
};