#include "common.h"
#include "vec3.h"
//#include "geo.h"

using namespace std;



int main( int argc, char * argv[] )
{
    clock_t t = clock();
    srand(time(NULL));
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer( SZELES, MAGAS, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE, &window, &renderer );
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderClear( renderer );
    SDL_RenderPresent( renderer );



    /*
    cout<<cos(0)<<" "<<cos(90)<<" "<<cos(M_PI)<<endl;

    for (float i=0; i<360; i++){
        float szog = i*M_PI*2.f/360.f;
        vec2 temp(cos(szog),sin(szog));
        cout<<"( "<<cos(szog)<<", "<<sin(szog)<<" ): ";
        cout<<vec2ToDeg(temp)<<endl;
    }
    */

    /*
    vec3 P0(0,0,0);
    vec3 P1(1,0,0);
    vec3 Q0(3,1,0);
    vec3 Q1(3,2,0);
    double res = 0, s = 0, t = 0;
    vec3 closest[2];
    DistanceSegments3Console(P0,P1,Q0,Q1,res,s,t,closest);
    cout<<res<<", "<<s<<", "<<t<<", "<<closest[0]<<", "<<closest[1]<<endl;

    vec2 p0(0.0f, 1.0f), p1(2.0f, 1.0f), q0(0.0f, 1.0f), q1(2.0f, 1.0f);
    vec2 closest2[2];
    bool Dret = false;
    DistanceSegments2(p0,p1,q0,q1,res,s,t,closest2,Dret);
    cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
    */
    cout<<"mainTime: "<<clock()-t<<endl;
    simulation( *window, *renderer);
    return 0;
}
