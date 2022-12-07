#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "vec3.h"
#include "common.h"
//#include "geo.h"

struct Kamera{
    vec2 pos;
    float x = SZELES, y = MAGAS;
    float zoom;
    bool W=false, A=false, S=false, D=false;

    Kamera(){pos=vec2(-40,-40); zoom=SZELES;}

    vec2 valosLekepezese(vec2 valos){
        vec2 k = valos - pos;
        k/=zoom; k.x*=SZELES; k.y*=MAGAS;
        return k;
    }

    vec2 kepiLekepzese(vec2 kepi){
        kepi.y/=MAGAS; kepi.x/=SZELES; kepi*=zoom;
        kepi += pos;
        return kepi;
    }

    void handleEvent(SDL_Event ev);

    /// kamerát mozgatja
    void moveCamera(clock_t dt){
        float globalSpeedModyf = 0.6f;
        float speedModyf = zoom/SZELES;
        if (W){
            pos.y+=dt*globalSpeedModyf*speedModyf;
        }
        if (A){
            pos.x+=dt*globalSpeedModyf*speedModyf;
        }
        if (S){
            pos.y-=dt*globalSpeedModyf*speedModyf;
        }
        if (D){
            pos.x-=dt*globalSpeedModyf*speedModyf;
        }
    }
};





#endif // CAMERA_H_INCLUDED
