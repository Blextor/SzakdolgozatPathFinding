#include "camera.h"

using namespace std;

void Kamera::handleEvent(SDL_Event ev){
    if (ev.type==SDL_KEYUP){
        if (ev.key.keysym.sym == SDLK_w){
            W=false;
        }
        if (ev.key.keysym.sym == SDLK_a){
            A=false;
        }
        if (ev.key.keysym.sym == SDLK_s){
            S=false;
        }
        if (ev.key.keysym.sym == SDLK_d){
            D=false;
        }
    }
    if (ev.type==SDL_KEYDOWN){
        if (ev.key.keysym.sym == SDLK_w){
            W=true;
        }
        if (ev.key.keysym.sym == SDLK_a){
            A=true;
        }
        if (ev.key.keysym.sym == SDLK_s){
            S=true;
        }
        if (ev.key.keysym.sym == SDLK_d){
            D=true;
        }
        if (ev.key.keysym.sym == SDLK_r){
            zoom=max(SZELES,MAGAS);
            pos=vec2(-40,-40);
        }
    }

    if (ev.type==SDL_MOUSEWHEEL){
        cout<<ev.wheel.y<<endl;
        zoom*=pow(1.1f,ev.wheel.y);
    }
}








