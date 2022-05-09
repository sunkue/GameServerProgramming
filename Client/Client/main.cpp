#include "stdafx.h"
#include "KeyboardEvent.h"
#include "System.h"
#include "Game.h"
#include "Renderer.h"
#include "Networker.h"

void DrawGui();

void DoNextFrame()
{
	KEY_BOARD_EVENT_MANAGER::get().ProcessInput();

	System::get().update();
	Game::get().update();
	Renderer::get().draw();

	SleepEx(0, true);

	DrawGui();

	glfwPollEvents();
	glfwSwapBuffers(System::get().window);
}

void DrawGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	gui::NewFrame();
	gui::Begin("PlayerInfo", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	auto pos = Game::get().GetPlayer().get_pos();
	gui::Text(("Positon :: "s + to_string(pos.x) + " "s + to_string(pos.y)).c_str());
	// HP x/x
	// LEVEL 
	// EXP x/x
	gui::End();

	gui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(gui::GetDrawData());
}

void BindDefaultInputFuncs()
{
	glfwSetFramebufferSizeCallback(System::get().window,
		[](GLFWwindow* window, int w, int h)
		{});

	glfwSetKeyCallback(System::get().window,
		[](GLFWwindow* window, int key, int code, int action, int modifiers)
		{ KEY_BOARD_EVENT_MANAGER::get().KeyBoard(window, key, code, action, modifiers);  });


}

int main()
{
	Networker::get().start();

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(System::get().screen.width, System::get().screen.height, "SUNKUE", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	System::get().window = window;

	BindDefaultInputFuncs();

	{
		IMGUI_CHECKVERSION();
		gui::CreateContext();

		ImGuiIO& io = gui::GetIO();

		gui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}


	while (false == Networker::get().get_ready() && !glfwWindowShouldClose(window))
	{
		SleepEx(0, true);
	}

	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		DoNextFrame();
	}


	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		gui::DestroyContext();
	}

	glfwTerminate();
}

