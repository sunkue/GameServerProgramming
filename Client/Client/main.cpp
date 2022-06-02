#include "stdafx.h"
#include "KeyboardEvent.h"
#include "System.h"
#include "Game.h"
#include "Renderer.h"
#include "Networker.h"

void DoNextFrame()
{
	KEY_BOARD_EVENT_MANAGER::Get().ProcessInput();

	System::Get().Update();
	Game::Get().update();
	Renderer::Get().Draw();
	
	SleepEx(0, true);

	glfwPollEvents();
	glfwSwapBuffers(System::Get().Window);
}

void BindDefaultInputFuncs()
{
	glfwSetFramebufferSizeCallback(System::Get().Window,
		[](GLFWwindow* window, int w, int h)
		{});

	glfwSetKeyCallback(System::Get().Window,
		[](GLFWwindow* window, int key, int code, int action, int modifiers)
		{ KEY_BOARD_EVENT_MANAGER::Get().KeyBoard(window, key, code, action, modifiers);  });


}

int main()
{
	Networker::Get().Start();

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	while (!Networker::Get().GetReady())
	{
		SleepEx(0, true);
	}

	GLFWwindow* window = glfwCreateWindow(System::Get().Screen.width, System::Get().Screen.height, "SUNKUE", NULL, NULL);
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

	System::Get().Window = window;

	BindDefaultInputFuncs();
	
	Game::Get().init();

	{
		IMGUI_CHECKVERSION();
		gui::CreateContext();
		gui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 450");
		auto& io = gui::GetIO();
		io.Fonts->AddFontFromFileTTF("Resource/Font/³ª´®¼Õ±Û¾¾ ´ÞÀÇ±Ëµµ.ttf", 15.f, nullptr, io.Fonts->GetGlyphRangesKorean());
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

