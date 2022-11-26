#include "common.h"
#include "vec3.h"

using namespace std;


struct Pont{
    vec2 p;
/*
    Pont() {}
    Pont(float x, float y) :p.x(x), p.y(y) {}
    Pont(const vec2& p) : p(p) {}
    */
};




struct Szakasz{
    vec2 p1;
    vec2 p2;

    Szakasz(){}
    Szakasz(vec2 q1, vec2 q2){
        p1=q1; p2=q2;
    }

    Szakasz inv(){
        return Szakasz(p2,p1);
    }

    void draw(SDL_Renderer &renderer, vec2 cameraPos, int r = 255, int g = 255, int b = 23){
            lineRGBA(&renderer,
                     p1.x-cameraPos.x,p1.y-cameraPos.y,
                     p2.x-cameraPos.x,p2.y-cameraPos.y,
                     r,g,b,255);
    }
};

bool operator== (const Szakasz& lhs,const Szakasz& rhs) {

    return (lhs.p1==rhs.p1 && lhs.p2==rhs.p2) || (lhs.p2==rhs.p1 && lhs.p1==rhs.p2);
}

bool operator< (const Szakasz& lhs,const Szakasz& rhs) {
    //if (lhs==rhs)
        //return false;
    if (lhs.p1==rhs.p1)
        return lhs.p2<rhs.p2;
    return lhs.p1<rhs.p1;
}



bool metszikEgymast(Szakasz sz1, Szakasz sz2){
    double res = 0, s = 0, t = 0;
    vec2 closest2[2];
    bool Dret = false;
    DistanceSegments2(sz1.p1,sz1.p2,sz2.p1,sz2.p2,res,s,t,closest2,Dret);
    return (!Dret && res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) ));
}

bool metszikEgymastPrec(Szakasz sz1, Szakasz sz2){
    double res = 0, s = 0, t = 0;
    vec2 closest2[2];
    bool Dret = false;
    DistanceSegments2(sz1.p1,sz1.p2,sz2.p1,sz2.p2,res,s,t,closest2,Dret);
    return (!Dret && res<EPSZ2 && ((t>EPSZ2 && t<1.0f-EPSZ2) || (s>EPSZ2 && s<1.0f-EPSZ2) ));
}

bool metszikVagyAtlapolodnak(Szakasz sz1, Szakasz sz2){
    if ((sz1.p1 == sz2.p1 && sz1.p2 == sz2.p2) || (sz1.p2 == sz2.p1 && sz1.p1 == sz2.p2))
        return true;
    double res = 0, s = 0, t = 0;
    vec2 closest2[2];
    bool Dret = false;
    DistanceSegments2(sz1.p1,sz1.p2,sz2.p1,sz2.p2,res,s,t,closest2,Dret);
    return (res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) ));
}

bool metszikVagyAtlapolodnakPrec(Szakasz sz1, Szakasz sz2){
    if ((sz1.p1 == sz2.p1 && sz1.p2 == sz2.p2) || (sz1.p2 == sz2.p1 && sz1.p1 == sz2.p2))
        return true;
    double res = 0, s = 0, t = 0;
    vec2 closest2[2];
    bool Dret = false;
    DistanceSegments2(sz1.p1,sz1.p2,sz2.p1,sz2.p2,res,s,t,closest2,Dret);
    return (res<EPSZ2 && ((t>EPSZ2 && t<1.0f-EPSZ2) || (s>EPSZ2 && s<1.0f-EPSZ2) ));
}

struct Haromszog{
    bool torolt = false;
    vec2 A, B, C;
    int id;

    double area()
    {
       return abs((A.x*(B.y-C.y) + B.x*(C.y-A.y)+ C.x*(A.y-B.y))/2.0);
    }

    bool benneVanAPont(vec2 pont){
        vec2 csucs(pont);
        //Csucs *cs1 = &csucs;
        double elvileg = Haromszog(A,B,csucs).area() + Haromszog(A,C,csucs).area() + Haromszog(C,B,csucs).area();
        double epszilon = 0.01;
        if (area()+epszilon>=elvileg)
            return true;
        return false;
    }

    Haromszog(){
        //A=nullptr; B=nullptr; C=nullptr;
    }
    Haromszog(vec2 a, vec2 b, vec2 c){
        A=a; B=b; C=c;
    }
};

struct Sikidom{
    vector<Szakasz> szakaszok;
    bool nyilt = false;
    bool belso = true;

    void draw(SDL_Renderer &renderer, vec2 cameraPos, bool navMesh=false){
        int r = 255, g=0, b=0;
        if (navMesh){
            r=0; g=255;

            filledTrigonRGBA(&renderer,
                       szakaszok[0].p1.x-cameraPos.x,szakaszok[0].p1.y-cameraPos.y,
                       szakaszok[0].p2.x-cameraPos.x,szakaszok[0].p2.y-cameraPos.y,
                       szakaszok[1].p2.x-cameraPos.x,szakaszok[1].p2.y-cameraPos.y,
                       g,b,r,255);

        }
        for (int i=0; i<szakaszok.size(); i++){
            lineRGBA(&renderer,
                     szakaszok[i].p1.x-cameraPos.x,szakaszok[i].p1.y-cameraPos.y,
                     szakaszok[i].p2.x-cameraPos.x,szakaszok[i].p2.y-cameraPos.y,
                     r,g,b,255);
        }
        for (int i=0; i<szakaszok.size(); i++){
            filledCircleRGBA(&renderer,szakaszok[i].p1.x-cameraPos.x,szakaszok[i].p1.y-cameraPos.y,5,r,g,130,255);
        }
    }

    vector<Szakasz> belsoAtlok(){
        vector<Szakasz> atlok;  /// ebbe kerulnek majd bele az atlok
        if (!nyilt && belso){   /// ha nyilt, vagy nem a belso a fontos, akkor nem kell foglalkozni vele
            return atlok;
        }
        vector<Szakasz> oldalak = szakaszok;
        vector<Szakasz> atlokEsOldalak = oldalak;   /// ezekkel nem szabad metszeni magunkat
        vector<vec2> csucsok;                       /// miket akarunk osszekottetni
        ///cout<<"OLDALAK"<<endl;
        for (int i=0; i<oldalak.size(); i++){
            ///cout<<oldalak[i].p1.x<<" "<<oldalak[i].p1.y<<", "<<oldalak[i].p2.x<<" "<<oldalak[i].p2.y<<" "<<endl;
            csucsok.push_back(oldalak[i].p1);
        }
        for (int i=0; i<csucsok.size(); i++){
            for (int j=i; j<csucsok.size(); j++){   /// megnézünk minden csúcspárost
                if (i==j)
                    continue;
                bool siker = true;
                Szakasz sz(csucsok[i],csucsok[j]);  /// szakaszt képzünk belőlük
                for (int k=0; k<atlokEsOldalak.size(); k++){
                    if (metszikVagyAtlapolodnak(sz,atlokEsOldalak[k])){ /// ha valamivel átlapolódnok, akkor baj van
                        siker = false;
                    }
                }
                if (siker){ /// ha nem lapolódtak át, akkor mind az átlókhoz, mind az ÉsOldalakhoz hozzáadjuk őket
                    atlok.push_back(sz);
                    atlokEsOldalak.push_back(sz);
                }
            }
        }
        vector<vector<int>> szomszedCsucsIdx(csucsok.size()); /// eltárolom, hogy mely csúcsok melyekkel vannak összekötve
        for (int i=0; i<atlokEsOldalak.size(); i++){    /// megnézem, hogy végül kik lettek összekötve
            //vec2 p1, p2;
            int p1, p2;
            for (int j=0; j<csucsok.size(); j++){
                if (atlokEsOldalak[i].p1==csucsok[j])
                    p1=j;
                if (atlokEsOldalak[i].p2==csucsok[j])
                    p2=j;
            }
            szomszedCsucsIdx[p1].push_back(p2);
            szomszedCsucsIdx[p2].push_back(p1);
        }

        bool stop = false;
        int cnt=0;
        cout<<"atlokBefore: "<<atlok.size()<<endl;
        while(!stop && true){
            for (int i=0; i<szomszedCsucsIdx.size(); i++){
                ///cout<<"Szomszedok: ";
                for (int j=0; j<szomszedCsucsIdx[i].size(); j++){
                    ///cout<<szomszedCsucsIdx[i][j]<<", ";
                }
                ///cout<<endl;
            }
            cnt++;
            stop=true;
            for (int i=0; i<csucsok.size(); i++){
                for (int j=i; j<csucsok.size(); j++){
                    if (j==i)
                        continue;
                    bool szomszedok = false;
                    for (int k=0; k<szomszedCsucsIdx[i].size(); k++){
                        if (szomszedCsucsIdx[i][k]==j)
                            szomszedok=true;
                    }
                    if (!szomszedok)
                        continue;
                    int a = -1, b = -1;
                    int aIdxK=-1, aIdxL=-1, bIdxK=-1, bIdxL=-1;
                    for (int k=0; k<szomszedCsucsIdx[i].size(); k++){
                        for (int l=0; l<szomszedCsucsIdx[j].size(); l++){
                            if (szomszedCsucsIdx[i][k]==szomszedCsucsIdx[j][l]){
                                if (DEBUG) cout<<"kozos szomszed: "<<i<<" "<<j<<" -> "<<szomszedCsucsIdx[i][k]<<endl;
                                if (a==-1){
                                    a=szomszedCsucsIdx[i][k];
                                    aIdxK = k;
                                    aIdxL = l;
                                }
                                else if (b==-1){
                                    b=szomszedCsucsIdx[i][k];
                                    bIdxK = k;
                                    bIdxL = l;
                                }
                                else
                                    cout<<"ERROR"<<endl;
                            }
                        }
                    }
                    if (a*b<0){
                        bool oldal=false;
                        for (int z=0; z<oldalak.size(); z++){
                            if ((oldalak[z].p1==csucsok[i] && oldalak[z].p2==csucsok[j]) || (oldalak[z].p2==csucsok[i] && oldalak[z].p1==csucsok[j])){
                                oldal=true;
                                break;
                            }

                        }
                        if (oldal && DEBUG)
                            cout<<"oldal: "<<i<<" "<<j<<endl;
                        if (!oldal){
                            if (DEBUG) cout<<"NEM oldal: "<<i<<" "<<j<<endl;
                            stop=false;
                            for (int k=0; k<szomszedCsucsIdx[a].size();k++){
                                if (szomszedCsucsIdx[a][k]==i || szomszedCsucsIdx[a][k]==j){
                                    //szomszedCsucsIdx[a].erase(szomszedCsucsIdx[a].begin()+k);
                                    //k--;
                                }
                            }
                            int cnt2 = 0;
                            for (int k=0; k<atlok.size(); k++){
                                if ((atlok[k].p1==csucsok[i] && atlok[k].p2==csucsok[j]) || (atlok[k].p2==csucsok[i] && atlok[k].p1==csucsok[j])){
                                    atlok.erase(atlok.begin()+k);
                                    k--;
                                    cnt2++;
                                    //break;
                                }
                            }
                            if (DEBUG) cout<<"CNT2: "<<cnt2<<endl;
                            if (DEBUG) cout<<"torol: "<<i<<" "<<j<<endl;
                            szomszedCsucsIdx[i].erase(std::remove(szomszedCsucsIdx[i].begin(), szomszedCsucsIdx[i].end(), j), szomszedCsucsIdx[i].end());
                            szomszedCsucsIdx[j].erase(std::remove(szomszedCsucsIdx[j].begin(), szomszedCsucsIdx[j].end(), i), szomszedCsucsIdx[j].end());
                            //szomszedCsucsIdx[i].erase(szomszedCsucsIdx[i].begin()+aIdxK);
                            //szomszedCsucsIdx[j].erase(szomszedCsucsIdx[j].begin()+aIdxL);
                        }
                    }
                }
            }
        }
        for (int i=0; i<szomszedCsucsIdx.size() && DEBUG; i++){
            cout<<"Szomszedok: ";
            for (int j=0; j<szomszedCsucsIdx[i].size(); j++){
                cout<<szomszedCsucsIdx[i][j]<<", ";
            }
            cout<<endl;
        }
        if (DEBUG) cout<<"CNT: "<<cnt<<endl;
        if (DEBUG) cout<<"atlok: "<<atlok.size()<<endl;
        return atlok;
    }
};

struct Palya{
    float sizeX = 400, sizeY = 400;
    float posX = 0, posY = 0;
    int sok = 0;
    vector<Sikidom> sikidomok;
    vector<Sikidom> navMesh;
    vector<vector<Szakasz>> belsoAtlok;
    vector<Szakasz> BAszakasz;

    Palya(){

        Sikidom temp;
        temp.szakaszok.push_back(Szakasz(vec2(0,0),vec2(0,400)));
        temp.szakaszok.push_back(Szakasz(vec2(0,400),vec2(400,400)));
        temp.szakaszok.push_back(Szakasz(vec2(400,400),vec2(400,0)));
        temp.szakaszok.push_back(Szakasz(vec2(400,0),vec2(0,0)));
        /*
        temp.szakaszok.push_back(Szakasz(vec2(0,0),vec2(0,400)));
        temp.szakaszok.push_back(Szakasz(vec2(0,400),vec2(400,400)));
        temp.szakaszok.push_back(Szakasz(vec2(400,400),vec2(400,100)));
        temp.szakaszok.push_back(Szakasz(vec2(400,100),vec2(300,100)));
        temp.szakaszok.push_back(Szakasz(vec2(300,100),vec2(300,0)));
        temp.szakaszok.push_back(Szakasz(vec2(300,0),vec2(0,0)));
        */
        sikidomok.push_back(temp);
        temp.belso=false;

        vector<vec2> kbPontok;
        //*
        kbPontok.push_back(vec2(100,100));
        kbPontok.push_back(vec2(300,300));
        kbPontok.push_back(vec2(300,100));
        kbPontok.push_back(vec2(100,300));
        kbPontok.push_back(vec2(200,200));
        kbPontok.push_back(vec2(200,100));
        kbPontok.push_back(vec2(100,200));
        kbPontok.push_back(vec2(200,300));
        kbPontok.push_back(vec2(300,200));
        //*/
        if (RANDOM)
            srand(time(NULL));
        float PI = 3.1415f;
        for (int i=0; i<kbPontok.size(); i++){
            int hanySzog = 3+rand()%6;
            float kezdoszog = rand()%360;
            temp.szakaszok.clear();
            for (int j=0; j<hanySzog-1; j++){
                vec2 uj, regi;
                int radius = 40;
                uj.x = kbPontok[i].x+radius*cos((kezdoszog+j*(360.0f/hanySzog))*PI/180.f);
                uj.y = kbPontok[i].y+radius*sin((kezdoszog+j*(360.0f/hanySzog))*PI/180.f);
                regi.x = kbPontok[i].x+radius*cos((kezdoszog+(j-1)*(360.0f/hanySzog))*PI/180.f);
                regi.y = kbPontok[i].y+radius*sin((kezdoszog+(j-1)*(360.0f/hanySzog))*PI/180.f);
                temp.szakaszok.push_back(Szakasz(regi,uj));
            }
            //if (hanySzog>3){
            temp.szakaszok.push_back(Szakasz(temp.szakaszok[temp.szakaszok.size()-1].p2,temp.szakaszok[0].p1));
            sikidomok.push_back(temp);
            //}
        }
        /*
        temp.szakaszok.clear();
        temp.szakaszok.push_back(Szakasz(vec2(100,100),vec2(100,175)));
        temp.szakaszok.push_back(Szakasz(vec2(100,175),vec2(175,175)));
        temp.szakaszok.push_back(Szakasz(vec2(175,175),vec2(175,100)));
        temp.szakaszok.push_back(Szakasz(vec2(175,100),vec2(100,100)));
        temp.belso=false;
        sikidomok.push_back(temp);

        temp.szakaszok.clear();
        temp.szakaszok.push_back(Szakasz(vec2(225,225),vec2(225,300)));
        temp.szakaszok.push_back(Szakasz(vec2(225,300),vec2(300,300)));
        temp.szakaszok.push_back(Szakasz(vec2(300,300),vec2(300,225)));
        temp.szakaszok.push_back(Szakasz(vec2(300,225),vec2(225,225)));
        temp.belso=false;
        sikidomok.push_back(temp);

        temp.szakaszok.clear();
        temp.szakaszok.push_back(Szakasz(vec2(100,200),vec2(200,200)));
        temp.szakaszok.push_back(Szakasz(vec2(200,200),vec2(200,300)));
        temp.szakaszok.push_back(Szakasz(vec2(200,300),vec2(175,300)));
        temp.szakaszok.push_back(Szakasz(vec2(175,300),vec2(175,225)));
        temp.szakaszok.push_back(Szakasz(vec2(175,225),vec2(125,225)));
        temp.szakaszok.push_back(Szakasz(vec2(125,225),vec2(125,300)));
        temp.szakaszok.push_back(Szakasz(vec2(125,300),vec2(100,300)));
        temp.szakaszok.push_back(Szakasz(vec2(100,300),vec2(100,200)));
        temp.belso=false;
        sikidomok.push_back(temp);

        temp.szakaszok.clear();
        temp.szakaszok.push_back(Szakasz(vec2(100,100),vec2(200,100)));
        temp.szakaszok.push_back(Szakasz(vec2(200,100),vec2(200,200)));
        temp.szakaszok.push_back(Szakasz(vec2(200,200),vec2(200,300)));
        temp.szakaszok.push_back(Szakasz(vec2(200,300),vec2(300,300)));
        temp.szakaszok.push_back(Szakasz(vec2(300,300),vec2(300,200)));
        temp.szakaszok.push_back(Szakasz(vec2(300,200),vec2(200,200)));
        temp.szakaszok.push_back(Szakasz(vec2(200,200),vec2(100,200)));
        temp.szakaszok.push_back(Szakasz(vec2(100,200),vec2(100,100)));
        temp.belso=false;
        sikidomok.push_back(temp);
        */
    }

    void bakeNavMesh(){
        vector<vector<vec2>> halmaz;
        vector<vector<bool>> sikeresSzakaszok;
        vector<Szakasz> szakaszok;
        for (int i=0; i<sikidomok.size(); i++){
            vector<vec2> temp;
            vector<bool> tempB(sikidomok[i].szakaszok.size(),false);
            if (sikidomok[i].nyilt)
                temp.push_back(sikidomok[i].szakaszok[0].p1);
            for (int j=0; j<sikidomok[i].szakaszok.size(); j++){
                temp.push_back(sikidomok[i].szakaszok[j].p2);
                szakaszok.push_back(sikidomok[i].szakaszok[j]);
            }
            sikeresSzakaszok.push_back(tempB);
            halmaz.push_back(temp);
        }
        vector<Szakasz> belsoAtlokTemp;
        for (int i=0; i<sikidomok.size(); i++){
            vector<Szakasz> temp = sikidomok[i].belsoAtlok();
            belsoAtlokTemp.insert(belsoAtlokTemp.end(), temp.begin(), temp.end());
        }
        cout<<"belsoAtlokTemp "<<belsoAtlokTemp.size()<<endl;
        cout<<"szakaszok.size()"<<szakaszok.size()<<endl;
        cout<<halmaz.size()<<endl;
        int pontCnt = 0;
        for (int i=0; i<halmaz.size(); i++)
            pontCnt+=halmaz[i].size();
        cout<<pontCnt<<endl;
        for (int j=0; j<halmaz.size(); j++){
            if (DEBUG) cout<<"j: "<<j<<endl;
            for (int i=0; i<halmaz[j].size(); i++){
                if (DEBUG) cout<<"i: "<<i<<endl;
                vec2 p1 = halmaz[j][i];
                int tidx = i+1;
                if (i+1>=halmaz[j].size())
                    tidx=0;
                vec2 p2 = halmaz[j][tidx];
                for (int k=0; k<halmaz.size(); k++){
                    ///cout<<"k: "<<k<<endl;
                    if (k==j) /// nicns saját magával történõ háromszögezés
                        continue;
                    for (int l=0; l<halmaz[k].size(); l++){
                        bool siker = true;
                        ///cout<<"l: "<<l<<endl;
                        vec2 p3 = halmaz[k][l];
                        Szakasz sz1(p1,p3);
                        Szakasz sz2(p2,p3);
                        /// EZ NEM SZUKSEGES
                        for (int z=0; z<belsoAtlokTemp.size(); z++){
                            if (metszikVagyAtlapolodnak(sz1,belsoAtlokTemp[z])){
                                if (DEBUG) cout<<"SIKER, JIPPI1"<<endl;
                                siker=false;
                                break;
                            }
                            if (metszikVagyAtlapolodnak(sz2,belsoAtlokTemp[z])){
                                if (DEBUG) cout<<"SIKER, JIPPI2"<<endl;
                                siker=false;
                                break;
                            }
                        }
                        ///cout<<"alma"<<endl;
                        for (int z=0; z<szakaszok.size(); z++){
                            ///cout<<"z: "<<z<<endl;
                            double res = 0, s = 0, t = 0;
                            //vec2 p0(sz1.p1.x,sz1.p2.y), p1(2.0f, 1.0f), q0(0.0f, 1.0f), q1(2.0f, 1.0f);
                            vec2 closest2[2];
                            bool Dret = false;

                            DistanceSegments2(sz1.p1,sz1.p2,szakaszok[z].p1,szakaszok[z].p2,res,s,t,closest2,Dret);
                            if (res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) )){
                                if (DEBUG) cout<<"BAJOS1"<<endl;
                                if (DEBUG) cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
                                if (DEBUG) cout<<"i: "<<i<<", j: "<<j<<endl;
                                if (DEBUG) cout<<p1.x<<" "<<p1.y<<", "<<p3.x<<" "<<p3.y<<endl;
                                if (DEBUG) cout<<szakaszok[z].p1.x<<" "<<szakaszok[z].p1.y<<", "<<szakaszok[z].p2.x<<" "<<szakaszok[z].p2.y<<endl;
                                siker=false;
                                continue;
                            }
                            //if ()
                            DistanceSegments2(sz2.p1,sz2.p2,szakaszok[z].p1,szakaszok[z].p2,res,s,t,closest2,Dret);
                            if (res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) )){
                                if (DEBUG) cout<<"BAJOS2"<<endl;
                                if (DEBUG) cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
                                if (DEBUG) cout<<"i: "<<i<<", j: "<<j<<endl;
                                if (DEBUG) cout<<p2.x<<" "<<p2.y<<", "<<p3.x<<" "<<p3.y<<endl;
                                if (DEBUG) cout<<szakaszok[z].p1.x<<" "<<szakaszok[z].p1.y<<", "<<szakaszok[z].p2.x<<" "<<szakaszok[z].p2.y<<endl;
                                siker=false;
                                continue;
                            }
                            ///cout<<"korte"<<endl;
                            Haromszog haromszog(p1,p2,p3);
                            ///cout<<"korte2"<<endl;
                            bool ures = true;
                            for (int zz=0; zz<halmaz.size(); zz++){
                                ///if (DEBUG) cout<<"zz: "<<zz<<endl;
                                for (int zzz=0; zzz<halmaz[zz].size(); zzz++){
                                    ///if (DEBUG) cout<<"zzz: "<<zzz<<endl;
                                    if ((zz==j && (i==zzz || tidx==zzz)) || (zz==k && zzz==l))
                                        continue;

                                    ///if (DEBUG) cout<<"banan"<<endl;
                                    if (haromszog.benneVanAPont(halmaz[zz][zzz])){
                                        zz=halmaz.size()-1;
                                        zzz=halmaz[zz].size()-1;
                                        ures = false;
                                    }
                                    ///cout<<"banan2"<<endl;
                                }
                                ///cout<<"banan3"<<endl;
                            }
                            ///cout<<"banan4"<<endl;
                            if (!ures){
                                if (DEBUG) cout<<"URES"<<endl;
                                siker=false;
                                continue;
                            }
                            if (DEBUG) cout<<"kazmer"<<endl;
                            //siker=true;
                            //l=INT_MAX;
                            //k=halmaz.size()-1;
                            //z=INT_MAX;
                            //cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
                        }
                        if (siker){
                            if (DEBUG) cout<<"SIKER"<<endl;
                            Sikidom tempS;
                            tempS.szakaszok.push_back(Szakasz(p1,p2));
                            tempS.szakaszok.push_back(Szakasz(p2,p3));
                            tempS.szakaszok.push_back(Szakasz(p3,p1));
                            if (DEBUG) cout<<"JEEJ"<<endl;
                            szakaszok.push_back(tempS.szakaszok[1]);
                            szakaszok.push_back(tempS.szakaszok[2]);
                            if (DEBUG) cout<<szakaszok.size()<<endl;
                            navMesh.push_back(tempS);
                            sikeresSzakaszok[j][i]=true;
                        }
                    }
                }
            }
        }

        cout<<"EDDIG JO"<<endl;
        for (int j=0; j<halmaz.size(); j++){
            for (int i=0; i<halmaz[j].size(); i++){
                if (sikeresSzakaszok[j][i])
                    continue;
                vec2 p1 = halmaz[j][i];
                int tidx = i+1;
                if (i+1>=halmaz[j].size())
                    tidx=0;
                vec2 p2 = halmaz[j][tidx];
                for (int k=0; k<halmaz[j].size(); k++){
                    if (k==i || k==tidx)
                        continue;
                    bool siker = true;
                    ///cout<<"l: "<<l<<endl;
                    vec2 p3 = halmaz[j][k];
                    Szakasz sz1(p1,p3);
                    Szakasz sz2(p2,p3);
                    for (int z=0; z<belsoAtlokTemp.size(); z++){
                        if (metszikVagyAtlapolodnak(sz1,belsoAtlokTemp[z])){
                            if (DEBUG) cout<<"SIKER, JIPPI1"<<endl;
                            siker=false;
                            break;
                        }
                        if (metszikVagyAtlapolodnak(sz2,belsoAtlokTemp[z])){
                            if (DEBUG) cout<<"SIKER, JIPPI2"<<endl;
                            siker=false;
                            break;
                        }
                    }

                    for (int z=0; z<szakaszok.size(); z++){
                        ///cout<<"z: "<<z<<endl;
                        double res = 0, s = 0, t = 0;
                        //vec2 p0(sz1.p1.x,sz1.p2.y), p1(2.0f, 1.0f), q0(0.0f, 1.0f), q1(2.0f, 1.0f);
                        vec2 closest2[2];
                        bool Dret = false;
                        DistanceSegments2(sz1.p1,sz1.p2,szakaszok[z].p1,szakaszok[z].p2,res,s,t,closest2,Dret);
                        if (!Dret && res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) )){
                            if (DEBUG) cout<<"BAJOS1"<<endl;
                            if (DEBUG) cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
                            if (DEBUG) cout<<"i: "<<i<<", j: "<<j<<endl;
                            if (DEBUG) cout<<p1.x<<" "<<p1.y<<", "<<p3.x<<" "<<p3.y<<endl;
                            if (DEBUG) cout<<szakaszok[z].p1.x<<" "<<szakaszok[z].p1.y<<", "<<szakaszok[z].p2.x<<" "<<szakaszok[z].p2.y<<endl;
                            siker=false;
                            continue;
                        }
                        //if ()
                        DistanceSegments2(sz2.p1,sz2.p2,szakaszok[z].p1,szakaszok[z].p2,res,s,t,closest2,Dret);
                        if (!Dret && res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) )){
                            if (DEBUG) cout<<"BAJOS2"<<endl;
                            if (DEBUG) cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
                            if (DEBUG) cout<<"i: "<<i<<", j: "<<j<<endl;
                            if (DEBUG) cout<<p2.x<<" "<<p2.y<<", "<<p3.x<<" "<<p3.y<<endl;
                            if (DEBUG) cout<<szakaszok[z].p1.x<<" "<<szakaszok[z].p1.y<<", "<<szakaszok[z].p2.x<<" "<<szakaszok[z].p2.y<<endl;
                            siker=false;
                            continue;
                        }
                        ///cout<<"korte"<<endl;
                        Haromszog haromszog(p1,p2,p3);
                        ///cout<<"korte2"<<endl;
                        bool ures = true;
                        for (int zz=0; zz<halmaz.size(); zz++){
                            if (DEBUG) cout<<"zz: "<<zz<<endl;
                            for (int zzz=0; zzz<halmaz[zz].size(); zzz++){
                                if (DEBUG) cout<<"zzz: "<<zzz<<endl;
                                if ((zz==j && (i==zzz || tidx==zzz)) || (zz==j && zzz==k))
                                    continue;

                                if (DEBUG) cout<<"banan"<<endl;
                                if (haromszog.benneVanAPont(halmaz[zz][zzz])){
                                    zz=halmaz.size()-1;
                                    zzz=halmaz[zz].size()-1;
                                    ures = false;
                                }
                                ///cout<<"banan2"<<endl;
                            }
                            ///cout<<"banan3"<<endl;
                        }
                        ///cout<<"banan4"<<endl;
                        if (!ures){
                            if (DEBUG) cout<<"URES"<<endl;
                            siker=false;
                            continue;
                        }

                        if (DEBUG) cout<<"kazmer"<<endl;
                        //siker=true;
                        //l=INT_MAX;
                        //k=halmaz.size()-1;
                        //z=INT_MAX;
                        //cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
                    }
                    if (siker){
                        for (int kk=0; k<navMesh.size(); k++){
                            /// LEHET KI KÉNE SZŰRNI AZ EGYBEVÁGÓ HÁROMSZÖGEKET
                        }
                        if (DEBUG) cout<<"SIKER"<<endl;
                        Sikidom tempS;
                        tempS.szakaszok.push_back(Szakasz(p1,p2));
                        tempS.szakaszok.push_back(Szakasz(p2,p3));
                        tempS.szakaszok.push_back(Szakasz(p3,p1));
                        if (DEBUG) cout<<"JEEJ"<<endl;
                        belsoAtlokTemp.push_back(tempS.szakaszok[0]);
                        szakaszok.push_back(tempS.szakaszok[1]);
                        szakaszok.push_back(tempS.szakaszok[2]);
                        if (DEBUG) cout<<szakaszok.size()<<endl;
                        navMesh.push_back(tempS);
                        sikeresSzakaszok[j][i]=true;
                    }
                }
            }
        }

        cout<<"SZAKASZOK"<<endl;
        for (int i=0; i<szakaszok.size(); i++){
            for (int j=0; j<szakaszok.size(); j++){
                if (j==i)
                    continue;
                double res = 0, s = 0, t = 0;
                vec2 closest2[2];
                bool Dret = false;
                DistanceSegments2(szakaszok[i].p1,szakaszok[i].p2,szakaszok[j].p1,szakaszok[j].p2,res,s,t,closest2,Dret);
                if ((res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) ))){
                    cout<<i<<" "<<j<<endl;
                    cout<<szakaszok[i].p1.x<<" "<<szakaszok[i].p1.y<<", "<<szakaszok[i].p2.x<<" "<<szakaszok[i].p2.x<<endl;
                    cout<<szakaszok[j].p1.x<<" "<<szakaszok[j].p1.y<<", "<<szakaszok[j].p2.x<<" "<<szakaszok[j].p2.x<<endl;
                    cout<<endl;
                }
            }

            if ((szakaszok[i].p1 == vec2(225,225) && szakaszok[i].p2 == vec2(400,0)) || (szakaszok[i].p1 == vec2(400,0) && szakaszok[i].p2 == vec2(225,225))){
                cout<<"ITT VAGYOK 1"<<endl;
            }
            if ((szakaszok[i].p1 == vec2(200,200) && szakaszok[i].p2 == vec2(300,225)) || (szakaszok[i].p1 == vec2(300,225) && szakaszok[i].p2 == vec2(200,200))){
                cout<<"ITT VAGYOK 2"<<endl;
            }
        }
        double res = 0, s = 0, t = 0;
        vec2 closest2[2];
        bool Dret = false;
        DistanceSegments2(vec2(225,225),vec2(400,0),vec2(200,200),vec2(300,225),res,s,t,closest2,Dret);
        if (DEBUG) cout<<"BAJOS2"<<endl;
        if (DEBUG) cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
        if (DEBUG) cout<<"navMesh.size() "<<navMesh.size()<<endl;


        clock_t t2 = clock();
        fillNavMesh();
        cout<<"fillNavMesh() time: "<<clock()-t2<<endl;

    }

    void fillNavMesh(){
        set<Szakasz> szakaszokA, szakaszokB;
        cout<<"navMesh.size(): "<<navMesh.size()<<endl;
        for (int i=0; i<navMesh.size(); i++){
            for (int j=0; j<navMesh[i].szakaszok.size(); j++){
                Szakasz temp(navMesh[i].szakaszok[j].p2,navMesh[i].szakaszok[j].p1);
                if (szakaszokA.find(navMesh[i].szakaszok[j])==szakaszokA.end() &&
                    szakaszokA.find(temp)==szakaszokA.end()
                    ){
                    szakaszokA.insert(navMesh[i].szakaszok[j]);
                }
                else if (szakaszokB.find(navMesh[i].szakaszok[j])==szakaszokB.end() &&
                    szakaszokB.find(temp)==szakaszokB.end()
                    ){
                    szakaszokB.insert(navMesh[i].szakaszok[j]);
                }
            }
        }
        cout<<"szakaszokA0.size(): "<<szakaszokA.size()<<endl;
        cout<<"szakaszokB.size(): "<<szakaszokB.size()<<endl;
        for (int i=0; i<sikidomok.size(); i++){
            for (int j=0; j<sikidomok[i].szakaszok.size(); j++){
                Szakasz temp(sikidomok[i].szakaszok[j].p2,sikidomok[i].szakaszok[j].p1);
                if (szakaszokA.find(sikidomok[i].szakaszok[j])==szakaszokA.end() &&
                    szakaszokA.find(temp)==szakaszokA.end()
                    ){
                    szakaszokA.insert(sikidomok[i].szakaszok[j]);
                }
                else if (szakaszokB.find(sikidomok[i].szakaszok[j])==szakaszokB.end() &&
                    szakaszokB.find(temp)==szakaszokB.end()
                    ){
                    szakaszokB.insert(sikidomok[i].szakaszok[j]);
                }
            }
        }
        cout<<"szakaszokA1.size(): "<<szakaszokA.size()<<endl;
        cout<<"szakaszokB.size(): "<<szakaszokB.size()<<endl;
        BAszakasz.assign(szakaszokB.begin(),szakaszokB.end());
        cout<<"BAszakasz.size(): "<<BAszakasz.size()<<endl;

        vector<Szakasz> kivonandoSzakaszok(szakaszokB.begin(),szakaszokB.end());
        cout<<"kivonandoSzakaszok.size(): "<<kivonandoSzakaszok.size()<<endl;
        for (int i=0; i<kivonandoSzakaszok.size(); i++){
            Szakasz temp(kivonandoSzakaszok[i].p2,kivonandoSzakaszok[i].p1);
            if (szakaszokA.find(kivonandoSzakaszok[i])!=szakaszokA.end())
                szakaszokA.erase(szakaszokA.find(kivonandoSzakaszok[i]));
            else if (szakaszokA.find(temp)!=szakaszokA.end())
                szakaszokA.erase(szakaszokA.find(temp));
        }
        cout<<"szakaszokA2.size(): "<<szakaszokA.size()<<endl;

        //while (szakaszokA.size()!=0){
        vector<vec2> csucsok;
        for (int i=0; i<sikidomok.size();i++){
            for (int j=0; j<sikidomok[i].szakaszok.size(); j++){
                csucsok.push_back(sikidomok[i].szakaszok[j].p1);
            }
        }
        if (true){
            vector<Szakasz> megmaradtSzakaszok(szakaszokA.begin(),szakaszokA.end());
            for (int i=0; i<megmaradtSzakaszok.size(); i++){
                for (int j=i+1; j<megmaradtSzakaszok.size(); j++){
                    Sikidom sikidom;
                    //sikidom.szakaszok.push_back(megmaradtSzakaszok[i]);
                    //sikidom.szakaszok.push_back(megmaradtSzakaszok[j]);
                    Szakasz sz, torlesre;
                    vec2 kozos;
                    bool siker = false;
                    Szakasz sz11(megmaradtSzakaszok[i].p1,megmaradtSzakaszok[j].p1);
                    Szakasz sz12(megmaradtSzakaszok[i].p1,megmaradtSzakaszok[j].p2);
                    Szakasz sz21(megmaradtSzakaszok[i].p2,megmaradtSzakaszok[j].p1);
                    Szakasz sz22(megmaradtSzakaszok[i].p2,megmaradtSzakaszok[j].p2);
                    if (megmaradtSzakaszok[i].p1==megmaradtSzakaszok[j].p1){
                        if (szakaszokA.find(sz22) != szakaszokA.end()){
                            torlesre = sz22;
                            siker = true;
                        } else if (szakaszokA.find(sz22.inv()) != szakaszokA.end()){
                            torlesre = sz22.inv();
                            siker = true;
                        }
                        kozos = megmaradtSzakaszok[i].p1;
                        sz = Szakasz(megmaradtSzakaszok[i].p2,megmaradtSzakaszok[j].p2);
                    }
                    else if (megmaradtSzakaszok[i].p2==megmaradtSzakaszok[j].p1){
                        if (szakaszokA.find(sz12) != szakaszokA.end()){
                            torlesre = sz12;
                            siker = true;
                        } else if (szakaszokA.find(sz12.inv()) != szakaszokA.end()){
                            torlesre = sz12.inv();
                            siker = true;
                        }
                        kozos = megmaradtSzakaszok[i].p2;
                        sz = Szakasz(megmaradtSzakaszok[i].p1,megmaradtSzakaszok[j].p2);
                    }
                    else if (megmaradtSzakaszok[i].p1==megmaradtSzakaszok[j].p2){
                        if (szakaszokA.find(sz21) != szakaszokA.end()){
                            torlesre = sz21;
                            siker = true;
                        } else if (szakaszokA.find(sz21.inv()) != szakaszokA.end()){
                            torlesre = sz21.inv();
                            siker = true;
                        }
                        kozos = megmaradtSzakaszok[i].p1;
                        sz = Szakasz(megmaradtSzakaszok[i].p2,megmaradtSzakaszok[j].p1);
                    }
                    else if (megmaradtSzakaszok[i].p2==megmaradtSzakaszok[j].p2){
                        if (szakaszokA.find(sz11) != szakaszokA.end()){
                            torlesre = sz11;
                            siker = true;
                        } else if (szakaszokA.find(sz11.inv()) != szakaszokA.end()){
                            torlesre = sz11.inv();
                            siker = true;
                        }
                        kozos = megmaradtSzakaszok[i].p2;
                        sz = Szakasz(megmaradtSzakaszok[i].p1,megmaradtSzakaszok[j].p1);
                    }
                    else {
                        continue;
                    }
                    int csCnt = 0;
                    Haromszog hrsz(sz.p1,sz.p2,kozos);
                    for (int k=0; k<csucsok.size(); k++){
                        if (hrsz.benneVanAPont(csucsok[k]))
                            csCnt++;
                    }
                    if (csCnt>3)
                        siker=false;
                    if (siker){
                        sikidom.szakaszok.push_back(sz);
                        sikidom.szakaszok.push_back(Szakasz(sz.p2,kozos));
                        sikidom.szakaszok.push_back(Szakasz(kozos,sz.p1));
                        navMesh.push_back(sikidom);
                        if (szakaszokA.find(megmaradtSzakaszok[i])!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[i]));
                        if (szakaszokA.find(megmaradtSzakaszok[i].inv())!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[i].inv()));
                        if (szakaszokA.find(megmaradtSzakaszok[j])!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[j]));
                        if (szakaszokA.find(megmaradtSzakaszok[j].inv())!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[j].inv()));
                        szakaszokA.erase(szakaszokA.find(torlesre));
                        //if (szakaszokA.find(sz)!=szakaszokA.end())
                            //szakaszokA.erase(szakaszokA.find(sz));
                        //i=INT_MAX; j=INT_MAX;
                    }
                }
            }
        }
        cout<<"szakaszokA3.size(): "<<szakaszokA.size()<<endl;

        vector<Szakasz> sikidomOldalak;
        for (int i=0; i<sikidomok.size();i++){
            sikidomOldalak.insert(sikidomOldalak.end(),sikidomok[i].szakaszok.begin(),sikidomok[i].szakaszok.end());
        }

        vector<Sikidom> belsoSikidomok = szakaszokSikidomokbaSzervezese(szakaszokA);

        if (false){
            vector<Szakasz> megmaradtSzakaszok(szakaszokA.begin(),szakaszokA.end());
            vector<Szakasz> ujSzakaszok;
            for (int i=0; i<megmaradtSzakaszok.size(); i++){
                for (int j=i+1; j<megmaradtSzakaszok.size(); j++){
                    Sikidom sikidom;
                    //sikidom.szakaszok.push_back(megmaradtSzakaszok[i]);
                    //sikidom.szakaszok.push_back(megmaradtSzakaszok[j]);
                    Szakasz sz, torlesre;
                    vec2 kozos;
                    bool siker = false;
                    Szakasz sz11(megmaradtSzakaszok[i].p1,megmaradtSzakaszok[j].p1);
                    Szakasz sz12(megmaradtSzakaszok[i].p1,megmaradtSzakaszok[j].p2);
                    Szakasz sz21(megmaradtSzakaszok[i].p2,megmaradtSzakaszok[j].p1);
                    Szakasz sz22(megmaradtSzakaszok[i].p2,megmaradtSzakaszok[j].p2);
                    if (megmaradtSzakaszok[i].p1==megmaradtSzakaszok[j].p1){
                        sz = Szakasz(megmaradtSzakaszok[i].p2,megmaradtSzakaszok[j].p2);
                        kozos = megmaradtSzakaszok[i].p1;
                        siker = true;
                    }
                    else if (megmaradtSzakaszok[i].p2==megmaradtSzakaszok[j].p1){
                        sz = Szakasz(megmaradtSzakaszok[i].p1,megmaradtSzakaszok[j].p2);
                        kozos = megmaradtSzakaszok[i].p2;
                        siker = true;
                    }
                    else if (megmaradtSzakaszok[i].p1==megmaradtSzakaszok[j].p2){
                        sz = Szakasz(megmaradtSzakaszok[i].p2,megmaradtSzakaszok[j].p1);
                        kozos = megmaradtSzakaszok[i].p1;
                        siker = true;
                    }
                    else if (megmaradtSzakaszok[i].p2==megmaradtSzakaszok[j].p2){
                        sz = Szakasz(megmaradtSzakaszok[i].p1,megmaradtSzakaszok[j].p1);
                        kozos = megmaradtSzakaszok[i].p2;
                        siker = true;
                    }
                    else {
                        continue;
                    }
                    if (metszikVagyAtlapolodnak(sz,megmaradtSzakaszok[i]) || metszikVagyAtlapolodnak(sz,megmaradtSzakaszok[j]))
                        siker = false;
                    for (int k=0; k<sikidomOldalak.size(); k++){
                        if (metszikVagyAtlapolodnak(sikidomOldalak[k],sz)){
                            siker = false;
                            break;
                        }
                    }
                    for (int k=0; k<ujSzakaszok.size(); k++){
                        if (metszikEgymast(ujSzakaszok[k],sz)){
                            siker = false;
                            break;
                        }
                    }
                    if (siker){
                        ujSzakaszok.push_back(sz);
                        sikidom.szakaszok.push_back(sz);
                        sikidom.szakaszok.push_back(Szakasz(sz.p2,kozos));
                        sikidom.szakaszok.push_back(Szakasz(kozos,sz.p1));
                        navMesh.push_back(sikidom);
                        if (szakaszokA.find(megmaradtSzakaszok[i])!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[i]));
                        if (szakaszokA.find(megmaradtSzakaszok[i].inv())!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[i].inv()));
                        if (szakaszokA.find(megmaradtSzakaszok[j])!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[j]));
                        if (szakaszokA.find(megmaradtSzakaszok[j].inv())!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[j].inv()));
                        //szakaszokA.erase(szakaszokA.find(torlesre));
                        //if (szakaszokA.find(sz)!=szakaszokA.end())
                            //szakaszokA.erase(szakaszokA.find(sz));
                        //i=INT_MAX; j=INT_MAX;
                    }
                    cout<<"H"<<endl;
                }
            }
        }
        cout<<"szakaszokA.size(): "<<szakaszokA.size()<<endl;
        BAszakasz.clear();
        BAszakasz.assign(szakaszokA.begin(),szakaszokA.end());
        cout<<"BAszakasz.size(): "<<BAszakasz.size()<<endl;
        //}
    }

    vector<Sikidom> szakaszokSikidomokbaSzervezese(set<Szakasz> szakaszokA){
        vector<Szakasz> megmaradtSzakaszok(szakaszokA.begin(),szakaszokA.end());
        vector<vec2> csucsok;
        vector<int> csucsokFokszama;
        vector<vector<int>> csucsokhozSzakaszok;
        for (int i=0; i<megmaradtSzakaszok.size(); i++){
            csucsok.push_back(megmaradtSzakaszok[i].p1);
            csucsok.push_back(megmaradtSzakaszok[i].p2);
        }
        set<vec2> tempCsucsok;
        copy(csucsok.begin(), csucsok.end(),inserter(tempCsucsok, tempCsucsok.end()));
        csucsok.clear();
        csucsok.assign(tempCsucsok.begin(),tempCsucsok.end());
        csucsokFokszama.resize(csucsok.size());
        csucsokhozSzakaszok.resize(csucsok.size());
        for (int i=0; i<megmaradtSzakaszok.size(); i++){
            for (int j=0; j<csucsok.size(); j++){
                if (csucsok[j]==megmaradtSzakaszok[i].p1)
                    csucsokFokszama[j]++;
                else if (csucsok[j]==megmaradtSzakaszok[i].p2)
                    csucsokFokszama[j]++;
                else
                    continue;
                csucsokhozSzakaszok[j].push_back(i);
            }
        }
        cout<<"CSUCSOK FOKSZAMA: "<<endl;
        for (int i=0; i<csucsokFokszama.size(); i++){
            cout<<csucsokFokszama[i]<<endl;
        }

        vector<Sikidom> ret;
        vector<Szakasz> osszesSzakasz;
        set<vec2> osszesCsucsSet;
        for (int i=0; i<navMesh.size(); i++){
            for (int j=0; j<navMesh[i].szakaszok.size(); j++){
                osszesSzakasz.push_back(navMesh[i].szakaszok[j]);
                osszesCsucsSet.insert(navMesh[i].szakaszok[j].p1);
            }
        }
        vector<vec2> osszesCsucs; osszesCsucs.assign(osszesCsucsSet.begin(),osszesCsucsSet.end());
        set<Szakasz> tempOsszesSzakasz;
        copy(osszesSzakasz.begin(), osszesSzakasz.end(),inserter(tempOsszesSzakasz, tempOsszesSzakasz.end()));
        for (int i=0; i<megmaradtSzakaszok.size(); i++){
            if (tempOsszesSzakasz.find(megmaradtSzakaszok[i]) != tempOsszesSzakasz.end())
                tempOsszesSzakasz.erase(tempOsszesSzakasz.find(megmaradtSzakaszok[i]));
            if (tempOsszesSzakasz.find(megmaradtSzakaszok[i].inv()) != tempOsszesSzakasz.end())
                tempOsszesSzakasz.erase(tempOsszesSzakasz.find(megmaradtSzakaszok[i].inv()));
        }
        osszesSzakasz.clear();
        osszesSzakasz.assign(tempOsszesSzakasz.begin(),tempOsszesSzakasz.end());

        vector<vector<int>> ujHaromszogekIdx;
        cout<<"EDDIG RENDBEN"<<endl;
        for (int idx=0; idx<csucsok.size(); idx++){
            bool ujHaromszog = false;
            for (int i=0; i<csucsok.size(); i++){
                if (i==idx)
                    continue;
                for (int j=i+1; j<csucsok.size(); j++){
                    if (j==idx)
                        continue;
                    set<int> csucsIdxSet={idx,i,j};
                    vector<int> csucsIdx;
                    csucsIdx.assign(csucsIdxSet.begin(),csucsIdxSet.end());
                    bool siker = true;
                    for (int k=0; k<ujHaromszogekIdx.size(); k++){
                        if (ujHaromszogekIdx[k][0]==csucsIdx[0] &&
                            ujHaromszogekIdx[k][1]==csucsIdx[1] &&
                            ujHaromszogekIdx[k][2]==csucsIdx[2]){
                            siker=false;
                            break;
                        }
                    }
                    if (!siker)
                        continue;
                    Szakasz a(csucsok[idx],csucsok[i]), b(csucsok[i],csucsok[j]), c(csucsok[j],csucsok[idx]);
                    for (int k=0; k<osszesSzakasz.size(); k++){
                        if (metszikEgymast(a,osszesSzakasz[k]) ||
                            metszikEgymast(b,osszesSzakasz[k]) ||
                            metszikEgymast(c,osszesSzakasz[k])){
                            siker = false;
                            break;
                        }
                    }
                    Haromszog haromszog(csucsok[idx],csucsok[i],csucsok[j]);
                    for (int k=0; k<osszesCsucs.size(); k++){
                        if (osszesCsucs[k]!=haromszog.A &&
                            osszesCsucs[k]!=haromszog.B &&
                            osszesCsucs[k]!=haromszog.C){
                            if (haromszog.benneVanAPont(osszesCsucs[k])){
                                siker=false;
                                break;
                            }
                        }
                    }
                    /*
                    for (int k=0; k<megmaradtSzakaszok.size(); k++){
                        if (metszikEgymast(a,megmaradtSzakaszok[k]) ||
                            metszikEgymast(b,megmaradtSzakaszok[k]) ||
                            metszikEgymast(b,osszesSzakasz[k])){
                            siker = false;
                            break;
                        }
                    }
                    */
                    if (!siker)
                        continue;
                    ujHaromszog=true;
                    Sikidom temp;
                    temp.szakaszok.push_back(a);
                    temp.szakaszok.push_back(b);
                    temp.szakaszok.push_back(c);
                    ujHaromszogekIdx.push_back(csucsIdx);
                    navMesh.push_back(temp);
                    osszesSzakasz.push_back(a);
                    osszesSzakasz.push_back(b);
                    osszesSzakasz.push_back(c);
                }
            }
            if (!ujHaromszog)
                break;
        }

        /*
        while (true){
        int minFokszam = 0;//, idx=-1;

            for (int i=0; i<csucsokFokszama.size(); i++){
                if (csucsokFokszama[i]<minFokszam && csucsokFokszama[i]!=0){
                    minFokszam=csucsokFokszama[i];
                    idx=i;
                }
            }
            if (idx==-1){
                break;
            }
            Sikidom temp;
            while(true){
                for (int i=0; i<megmaradtSzakaszok.size(); i++){

                }
            }
        }
        */
        return ret;
    }

    void bakeAtloNavMesh() {
        for (int i=0; i<sikidomok.size(); i++){
            vector<Szakasz> temp = sikidomok[i].belsoAtlok();
            belsoAtlok.push_back(temp);
        }
    }

    void draw(SDL_Renderer &renderer, vec2 cameraPos){
        boxRGBA(&renderer,posX-cameraPos.x,posY-cameraPos.y,posX+sizeX-cameraPos.x,posY+sizeY-cameraPos.y,100,100,100,255);
        for (int i=0; i<sikidomok.size(); i++){
            sikidomok[i].draw(renderer,cameraPos);
        }
        for (int i=0; i<navMesh.size(); i++){
            if (i+1!=sok%(navMesh.size()+1) && sok%(navMesh.size()+1)!=0)
                continue;
            navMesh[i].draw(renderer,cameraPos,true);
        }
        for (int i=0; i<belsoAtlok.size(); i++){
            for (int j=0; j<belsoAtlok[i].size(); j++){
                belsoAtlok[i][j].draw(renderer,cameraPos);
            }
        }
        for (int i=0; i<BAszakasz.size(); i++){
            BAszakasz[i].draw(renderer,cameraPos,0,0,0);
        }
    }
};



vec2 camera(-40,-40);
bool W=false,A=false,S=false,D=false;
Palya palya;


void EventHandle(SDL_Event ev){

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
            Szakasz sz1(vec2(0,100),vec2(100,0));
            Szakasz sz2(vec2(100,0),vec2(0,100));
            cout<<true<<(sz1==sz1)<<(sz1==sz2)<<endl;
        }
    }

    if (ev.type == SDL_QUIT)
        exit(3);
}

void moveCamera(clock_t dt){
    if (W){
        camera.y+=dt*1.0f;
    }
    if (A){
        camera.x+=dt*1.0f;
    }
    if (S){
        camera.y-=dt*1.0f;
    }
    if (D){
        camera.x-=dt*1.0f;
    }
}

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



    while(!stop){


        if (clock()>t1+CLOCKS_PER_SEC/targetFPS){
            dt = t1 - clock();
            t1=clock();
            frame = true;
        } else {
            frame = false;
            Sleep(1);
        }


        if (clock()>=last_sec+1000){
            last_sec=clock();
            //cout<<framesInLastSec<<endl;
            framesInLastSec = 0;
        }

        if (SDL_PollEvent(&ev)){
            EventHandle(ev);
        }

        if (frame){
            framesInLastSec++;
            SDL_SetRenderDrawColor( &renderer, 0, 0, 0, 255 );
            SDL_RenderClear(&renderer);
            moveCamera(dt);
            //SDL_RenderPresent(&renderer);
            palya.draw(renderer,camera);
            SDL_RenderPresent(&renderer);
            //megjelenites(renderer,window,palya,step_cnt);
        }
    }
}
