#include "common.h"
#include "vec3.h"
#include "geo.h"
#include "camera.h"

using namespace std;

struct Vilag{
    float agentRadius = 5.0f;
    vector<Sikidom> alaprajz;
    vector<Sikidom> alaprajzhozTartozo;
    vector<Sikidom> bejarhatoTerulet;

    Sikidom ujsikidom;
    vec2 ujSikidomTempPont;
    bool elsoPont = true;

    Vilag(){}

    void ujSikidomReset(){
        ujsikidom=Sikidom();
        ujSikidomTempPont=vec2();
        elsoPont=true;
    }

    void utolsoAlaprajzTorlese(){
        alaprajz.pop_back();
    }

    Sikidom alaprajzToTartozo(int idx){
        Sikidom jelenAlap = alaprajz[idx];
        vector<vec2> tartozoCsucsok;
        cout<<"KEZD"<<endl;
        for (int i=0; i<jelenAlap.szakaszok.size(); i++){
            vec2 AB = jelenAlap.szakaszok[i].p2 - jelenAlap.szakaszok[i].p1;
            int j = (i+1)%jelenAlap.szakaszok.size();
            vec2 BC = jelenAlap.szakaszok[j].p2 - jelenAlap.szakaszok[j].p1;
            cout<<"A"<<endl;
            AB = AB.normalize();
            BC = BC.normalize();
            vec2 AC = AB + BC;
            vec2 ABT = AB; ABT.rotate(90); ABT = ABT.normalize();
            vec2 ACT = AC;
            ACT = ACT.normalize();
            ACT.rotate(90);
            Szakasz sz1(jelenAlap.szakaszok[i].p1+ABT*agentRadius,jelenAlap.szakaszok[i].p1+ABT*agentRadius + AB * 2000.f);
            Szakasz sz2(jelenAlap.szakaszok[i].p2,jelenAlap.szakaszok[i].p2+ ACT * 2000.f);
            cout<<"B"<<endl;
            //cout<<sz1.p1.x<<" "<<sz1.p1.y<<", "<<sz1.p2.x<<" "<<sz1.p2.y<<endl;
            //cout<<sz2.p1.x<<" "<<sz2.p1.y<<", "<<sz2.p2.x<<" "<<sz2.p2.y<<endl;

            if (tartozoCsucsok.size()!=0){
                //tartozoCsucsok[tartozoCsucsok.size()-1]=jelenAlap.szakaszok[j].legyenLegalabbXTavra(tartozoCsucsok[tartozoCsucsok.size()-1],agentRadius);
            }
            cout<<"C"<<endl;
            double res = 0, s = 0, t = 0;
            vec2 closest2[2];
            bool Dret = false;
            DistanceSegments2(sz1.p1,sz1.p2,sz2.p1,sz2.p2,res,s,t,closest2,Dret);
            cout<<"C2"<<endl;
            if ((closest2[0]-sz1.p1).length()>(jelenAlap.szakaszok[i].p2+ABT*agentRadius-sz1.p1).length()){
                vec2 temp = jelenAlap.szakaszok[i].p2+ABT*agentRadius;
                vec2 b = jelenAlap.szakaszok[i].p2+ACT*agentRadius;
                if (tartozoCsucsok.size()==0 || b.dist(temp)<b.dist(tartozoCsucsok.back()))
                    tartozoCsucsok.push_back(jelenAlap.szakaszok[i].p2+ABT*agentRadius);
                tartozoCsucsok.push_back(b);
                vec2 BCT = BC;
                BCT.rotate(90); BCT = BCT.normalize();
                tartozoCsucsok.push_back(jelenAlap.szakaszok[i].p2+BCT*agentRadius);
            } else {
                tartozoCsucsok.push_back(closest2[1]); /// szükséges az 1 a vágáshoz
            }
            cout<<"D"<<endl;
            if (!Dret && res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) )){
                //cout<<"OK"<<endl;
            } else {
                //cout<<"MI A BAJ?"<<endl;
            }

            //metszikEgymast()
        }
        jelenAlap.szakaszok.clear();
        cout<<"ALMA"<<endl;
        for (int i=0; i<tartozoCsucsok.size(); i++){
            if (tartozoCsucsok[i]==tartozoCsucsok[(i+1)%tartozoCsucsok.size()])
                tartozoCsucsok.erase(tartozoCsucsok.begin()+i--);
        }
        cout<<"BALMA"<<endl;
        for (int i=0; i<tartozoCsucsok.size(); i++){
            int j = (i+1)%tartozoCsucsok.size();
            int k = (i+2)%tartozoCsucsok.size();
            int l = (i+3)%tartozoCsucsok.size();
            Szakasz sz1(tartozoCsucsok[i],tartozoCsucsok[j]);
            Szakasz sz2(tartozoCsucsok[k],tartozoCsucsok[l]);
            if (metszikEgymast(sz1,sz2)){

                cout<<"CALMA"<<endl;
                vec2 hol = metszikEgymastHol(sz1,sz2);
                if (j==tartozoCsucsok.size()-1){
                    tartozoCsucsok.erase(tartozoCsucsok.begin()+j);
                    tartozoCsucsok[0]=hol;
                } else {
                    tartozoCsucsok.erase(tartozoCsucsok.begin()+j);
                    tartozoCsucsok[j]=hol;
                }
                sz1.p2=hol;

                cout<<"DALMA"<<endl;
            }
            jelenAlap.szakaszok.push_back(sz1);
        }

        cout<<"EALMA"<<endl;
        jelenAlap.belso=(idx==0);
        return jelenAlap;
    }

    void alaprajzhozTartozoLetrehozasa(){
        alaprajzhozTartozo.clear();
        alaprajzhozTartozo.resize(alaprajz.size());
        for (int i=0; i<alaprajz.size(); i++)
            alaprajzhozTartozo[i]=alaprajzToTartozo(i);

        ///alaprajzhozTartozo=alaprajz;
    }

    void draw(SDL_Renderer &renderer, Kamera kamera){

        for (int i=0; i<alaprajz.size(); i++){
            for (int j=0; j<alaprajz[i].szakaszok.size(); j++){
                lineRGBA(&renderer,
                         kamera.valosLekepezese(alaprajz[i].szakaszok[j].p1).x,kamera.valosLekepezese(alaprajz[i].szakaszok[j].p1).y,
                         kamera.valosLekepezese(alaprajz[i].szakaszok[j].p2).x,kamera.valosLekepezese(alaprajz[i].szakaszok[j].p2).y,
                         52,240,120,255);
                filledCircleRGBA(&renderer,
                         kamera.valosLekepezese(alaprajz[i].szakaszok[j].p1).x,kamera.valosLekepezese(alaprajz[i].szakaszok[j].p1).y, 5,
                         60,25,40,255);
            }
        }


        for (int i=0; i<ujsikidom.szakaszok.size(); i++){
            lineRGBA(&renderer,
                     kamera.valosLekepezese(ujsikidom.szakaszok[i].p1).x,kamera.valosLekepezese(ujsikidom.szakaszok[i].p1).y,
                     kamera.valosLekepezese(ujsikidom.szakaszok[i].p2).x,kamera.valosLekepezese(ujsikidom.szakaszok[i].p2).y,
                     52,240,120,255);
            filledCircleRGBA(&renderer,
                     kamera.valosLekepezese(ujsikidom.szakaszok[i].p1).x,kamera.valosLekepezese(ujsikidom.szakaszok[i].p1).y, 5,
                     60,25,40,255);
        }

        for (int i=0; i<alaprajzhozTartozo.size(); i++){
            for (int j=0; j<alaprajzhozTartozo[i].szakaszok.size(); j++){
                lineRGBA(&renderer,
                         kamera.valosLekepezese(alaprajzhozTartozo[i].szakaszok[j].p1).x,kamera.valosLekepezese(alaprajzhozTartozo[i].szakaszok[j].p1).y,
                         kamera.valosLekepezese(alaprajzhozTartozo[i].szakaszok[j].p2).x,kamera.valosLekepezese(alaprajzhozTartozo[i].szakaszok[j].p2).y,
                         150,215,100,255);
                filledCircleRGBA(&renderer,
                         kamera.valosLekepezese(alaprajzhozTartozo[i].szakaszok[j].p1).x,kamera.valosLekepezese(alaprajzhozTartozo[i].szakaszok[j].p1).y, 5,
                         60,160,128,200);
            }
        }
    }
};


/// megjelenítéshez szükséges globális változók
//vec2 camera(-40,-40);
Kamera kamera;
/// és mindenek atyja
//Palya palya;
Vilag vilag;
Palya palya(true);

bool folyamatban = false;
float f1 = 1.f;

/// eseméyneket, bemeneteket itt kezelem le
void EventHandle(SDL_Event ev){
    kamera.handleEvent(ev);
    if (ev.type==SDL_KEYUP){

        if (ev.key.keysym.sym == SDLK_y){
            folyamatban=false;
            f1=1;
        }
        if (ev.key.keysym.sym == SDLK_x){
            folyamatban=false;
            f1=-1;
        }
    }
    if (ev.type==SDL_KEYDOWN){

        if (ev.key.keysym.sym == SDLK_c){ /// elkészíti a NavMesh oldalait
            clock_t t = clock();
            vilag.alaprajzhozTartozoLetrehozasa();
            cout<<"NavMesh oldalai: "<<clock()-t<<endl;
        }
        if (ev.key.keysym.sym == SDLK_z){ /// utolsó síkidom törlése
            clock_t t = clock();
            vilag.utolsoAlaprajzTorlese();
            cout<<"utolso sikidom torlese: "<<clock()-t<<endl;
        }
        if (ev.key.keysym.sym == SDLK_u){ /// NavMesh oldlait átmásolja
            clock_t t = clock();
            palya.sikidomok=vilag.alaprajzhozTartozo;
            cout<<"NavMesh oldalait atmasolja: "<<clock()-t<<endl;
        }
        if (ev.key.keysym.sym == SDLK_i){ /// Elkészíti a NAvMesh-t
            clock_t t = clock();
            palya.bakeNavMesh();
            cout<<"NavMesh bake: "<<clock()-t<<endl;
        }
        if (ev.key.keysym.sym == SDLK_g){ /// Kirajzolja a nem használható átlóit a síkidomoknak
            clock_t t = clock();
            palya.bakeAtloNavMesh(true);
            cout<<"NavMesh atloi: "<<clock()-t<<endl;
        }

        if (ev.key.keysym.sym == SDLK_j){
            vilag.agentRadius++;
            cout<<"agentRadius: "<<vilag.agentRadius<<endl;
        }
        if (ev.key.keysym.sym == SDLK_k){
            vilag.agentRadius--;
            cout<<"agentRadius: "<<vilag.agentRadius<<endl;
        }
        if (ev.key.keysym.sym == SDLK_k){
            vilag.agentRadius--;
            cout<<"agentRadius: "<<vilag.agentRadius<<endl;
        }
        if (ev.key.keysym.sym == SDLK_y){
            folyamatban=true;
            f1=1;
        }
        if (ev.key.keysym.sym == SDLK_x){
            folyamatban=true;
            f1=-1;
        }

        if (ev.key.keysym.sym == SDLK_r){ /// pálya reset
            clock_t t = clock();
            palya.navMesh.clear();
            palya.sikidomok.clear();
            palya.belsoAtlok.clear();
            cout<<"NavMesh reset: "<<clock()-t<<endl;
        }
        if (ev.key.keysym.sym == SDLK_t){ /// világ reset
            clock_t t = clock();
            vilag.alaprajz.clear();
            vilag.alaprajzhozTartozo.clear();
            cout<<"alaprajz reset: "<<clock()-t<<endl;
        }
        /*
        if (ev.key.keysym.sym == SDLK_e){
            clock_t ti = clock();
            palya.bakeNavMesh();
            cout<<"bakeNavMesh Time: "<<clock()-ti<<endl;
        }
        if (ev.key.keysym.sym == SDLK_g){
            palya.bakeAtloNavMesh();
        }
        if (ev.key.keysym.sym == SDLK_f){
            palya.sok++;
            cout<<"Palya.sok "<<palya.sok%(palya.navMesh.size()+1)<<endl;
        }
        if (ev.key.keysym.sym == SDLK_v){
            palya.sok--;
            cout<<"Palya.sok "<<palya.sok%(palya.navMesh.size()+1)<<endl;
        }
        */
    }

    if (ev.type==SDL_MOUSEWHEEL){
        //cout<<ev.wheel.y<<endl;
    }

    if (ev.type==SDL_MOUSEBUTTONDOWN){
        if (ev.button.button == 1){
            if (vilag.elsoPont){
                vilag.elsoPont=false;
                vilag.ujSikidomTempPont=kamera.kepiLekepzese(vec2(ev.button.x,ev.button.y));
            } else {
                if (vilag.ujsikidom.szakaszok.size()==0){
                    Szakasz uj(vilag.ujSikidomTempPont,kamera.kepiLekepzese(vec2(ev.button.x,ev.button.y)));
                    vilag.ujsikidom.szakaszok.push_back(uj);
                }
                else{
                    Szakasz uj(vilag.ujsikidom.szakaszok[vilag.ujsikidom.szakaszok.size()-1].p2,kamera.kepiLekepzese(vec2(ev.button.x,ev.button.y)));
                    vilag.ujsikidom.szakaszok.push_back(uj);
                }
            }
        }
        if (ev.button.button == 2){
            vilag.ujSikidomReset();
        }
        if (ev.button.button == 3){
            vilag.ujsikidom.szakaszok.push_back(
                Szakasz(vilag.ujsikidom.szakaszok[vilag.ujsikidom.szakaszok.size()-1].p2,vilag.ujsikidom.szakaszok[0].p1));
            vilag.alaprajz.push_back(vilag.ujsikidom);
            vilag.ujSikidomReset();
        }
        cout<<ev.button.type<<endl;
        cout<<(int)ev.button.button<<endl;
        cout<<(int)ev.button.clicks<<endl;
    }


    if (ev.type == SDL_QUIT)
        exit(3);
}



/// MAIN
void simulation(SDL_Window &window, SDL_Renderer &renderer){
    clock_t t1 = clock();
    int targetFPS = 60;
    bool stop = false;
    srand(100);
    SDL_Event ev;
    cout<<"tick "<<clock()-t1<<endl;
    int framesInLastSec = 0;
    clock_t last_sec=clock();
    bool frame=true;
    clock_t dt = 0;


    /// megjelenítési és eseménykezelő ciklus
    while(!stop){

        /// beállított képkockafrissítési gyakoriságot próbálja tartani
        if (clock()>=t1+CLOCKS_PER_SEC/targetFPS){
            dt = t1 - clock();
            t1=clock();
            frame = true;
        } else {
            frame = false;
            Sleep(1);
        }

        // DEBUG
        if (clock()>=last_sec+1000){
            last_sec=clock();
            //cout<<framesInLastSec<<endl;
            framesInLastSec = 0;
        }

        /// események lekérdezése, és feldolgozása
        if (SDL_PollEvent(&ev)){
            EventHandle(ev);
        }

        /// egy képkocka
        if (frame){
            framesInLastSec++;
            SDL_SetRenderDrawColor( &renderer, 0, 0, 0, 255 ); /// tisztító szín
            SDL_RenderClear(&renderer); /// tiszta
            kamera.moveCamera(dt); /// mozgatja a kamerát
            if (folyamatban){
                vilag.agentRadius+=dt/1000.f*f1;
                if (vilag.agentRadius<0)
                    vilag.agentRadius=0;
                vilag.alaprajzhozTartozoLetrehozasa();
            }
            palya.draw(renderer,kamera); /// kirajzolja a pályát
            vilag.draw(renderer,kamera);
            SDL_RenderPresent(&renderer); /// meg is jeleníti
            //megjelenites(renderer,window,palya,step_cnt);
        }
    }
}
