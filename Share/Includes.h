#pragma once


#pragma comment (lib,"WS2_32.lib")
#include <WS2tcpip.h>

#pragma warning(push)
#pragma warning(disable: 26495)
#pragma warning(disable: 26812)
#pragma warning(disable: 4005)

#include "Dependencies/OPGL/glm/glm.hpp"
#include "Dependencies/OPGL/glm/gtx/projection.hpp"
#include "Dependencies/OPGL/glm/gtx/transform.hpp"
#include "Dependencies/OPGL/glm/gtx/quaternion.hpp"
#include "Dependencies/OPGL/glm/gtx/matrix_decompose.hpp"
#include "Dependencies/OPGL/glm/ext.hpp"

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
#include <list>
#include <mutex>
#include <shared_mutex>
#include <set>
#include <unordered_set>
#include <concurrent_unordered_set.h>
#include <concurrent_unordered_map.h>
#include <concurrent_priority_queue.h>
#include <concurrent_queue.h>
#include <concurrent_vector.h>
#include <sqlext.h>

using namespace std;
using namespace concurrency;
#include "SocketUtil.h"
