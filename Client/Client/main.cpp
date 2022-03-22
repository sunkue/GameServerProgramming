#include "stdafx.h"
#include "KeyboardEvent.h"
#include "System.h"
#include "Game.h"
#include "Renderer.h"
#include "Networker.h"

void DoNextFrame()
{
	KEY_BOARD_EVENT_MANAGER::get().ProcessInput();

	System::get().update();
	Game::get().update();
	Renderer::get().draw();

	SleepEx(0, true);

	glfwPollEvents();
	glfwSwapBuffers(System::get().window);
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

	while (false == Networker::get().get_ready())
	{
		if (glfwWindowShouldClose(window))
		{
			break;
		}

		SleepEx(0, true);
	}

	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		DoNextFrame();
	}

	glfwTerminate();
}

