#include "common.h"
#include "fileutils.h"
#include "sdl/include/SDL_filesystem.h"

fs::path GetMediaPath(const char* pszMediaName)
{
    fs::path basePath(SDL_GetBasePath());
    basePath = basePath / "assets" / pszMediaName;
    if (fs::exists(basePath))
    {
        return fs::absolute(basePath);
    }
    return fs::path();
}

fs::path GetMediaPath(const char* pszMediaName, const fs::path& rootPath)
{
    auto basePath = rootPath / pszMediaName;
    if (fs::exists(basePath))
    {
        return fs::absolute(basePath);
    }
    return (rootPath / pszMediaName);
}

fs::path GetDir(const fs::path& path)
{
    return path.parent_path();
}

std::string ReadFile(const fs::path& fileName)
{
    std::ifstream ifs(fileName);
    if (!ifs.is_open())
    {
        return std::string();
    }
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return buffer.str();
}