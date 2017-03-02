#pragma once

enum class AppMode
{
    Display2D = 0,
    Display3D
};

class AppSettings
{
public:
    static AppSettings& Instance();

    AppMode GetMode() const;
    void SetMode(AppMode mode);

private:
    AppMode m_currentMode = AppMode::Display2D;
};
