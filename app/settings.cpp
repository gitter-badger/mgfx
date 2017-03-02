#include "common.h"
#include "settings.h"

AppSettings& AppSettings::Instance()
{
    static AppSettings settings;
    return settings;
}

AppMode AppSettings::GetMode() const
{
    return m_currentMode;
}

void AppSettings::SetMode(AppMode mode)
{
    m_currentMode = mode;
}