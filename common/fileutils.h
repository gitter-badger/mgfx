#pragma once

namespace fs = std::experimental::filesystem::v1;

std::string GetMediaPath(const char* pszMediaName);
std::string GetDir(const char* pszPath);
std::string ReadFile(const char* fileName);
