#pragma once

// Don't let windows include the silly min/max macro
#define NOMINMAX

// Standard library, used all the time.
#include <cstdint>  // uint64_t, etc.
#include <string>
#include <chrono>     // Timing
#include <algorithm>  // Various useful things
#include <memory>     // shared_ptr

// Containers, used all the time.
#include <vector>
#include <map>

// IO
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

// Include GLM Math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/glm/gtx/rotate_vector.hpp"
#include "glm/glm/gtc/quaternion.hpp"
#include "glm/glm/gtc/random.hpp"
#include "glm/glm/gtx/string_cast.hpp"

// Extra math stuff
#include "mathutils.h"

// File utilities
#include "fileutils.h"

// 2D Gui
#include <imgui/imgui.h>

// Platform toolkit ; audio, windows, input, etc.
#include <SDL.h>

// For logging events to file
#include <easylogging/src/easylogging++.h>


