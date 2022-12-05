#include "common.h"
#include "vec3.h"
#include "geo.h"
#include "camera.h"

using namespace std;

bool drawFalak = true;
bool drawAjtok = false;
bool drawNavMesh = false;
bool drawPoziciok = false;




struct Hely{
    vec2 szobaPoz;
    vec2 valosPoz;

    bool foglalt = false;

    float ertek=0;
};

struct Vilag{
    float agentRadius = 10.0f;
    vector<Sikidom> alaprajz;
    vector<Sikidom> alaprajzhozTartozo;
    vector<Sikidom> bejarhatoTerulet;

    Sikidom ujsikidom;
    vec2 ujSikidomTempPont;
    bool elsoPont = true;

    vector<int> ajtok;

    vector<Hely> diakok;
    Hely oktato;
    bool atjarhato = false;

    Vilag(){oktato.szobaPoz= vec2(-2000.f,-2000.f);}

    void ujSikidomReset(){
        ujsikidom=Sikidom();
        ujSikidomTempPont=vec2();
        elsoPont=true;
    }

    void utolsoAlaprajzTorlese(){
        alaprajz.pop_back();
    }

    void utolsoDiakTorlese(){
        diakok.pop_back();
    }
    void OktatoTorlese(){
        oktato.szobaPoz=vec2(-2000.f,-2000.f);
    }

    void SetAjto(vec2 pos){
        float minimalDis = INT_MAX;
        int oldalIdx = -1;
        for (int i=0; i<alaprajz[0].szakaszok.size();i++){
            if (alaprajz[0].szakaszok[i].vec2TavR(pos)<minimalDis){
                minimalDis=alaprajz[0].szakaszok[i].vec2TavR(pos);
                oldalIdx=i;
            }
        }
        if (minimalDis<10){
            ajtok.push_back(oldalIdx);
        }
    }

    void save(){
        string name;
        cout<<"Elmentendo szoba neve: ";
        cin>>name;
        ofstream file("szobak/"+name+".txt");
        file<<name<<endl;
        file<<alaprajz.size()<<endl;
        for (int j=0; j<alaprajz.size(); j++){
            file<<alaprajz[j].szakaszok.size()<<endl;
            for (int i=0; i<alaprajz[j].szakaszok.size(); i++){
                file<<alaprajz[j].szakaszok[i].p1.x<<" "<<alaprajz[j].szakaszok[i].p1.y<<endl;
            }
        }
        file<<ajtok.size()<<endl;
        for (int i=0; i<ajtok.size(); i++)
            file<<ajtok[i]<<endl;
        file<<diakok.size()<<endl;
        for (int i=0; i<diakok.size(); i++)
            file<<diakok[i].szobaPoz.x<<" "<<diakok[i].szobaPoz.y<<" "<<diakok[i].ertek<<endl;
        if (oktato.szobaPoz.x!=-2000 && oktato.szobaPoz.y!=-2000){
            file<<1<<endl;
            file<<oktato.szobaPoz.x<<" "<<oktato.szobaPoz.y<<endl;
        }
        else
            file<<0<<endl;
        file<<atjarhato<<endl;
        file.close();
    }

    Sikidom alaprajzToTartozo(int idx){
        Sikidom jelenAlap = alaprajz[idx];
        vector<vec2> tartozoCsucsok;
        //cout<<"KEZD"<<endl;
        for (int i=0; i<jelenAlap.szakaszok.size(); i++){
            vec2 AB = jelenAlap.szakaszok[i].p2 - jelenAlap.szakaszok[i].p1;
            int j = (i+1)%jelenAlap.szakaszok.size();
            vec2 BC = jelenAlap.szakaszok[j].p2 - jelenAlap.szakaszok[j].p1;
            //cout<<"A"<<endl;
            AB = AB.normalize();
            BC = BC.normalize();
            vec2 AC = AB + BC;
            vec2 ABT = AB; ABT.rotate(90); ABT = ABT.normalize();
            vec2 ACT = AC;
            ACT = ACT.normalize();
            ACT.rotate(90);
            Szakasz sz1(jelenAlap.szakaszok[i].p1+ABT*agentRadius,jelenAlap.szakaszok[i].p1+ABT*agentRadius + AB * 2000.f);
            Szakasz sz2(jelenAlap.szakaszok[i].p2,jelenAlap.szakaszok[i].p2+ ACT * 2000.f);
            //cout<<"B"<<endl;
            //cout<<sz1.p1.x<<" "<<sz1.p1.y<<", "<<sz1.p2.x<<" "<<sz1.p2.y<<endl;
            //cout<<sz2.p1.x<<" "<<sz2.p1.y<<", "<<sz2.p2.x<<" "<<sz2.p2.y<<endl;

            if (tartozoCsucsok.size()!=0){
                //tartozoCsucsok[tartozoCsucsok.size()-1]=jelenAlap.szakaszok[j].legyenLegalabbXTavra(tartozoCsucsok[tartozoCsucsok.size()-1],agentRadius);
            }
            //cout<<"C"<<endl;
            double res = 0, s = 0, t = 0;
            vec2 closest2[2];
            bool Dret = false;
            DistanceSegments2(sz1.p1,sz1.p2,sz2.p1,sz2.p2,res,s,t,closest2,Dret);
            //cout<<"C2"<<endl;
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
            //cout<<"D"<<endl;
            if (!Dret && res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) )){
                //cout<<"OK"<<endl;
            } else {
                //cout<<"MI A BAJ?"<<endl;
            }

            //metszikEgymast()
        }
        jelenAlap.szakaszok.clear();
        //cout<<"ALMA"<<endl;
        for (int i=0; i<tartozoCsucsok.size(); i++){
            if (tartozoCsucsok[i]==tartozoCsucsok[(i+1)%tartozoCsucsok.size()])
                tartozoCsucsok.erase(tartozoCsucsok.begin()+i--);
        }
        //cout<<"BALMA"<<endl;
        for (int i=0; i<tartozoCsucsok.size(); i++){
            int j = (i+1)%tartozoCsucsok.size();
            int k = (i+2)%tartozoCsucsok.size();
            int l = (i+3)%tartozoCsucsok.size();
            Szakasz sz1(tartozoCsucsok[i],tartozoCsucsok[j]);
            Szakasz sz2(tartozoCsucsok[k],tartozoCsucsok[l]);
            if (metszikEgymast(sz1,sz2)){

                vec2 hol = metszikEgymastHol(sz1,sz2);
                if (j==tartozoCsucsok.size()-1){
                    tartozoCsucsok.erase(tartozoCsucsok.begin()+j);
                    tartozoCsucsok[0]=hol;
                } else {
                    tartozoCsucsok.erase(tartozoCsucsok.begin()+j);
                    tartozoCsucsok[j]=hol;
                }
                sz1.p2=hol;

            }
            jelenAlap.szakaszok.push_back(sz1);
        }

        //cout<<"EALMA"<<endl;
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

    void draw(SDL_Renderer &renderer, Kamera kamera,bool alaprajzDraw=true){

        int r = 52;
        if (atjarhato)
            r=152;

        for (int i=0; i<alaprajz.size() && alaprajzDraw; i++){
            for (int j=0; j<alaprajz[i].szakaszok.size(); j++){
                lineRGBA(&renderer,
                         kamera.valosLekepezese(alaprajz[i].szakaszok[j].p1).x,kamera.valosLekepezese(alaprajz[i].szakaszok[j].p1).y,
                         kamera.valosLekepezese(alaprajz[i].szakaszok[j].p2).x,kamera.valosLekepezese(alaprajz[i].szakaszok[j].p2).y,
                         r,240,100+r,255);
                /*
                filledCircleRGBA(&renderer,
                         kamera.valosLekepezese(alaprajz[i].szakaszok[j].p1).x,kamera.valosLekepezese(alaprajz[i].szakaszok[j].p1).y, 5,
                         60,25,40,255);
                */
            }
        }

        /*
        for (int i=0; i<ujsikidom.szakaszok.size(); i++){
            lineRGBA(&renderer,
                     kamera.valosLekepezese(ujsikidom.szakaszok[i].p1).x,kamera.valosLekepezese(ujsikidom.szakaszok[i].p1).y,
                     kamera.valosLekepezese(ujsikidom.szakaszok[i].p2).x,kamera.valosLekepezese(ujsikidom.szakaszok[i].p2).y,
                     52,240,120,255);
            filledCircleRGBA(&renderer,
                     kamera.valosLekepezese(ujsikidom.szakaszok[i].p1).x,kamera.valosLekepezese(ujsikidom.szakaszok[i].p1).y, 5,
                     60,25,40,255);
        }
        */

        /*
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
        */
        if (drawPoziciok){
            for (int i=0; i<diakok.size(); i++){
                filledCircleRGBA(&renderer,
                         kamera.valosLekepezese(diakok[i].szobaPoz).x,kamera.valosLekepezese(diakok[i].szobaPoz).y, agentRadius*SZELES/kamera.zoom,
                         150,150,255,200);
            }
            if (!atjarhato)
                filledCircleRGBA(&renderer,
                         kamera.valosLekepezese(oktato.szobaPoz).x,kamera.valosLekepezese(oktato.szobaPoz).y, agentRadius*SZELES/kamera.zoom,
                         255,150,100,200);
        }
        /*
        for (int i=0; i<ajtok.size(); i++){
            lineRGBA(&renderer,
                     kamera.valosLekepezese(alaprajz[0].szakaszok[ajtok[i]].p1).x,kamera.valosLekepezese(alaprajz[0].szakaszok[ajtok[i]].p1).y,
                     kamera.valosLekepezese(alaprajz[0].szakaszok[ajtok[i]].p2).x,kamera.valosLekepezese(alaprajz[0].szakaszok[ajtok[i]].p2).y,
                     255,255,0,255);
        }
        */
    }
};




struct Kapacitas{
    vector<Hely> oktatoknak;
    vector<Hely> diakoknak;

    int maxDiak, maxOktato;
};

struct Szoba{
    int id;
    string nev;
    bool atjarhato = false;

    vector<Sikidom> alaprajz; /// alaprajz szerinti
    vector<float> agentSizes;
    vector<Vilag> navigaciosTerSzeleAS;
    vector<Palya> navigaciosTerAS;
    //vector<Sikidom> belsoTer; /// termek berendezései

    vector<Hely> diakok;
    Hely oktato;

    vector<int> kijarat; /// ???
    vector<int> szomszedok;

    void loadSzobaFromFile(string file){
        ifstream inpF("szobak/"+file+".txt");
        inpF>>nev;
        int sikidomCnt = -1;
        inpF>>sikidomCnt;
        for (int i=0; i<sikidomCnt; i++){
            int szakaszCnt = -1;
            inpF>>szakaszCnt;
            vector<vec2> csucsok;
            for (int j=0; j<szakaszCnt; j++){
                vec2 temp;
                inpF>>temp.x>>temp.y;
                csucsok.push_back(temp);
            }
            Sikidom oldalak;
            for (int j=0; j<csucsok.size(); j++){
                Szakasz temp(csucsok[j],csucsok[(j+1)%csucsok.size()]);
                oldalak.szakaszok.push_back(temp);
            }
            alaprajz.push_back(oldalak);
        }
        int ajtoCnt = -1;
        inpF>>ajtoCnt;
        for (int j=0; j<ajtoCnt; j++){
            int kijaratT = -1;
            inpF>>kijaratT;
            kijarat.push_back(kijaratT);
        }
        int diakCnt = -1;
        inpF>>diakCnt;
        for (int j=0; j<diakCnt; j++){
            Hely diakH;
            inpF>>diakH.szobaPoz.x>>diakH.szobaPoz.y>>diakH.ertek;
            diakok.push_back(diakH);
        }
        int oktatoCnt = -1;
        inpF>>oktatoCnt;
        for (int j=0; j<oktatoCnt; j++){
            inpF>>oktato.szobaPoz.x>>oktato.szobaPoz.y>>oktato.ertek;
        }
        inpF>>atjarhato;
        inpF.close();
    }

    void rotateSzoba(float deg, vec2 around){
        for (int i=0; i<alaprajz.size();i++){
            for (int j=0; j<alaprajz[i].szakaszok.size(); j++){
                vec2 P1 = alaprajz[i].szakaszok[j].p1;
                vec2 P2 = alaprajz[i].szakaszok[j].p2;
                P1-=around; P2-=around;
                P1.rotate(deg); P2.rotate(deg);
                P1+=around; P2+=around;
                alaprajz[i].szakaszok[j].p1=P1;
                alaprajz[i].szakaszok[j].p2=P2;
            }
        }
    }

    void moveSzoba(vec2 way){
        for (int i=0; i<alaprajz.size();i++){
            for (int j=0; j<alaprajz[i].szakaszok.size(); j++){
                alaprajz[i].szakaszok[j].p1+=way;
                alaprajz[i].szakaszok[j].p2+=way;
            }
        }
        for (int i=0; i<diakok.size(); i++)
            diakok[i].szobaPoz+=way;
        oktato.szobaPoz+=way;
    }

    void getIntrest(int ajto1Idx, Szoba szulo, int ajto2Idx){
        Szakasz ajto1 = alaprajz[0].szakaszok[kijarat[ajto1Idx]];
        Szakasz ajto2 = szulo.alaprajz[0].szakaszok[szulo.kijarat[ajto2Idx]];
        vec2 a = ajto2.p2 + ajto2.p1; a /= 2;
        vec2 b = ajto1.p2 + ajto1.p1; b /= 2;
        moveSzoba(a-b);
    }

    void createVilag(vector<float> agentSizesT){
        agentSizes=agentSizesT;
        navigaciosTerSzeleAS.resize(agentSizesT.size());
        navigaciosTerAS.resize(agentSizesT.size());
        for (int i=0; i<agentSizesT.size();i++){
            navigaciosTerSzeleAS[i] = Vilag();
            navigaciosTerSzeleAS[i].alaprajz=alaprajz;
            navigaciosTerSzeleAS[i].ajtok=kijarat;
            navigaciosTerSzeleAS[i].diakok=diakok;
            navigaciosTerSzeleAS[i].oktato=oktato;
            navigaciosTerSzeleAS[i].agentRadius=agentSizes[i];
            navigaciosTerSzeleAS[i].alaprajzhozTartozoLetrehozasa();
            navigaciosTerSzeleAS[i].atjarhato=atjarhato;
            navigaciosTerAS[i].sikidomok=navigaciosTerSzeleAS[i].alaprajzhozTartozo;
            navigaciosTerAS[i].bakeNavMesh2();
            //navigaciosTerAS[i].bakeAtloNavMesh();
            //navigaciosTerAS[i].bakeKulsoNavMesh2(false);
        }
    }

    void draw(SDL_Renderer &renderer, Kamera kamera, int agentSP = 0){

        /**
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
        */

        /*
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
        */
        navigaciosTerSzeleAS[agentSP].draw(renderer,kamera,false);
        if (drawNavMesh)
            navigaciosTerAS[agentSP].draw(renderer,kamera);
    }
};

struct Ajto{
    vec2 a,b,c,d;
};

struct Emelet{
    vector<Szoba> szobak;
    //vector<vector<int>> szomszedosSzobak;
    vector<vector<int>> szobakSzomszedjai; /// másik szobaIndexek
    vector<vector<int>> szobakSzomszedjainakAjtoi; /// másik szobába vezető ajtók indexe

    int agentSP = 0;

    vector<float> agentSizes;
    vector<Vilag> egeszVaS;
    vector<vector<Ajto>> ajtokAS;
    vector<Palya> pegeszAS;

    void getAjtok(){
        for (int ZZZ=0; ZZZ<agentSizes.size(); ZZZ++){
            vector<Ajto> ajtok;
            for (int i=0; i<szobakSzomszedjai.size(); i++){
                for (int j=0; j<szobakSzomszedjai[i].size(); j++){
                    if (szobakSzomszedjai[i][j]<i)
                        continue;
                    Ajto ajto;

                    int masodikSzobaIdx = szobakSzomszedjai[i][j];
                    int elsoSzobaAjtoSzakaszIdx = -1, masodikSzobaAjtoSzakaszIdx = -1;
                    int elsoSzobaAjtoIdx = -1, masodikSzobaAjtoIdx = -1;
                    elsoSzobaAjtoIdx=szobakSzomszedjainakAjtoi[i][j];
                    elsoSzobaAjtoSzakaszIdx=szobak[i].kijarat[elsoSzobaAjtoIdx];
                    Szakasz ajto1 = szobak[i].alaprajz[0].szakaszok[elsoSzobaAjtoSzakaszIdx];

                    for (int k=0; k<szobakSzomszedjai[masodikSzobaIdx].size(); k++){
                        if (szobakSzomszedjai[masodikSzobaIdx][k]==i){
                            masodikSzobaAjtoIdx=szobakSzomszedjainakAjtoi[masodikSzobaIdx][k];
                        }
                    }
                    masodikSzobaAjtoSzakaszIdx=szobak[masodikSzobaIdx].kijarat[masodikSzobaAjtoIdx];
                    Szakasz ajto2 = szobak[masodikSzobaIdx].alaprajz[0].szakaszok[masodikSzobaAjtoSzakaszIdx];

                    float aT = INT_MAX, bT=INT_MAX;
                    for (int k=0; k<szobak[i].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok.size(); k++){
                        if (szobak[i].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1.dist(ajto1.p1)<aT){
                            aT = szobak[i].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1.dist(ajto1.p1);
                            ajto.a=szobak[i].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1;
                        }
                        if (szobak[i].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1.dist(ajto1.p2)<bT){
                            bT = szobak[i].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1.dist(ajto1.p2);
                            ajto.b=szobak[i].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1;
                        }
                    }

                    float cT = INT_MAX, dT=INT_MAX;
                    for (int k=0; k<szobak[masodikSzobaIdx].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok.size(); k++){
                        if (szobak[masodikSzobaIdx].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1.dist(ajto2.p1)<cT){
                            cT = szobak[masodikSzobaIdx].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1.dist(ajto2.p1);
                            ajto.c=szobak[masodikSzobaIdx].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1;
                        }
                        if (szobak[masodikSzobaIdx].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1.dist(ajto2.p2)<dT){
                            dT = szobak[masodikSzobaIdx].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1.dist(ajto2.p2);
                            ajto.d=szobak[masodikSzobaIdx].navigaciosTerSzeleAS[ZZZ].alaprajzhozTartozo[0].szakaszok[k].p1;
                        }
                    }
                    ajtok.push_back(ajto);
                }
            }
            ajtokAS.push_back(ajtok);
        }
    }

    bool gSZDEBUG = true;
    vector<Szakasz> getSzobaHatar(int szobaIdx, int fromIdx=-1){ /// lehetőleg az ajtók ne legyenek a síkodomnak a szakaszlistájának elején és végén is
        vector<Szakasz> ret;
        vec2 startPont;
        int startIdx = 0;
        vec2 endPont;
        if (gSZDEBUG) cout<<"a"<<endl;
        if (fromIdx==-1){
            startPont=szobak[szobaIdx].alaprajz[0].szakaszok[0].p1;
            startIdx = 0;
            endPont = startPont;
            if (gSZDEBUG) cout<<"b"<<endl;
        } else {
            int idx = -1;
            for (int i=0; i<szobakSzomszedjai[szobaIdx].size(); i++){
                if (szobakSzomszedjai[szobaIdx][i]==fromIdx){
                    idx=i;
                    break;
                }
            }
            if (gSZDEBUG) cout<<"c"<<endl;
            int szIdx = (szobak[szobaIdx].kijarat[szobakSzomszedjainakAjtoi[szobaIdx][idx]]+2);
            int modulo = szobak[szobaIdx].alaprajz[0].szakaszok.size();
            startPont=szobak[szobaIdx].alaprajz[0].szakaszok[szIdx%modulo].p1;
            endPont=szobak[szobaIdx].alaprajz[0].szakaszok[(szIdx+modulo-4)%modulo].p1;
            startIdx = (szIdx)%modulo;
        }
        int modulo = szobak[szobaIdx].alaprajz[0].szakaszok.size();

        if (gSZDEBUG) cout<<"d"<<endl;
        for (int i=startIdx; i<startIdx+modulo; i++){

            if (gSZDEBUG) cout<<"e "<<i<<endl;
            bool ajto = false;
            int masikSzobaIdx = 1;
            Szakasz kovetkezoSzakasz;
            for (int j=0; j<szobakSzomszedjainakAjtoi[szobaIdx].size(); j++){
                if (gSZDEBUG) cout<<"e2"<<endl;
                if (gSZDEBUG) cout<<i<<" "<<szobak[szobaIdx].kijarat[szobakSzomszedjainakAjtoi[szobaIdx][j]]<<endl;
                if ((i%modulo)==szobak[szobaIdx].kijarat[szobakSzomszedjainakAjtoi[szobaIdx][j]]){
                    if (gSZDEBUG) cout<<"mé"<<endl;
                    ajto = true;
                    masikSzobaIdx = szobakSzomszedjai[szobaIdx][j];
                    kovetkezoSzakasz = szobak[szobaIdx].alaprajz[0].szakaszok[(i+2)%modulo];
                    i++;
                    break;
                }
            }
            if (gSZDEBUG) cout<<"f"<<endl;
            if (ajto){
                if (gSZDEBUG) cout<<"ret.size() "<<ret.size()<<endl;
                if (ret.size()!=0)
                    ret.pop_back();
                if (gSZDEBUG) cout<<"i "<<masikSzobaIdx<<" "<<szobaIdx<<endl;
                vector<Szakasz> temp = getSzobaHatar(masikSzobaIdx,szobaIdx);
                if (gSZDEBUG) cout<<"JJJJJ ret.size(): "<<ret.size()<<" "<<temp.size()<<endl;
                if (ret.size()!=0){

                    ret.push_back(Szakasz(ret.back().p2,temp[0].p1));
                }
                ret.insert(ret.end(),temp.begin(),temp.end());
                ret.push_back(Szakasz(temp[temp.size()-1].p2,kovetkezoSzakasz.p1));
                if (gSZDEBUG) cout<<"o"<<endl;
            } else {
                if (gSZDEBUG) cout<<"f2"<<endl;
                ret.push_back(szobak[szobaIdx].alaprajz[0].szakaszok[i%modulo]);
            }
            if (gSZDEBUG) cout<<"g"<<endl;
            if (szobak[szobaIdx].alaprajz[0].szakaszok[i%modulo].p1 == endPont && i!=startIdx){
                if (gSZDEBUG) cout<<"ZZZ"<<endl;
                break;
            }
        }
        if (gSZDEBUG) cout<<"h"<<endl;
        return ret;
    }

    Emelet(){
        clock_t t=clock();
        int szCnt = 11;
        szobak.resize(szCnt);
        szobakSzomszedjai.resize(szCnt);
        szobakSzomszedjainakAjtoi.resize(szCnt);
        szobak[0].loadSzobaFromFile("liftLepcso");
        szobak[1].loadSzobaFromFile("liftFolyoso");
        szobak[2].loadSzobaFromFile("B410");
        szobak[3].loadSzobaFromFile("Gfolyoso");
        szobak[4].loadSzobaFromFile("atriumFolyoso");
        szobak[5].loadSzobaFromFile("B413");
        szobak[6].loadSzobaFromFile("laborFolyoso");
        szobak[7].loadSzobaFromFile("L408");
        szobak[8].loadSzobaFromFile("L407");
        szobak[9].loadSzobaFromFile("L406");
        szobak[10].loadSzobaFromFile("L405_5");
        szobakSzomszedjai[0].push_back(1);
        szobakSzomszedjainakAjtoi[0].push_back(0);
        szobakSzomszedjai[1].push_back(0);
        szobakSzomszedjainakAjtoi[1].push_back(3);
        szobakSzomszedjai[1].push_back(3);
        szobakSzomszedjainakAjtoi[1].push_back(0);
        szobakSzomszedjai[1].push_back(2);
        szobakSzomszedjainakAjtoi[1].push_back(1);
        szobakSzomszedjai[1].push_back(4);
        szobakSzomszedjainakAjtoi[1].push_back(2);
        szobakSzomszedjai[2].push_back(1);
        szobakSzomszedjainakAjtoi[2].push_back(0);
        szobakSzomszedjai[3].push_back(1);
        szobakSzomszedjainakAjtoi[3].push_back(0);
        szobakSzomszedjai[4].push_back(1);
        szobakSzomszedjainakAjtoi[4].push_back(0);
        szobakSzomszedjai[4].push_back(5);
        szobakSzomszedjainakAjtoi[4].push_back(2);
        szobakSzomszedjai[5].push_back(4);
        szobakSzomszedjainakAjtoi[5].push_back(0);

        szobakSzomszedjai[3].push_back(6);
        szobakSzomszedjainakAjtoi[3].push_back(2);
        szobakSzomszedjai[6].push_back(3);
        szobakSzomszedjainakAjtoi[6].push_back(0);

        szobakSzomszedjai[6].push_back(7);
        szobakSzomszedjainakAjtoi[6].push_back(1);
        szobakSzomszedjai[7].push_back(6);
        szobakSzomszedjainakAjtoi[7].push_back(0);

        szobakSzomszedjai[6].push_back(8);
        szobakSzomszedjainakAjtoi[6].push_back(2);
        szobakSzomszedjai[8].push_back(6);
        szobakSzomszedjainakAjtoi[8].push_back(0);

        szobakSzomszedjai[6].push_back(9);
        szobakSzomszedjainakAjtoi[6].push_back(3);
        szobakSzomszedjai[9].push_back(6);
        szobakSzomszedjainakAjtoi[9].push_back(0);

        szobakSzomszedjai[6].push_back(10);
        szobakSzomszedjainakAjtoi[6].push_back(4);
        szobakSzomszedjai[10].push_back(6);
        szobakSzomszedjainakAjtoi[10].push_back(0);

        szobak[1].getIntrest(3,szobak[0],0);
        szobak[2].getIntrest(0,szobak[1],1);
        szobak[3].getIntrest(0,szobak[1],0);
        szobak[4].getIntrest(0,szobak[1],2);
        szobak[5].getIntrest(0,szobak[4],2);
        szobak[6].getIntrest(0,szobak[3],2);
        szobak[7].getIntrest(0,szobak[6],1);
        szobak[8].getIntrest(0,szobak[6],2);
        szobak[9].getIntrest(0,szobak[6],3);
        szobak[10].getIntrest(0,szobak[6],4);

        agentSizes.clear();
        for (float i=10.f; i<=15.f; i+=0.5f){
            agentSizes.push_back(i);
        }

        for (int i=0; i<szobak.size(); i++){
            cout<<"create: "<<i<<endl;
            szobak[i].createVilag(agentSizes);
        }


        vector<Szakasz> temp = getSzobaHatar(0);
        Vilag tempV;
        tempV.alaprajz.resize(1);
        tempV.alaprajz[0].szakaszok=temp;
        for (int i=0; i<szobak.size(); i++){
            for (int j=1; j<szobak[i].alaprajz.size(); j++){
                tempV.alaprajz.push_back(szobak[i].alaprajz[j]);
            }
        }

        egeszVaS.resize(agentSizes.size());
        pegeszAS.resize(agentSizes.size());
        for (int i=0; i<agentSizes.size(); i++){
            egeszVaS[i]=tempV;
            egeszVaS[i].agentRadius=agentSizes[i];
            egeszVaS[i].alaprajzhozTartozoLetrehozasa();
        }
        getAjtok();

        //pegesz.sikidomok=egeszV.alaprajzhozTartozo;
        //pegesz.bakeNavMesh();
        cout<<"EMELET: "<<clock()-t<<endl;

    }

    void draw(SDL_Renderer &renderer, Kamera kamera){
        if (drawFalak)
            egeszVaS[agentSP].draw(renderer,kamera);
        //pegesz.draw(renderer,kamera);
        for (int i=0; i<szobak.size(); i++)
            szobak[i].draw(renderer,kamera,agentSP);
        for (int i=0; i<ajtokAS[agentSP].size() && drawAjtok; i++){
            vector<Ajto> ajtok = ajtokAS[agentSP];
            filledTrigonRGBA(&renderer,
                            kamera.valosLekepezese(ajtok[i].a).x,kamera.valosLekepezese(ajtok[i].a).y,
                            kamera.valosLekepezese(ajtok[i].b).x,kamera.valosLekepezese(ajtok[i].b).y,
                            kamera.valosLekepezese(ajtok[i].c).x,kamera.valosLekepezese(ajtok[i].c).y,
                            255,0,0,255);
            trigonRGBA(&renderer,
                            kamera.valosLekepezese(ajtok[i].a).x,kamera.valosLekepezese(ajtok[i].a).y,
                            kamera.valosLekepezese(ajtok[i].b).x,kamera.valosLekepezese(ajtok[i].b).y,
                            kamera.valosLekepezese(ajtok[i].c).x,kamera.valosLekepezese(ajtok[i].c).y,
                            0,255,0,255);
            filledTrigonRGBA(&renderer,
                            kamera.valosLekepezese(ajtok[i].d).x,kamera.valosLekepezese(ajtok[i].d).y,
                            kamera.valosLekepezese(ajtok[i].a).x,kamera.valosLekepezese(ajtok[i].a).y,
                            kamera.valosLekepezese(ajtok[i].c).x,kamera.valosLekepezese(ajtok[i].c).y,
                            255,0,0,255);
            trigonRGBA(&renderer,
                            kamera.valosLekepezese(ajtok[i].d).x,kamera.valosLekepezese(ajtok[i].d).y,
                            kamera.valosLekepezese(ajtok[i].a).x,kamera.valosLekepezese(ajtok[i].a).y,
                            kamera.valosLekepezese(ajtok[i].c).x,kamera.valosLekepezese(ajtok[i].c).y,
                            0,255,0,255);
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
Szoba szoba;
Emelet emelet;

bool folyamatban = false;
float f1 = 1.f;
int szerkesztoMod = 0; /// 0 csúcsok, 1 diákok, 2 oktató

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
        if (ev.key.keysym.sym == SDLK_1){
            drawFalak=!drawFalak;
        }
        if (ev.key.keysym.sym == SDLK_2){
            drawAjtok=!drawAjtok;
        }
        if (ev.key.keysym.sym == SDLK_3){
            drawNavMesh=!drawNavMesh;
        }
        if (ev.key.keysym.sym == SDLK_4){
            drawPoziciok=!drawPoziciok;
        }
        if (ev.key.keysym.sym == SDLK_p){
            emelet.agentSP = (emelet.agentSP+1)%emelet.agentSizes.size();
            cout<<"emelet.agentSP: "<<emelet.agentSP<<endl;
        }
        if (ev.key.keysym.sym == SDLK_q){
            szerkesztoMod++;
            szerkesztoMod=szerkesztoMod%4;
            cout<<"szerkesztoMod: "<<szerkesztoMod<<endl;
        }
        if (ev.key.keysym.sym == SDLK_o){
            vilag.atjarhato=!vilag.atjarhato;
            cout<<"vilag.atjarhato: "<<(bool)vilag.atjarhato<<true<<endl;
        }
        if (ev.key.keysym.sym == SDLK_l){
            string name; cout<<"Betoltendo szoba: "; cin>>name;
            szoba.loadSzobaFromFile(name);
            szoba.createVilag({10.f});
            cout<<"loadSzobaFromFile: "<<endl;
        }

        if (ev.key.keysym.sym == SDLK_m){
            if (szerkesztoMod==1)
                vilag.diakok.pop_back();
            if (szerkesztoMod==2)
                vilag.OktatoTorlese();
            if (szerkesztoMod==3)
                vilag.ajtok.pop_back();
            cout<<"szerkesztoMod: "<<szerkesztoMod<<endl;
        }
        if (ev.key.keysym.sym == SDLK_n){
            vilag.save();
            cout<<"vilag.save(): "<<endl;
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
        if (szerkesztoMod==0){
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
        } else if (szerkesztoMod==1){
            Hely temp; temp.szobaPoz=vec2(kamera.kepiLekepzese(vec2(ev.button.x,ev.button.y)));
            vilag.diakok.push_back(temp);
        } else if (szerkesztoMod==2){
            Hely temp; temp.szobaPoz=vec2(kamera.kepiLekepzese(vec2(ev.button.x,ev.button.y)));
            vilag.oktato=temp;
        } else if (szerkesztoMod==3){
            vilag.SetAjto(kamera.kepiLekepzese(vec2(ev.button.x,ev.button.y)));
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
            SDL_SetRenderDrawColor( &renderer, 50, 50, 50, 255 ); /// tisztító szín
            SDL_RenderClear(&renderer); /// tiszta
            kamera.moveCamera(dt); /// mozgatja a kamerát
            /*
            if (folyamatban){
                vilag.agentRadius+=dt/1000.f*f1;
                if (vilag.agentRadius<0)
                    vilag.agentRadius=0;
                vilag.alaprajzhozTartozoLetrehozasa();
            }
            filledCircleRGBA(&renderer,kamera.valosLekepezese(vec2(0,0)).x,kamera.valosLekepezese(vec2(0,0)).y,10,255,255,255,255);
            palya.draw(renderer,kamera); /// kirajzolja a pályát
            vilag.draw(renderer,kamera);
            szoba.draw(renderer,kamera);
            */
            emelet.draw(renderer,kamera);
            SDL_RenderPresent(&renderer); /// meg is jeleníti
            //megjelenites(renderer,window,palya,step_cnt);
        }
    }
}
