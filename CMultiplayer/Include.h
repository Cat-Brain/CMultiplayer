#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdbool.h>
#include "FastNoiseLite.h"
#include "resource.h"
#include <math.h>
#include <ft2build.h>
#include <Windows.h>
#include <glad/glad.h>
#include <omp.h>
#include <GLFW/glfw3.h>
#include FT_FREETYPE_H
#include <cglm/cglm.h>



#pragma comment(lib, "Ws2_32.lib")



#define DEFAULT_PORT ("27015")
#define DEFAULT_BUFLEN (512)

#define WINDOW_DEFAULT_WIDTH (800)
#define WINDOW_DEFAULT_HEIGHT (600)

#define WINDOW_NAME ("Multiplayer game! =]")

#define MAX_USERNAME_LENGTH (100)
#define MAX_IP_ADDRESS_LENGTH (100)



typedef unsigned int uint;