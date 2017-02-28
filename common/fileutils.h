#pragma once

namespace fs = std::experimental::filesystem::v1;

fs::path GetMediaPath(const char* pszMediaName);
fs::path GetMediaPath(const char* pszMediaName, const fs::path& rootPath);
fs::path GetDir(const fs::path& path);
std::string ReadFile(const fs::path& fileName);
