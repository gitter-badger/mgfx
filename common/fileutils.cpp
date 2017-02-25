#include "common.h"
#include "fileutils.h"
#include "sdl/include/SDL_filesystem.h"



std::string GetMediaPath(const char* pszMediaName)
{
    fs::path basePath(SDL_GetBasePath());
    return (basePath / "assets" / pszMediaName).generic_string();
}

std::string GetDir(const char* pszPath)
{
    fs::path p(pszPath);
    return p.parent_path().string() + "/";
}

std::string ReadFile(const char* fileName)
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