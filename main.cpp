// ImGui - standalone example application for SDL2 + OpenGL
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <imgui.h>
#include "imgui_impl_sdl_gl3.h"
#include <stdio.h>
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <SDL.h>

static GLuint
compile_shader(GLenum type, const GLchar *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint param;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &param);
    if (!param) {
        GLchar log[4096];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        fprintf(stderr, "error: %s: %s\n",
            type == GL_FRAGMENT_SHADER ? "frag" : "vert", (char *)log);
        exit(EXIT_FAILURE);
    }
    return shader;
}

static GLuint
link_program(GLuint vert, GLuint frag)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    GLint param;
    glGetProgramiv(program, GL_LINK_STATUS, &param);
    if (!param) {
        GLchar log[4096];
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        fprintf(stderr, "error: link: %s\n", (char *)log);
        exit(EXIT_FAILURE);
    }
    return program;
}

const float SQUARE[] = {
    -1.0f,  1.0f,
    -1.0f, -1.0f,
     1.0f,  1.0f,
     1.0f, -1.0f
};
const int ATTRIB_POINT = 0;
struct graphics_context {
    GLuint program;
    GLint uniform_angle;
    GLuint vbo_point;
    GLuint vao_point;
    double angle = 0.0;
};
#define countof(x) (sizeof(x) / sizeof(0[x]))

static void
render(struct graphics_context *context)
{
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, (int)100, (int)100);
    glUseProgram(context->program);
    glUniform1f(context->uniform_angle, (float)context->angle);
    glBindVertexArray(context->vao_point);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, countof(SQUARE) / 2);
    glBindVertexArray(0);
    glUseProgram(0);

    /* Physics */
    context->angle += 1.0;
    if (context->angle > 2 * M_PI)
        context->angle -= 2 * M_PI;

}

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window *window = SDL_CreateWindow("GLShell", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    gl3wInit();

    /* Shader sources */
    const GLchar *vert_shader =
        "#version 330\n"
        "layout(location = 0) in vec2 point;\n"
        "uniform float angle;\n"
        "void main() {\n"
        "    mat2 rotate = mat2(cos(angle), -sin(angle),\n"
        "                       sin(angle), cos(angle));\n"
        "    gl_Position = vec4(0.75 * rotate * point, 0.0, 1.0);\n"
        "}\n";
    const GLchar *frag_shader =
        "#version 330\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    color = vec4(1, 0.15, 0.15, 0);\n"
        "}\n";

    /* Compile and link OpenGL program */
    GLuint vert = compile_shader(GL_VERTEX_SHADER, vert_shader);
    GLuint frag = compile_shader(GL_FRAGMENT_SHADER, frag_shader);
    graphics_context context;
    context.program = link_program(vert, frag);
    context.uniform_angle = glGetUniformLocation(context.program, "angle");
    glDeleteShader(frag);
    glDeleteShader(vert);

    /* Prepare vertex buffer object (VBO) */
    glGenBuffers(1, &context.vbo_point);
    glBindBuffer(GL_ARRAY_BUFFER, context.vbo_point);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SQUARE), SQUARE, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Prepare vertrex array object (VAO) */
    glGenVertexArrays(1, &context.vao_point);
    glBindVertexArray(context.vao_point);
    glBindBuffer(GL_ARRAY_BUFFER, context.vbo_point);
    glVertexAttribPointer(ATTRIB_POINT, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(ATTRIB_POINT);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // Setup ImGui binding
    ImGui_ImplSdlGL3_Init(window);

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

    // Main loop
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
        }
        render(&context);

        ImGui_ImplSdlGL3_NewFrame(window);

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            static float f = 0.0f;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            if (ImGui::Button("Test Window")) show_test_window ^= 1;
            if (ImGui::Button("Another Window")) show_another_window ^= 1;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (show_another_window)
        {
            ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello");
            ImGui::End();
        }

        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }

        // Rendering
//        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
 //       glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        ImGui::Render();
        SDL_GL_SwapWindow(window);
    }
    glDeleteVertexArrays(1, &context.vao_point);
    glDeleteBuffers(1, &context.vbo_point);
    glDeleteProgram(context.program);
    // Cleanup
    ImGui_ImplSdlGL3_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
