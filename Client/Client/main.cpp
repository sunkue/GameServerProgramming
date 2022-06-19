#include "stdafx.h"
#include "KeyboardEvent.h"
#include "System.h"
#include "Game.h"
#include "Renderer.h"
#include "Networker.h"

void DoNextFrame()
{
	KeyboardEventManager::Get().ProcessInput();
	MouseEventManager::Get().ProcessInput();

	System::Get().Update();
	Game::Get().Update();
	Renderer::Get().Draw();
	
	SleepEx(0, true);

	glfwPollEvents();
	glfwSwapBuffers(System::Get().Window);
}

void BindEventFuncs()
{
	glfwSetFramebufferSizeCallback(System::Get().Window,
		[](GLFWwindow* window, int w, int h)
		{});

	glfwSetKeyCallback(System::Get().Window,
		[](GLFWwindow* window, int key, int code, int action, int modifiers)
		{ KeyboardEventManager::Get().Keyboard(window, key, code, action, modifiers);  });
	glfwSetScrollCallback(System::Get().Window,
		[](GLFWwindow* window, double xoffset, double yoffset)
		{ MouseEventManager::Get().Scroll(window, xoffset, yoffset);  });
	glfwSetMouseButtonCallback(System::Get().Window,
		[](GLFWwindow* window, int key, int action, int modifiers)
		{ MouseEventManager::Get().MouseButton(window, key, action, modifiers);  });
	glfwSetCursorPosCallback(System::Get().Window,
		[](GLFWwindow* window, double xpos, double ypos)
		{ MouseEventManager::Get().CursorPosition(window, xpos, ypos);  });
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

	BindEventFuncs();
	
	Game::Get().Init();

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

