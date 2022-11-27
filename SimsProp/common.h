#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <time.h>
#include <math.h>
#include <algorithm>


#include <time.h>
#include <math.h>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <system_error>
#include <windows.h>
#include <stdlib.h>


#define SZELES 480
#define MAGAS 480


#define EPSZ 0.001f
#define EPSZ2 0.000001f
#define DEBUG false
#define RANDOM false


/**
https://liu.diva-portal.org/smash/get/diva2:1560399/FULLTEXT01.pdf szinte a teljes NavMesh (nem gondol a járókelõk szélességére)



*/

void simulation(SDL_Window &window, SDL_Renderer &renderer);

#endif // COMMON_H_INCLUDED
