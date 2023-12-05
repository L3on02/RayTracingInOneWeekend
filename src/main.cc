#define GLEW_STATIC

#include <GL/glew.h> // has to be included first!
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>

#include "rtweekend.hh"

#include "camera.hh"
#include "color.hh"
#include "hittable_list.hh"
#include "sphere.hh"
#include "material.hh"

auto create_window(int height, double aspect_ratio) {
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
    //glfwWindowHint(GLFW_DECORATED, false);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Anti Aliasing - Multisampling
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    std::ostringstream title;
    title << "ParallelSystems - (" << width << "x" << height << ")";

    return glfwCreateWindow(width, height, title.str().c_str(), nullptr, nullptr);
}

void handleEvents(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
}

bool show_render = false;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		show_render = !show_render;
	}
}

static void errorCallback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

ImGuiIO& setupImGUI(GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return io;
}

void render() {
    //clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLubyte* read_ppm(const char* filename, int* width, int* height) {
    GLubyte *texture; // The texture image
    std::ifstream file;
    file.open(filename, std::ios::in);
    if (file.fail()) {
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
    for(m = texture_height - 1; m >= 0; m--) {
        for (n = 0; n < texture_width; n++) {
            file >> c;
            texture[(m * texture_width + n) * 4] = (GLubyte) c;
            file >> c;
            texture[(m * texture_width + n) * 4 + 1] = (GLubyte) c;
            file >> c;
            texture[(m * texture_width + n) * 4 + 2] = (GLubyte) c;
            texture[(m * texture_width + n) * 4 + 3] = (GLubyte) 255;
        }
    }
    file.close();
    return texture;
}

GLuint render_image(int width, int height) {
    std::string filename = std::filesystem::current_path().string().append("/image.ppm").c_str();
    GLubyte* image_data = read_ppm(filename.c_str(), &width, &height);

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

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

int main() {
    // World
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    } 

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
	
	glfwSetErrorCallback(errorCallback);

    int window_height = 720;
    auto window = create_window(window_height, 16.0/9.0);
    if (!window) {
        printf("Creation of window failed!");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        printf("Init of glew failed! %s\n", glewGetErrorString(err));
    }

    // Setup Dear ImGui context
    ImGuiIO& io = setupImGUI(window);

    // Our state
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.0f, 0.1f, 0.2f, 1.00f);

    camera cam;
    GLuint image;

    //glClearColor(0.0f, 0.1f, 0.2f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        handleEvents(window);
        glfwMakeContextCurrent(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		
        //render();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking;
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_AutoHideTabBar;
            dockspace_flags |= ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace", reinterpret_cast<bool *>(true), window_flags);
            ImGui::PopStyleVar();
            ImGui::PopStyleVar(2);

            // Submit the DockSpace
            ImGuiID dockspace_id = ImGui::GetID("Dockspace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            ImGui::End();
        }
        {
            if (show_render) {
                const ImGuiViewport *viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar;
                window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

                ImGui::Begin("Image", reinterpret_cast<bool *>(true), window_flags);
                ImGui::PopStyleVar();
                ImGui::PopStyleVar(2);

                //int image_width = std::ceil(cam.image_height * cam.aspect_ratio);
                //image = image = render_image(image_width, cam.image_height);
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImVec2 ws = ImGui::GetContentRegionAvail();
                int max_x = ws.y * cam.aspect_ratio;
                int centering = (ws.x - max_x) / 2;
                ImGui::GetWindowDrawList()->AddImage(
                        reinterpret_cast<ImTextureID>(image),
                        ImVec2(pos.x + centering, pos.y),
                        ImVec2(pos.x + max_x + centering, ws.y + pos.y),
                        ImVec2(0, 1), ImVec2(1, 0));
                /*if (image)
                    ImGui::Image(reinterpret_cast<ImTextureID>(image), {(float) cam.image_width, (float) image_height},
                                 ImVec2(0, 1), ImVec2(1, 0));*/
                ImGui::End();
            }
        }
        {
            ImGui::Begin("Renderer");
            enum Aspect_Ratio { AR43, AR169, Ratio_COUNT };
            static int ar = AR169;
            const double aspect_ratios[Ratio_COUNT] = { 4.0 / 3.0, 16.0 / 9.0 };
            const char* aspect_ratio_names[Ratio_COUNT] = { "4:3", "16:9" };
            const char* aspect_ratio = (ar >= 0 && ar < Ratio_COUNT) ? aspect_ratio_names[ar] : "Unknown";

            enum Image_Height { IW480P, IW720P, IW1080P, IW4K, IW8K, Height_COUNT };
            static int ih = IW720P;
            const int image_heights[Height_COUNT] = { 480, 720, 1080, 2160, 4320 };
            const char* height = (ih >= 0 && ih < Height_COUNT) ? std::to_string(image_heights[ih]).c_str() : "Unknown";

            if (ImGui::CollapsingHeader("Image Options", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("Aspect Ratio", &ar, 0, Ratio_COUNT - 1, aspect_ratio);
                ImGui::SliderInt("Image Height", &ih, 0, Height_COUNT - 1, height);
            }

            enum SPP { SPP1, SPP10, SPP50, SPP100, SPP_COUNT };
            static int spp = SPP10;
            const int spp_values[SPP_COUNT] = { 1, 10, 50, 100 };

            enum Depth { D1, D10, D25, D50, DEPTH_COUNT };
            static int depth = D50;
            const int depth_values[DEPTH_COUNT] = { 1, 10, 25, 50 };

            static int cpu_count = std::thread::hardware_concurrency();
            if (ImGui::CollapsingHeader("Render Options", ImGuiTreeNodeFlags_DefaultOpen)) {
                const char* spp_name = (spp >= 0 && spp < SPP_COUNT) ? std::to_string(spp_values[spp]).c_str() : "Unknown";
                ImGui::SliderInt("Samples per Pixel", &spp, 0, SPP_COUNT - 1, spp_name);
                const char* depth_name = (depth >= 0 && depth < DEPTH_COUNT) ? std::to_string(depth_values[depth]).c_str() : "Unknown";
                ImGui::SliderInt("Max Depth", &depth, 0, DEPTH_COUNT - 1, depth_name);
                ImGui::SliderInt("CPU Cores", &cpu_count, 1, std::thread::hardware_concurrency());
            }

            static int fov = 20;
            static int look_from[3] = {13,2,3};
            static int look_at[3] = {0,0,0};
            static double focal_length = 10.0;
            static float defocus_angle = 0.6f;
            if (ImGui::CollapsingHeader("Camera Options", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("FOV", &fov, 0, 90);
                ImGui::InputInt3("Camera Position", look_from);
                ImGui::InputInt3("Focus Point", look_at);
                ImGui::SliderAngle("Defocus Angle", &defocus_angle, 0, 180);
                ImGui::InputDouble("Focal Length", &focal_length, 0, 0, "%.1f");
            }
            ImGui::NewLine();

            ImVec2 sz = ImVec2(ImGui::GetWindowWidth() * 0.3f, 0.0f);
            if (ImGui::Button("Render", sz)) {
                cam.aspect_ratio = aspect_ratios[ar];
                cam.image_height = image_heights[ih];
                cam.samples_per_pixel = spp_values[spp];
                cam.max_depth = depth_values[depth];

                cam.processor_count = cpu_count;

                cam.vfov = fov;
                cam.lookfrom = point3(look_from[0],look_from[1],look_from[2]);
                cam.lookat = point3(look_at[0],look_at[1],look_at[2]);
                cam.vup = vec3(0,1,0);

                cam.defocus_angle = defocus_angle;
                cam.focus_dist = focal_length;

                cam.render(world);
                image = render_image(std::ceil(cam.image_height * cam.aspect_ratio), cam.image_height);
                show_render = true;
            }
            if (cam.last_render_time > 0) {
                //ImGui::SameLine();
                ImGui::Text("Last render: %.3fs | %dx%d", cam.last_render_time, cam.image_height,
                            (int) std::ceil(cam.image_height * cam.aspect_ratio));
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
		
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
	
	glfwDestroyWindow(window);
    glfwTerminate();

    //cam.render(world);
    return 0;
}