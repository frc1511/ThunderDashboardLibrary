#include <ThunderDashboard/dashboard.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <ThunderDashboard/font_manager.h>

using namespace frc1511;

FontManager FontManager::instance;

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 700

// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
# error "OpenGL ES 2 is not supported"
#elif defined(__APPLE__)
# define GLSL_VERSION "#version 150" // GL 3.2 + GLSL 150
#else
# define GLSL_VERSION "#version 130" // GL 3.0 + GLSL 130
#endif

static void glfwErrorCallback(int error, const char* description) {
  std::cerr << "ERROR: GLFW error " << error << ": " << description << '\n';
}

static void set_imgui_style();

#ifdef THUNDER_WINDOWS
#include <Windows.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
#else
int main(int argc, char** argv) {
#endif
  int exit_code = 0;
  
  // --- Initialize GLFW ---
  
  glfwSetErrorCallback(glfwErrorCallback);
  if (!glfwInit()) {
    exit(-1);
  }
  
  // --- Initialize OpenGL ---
  
  // Decide GL+GLSL versions
#ifdef __APPLE__
  // GL 3.2 + GLSL 150
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
  // GL 3.0 + GLSL 130
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
  
  // --- Initialize window ---
  
  GLFWwindow* window;
  if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "1511 Dashboard", nullptr, nullptr))) {
    exit(-1);
  }
  
  glfwMakeContextCurrent(window);
  // VSync.
  glfwSwapInterval(true);

  // Load OpenGL functions.
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  // --- Initialize Dashboard ---
  
  ThunderDashboard* dashboard = get_dashboard();
  
  glfwSetWindowUserPointer(window, reinterpret_cast<void*>(dashboard));
  
  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    reinterpret_cast<ThunderDashboard*>(glfwGetWindowUserPointer(window))->handle_keyboard(key, scancode, action, mods);
  });

  dashboard->init_window(window);
  dashboard->init();
  
  // --- Initialize ImGui ---
  
  IMGUI_CHECKVERSION();
  
  ImGuiContext* context = ImGui::CreateContext();
  
  ImGuiIO* io = &ImGui::GetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  
  set_imgui_style();

  // Setup user data.
  ImGuiSettingsHandler ini_handler;
  ini_handler.UserData = reinterpret_cast<void*>(dashboard);
  
  ini_handler.ClearAllFn = [](ImGuiContext*, ImGuiSettingsHandler* handler) { reinterpret_cast<ThunderDashboard*>(handler->UserData)->data_clear(); };
  ini_handler.ReadInitFn = [](ImGuiContext*, ImGuiSettingsHandler*) { };
  ini_handler.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler* handler, const char* name) -> void* { return reinterpret_cast<void*>(reinterpret_cast<ThunderDashboard*>(handler->UserData)->data_should_open(name)); };
  ini_handler.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler* handler, void*, const char* line) { reinterpret_cast<ThunderDashboard*>(handler->UserData)->data_read_line(line); };
  ini_handler.ApplyAllFn = [](ImGuiContext*, ImGuiSettingsHandler* handler) { reinterpret_cast<ThunderDashboard*>(handler->UserData)->data_apply(handler->TypeName); };
  ini_handler.WriteAllFn = [](ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf) { reinterpret_cast<ThunderDashboard*>(handler->UserData)->data_write(handler->TypeName, buf); };
  
  ini_handler.TypeName = dashboard->get_data_name();
  ini_handler.TypeHash = ImHashStr(ini_handler.TypeName);
  context->SettingsHandlers.push_back(ini_handler);
  
  // Setup OpenGL backend.
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(GLSL_VERSION);
  
  // --- The main loop ---
  
  while (dashboard->is_running()) {
    if (glfwWindowShouldClose(window)) {
      dashboard->close();
    }
    
    glfwPollEvents();
    
    // --- Begin the next frame ---
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    bool main_running = dashboard->is_running();

    if (!ImGui::Begin("1511 Dashboard", &main_running,
                  ImGuiWindowFlags_MenuBar
                | ImGuiWindowFlags_NoDocking
                | ImGuiWindowFlags_NoTitleBar
                | ImGuiWindowFlags_NoCollapse
                | ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_NoMove
                | ImGuiWindowFlags_NoBringToFrontOnFocus
                | ImGuiWindowFlags_NoNavFocus)) {
      ImGui::End();
      dashboard->close();
      break;
    }
    
    ImGui::PopStyleVar(3);
    
    ImGuiIO* io = &ImGui::GetIO();
    
    // Submit the DockSpace
    if (io->ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspaceId = ImGui::GetID("DockSpace");
      ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }
    
    // --- Run the application ---
    
    dashboard->present();
    
    ImGui::End();
    
    // --- Render ---
    
    ImGui::Render();
    
    int buf_width, buf_height;
    glfwGetFramebufferSize(window, &buf_width, &buf_height);
    
    glViewport(0, 0, buf_width, buf_height);
    ImVec4 clear_color = ImVec4(0.05f, 0.07f, 0.09f, 1.0f);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }
    
    glfwSwapBuffers(window);
  }
  
  // --- De-initialize ImGui ---
  
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // --- De-initialize GLFW window ---
  
  glfwDestroyWindow(window);
  glfwTerminate();
  
  return exit_code;
}

#include <Ubuntu_Regular_ttf.h>
#include <Ubuntu_Bold_ttf.h>
// #include <Roboto_Regular_ttf.h>
// #include <Roboto_Bold_ttf.h>
#include <FontAwesome_Regular_ttf.h>
#include <FontAwesome_Solid_ttf.h>

#include <IconsFontAwesome5.h>

static void set_imgui_style() {
  ImGui::StyleColorsDark();
  ImGuiStyle& style = ImGui::GetStyle();
  style.PopupRounding = 2;
  style.WindowRounding = 2;
  style.ChildRounding = 2;
  style.FrameRounding = 2;
  style.ScrollbarRounding = 12;
  style.GrabRounding = 2;
  style.TabRounding = 2;
  style.WindowMenuButtonPosition = ImGuiDir_None;
  style.WindowTitleAlign.x = 0.5f;
  
  // Colors.
  ImVec4 yellow_high  = ImVec4(1.00f, 0.95f, 0.00f, 1.00f);
  ImVec4 yellow_low   = ImVec4(1.00f, 0.95f, 0.00f, 0.70f);
  ImVec4 red_very_low = ImVec4(0.93f, 0.11f, 0.14f, 0.20f);
  ImVec4 red_low      = ImVec4(0.93f, 0.11f, 0.14f, 0.50f);
  ImVec4 red_mid      = ImVec4(0.93f, 0.11f, 0.14f, 0.65f);
  ImVec4 red_high     = ImVec4(0.93f, 0.11f, 0.14f, 0.75f);
  ImVec4 grey_low     = ImVec4(0.10f, 0.12f, 0.15f, 1.00f);
  ImVec4 grey_mid     = ImVec4(0.13f, 0.15f, 0.17f, 1.00f);
  ImVec4 grey_high    = ImVec4(0.14f, 0.16f, 0.18f, 1.00f);
  ImVec4 bg           = ImVec4(0.05f, 0.07f, 0.09f, 1.00f);
  ImVec4 border       = ImVec4(0.17f, 0.18f, 0.21f, 1.00f);
  ImVec4 title_high   = ImVec4(0.09f, 0.11f, 0.13f, 1.00f);
  ImVec4 title_low    = ImVec4(0.07f, 0.09f, 0.11f, 1.00f);
  
  style.Colors[ImGuiCol_WindowBg] = bg;
  style.Colors[ImGuiCol_PopupBg] = bg;
  style.Colors[ImGuiCol_ChildBg] = bg;
  
  style.Colors[ImGuiCol_Border] = border;
  
  style.Colors[ImGuiCol_TitleBg] = title_low;
  style.Colors[ImGuiCol_TitleBgCollapsed] = title_low;
  style.Colors[ImGuiCol_TitleBgActive] = title_high;
  
  style.Colors[ImGuiCol_MenuBarBg] = title_high;
  
  style.Colors[ImGuiCol_Header] = grey_low;
  style.Colors[ImGuiCol_HeaderHovered] = grey_mid;
  style.Colors[ImGuiCol_HeaderActive] = grey_high;
  
  style.Colors[ImGuiCol_FrameBg] = grey_low;
  style.Colors[ImGuiCol_FrameBgHovered] = grey_mid;
  style.Colors[ImGuiCol_FrameBgActive] = grey_high;
  
  style.Colors[ImGuiCol_Button] = red_low;
  style.Colors[ImGuiCol_ButtonHovered] = red_mid;
  style.Colors[ImGuiCol_ButtonActive] = red_high;
  
  style.Colors[ImGuiCol_CheckMark] = red_high;
  style.Colors[ImGuiCol_SliderGrab] = red_low;
  style.Colors[ImGuiCol_SliderGrabActive] = red_high;
  
  style.Colors[ImGuiCol_SeparatorHovered] = yellow_low;
  style.Colors[ImGuiCol_SeparatorActive] = yellow_high;
  
  style.Colors[ImGuiCol_ResizeGrip] = red_very_low;
  style.Colors[ImGuiCol_ResizeGripHovered] = red_mid;
  style.Colors[ImGuiCol_ResizeGripActive] = red_high;
  
  style.Colors[ImGuiCol_Tab] = grey_mid;
  style.Colors[ImGuiCol_TabHovered] = red_mid;
  style.Colors[ImGuiCol_TabActive] = red_high;
  style.Colors[ImGuiCol_TabUnfocused] = grey_low;
  style.Colors[ImGuiCol_TabUnfocusedActive] = red_low;
  
  style.Colors[ImGuiCol_DockingPreview] = red_low;
  style.Colors[ImGuiCol_DockingEmptyBg] = bg;

  style.Colors[ImGuiCol_PlotHistogram] = red_mid;
  style.Colors[ImGuiCol_PlotHistogramHovered] = red_high;

  style.Colors[ImGuiCol_TableHeaderBg] = grey_low;
  style.Colors[ImGuiCol_TableBorderStrong] = grey_mid;
  style.Colors[ImGuiCol_TableBorderLight] = grey_mid;
  style.Colors[ImGuiCol_TableRowBg] = bg;
  style.Colors[ImGuiCol_TableRowBgAlt] = bg;
  
  ImGuiIO* io = &ImGui::GetIO();
  
  // Tell ImGui not to free fonts from memory.
  ImFontConfig font_cfg;
  font_cfg.FontDataOwnedByAtlas = false;

  static const ImWchar* ranges = io->Fonts->GetGlyphRangesDefault();

  FontManager::get()->regular = io->Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(Ubuntu_Regular_ttf), Ubuntu_Regular_ttf_size, 15.0f, &font_cfg, ranges);

  font_cfg.MergeMode = true;

  static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
  font_cfg.PixelSnapH = true;
  io->Fonts->AddFontFromMemoryTTF(FontAwesome_Regular_ttf, FontAwesome_Regular_ttf_size, 15.0f, &font_cfg, icon_ranges);
  io->Fonts->AddFontFromMemoryTTF(FontAwesome_Solid_ttf, FontAwesome_Solid_ttf_size, 15.0f, &font_cfg, icon_ranges);

  font_cfg.MergeMode = false;
  font_cfg.PixelSnapH = false;

  FontManager::get()->big = io->Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(Ubuntu_Bold_ttf), Ubuntu_Bold_ttf_size, 50.0f, &font_cfg, ranges);

  io->ConfigWindowsMoveFromTitleBarOnly = true;
}
