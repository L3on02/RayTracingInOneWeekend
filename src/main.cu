#include "gui.cuh"
#include "cuda/gpu_render.cuh"
#include "cpp/cpu_render.hh"
#include <thread>

int main()
{
    glfwSetErrorCallback(errorCallback);

    int window_height = 1080;
    auto window = create_window(window_height, 16.0 / 9.0);
    if (!window)
    {
        printf("Creation of window failed!");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        printf("Init of glew failed! %s\n", glewGetErrorString(err));
    }

    // Setup Dear ImGui context
    ImGuiIO &io = setupImGUI(window);

    // Our state
    ImVec4 clear_color = ImVec4(0.0f, 0.1f, 0.2f, 1.00f);

    GLuint image;

    float image_aspect_ratio = 16.0f / 9.0f;
    double last_render_time = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        handleEvents(window);
        glfwMakeContextCurrent(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
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
            if (show_render)
            {
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

                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImVec2 ws = ImGui::GetContentRegionAvail();
                int max_x = ws.y * image_aspect_ratio;
                int centering = (ws.x - max_x) / 2;
                ImGui::GetWindowDrawList()->AddImage(
                    reinterpret_cast<ImTextureID>(image),
                    ImVec2(pos.x + centering, pos.y),
                    ImVec2(pos.x + max_x + centering, ws.y + pos.y),
                    ImVec2(0, 1), ImVec2(1, 0));
                ImGui::End();
            }
        }
        {
            ImGui::Begin("Renderer");
            enum Aspect_Ratio
            {
                AR43,
                AR169,
                Ratio_COUNT
            };
            static int ar = AR169;
            const double aspect_ratios[Ratio_COUNT] = {4.0 / 3.0, 16.0 / 9.0};
            const char *aspect_ratio_names[Ratio_COUNT] = {"4:3", "16:9"};
            const char *aspect_ratio = (ar >= 0 && ar < Ratio_COUNT) ? aspect_ratio_names[ar] : "Unknown";

            enum Image_Height
            {
                IW720P,
                IW1080P,
                IW1440P,
                IW4K,
                Height_COUNT
            };
            static int ih = IW1080P;
            const int image_heights[Height_COUNT] = {720, 1080, 1440, 2160};
            const char *height = (ih >= 0 && ih < Height_COUNT) ? std::to_string(image_heights[ih]).c_str() : "Unknown";

            if (ImGui::CollapsingHeader("Image Options", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SliderInt("Aspect Ratio", &ar, 0, Ratio_COUNT - 1, aspect_ratio);
                ImGui::SliderInt("Image Height", &ih, 0, Height_COUNT - 1, height);
            }

            enum Render_Method
            {
                CPU,
                GPU,
                OPTION_COUNT
            };
            static int rm = CPU;
            const bool render_methods[OPTION_COUNT] = {false, true};
            const char *render_method_names[OPTION_COUNT] = {"CPU", "GPU"};
            const char *render_method_name = (ar >= 0 && ar < OPTION_COUNT) ? render_method_names[rm] : "Unknown";

            enum SPP
            {
                SPP1,
                SPP10,
                SPP50,
                SPP100,
                SPP500,
                SPP1000,
                SPP_COUNT
            };
            static int spp = SPP10;
            const int spp_values[SPP_COUNT] = {1, 10, 50, 100, 500, 1000};

            enum Depth
            {
                D1,
                D10,
                D25,
                D50,
                DEPTH_COUNT
            };
            static int depth = D10;
            const int depth_values[DEPTH_COUNT] = {1, 10, 25, 50};

            static int cpu_count = std::thread::hardware_concurrency();
            static bool render_on_device = true;
            if (ImGui::CollapsingHeader("Render Options", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SliderInt("Render Method", &rm, 0, OPTION_COUNT - 1, render_method_name);
                const char *spp_name = (spp >= 0 && spp < SPP_COUNT) ? std::to_string(spp_values[spp]).c_str() : "Unknown";
                ImGui::SliderInt("Samples per Pixel", &spp, 0, SPP_COUNT - 1, spp_name);
                const char *depth_name = (depth >= 0 && depth < DEPTH_COUNT) ? std::to_string(depth_values[depth]).c_str() : "Unknown";
                ImGui::SliderInt("Max Depth", &depth, 0, DEPTH_COUNT - 1, depth_name);
                ImGui::SliderInt("CPU Cores", &cpu_count, 1, std::thread::hardware_concurrency());
            }

            static int fov = 20;
            static int look_from[3] = {13, 2, 3};
            static int look_at[3] = {0, 0, 0};
            static float defocus_angle = 0.6f;
            if (ImGui::CollapsingHeader("Camera Options", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SliderInt("FOV", &fov, 0, 90);
                ImGui::InputInt3("Camera Position", look_from);
                ImGui::InputInt3("Focal Point", look_at);
                ImGui::SliderFloat("Defocus Angle", &defocus_angle, 0, 1.0, "%.1f");
            }
            ImGui::NewLine();

            ImVec2 sz = ImVec2(ImGui::GetWindowWidth() * 0.3f, 0.0f);
            if (ImGui::Button("Render", sz))
            {
                image_aspect_ratio = aspect_ratios[ar];
                render_on_device = render_methods[rm];
                point cam_pos = {look_from[0], look_from[1], look_from[2]};
                point focal_point = {look_at[0], look_at[1], look_at[2]};

                if (render_on_device)
                    gpu_render(image_heights[ih], aspect_ratios[ar], spp_values[spp], depth_values[depth], cam_pos, focal_point, fov, defocus_angle, last_render_time);
                else
                    cpu_render(image_heights[ih], aspect_ratios[ar], spp_values[spp], depth_values[depth], cam_pos, focal_point, fov, defocus_angle, last_render_time);
                // void cpu_render(double _aspect_ratio, int _image_height, int _samples_per_pixel, int _max_depth, double _vfov, point _cam_pos, point _focal_point, double _aperture);

                image = render_image(std::ceil(image_heights[ih] * aspect_ratios[ar]), image_heights[ih]);
                show_render = true;
            }
            if (last_render_time > 0)
            {
                // ImGui::SameLine();
                ImGui::Text("Last render: %.3fs | %dx%d", last_render_time, image_heights[ih],
                            (int)std::ceil(image_heights[ih] * aspect_ratios[ar]));
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
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
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

    return 0;
}