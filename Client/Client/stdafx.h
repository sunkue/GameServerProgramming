#pragma once

#include "../../Share/Share.h"

#pragma comment(lib,"./Dependencies/OPGL/x64GL/glew32.lib")
#pragma comment(lib,"./Dependencies/OPGL/x64GL/glew32s.lib")
#pragma comment(lib,"./Dependencies/OPGL/x64GL/freeglut.lib")
#pragma comment(lib,"./Dependencies/GLFW/glfw3.lib")
#pragma comment(lib,"./Dependencies/GLFW/glfw3_mt.lib")
#pragma comment(lib,"./Dependencies/GLFW/glfw3dll.lib")

#pragma warning(push)
#pragma warning(disable: 26495)
#pragma warning(disable: 26812)
#pragma warning(disable: 4005)

#include "Dependencies/glad/include/glad/glad.h"

#include "Dependencies/GLFW/GLFW/glfw3.h"
#include "Dependencies/GLFW/GLFW/glfw3native.h"

#pragma warning(pop)

#include <string>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <chrono>
#include <array>
#include <queue>
#include <vector>
#include <memory>
#include <map>
#include <thread>
#include <future>


using namespace std;
