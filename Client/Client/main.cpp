#include "stdafx.h"
#include "KeyboardEvent.h"
#include "System.h"
#include "Game.h"
#include "Renderer.h"

void DoNextFrame()
{
	KEY_BOARD_EVENT_MANAGER::get().ProcessInput();

	System::get().update();
	Game::get().update();
	Renderer::get().draw();

	glfwPollEvents();
	glfwSwapBuffers(System::get().window);
}

void BindDefaultInputFuncs()
{

}

int main()
{
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


	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow* window, int w, int h)
		{  });
	
	glfwSetKeyCallback(window,
		[](GLFWwindow* window, int key, int code, int action, int modifiers)
		{ KEY_BOARD_EVENT_MANAGER::get().KeyBoard(window, key, code, action, modifiers);  });

	System::get().window = window;

	BindDefaultInputFuncs();

	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		DoNextFrame();
	}

	glfwTerminate();
}

