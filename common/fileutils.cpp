#include "common.h"
#include "fileutils.h"
#include "sdl/include/SDL_filesystem.h"

#include <filesystem>

namespace fs = std::experimental::filesystem::v1;

std::string GetMediaPath(const char* pszMediaName)
{
    fs::path basePath(SDL_GetBasePath());
    return (basePath / pszMediaName).generic_string();
}

std::string GetDir(const char* pszPath)
{
    fs::path p(pszPath);
    return p.parent_path().string() + "/";
}