#include "ImguiPanel.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Render/Light.h"

void ST::ImguiPanel::Init(GLFWwindow* window) {
	const char* glsl_version = "#version 130";
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding              = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
}

void ST::ImguiPanel::Close() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ST::ImguiPanel::Render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void ST::ImguiPanel::NewFrame() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ST::ImguiPanel::CreatePointLightPanel(const char* name, ST_REF<PointLight> light) {
	ImGui::Begin(name, 0, ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable);
	ImGui::DragFloat3("Position", reinterpret_cast<float*>(&light->_pos), 0.05f);
	ImGui::DragFloat3("Ambient", reinterpret_cast<float*>(&light->_ia), 0.01f, 0, 1);
	ImGui::DragFloat3("Diffuse", reinterpret_cast<float*>(&light->_id), 0.01f, 0, 1);
	ImGui::DragFloat3("Specular", reinterpret_cast<float*>(&light->_is), 0.01f, 0, 1);
	ImGui::DragFloat("Const", reinterpret_cast<float*>(&light->_const), 0.01f, 0, 1);
	ImGui::DragFloat("Linear", reinterpret_cast<float*>(&light->_linear), 0.01f, 0, 1);
	ImGui::DragFloat("Quadratic", reinterpret_cast<float*>(&light->_quadratic), 0.01f, 0, 1);
	ImGui::End();
}

void ST::ImguiPanel::CreateDirLightPanel(const char* name, ST_REF<DirLight> light) {
	ImGui::Begin(name, 0, ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable);
	ImGui::DragFloat3("Direction", reinterpret_cast<float*>(&light->_dir), 0.05f);
	ImGui::DragFloat3("Ambient", reinterpret_cast<float*>(&light->_ia), 0.01f, 0, 1);
	ImGui::DragFloat3("Diffuse", reinterpret_cast<float*>(&light->_id), 0.01f, 0, 1);
	ImGui::DragFloat3("Specular", reinterpret_cast<float*>(&light->_is), 0.01f, 0, 1);
	ImGui::End();
}

void ST::ImguiPanel::ShowDemoPanel() {
	ImGui::ShowDemoWindow();
}
