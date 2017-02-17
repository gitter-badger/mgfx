#pragma once
#include <imgui/imgui.h>
#include <GL/imgui_impl_sdl_gl3.h>
#include <stdio.h>
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <SDL.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <shader.hpp>
#include <texture.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>
