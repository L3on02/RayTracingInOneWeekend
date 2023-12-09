#define GLEW_STATIC

#include <GL/glew.h> // has to be included first!
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

GLFWwindow *create_window(int height, double aspect_ratio)
{
    int width = std::ceil(height * aspect_ratio);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, true);
    // glfwWindowHint(GLFW_DECORATED, false);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Anti Aliasing - Multisampling
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    std::ostringstream title;
    title << "ParallelSystems - (" << width << "x" << height << ")";

    return glfwCreateWindow(width, height, title.str().c_str(), nullptr, nullptr);
}

void handleEvents(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
}

bool show_render = false;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        show_render = !show_render;
    }
}

static void errorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

ImGuiIO &setupImGUI(GLFWwindow *window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    const char *glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return io;
}

void render()
{
    // clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLubyte *read_ppm(const char *filename, int *width, int *height)
{
    GLubyte *texture; // The texture image
    std::ifstream file;
    file.open(filename, std::ios::in);
    if (file.fail())
    {
        std::cerr << "Error loading the texture" << std::endl;
    }
    std::string skip;
    std::getline(file, skip);
    int texture_width, texture_height, max_value;

    file >> texture_width;
    file >> texture_height;
    file >> max_value;
    texture = new GLubyte[texture_width * texture_height * 4];

    int m, n, c;
    for (m = texture_height - 1; m >= 0; m--)
    {
        for (n = 0; n < texture_width; n++)
        {
            file >> c;
            texture[(m * texture_width + n) * 4] = (GLubyte)c;
            file >> c;
            texture[(m * texture_width + n) * 4 + 1] = (GLubyte)c;
            file >> c;
            texture[(m * texture_width + n) * 4 + 2] = (GLubyte)c;
            texture[(m * texture_width + n) * 4 + 3] = (GLubyte)255;
        }
    }
    file.close();
    return texture;
}

GLuint render_image(int width, int height)
{
    std::string filename = "/home/lmaag/RayTracingInOneWeekend/build-cuda/out.ppm"; // std::filesystem::current_path().string().append("/image.ppm").c_str();
    GLubyte *image_data = read_ppm(filename.c_str(), &width, &height);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Create a OpenGL texture identifier
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

    delete[] image_data;

    return texture;
}
