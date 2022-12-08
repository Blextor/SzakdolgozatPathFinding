#ifndef GEO_H_INCLUDED
#define GEO_H_INCLUDED

#include "common.h"
#include "vec3.h"
#include "camera.h"

using namespace std;

// NEM HASZNÁLT
struct Pont{
    vec2 p;
/*
    Pont() {}
    Pont(float x, float y) :p.x(x), p.y(y) {}
    Pont(const vec2& p) : p(p) {}
    */
};



float Heron(float a, float b, float c){
    float s = (a+b+c)/2;
    return sqrt(s*(s-a)*(s-b)*(s-c));
}


/// két pontot tartalmaz
struct Szakasz{
    vec2 p1;
    vec2 p2;

    Szakasz(){}
    Szakasz(vec2 q1, vec2 q2){
        p1=q1; p2=q2;
    }

    /// szükséges néha pontok felcserélése
    Szakasz inv(){
        return Szakasz(p2,p1);
    }

    vec2 feleP(){
        return (p1+p2)/2;
    }

    vec2 legyenLegalabbXTavra(vec2 p, float X){
        /// saját matek
        float x = vec2Tav(p); /// p távolsága a szakaszra illeszkedő egyenestől
        if (x>=X) /// ha legalább X
            return p; /// akkor nincs dolgunk
        float e = sqrt(pow(p2.dist(p),2)-pow(x,2)); /// p2 és a p-hez a legközelebbi pont távolsága
        vec2 p21 = p1-p2; /// kell egy vektor a p2-ből a legközelebbi ponthoz
        p21 = p21.normalize(); /// normalizáltan lehet venni az E-szeresét
        vec2 p21T = p21; p21T.rotate(-90); /// és onnan kell egy merőleges
        //p21T = p21T.normalize();
        return p2+p21*e+p21T*X; /// az meg X-szeresen
    }

    float vec2Tav(vec2 p){ /// szakasztól, mint egyenestől a pont távolsága
        /// saját matek
        float a = p1.dist(p); /// p1 és a pont távolsága
        float b = p2.dist(p); /// p2 és a p távolsága
        float c = p1.dist(p2); /// a szakasz hossza
        float x = pow((b*b+c*c-a*a)/(2*c),2) - b*b; /// Pithagoras-tétel felhasználásával
        x*=-1.f;
        return sqrt(x); /// a pont távolsága
    }

    float vec2TavR(vec2 p){ /// szakasztól, mint egyenestől a pont távolsága
        /// saját matek
        float a = p1.dist(p); /// p1 és a pont távolsága
        float b = p2.dist(p); /// p2 és a p távolsága
        float c = p1.dist(p2); /// a szakasz hossza
        float x = pow((b*b+c*c-a*a)/(2*c),2) - b*b; /// Pithagoras-tétel felhasználásával
        x*=-1.f;
        x=sqrt(x);
        if (a>b){
            float t = a;
            a=b;
            b=t;
        }
        if (a*a+c*c<b*b)
            return a;
        return sqrt(x); /// a pont távolsága
    }

    /// meg lehet jeleníteni
    void draw(SDL_Renderer &renderer, Kamera kamera, int r = 255, int g = 255, int b = 23){
            lineRGBA(&renderer,
                     kamera.valosLekepezese(p1).x,kamera.valosLekepezese(p1).y,
                     kamera.valosLekepezese(p2).x,kamera.valosLekepezese(p2).y,
                     r,g,b,255);
    }
};

/// két szakasz egyenlőségének definiálása, nem vektorok, nem irányítottak
bool operator== (const Szakasz& lhs,const Szakasz& rhs) {
    return (lhs.p1==rhs.p1 && lhs.p2==rhs.p2) || (lhs.p2==rhs.p1 && lhs.p1==rhs.p2);
}

/// set<> -hez szükséges a rendezésnél
bool operator< (const Szakasz& lhs,const Szakasz& rhs) {
    //if ((lhs.p1.x<lhs.p2.x && rhs.p1.x<rhs.p2.x))
    //if ((lhs.p1==rhs.p1 && lhs.p2==rhs.p2) || (lhs.p2==rhs.p1 && lhs.p1==rhs.p2))
    Szakasz lhs2;
    if (lhs.p1.x<lhs.p2.x){
        lhs2=lhs;
    } else if (lhs.p1.x==lhs.p2.x){
        if (lhs.p1.y<=lhs.p2.y){
            lhs2=lhs;
        } else {
            lhs2.p2=lhs.p1;
            lhs2.p1=lhs.p2;
        }
    } else {
        lhs2.p2=lhs.p1;
        lhs2.p1=lhs.p2;
    }

    Szakasz rhs2;
    if (rhs.p1.x<rhs.p2.x){
        rhs2=rhs;
    } else if (rhs.p1.x==rhs.p2.x){
        if (rhs.p1.y<=rhs.p2.y){
            rhs2=rhs;
        } else {
            rhs2.p2=rhs.p1;
            rhs2.p1=rhs.p2;
        }
    } else {
        rhs2.p2=rhs.p1;
        rhs2.p1=rhs.p2;
    }
        return false;
    if (lhs2.p1==rhs2.p1)
        return lhs2.p2<rhs2.p2;
    return lhs2.p1<rhs2.p1;
}


/// iterálás során szükséges, hogy hol tartok, és honnan jöttem
struct HonnanPont{
    vec2 p;
    vec2 honnan;

    HonnanPont(){}
    HonnanPont(vec2 P){p=P;}
    HonnanPont(vec2 P, vec2 HONNAN){p=P; honnan=HONNAN;}
};

/// ezek rendezéséhez szükség van erre
bool operator< (const HonnanPont& lhs,const HonnanPont& rhs) {
    return lhs.p<rhs.p;
}

/// precízebb
bool metszikEgymastPrec(Szakasz sz1, Szakasz sz2){
    double ma, ba, mb, bb;
    ma = (sz1.p1.y-sz1.p2.y)/(sz1.p1.x-sz1.p2.x);
    ba = sz1.p1.y - ma * sz1.p1.x;

    mb = (sz2.p1.y-sz2.p2.y)/(sz2.p1.x-sz2.p2.x);
    bb = sz2.p1.y - mb * sz2.p1.x;

    if (abs(ma-mb)<EPSZ)
        return false;

    double x = (bb-ba)/(ma-mb);
    if ((sz1.p1.x+EPSZ<=x && x+EPSZ<=sz1.p2.x) || (sz1.p2.x+EPSZ<=x && x<=sz1.p1.x)){
        if ((sz2.p1.x+EPSZ<=x && x+EPSZ<=sz2.p2.x) || (sz2.p2.x+EPSZ<=x && x<=sz2.p1.x))
            return true;
    }
    return false;
}

// NEM HASZNÁLT
bool metszikVagyAtlapolodnakPrec(Szakasz sz1, Szakasz sz2){
    if ((sz1.p1 == sz2.p1 && sz1.p2 == sz2.p2) || (sz1.p2 == sz2.p1 && sz1.p1 == sz2.p2))
        return true;
    double ma, ba, mb, bb;
    ma = (sz1.p1.y-sz1.p2.y)/(sz1.p1.x-sz1.p2.x);
    ba = sz1.p1.y - ma * sz1.p1.x;

    mb = (sz2.p1.y-sz2.p2.y)/(sz2.p1.x-sz2.p2.x);
    bb = sz2.p1.y - mb * sz2.p1.x;

    double x = (bb-ba)/(ma-mb);

    if (abs(ma-mb)<EPSZ2 && abs(ba-bb)<EPSZ2)
        return true;

    if ((sz1.p1.x+EPSZ<=x && x+EPSZ<=sz1.p2.x) || (sz1.p2.x+EPSZ<=x && x+EPSZ<=sz1.p1.x)){
        if ((sz2.p1.x+EPSZ<=x && x+EPSZ<=sz2.p2.x) || (sz2.p2.x+EPSZ<=x && x+EPSZ<=sz2.p1.x))
            return true;
    }
    return false;
}

/// függvény a metszésre, mert kell neki paraméter és sok helyet lehet ezzel spórolni
bool metszikEgymast(Szakasz sz1, Szakasz sz2, bool precizebb = true){
    if (!precizebb){
        double res = 0, s = 0, t = 0;
        vec2 closest2[2];
        bool Dret = false;
        DistanceSegments2(sz1.p1,sz1.p2,sz2.p1,sz2.p2,res,s,t,closest2,Dret);
        return (!Dret && res<EPSZ3);
    }
    return metszikEgymastPrec(sz1,sz2);
}

/// függvény a metszésre, mert kell neki paraméter és sok helyet lehet ezzel spórolni
vec2 metszikEgymastHol(Szakasz sz1, Szakasz sz2){

    double ma, ba, mb, bb;
    ma = (sz1.p1.y-sz1.p2.y)/(sz1.p1.x-sz1.p2.x);
    ba = sz1.p1.y - ma * sz1.p1.x;

    mb = (sz2.p1.y-sz2.p2.y)/(sz2.p1.x-sz2.p2.x);
    bb = sz2.p1.y - mb * sz2.p1.x;

    double x = (bb-ba)/(ma-mb);
    double y = ma*x+ba;
    return vec2(x,y);

    /*
    double res = 0, s = 0, t = 0;
    vec2 closest2[2];
    bool Dret = false;
    DistanceSegments2(sz1.p1,sz1.p2,sz2.p1,sz2.p2,res,s,t,closest2,Dret);
    return closest2[0];
    */
}



/// nincs megengedve az, hogy a metszésen kívül párhozamosan illeszkedjenek egmyásra, esetleg hogy ugyan azok legyenek
bool metszikVagyAtlapolodnak(Szakasz sz1, Szakasz sz2){
    return metszikVagyAtlapolodnakPrec(sz1,sz2);
    /*
    if ((sz1.p1 == sz2.p1 && sz1.p2 == sz2.p2) || (sz1.p2 == sz2.p1 && sz1.p1 == sz2.p2))
        return true;
    double res = 0, s = 0, t = 0;
    vec2 closest2[2];
    bool Dret = false;
    DistanceSegments2(sz1.p1,sz1.p2,sz2.p1,sz2.p2,res,s,t,closest2,Dret);
    return (res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) ));
    */
}





/// nem egy síkidom, külön Önlabon megvalósított osztály
struct Haromszog{
    bool torolt = false; // NEM HASZÁLT
    vec2 A, B, C; /// három csúcs
    int id; /// melyikhely tartozik

    double area() /// területét adja vissza
    {
       return abs((A.x*(B.y-C.y) + B.x*(C.y-A.y)+ C.x*(A.y-B.y))/2.0);
    }

    /// terület alapján adja meg, hogy egy pont részét képzi-e, vagy sem
    bool benneVanAPont(vec2 pont){
        vec2 csucs(pont);
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



float szakaszToDeg(Szakasz sz){
    vec2 szV = sz.p2-sz.p1; szV.normalize();
    float ret = vec2ToDeg(szV);
    return ret;
}

/// tetszőleges szakaszok halmaza, külsőleg kell rendezni a szakaszokat, hogy irányba álljanak
struct Sikidom{
    vector<Szakasz> szakaszok; /// síkidom oldalai
    bool nyilt = false; /// saját farkába harap a kígyó?
    bool belso = true; /// ezen belül vagy kívül lehet mozogni
    int szobaId = -1;

    /// megjelenítés
    void draw(SDL_Renderer &renderer, Kamera kamera, bool navMesh=false){
        int r = 255, g=0, b=0;
        if (navMesh){
            r=0; g=255;
            Haromszog harom(kamera.valosLekepezese(szakaszok[0].p1),kamera.valosLekepezese(szakaszok[0].p2),kamera.valosLekepezese(szakaszok[1].p2));
            for (int i=0; i<1; i++){
                if (harom.A.x>=0 && harom.A.x<=kamera.x && harom.A.y>=0 && harom.A.y<=kamera.y) continue;
                if (harom.B.x>=0 && harom.B.x<=kamera.x && harom.B.y>=0 && harom.B.y<=kamera.y) continue;
                if (harom.C.x>=0 && harom.C.x<=kamera.x && harom.C.y>=0 && harom.C.y<=kamera.y) continue;
                if (harom.benneVanAPont(vec2(0,0))) continue;
                if (harom.benneVanAPont(vec2(kamera.x,0))) continue;
                if (harom.benneVanAPont(vec2(0,kamera.y))) continue;
                if (harom.benneVanAPont(vec2(kamera.x,kamera.y))) continue;
                Szakasz a(vec2(0,0),vec2(kamera.x,0));
                Szakasz b(vec2(0,0),vec2(0,kamera.y));
                Szakasz c(vec2(0,kamera.y),vec2(kamera.x,kamera.y));
                Szakasz d(vec2(kamera.x,0),vec2(kamera.x,kamera.y));
                Szakasz A(harom.A,harom.B);
                Szakasz B(harom.A,harom.C);
                Szakasz C(harom.C,harom.B);
                if (metszikEgymast(A,a) || metszikEgymast(A,b,false) || metszikEgymast(A,c) || metszikEgymast(A,d,false)) continue;
                if (metszikEgymast(B,a) || metszikEgymast(B,b,false) || metszikEgymast(B,c) || metszikEgymast(B,d,false)) continue;
                if (metszikEgymast(C,a) || metszikEgymast(C,b,false) || metszikEgymast(C,c) || metszikEgymast(C,d,false)) continue;

                return;
            }
            filledTrigonRGBA(&renderer,
                kamera.valosLekepezese(szakaszok[0].p1).x,kamera.valosLekepezese(szakaszok[0].p1).y,
                kamera.valosLekepezese(szakaszok[0].p2).x,kamera.valosLekepezese(szakaszok[0].p2).y,
                kamera.valosLekepezese(szakaszok[1].p2).x,kamera.valosLekepezese(szakaszok[1].p2).y,
                       g,b,r,255);

        }
        for (size_t i=0; i<szakaszok.size(); i++){
            lineRGBA(&renderer,
                kamera.valosLekepezese(szakaszok[i].p1).x,kamera.valosLekepezese(szakaszok[i].p1).y,
                kamera.valosLekepezese(szakaszok[i].p2).x,kamera.valosLekepezese(szakaszok[i].p2).y,
                     g,g,b,255);
        }
        /*
        for (int i=0; i<szakaszok.size(); i++){
            filledCircleRGBA(&renderer,
                kamera.valosLekepezese(szakaszok[i].p1).x,kamera.valosLekepezese(szakaszok[i].p1).y,
                             5,r,g,130,255);
        }
        */
    }



    /// belső vagy az összes átló kiszámítása 2 (szögekkel)
    vector<Szakasz> belsoAtlok(bool osszes=false){
        //cout<<"H"<<endl;
        vector<Szakasz> atlok;  /// ebbe kerulnek majd bele az atlok
        if (nyilt){   /// ha nyilt, vagy nem a belso a fontos, akkor nem kell foglalkozni vele
            return atlok;
        }
        vector<Szakasz> oldalak = szakaszok;
        vector<Szakasz> atlokEsOldalak = oldalak;   /// ezekkel nem szabad metszeni magunkat
        vector<vec2> csucsok;                       /// miket akarunk osszekottetni
        ///cout<<"OLDALAK"<<endl;
        for (size_t i=0; i<oldalak.size(); i++){
            ///cout<<oldalak[i].p1.x<<" "<<oldalak[i].p1.y<<", "<<oldalak[i].p2.x<<" "<<oldalak[i].p2.y<<" "<<endl;
            csucsok.push_back(oldalak[i].p1);
        }
        for (size_t i=0; i<csucsok.size(); i++){
            for (size_t j=i; j<csucsok.size(); j++){   /// megnézünk minden csúcspárost
                if (i==j)
                    continue;
                bool siker = true;
                Szakasz sz(csucsok[i],csucsok[j]);  /// szakaszt képzünk belőlük
                for (size_t k=0; k<atlokEsOldalak.size(); k++){
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
        for (size_t i=0; i<atlokEsOldalak.size(); i++){    /// megnézem, hogy végül kik lettek összekötve
            int p1, p2; /// csúcsok sorszáma
            for (size_t j=0; j<csucsok.size(); j++){ /// összes csúcsot végignézve
                if (atlokEsOldalak[i].p1==csucsok[j]) /// ha egyezés van
                    p1=j;   /// akkor a sorszám meg is van
                if (atlokEsOldalak[i].p2==csucsok[j]) /// itt
                    p2=j;   /// szintúgy
            }
            szomszedCsucsIdx[p1].push_back(p2); /// majd a szomszédságot eltárolom
            szomszedCsucsIdx[p2].push_back(p1); /// költsönüsen
        }

        ///bool stop = false; /// ha baj van, álljon meg
        ///size_t cnt=0;  // CSAK DEBUG, hogy hányszor fut le
        if (DEBUG2) cout<<"atlokBefore: "<<atlok.size()<<endl;
        set<Szakasz> mentettOsszesAtlo(atlok.begin(),atlok.end()); /// ennyi az osszes egymást nem metsző átló
        if (osszes)
            return atlok;


        for (size_t i=0; i<szakaszok.size(); i++){
            Szakasz o1 = szakaszok[i];
            Szakasz o2 = szakaszok[(i+1)%szakaszok.size()];
            float minDeg = szakaszToDeg(o1.inv());
            float maxDeg = szakaszToDeg(o2);
            bool rotapota = false;
            if (minDeg>maxDeg){
                rotapota = true;
            }

            for (int j=0; j<(int)atlok.size(); j++){
                Szakasz atlo = atlok[j];
                if (atlo.p2 == o2.p1){
                    atlo.p2 = atlo.p1;
                    atlo.p1 = o2.p1;
                }
                if (atlo.p1==o2.p1){
                    if (!rotapota){
                        float k = vec2ToDeg(atlo.p2-atlo.p1);
                        if (k>minDeg+EPSZ && k+EPSZ<maxDeg){
                            /// OK
                        } else {
                            atlok.erase(atlok.begin()+j);
                            j--;
                            continue;
                        }
                    } else {
                        float k = vec2ToDeg(atlo.p2-atlo.p1);
                        if (k>minDeg-EPSZ || k-EPSZ<maxDeg){
                            /// OK
                        } else {
                            atlok.erase(atlok.begin()+j);
                            j--;
                            continue;
                        }
                    }
                }
            }
        }

        return atlok;
    }

    /// belső vagy a külső átlókért felel
    vector<Szakasz> belsoAtlok3(bool osszes=false){
        vector<Szakasz> atlok;  /// ebbe kerulnek majd bele az atlok
        if (nyilt){   /// ha nyilt, vagy nem a belso a fontos, akkor nem kell foglalkozni vele
            return atlok;
        }
        vector<Szakasz> oldalak = szakaszok;
        vector<Szakasz> atlokEsOldalak = oldalak;   /// ezekkel nem szabad metszeni magunkat
        vector<vec2> csucsok;                       /// miket akarunk osszekottetni
        ///cout<<"OLDALAK"<<endl;
        for (size_t i=0; i<oldalak.size(); i++){
            ///cout<<oldalak[i].p1.x<<" "<<oldalak[i].p1.y<<", "<<oldalak[i].p2.x<<" "<<oldalak[i].p2.y<<" "<<endl;
            csucsok.push_back(oldalak[i].p1);
        }
        for (size_t i=0; i<csucsok.size(); i++){
            for (size_t j=i; j<csucsok.size(); j++){   /// megnézünk minden csúcspárost
                if (i==j)
                    continue;
                bool siker = true;
                Szakasz sz(csucsok[i],csucsok[j]);  /// szakaszt képzünk belőlük
                for (size_t k=0; k<atlokEsOldalak.size(); k++){
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
        for (size_t i=0; i<atlokEsOldalak.size(); i++){    /// megnézem, hogy végül kik lettek összekötve
            int p1, p2; /// csúcsok sorszáma
            for (size_t j=0; j<csucsok.size(); j++){ /// összes csúcsot végignézve
                if (atlokEsOldalak[i].p1==csucsok[j]) /// ha egyezés van
                    p1=j;   /// akkor a sorszám meg is van
                if (atlokEsOldalak[i].p2==csucsok[j]) /// itt
                    p2=j;   /// szintúgy
            }
            szomszedCsucsIdx[p1].push_back(p2); /// majd a szomszédságot eltárolom
            szomszedCsucsIdx[p2].push_back(p1); /// költsönüsen
        }

        bool stop = false; /// ha baj van, álljon meg
        int cnt=0;  // CSAK DEBUG, hogy hányszor fut le
        if (DEBUG2) cout<<"atlokBefore: "<<atlok.size()<<endl;
        set<Szakasz> mentettOsszesAtlo(atlok.begin(),atlok.end()); /// ennyi az osszes egymást nem metsző átló
        if (osszes)
            return atlok;
        while(!stop){ /// amíg nics baj
            // CSAK DEBUG
            for (size_t i=0; i<szomszedCsucsIdx.size() && DEBUG; i++){
                cout<<"Szomszedok: ";
                for (size_t j=0; j<szomszedCsucsIdx[i].size(); j++){
                    cout<<szomszedCsucsIdx[i][j]<<", ";
                }
                cout<<endl;
            }
            cnt++;
            // EDDIG

            stop=true; /// elvileg nincs baj, lesz majd?
            for (int i=0; i<csucsok.size(); i++){ /// minden csúcsot
                for (int j=i+1; j<csucsok.size(); j++){ /// minden azt követő csúccsal
                    //if (j==i) /// minden *másik csúccsal
                      //  continue;
                    bool szomszedok = false; /// szomszédok?
                    for (int k=0; k<szomszedCsucsIdx[i].size(); k++){ /// szomszádsági listában szerepel?
                        if (szomszedCsucsIdx[i][k]==j) /// ha igen
                            szomszedok=true; /// akkor igen
                    }
                    if (!szomszedok) /// ha nem szomszédok
                        continue; /// akkor nem számít
                    int a = -1, b = -1; /// szerepel-e kétszer is?
                    int aIdxK=-1, aIdxL=-1, bIdxK=-1, bIdxL=-1;
                    for (int k=0; k<szomszedCsucsIdx[i].size(); k++){ /// minden szomszédján végigmegyünk
                        for (int l=0; l<szomszedCsucsIdx[j].size(); l++){ /// mind a két csúcsnak a szomszédjain
                            if (szomszedCsucsIdx[i][k]==szomszedCsucsIdx[j][l]){ /// ha közös szomszédjuk van
                                if (DEBUG) cout<<"kozos szomszed: "<<i<<" "<<j<<" -> "<<szomszedCsucsIdx[i][k]<<endl;
                                /// először fontos, hogy ne kebelezzen be másik csúcsot
                                int cntSz = 0; /// 3-ig szabad csak elszámolni
                                Haromszog hrs(csucsok[i],csucsok[j],csucsok[szomszedCsucsIdx[i][k]]); /// ellenőrző háromszög
                                for (int z = 0; z<csucsok.size(); z++){ /// minden csúcson végigmegy
                                    if (hrs.benneVanAPont(csucsok[z])) /// ha benne van
                                        cntSz++; /// az számít
                                    if (cntSz>3) /// ha több lett, mint 3
                                        break; /// akkor megvagyunk
                                }
                                if (cntSz>3) /// ha van más csúcs is, mint a háromszög 3 csúcsa
                                    continue; /// akkor az nem jó
                                if (a==-1){ /// ha még nincs közös csúcsuk
                                    a=szomszedCsucsIdx[i][k]; /// akkor eltároljuk az index-ét
                                    aIdxK = k; /// és szomszédsági indexét is
                                    aIdxL = l; /// mind a két csúcsnál
                                }
                                else if (b==-1){ /// ha már egy közös szomszédjuk volt, akkor ez amásodik lesz
                                    b=szomszedCsucsIdx[i][k]; /// eltároljuk, hogy van második közös szomszédjuk is
                                    bIdxK = k; // NEM HASZNÁLT
                                    bIdxL = l; // NEM HASZNÁLT
                                }
                                else{} // DEBUG, NE FORDULJON ELŐ, mert egy szakazhoz csak 2 háromszög tartozhat
                                    //cout<<"PERROR"<<endl;
                            }
                        }
                    }
                    if (a*b<0){ /// csles feltétel, ha CSAK 1 közös szomszédjuk van
                        bool oldal=false; /// a síkidom oldala?
                        for (int z=0; z<oldalak.size(); z++){ /// nézzük meg az összeset
                            if ((oldalak[z].p1==csucsok[i] && oldalak[z].p2==csucsok[j]) || (oldalak[z].p2==csucsok[i] && oldalak[z].p1==csucsok[j])){
                                oldal=true; /// ha a csúcsaik megfeleltethetőek, akkor oldal
                                break;
                            }
                        }
                        if (oldal && DEBUG)
                            cout<<"oldal: "<<i<<" "<<j<<endl;
                        if (!oldal){ /// ha ez nem oldal
                            if (DEBUG) cout<<"NEM oldal: "<<i<<" "<<j<<endl;
                            stop=false; /// akkor ez nem szimpatikus, és próbálom helyre állítani a rendet
                            // NEM HASZNÁLT
                            for (int k=0; k<szomszedCsucsIdx[a].size() && DEBUG;k++){ /// szomszédságokat végignézem
                                if (szomszedCsucsIdx[a][k]==i || szomszedCsucsIdx[a][k]==j){
                                    //szomszedCsucsIdx[a].erase(szomszedCsucsIdx[a].begin()+k);
                                    //k--;
                                }
                            }
                            int cnt2 = 0; // DEBUG
                            for (int k=0; k<atlok.size(); k++){ /// végignézem az átlókat
                                if ((atlok[k].p1==csucsok[i] && atlok[k].p2==csucsok[j]) || (atlok[k].p2==csucsok[i] && atlok[k].p1==csucsok[j])){
                                    atlok.erase(atlok.begin()+k); /// ha stimmel az oldal, akkor törlésre kerül sor
                                    k--; /// szükséges ilyenkor visszalépni
                                    cnt2++; // DEBUG
                                    //break;
                                }
                            }
                            if (DEBUG) cout<<"CNT2: "<<cnt2<<endl;
                            if (DEBUG) cout<<"torol: "<<i<<" "<<j<<endl;
                            /// törlöm a szomszédságukat kölcsönösen
                            szomszedCsucsIdx[i].erase(std::remove(szomszedCsucsIdx[i].begin(), szomszedCsucsIdx[i].end(), j), szomszedCsucsIdx[i].end());
                            szomszedCsucsIdx[j].erase(std::remove(szomszedCsucsIdx[j].begin(), szomszedCsucsIdx[j].end(), i), szomszedCsucsIdx[j].end());
                        }
                    }
                }
            }
        }
        // CSAK DEBUG
        for (size_t i=0; i<szomszedCsucsIdx.size() && DEBUG; i++){
            cout<<"Szomszedok: ";
            for (size_t j=0; j<szomszedCsucsIdx[i].size(); j++){
                cout<<szomszedCsucsIdx[i][j]<<", ";
            }
            cout<<endl;
        }
        if (DEBUG) cout<<"CNT: "<<cnt<<endl;
        if (DEBUG) cout<<"atlok: "<<atlok.size()<<endl;
        /// komplementere kell az eredmények, ha belülről járható be a síkidom
        if (belso){
            for (size_t i=0; i<atlok.size(); i++){ /// kivonom az összes átlót az összesből
                if (mentettOsszesAtlo.find(atlok[i])!=mentettOsszesAtlo.end()){
                    mentettOsszesAtlo.erase(mentettOsszesAtlo.find(atlok[i]));
                }
            }
            /// majd végül visszamásolom a jó megoldást egy clear után
            atlok.clear();
            atlok.assign(mentettOsszesAtlo.begin(),mentettOsszesAtlo.end());
        }
        return atlok;
    }
};


struct Palya{
    float sizeX = 400, sizeY = 400;
    float posX = 0, posY = 0;
    int sok = 0;
    vector<Sikidom> sikidomok; /// határoló síkidomok
    vector<Sikidom> navMesh;  /// bejárható tér
    vector<vector<Szakasz>> belsoAtlok; /// síkidomok belső átlói
    vector<vector<Szakasz>> kulsoAtlok;
    vector<Szakasz> joBelso;
    vector<Szakasz> BAszakasz; /// számításhoz szükséges szakaszok
    vector<Szakasz> szakaszBA; /// számításhoz szükséges szakaszok

    Palya(bool ures = true){
        if (ures)
            return;

        Sikidom temp;
        temp.szakaszok.push_back(Szakasz(vec2(200,100),vec2(300,100)));
        temp.szakaszok.push_back(Szakasz(vec2(300,100),vec2(300,200)));
        temp.szakaszok.push_back(Szakasz(vec2(300,200),vec2(400,200)));
        temp.szakaszok.push_back(Szakasz(vec2(400,200),vec2(400,300)));
        temp.szakaszok.push_back(Szakasz(vec2(400,300),vec2(300,300)));
        temp.szakaszok.push_back(Szakasz(vec2(300,300),vec2(300,400)));
        temp.szakaszok.push_back(Szakasz(vec2(300,400),vec2(200,400)));
        temp.szakaszok.push_back(Szakasz(vec2(200,400),vec2(200,300)));
        temp.szakaszok.push_back(Szakasz(vec2(200,300),vec2(100,300)));
        temp.szakaszok.push_back(Szakasz(vec2(100,300),vec2(100,200)));
        temp.szakaszok.push_back(Szakasz(vec2(100,200),vec2(200,200)));
        temp.szakaszok.push_back(Szakasz(vec2(200,200),vec2(200,100)));
        temp.belso=false;
        sikidomok.push_back(temp);
        /*
        temp.szakaszok.push_back(Szakasz(vec2(0,0),vec2(0,400)));
        temp.szakaszok.push_back(Szakasz(vec2(0,400),vec2(400,400)));
        temp.szakaszok.push_back(Szakasz(vec2(400,400),vec2(400,100)));
        temp.szakaszok.push_back(Szakasz(vec2(400,100),vec2(300,100)));
        temp.szakaszok.push_back(Szakasz(vec2(300,100),vec2(300,50)));
        temp.szakaszok.push_back(Szakasz(vec2(300,50),vec2(380,60)));
        temp.szakaszok.push_back(Szakasz(vec2(380,60),vec2(300,0)));
        temp.szakaszok.push_back(Szakasz(vec2(300,0),vec2(0,0)));

        sikidomok.push_back(temp);
        temp.belso=false;

        vector<vec2> kbPontok;

        kbPontok.push_back(vec2(100,100));
        kbPontok.push_back(vec2(300,300));
        kbPontok.push_back(vec2(300,100));
        kbPontok.push_back(vec2(100,300));
        kbPontok.push_back(vec2(200,200));
        kbPontok.push_back(vec2(200,100));
        kbPontok.push_back(vec2(100,200));
        kbPontok.push_back(vec2(200,300));
        kbPontok.push_back(vec2(300,200));

        if (RANDOM)
            srand(time(NULL));
        //srand(39);
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

            temp.szakaszok.push_back(Szakasz(temp.szakaszok[temp.szakaszok.size()-1].p2,temp.szakaszok[0].p1));
            sikidomok.push_back(temp);

        }

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
        */
    }

    void bakeNavMesh2(){
        bakeAtloNavMesh();
        //bakeKulsoNavMesh2();
        vector<Szakasz> nemMetszheto;
        for (size_t i=0; i<belsoAtlok.size(); i++){
            nemMetszheto.insert(nemMetszheto.end(),belsoAtlok[i].begin(),belsoAtlok[i].end());
        }
        vector<vec2> csucsok;
        for (size_t i=0; i<sikidomok.size(); i++){
            nemMetszheto.insert(nemMetszheto.end(),sikidomok[i].szakaszok.begin(),sikidomok[i].szakaszok.end());
            for (size_t j=0; j<sikidomok[i].szakaszok.size(); j++){
                csucsok.push_back(sikidomok[i].szakaszok[j].p1);
            }
        }
        vector<Szakasz> joBelsoAtlok;
        for (size_t i=0; i<csucsok.size(); i++){
            for (size_t j=0; j<csucsok.size(); j++){
                if (i==j)
                    continue;
                Szakasz sz(csucsok[i],csucsok[j]);
                bool baj = false;
                for (size_t k=0; k<nemMetszheto.size(); k++){
                    if (nemMetszheto[k]==sz || metszikEgymast(nemMetszheto[k],sz)){
                        baj = true;
                        break;
                    }
                }
                if (baj)
                    continue;
                nemMetszheto.push_back(sz);
                joBelsoAtlok.push_back(sz);
            }
        }
        //set<Szakasz> tempBelso;
        //copy(joBelsoAtlok.begin(),joBelsoAtlok.end(),inserter(tempBelso, tempBelso.end()));
        //joBelsoAtlok.clear();
        //joBelsoAtlok.assign(tempBelso.begin(),tempBelso.end());
        joBelso=joBelsoAtlok;


        joBelsoAtlok.insert(joBelsoAtlok.end(),joBelso.begin(),joBelso.end());
        for (int i=0; i<sikidomok.size(); i++){
            joBelsoAtlok.insert(joBelsoAtlok.end(),sikidomok[i].szakaszok.begin(),sikidomok[i].szakaszok.end());
        }
        //joBelso=joBelsoAtlok;
        int check = INT_MAX;
        //return;
        while (joBelsoAtlok.size()!=0 && check != joBelsoAtlok.size()){
            //cout<<joBelsoAtlok.size()<<endl;
            check=joBelsoAtlok.size();
            bool ok = true;
            for (int i=0; i<joBelsoAtlok.size() && ok;i++){
                for (int j=i+1; j<joBelsoAtlok.size() && ok;j++){
                    for (int k=j+1; k<joBelsoAtlok.size() && ok;k++){
                        if ((joBelsoAtlok[i]==joBelsoAtlok[j] || joBelsoAtlok[i]==joBelsoAtlok[k] || joBelsoAtlok[k]==joBelsoAtlok[j])){
                            continue;
                        }
                        set<vec2> harom;
                        harom.insert(joBelsoAtlok[i].p1);
                        harom.insert(joBelsoAtlok[i].p2);
                        harom.insert(joBelsoAtlok[j].p1);
                        harom.insert(joBelsoAtlok[j].p2);
                        harom.insert(joBelsoAtlok[k].p1);
                        harom.insert(joBelsoAtlok[k].p2);
                        if (harom.size()==3){
                            vector<vec2> cs; cs.assign(harom.begin(),harom.end());
                            Sikidom s;
                            s.szakaszok.push_back(Szakasz(cs[0],cs[1]));
                            s.szakaszok.push_back(Szakasz(cs[1],cs[2]));
                            s.szakaszok.push_back(Szakasz(cs[2],cs[0]));
                            bool marVan = false;
                            for (int l=0; l<navMesh.size(); l++){
                                bool a = false, b = false, c = false;
                                Szakasz temp = navMesh[l].szakaszok[0];
                                if (!(temp==joBelsoAtlok[i] || temp==joBelsoAtlok[j] || temp==joBelsoAtlok[k]))
                                    continue;
                                temp = navMesh[l].szakaszok[1];
                                if (!(temp==joBelsoAtlok[i] || temp==joBelsoAtlok[j] || temp==joBelsoAtlok[k]))
                                    continue;
                                temp = navMesh[l].szakaszok[2];
                                if (!(temp==joBelsoAtlok[i] || temp==joBelsoAtlok[j] || temp==joBelsoAtlok[k]))
                                    continue;
                                marVan=true;
                                break;
                            }
                            if (marVan)
                                continue;
                            Haromszog har(cs[0],cs[1],cs[2]);
                            int csCnt = 0;
                            for (int i=0; i<csucsok.size(); i++){
                                if (har.benneVanAPont(csucsok[i])){
                                    csCnt++;
                                    if (csCnt>3)
                                        break;
                                }
                            }
                            if (csCnt>3)
                                continue;
                            /*
                            for (int l=0; l<3; l++){
                                if (s.szakaszok[l]==joBelsoAtlok[i] || s.szakaszok[l]==joBelsoAtlok[j] || s.szakaszok[l]==joBelsoAtlok[k]){

                                } else {
                                    continue;
                                }
                            }
                            */
                            navMesh.push_back(s);

                            joBelsoAtlok.erase(joBelsoAtlok.begin()+i);
                            joBelsoAtlok.erase(joBelsoAtlok.begin()+j-1);
                            joBelsoAtlok.erase(joBelsoAtlok.begin()+k-2);
                            ok=false;
                            break;
                        }
                    }
                }
            }
        }
        //navMesh.clear();
        joBelso=joBelsoAtlok;
        /*
        set<Szakasz> tempBelso;
        copy(joBelso.begin(),joBelso.end(),inserter(tempBelso, tempBelso.end()));
        for (int i=0; i<navMesh.size(); i++){
            for (int j=0; j<navMesh[i].szakaszok.size();j++){
                if (tempBelso.find(navMesh[i].szakaszok[j]) != tempBelso.end()){
                    tempBelso.erase(tempBelso.find(navMesh[i].szakaszok[j]));
                }
            }
        }
        joBelso.clear();
        joBelso.assign(tempBelso.begin(),tempBelso.end());
        navMesh.clear();
        */
    }

    void bakeNavMesh(){
        /// minden síkidom oldalát felhasználva próbálok képezni egy-egy háromszöget valamelyik másik síkidom egyik csúcsával
        vector<vector<vec2>> halmaz; /// eltárolom a síkidomok csúcsait síkidomonként
        vector<vector<bool>> sikeresSzakaszok; /// és az, hogy összejöttek-e, mint szakaszok
        vector<Szakasz> szakaszok;  /// eltárolom az összes oldalát a síkidomoknak
        for (int i=0; i<sikidomok.size(); i++){ /// végigmegyek az összes síkidomon
            vector<vec2> temp;  /// ebbe gyűjtöm az aktuális síkidom csúcsait
            vector<bool> tempB(sikidomok[i].szakaszok.size(),false);    /// minden csúcsa hamis ???
            if (sikidomok[i].nyilt) /// ha nyílt a síkidom (jelenleg nincs olyan, és nem is lesz)
                temp.push_back(sikidomok[i].szakaszok[0].p1); /// akkor a kezdeti pontot is hozzáadjuk
            for (int j=0; j<sikidomok[i].szakaszok.size(); j++){ /// különben elegendő minden szakasz végpontja
                temp.push_back(sikidomok[i].szakaszok[j].p2); /// a végpontja a p2
                szakaszok.push_back(sikidomok[i].szakaszok[j]); /// és a szakaszokban eltárolom ezt az oldalt is
            }
            sikeresSzakaszok.push_back(tempB);  /// ???
            halmaz.push_back(temp); /// majd hozzácsapom a többi síkidomhoz
        }
        /// az összes síkidomon az átlók összegyűjtése okán
        vector<Szakasz> belsoAtlokTemp; /// ebbe tárolom el az összes átlót
        for (int i=0; i<sikidomok.size(); i++){ /// végigmegyek a síkidomokon
            vector<Szakasz> temp = sikidomok[i].belsoAtlok();   /// lekérem az átlóikat
            belsoAtlokTemp.insert(belsoAtlokTemp.end(), temp.begin(), temp.end()); /// majd hozzátoldom a meglévőkhöz
        }
        if (DEBUG2) cout<<"belsoAtlokTemp "<<belsoAtlokTemp.size()<<endl; /// DEBUG
        if (DEBUG2) cout<<"szakaszok.size()"<<szakaszok.size()<<endl;
        if (DEBUG2) cout<<halmaz.size()<<endl;

        for (int j=0; j<halmaz.size(); j++){ /// végigmegyek az összes síkidomon
            if (DEBUG) cout<<"j: "<<j<<endl;
            for (int i=0; i<halmaz[j].size(); i++){ /// és azoknak minden csúcsain
                if (DEBUG) cout<<"i: "<<i<<endl;
                vec2 p1 = halmaz[j][i]; /// és veszem az indexel csúcsot
                int tidx = (i+1)%halmaz[j].size(); /// és az azt követőt, vagy az elsőt, ha az utolsóhoz értünk
                //if (i+1>=halmaz[j].size())
                    //tidx=0;
                vec2 p2 = halmaz[j][tidx]; /// el is tárolom
                for (int k=0; k<halmaz.size(); k++){ /// újra végigmegyek a síkidomokon
                    if (k==j) /// nicns saját magával történõ háromszögezés
                        continue;
                    for (int l=0; l<halmaz[k].size(); l++){ /// végigmegyek a csúcsain megint
                        bool siker = true;  /// ha minden rendben megy
                        vec2 p3 = halmaz[k][l]; /// a lehetséges 3. csúcs a háromszöghöz
                        Szakasz sz1(p1,p3); /// van az eredeti szakaszunk, ami az oldal, és van két új szakaszunk
                        Szakasz sz2(p2,p3); /// ezeket fogom metszés szempontjából megvizsgálni
                        /// EZ NEM SZUKSEGES, de kellhet, mert gyorsabb mint a másik, viszont pontatlanabb
                        /// *pontatlan - nem ad hibás eredményt, csak kevesebb jót maximum
                        /// 51 ms helyett 14 ms lesz a generálás egyes esetekben (szükséges az "&& siker")
                        for (int z=0; z<belsoAtlokTemp.size() && siker; z++){ /// végigmegyek az összes átlón
                            if (metszikVagyAtlapolodnak(sz1,belsoAtlokTemp[z])){ /// ha metszést produkálnak
                                if (DEBUG) cout<<"SIKER, JIPPI1"<<endl; /// akkor baj van
                                siker=false;    /// és nem lesz jó háromszögenk
                                break;
                            }
                            if (metszikVagyAtlapolodnak(sz2,belsoAtlokTemp[z])){ /// ha metszést produkál a másik szakasz
                                if (DEBUG) cout<<"SIKER, JIPPI2"<<endl;/// akkor is baj van
                                siker=false; /// és szintén nem lesz jó háromszögenk
                                break;
                            }
                        }
                        /// ha még nicns elenvetés
                        for (int z=0; z<szakaszok.size() && siker; z++){ /// akkor végigmegyek az összes tiltott szakszon, oldalakon és átlókon is
                            double res = 0, s = 0, t = 0;   /// metszéshez szükséges paraméterek a visszatéréshez ( van már rá függvény) !!!
                            vec2 closest2[2];   /// szintúgy
                            bool Dret = false; /// szintúgy

                            //DistanceSegments2(sz1.p1,sz1.p2,szakaszok[z].p1,szakaszok[z].p2,res,s,t,closest2,Dret); /// mag a metszés keresése
                            //if (res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) )){ /// ha viszonylag metszéspontnak mondható
                            if (metszikEgymast(sz1,szakaszok[z])){
                                if (DEBUG) cout<<"BAJOS1"<<endl;
                                if (DEBUG) cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
                                if (DEBUG) cout<<"i: "<<i<<", j: "<<j<<endl;
                                if (DEBUG) cout<<p1.x<<" "<<p1.y<<", "<<p3.x<<" "<<p3.y<<endl;
                                if (DEBUG) cout<<szakaszok[z].p1.x<<" "<<szakaszok[z].p1.y<<", "<<szakaszok[z].p2.x<<" "<<szakaszok[z].p2.y<<endl;
                                siker=false;    /// akkor nem jó
                                continue;
                            }
                            //if ()
                            //DistanceSegments2(sz2.p1,sz2.p2,szakaszok[z].p1,szakaszok[z].p2,res,s,t,closest2,Dret); /// mag a metszés keresése a másik szakaszon
                            //if (res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) )){ /// ha viszonylag metszéspontnak mondható
                            if (metszikEgymast(sz2,szakaszok[z])){
                                if (DEBUG) cout<<"BAJOS2"<<endl;
                                if (DEBUG) cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
                                if (DEBUG) cout<<"i: "<<i<<", j: "<<j<<endl;
                                if (DEBUG) cout<<p2.x<<" "<<p2.y<<", "<<p3.x<<" "<<p3.y<<endl;
                                if (DEBUG) cout<<szakaszok[z].p1.x<<" "<<szakaszok[z].p1.y<<", "<<szakaszok[z].p2.x<<" "<<szakaszok[z].p2.y<<endl;
                                siker=false; /// akkor nem jó
                                continue;
                            }
                            Haromszog haromszog(p1,p2,p3); /// létrehozok egy háromszöget külön, spécibb mint egy síkidom
                            bool ures = true;   /// ha háromszög egy csúcsot sem tartalmaz (olyat tilos)
                            for (int zz=0; zz<halmaz.size() && ures && siker; zz++){ /// végigmegyek az összes síkidom
                                for (int zzz=0; zzz<halmaz[zz].size() && ures; zzz++){ /// összes csúcsán
                                    if ((zz==j && (i==zzz || tidx==zzz)) || (zz==k && zzz==l)) /// ha valamelyikkel egyezik, az nem számít
                                        continue;
                                    if (haromszog.benneVanAPont(halmaz[zz][zzz])){ /// ellenkező esetben nézze meg, hogy részét képzi-e
                                        ures = false;   /// akkor nem üres
                                    }
                                }
                            }
                            if (!ures){ /// ha nem üres
                                if (DEBUG) cout<<"NEM URES"<<endl;
                                siker=false; /// az baj
                                continue;
                            }
                            //siker=true;
                            //l=INT_MAX;
                            //k=halmaz.size()-1;
                            //z=INT_MAX;
                            //cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
                        }
                        if (siker){ /// ha kiállta a próbákat
                            if (DEBUG) cout<<"SIKER"<<endl;
                            Sikidom tempS; /// akkor létrehozok egy új síkidomot (ami tikon háromszög lehet csak)
                            tempS.szakaszok.push_back(Szakasz(p1,p2));  /// hozzáadom a szakaszokat körbejárásnak megfelelően
                            tempS.szakaszok.push_back(Szakasz(p2,p3));
                            tempS.szakaszok.push_back(Szakasz(p3,p1));
                            szakaszok.push_back(tempS.szakaszok[1]);    /// majd az eddig meglévő oldalakhoz hozzáadom az egyik szakaszt
                            szakaszok.push_back(tempS.szakaszok[2]);    /// és a másikat is
                            if (DEBUG) cout<<szakaszok.size()<<endl;
                            navMesh.push_back(tempS);   /// majd eltárolom (ezek csak háromszögek lehetnek)
                            sikeresSzakaszok[j][i]=true; /// és ez egy sikeres szakasz (azaz sikerült a síkidom oldalához találni háromszöget)
                        }
                    }
                }
            }
        }

        if (DEBUG2) cout<<"EDDIG JO"<<endl;
        /// rengeteg lehetséges háromszög maradhatott hátra, például ha egy sokszög önmagával kelljen képeznie háromszöget
        for (int j=0; j<halmaz.size(); j++){ /// végigmegyek a síkidomokon
            for (int i=0; i<halmaz[j].size(); i++){ /// azok csúcsain
                if (sikeresSzakaszok[j][i]) /// ha már a szakaszt megoldottam
                    continue; /// akkor jöhet a következő
                vec2 p1 = halmaz[j][i]; /// ez egy új csúcs
                int tidx = (i+1)%halmaz[j].size(); /// ez meg a következő a síkidomban
                //if (i+1>=halmaz[j].size())
                //    tidx=0;
                vec2 p2 = halmaz[j][tidx]; /// el is tárolom
                for (int k=0; k<halmaz[j].size(); k++){ /// végig megyek az összes csúcsán a síkidomnak
                    if (k==i || k==tidx) /// ha az előzőek valamelyike
                        continue; /// az nem jó
                    bool siker = true; /// nézzük meg, hogy elbukik-e valamin
                    vec2 p3 = halmaz[j][k]; /// ehhez kell ez a harmadik csúcs is
                    Szakasz sz1(p1,p3); /// és az általa képzett két szakasz
                    Szakasz sz2(p2,p3); /// két szakasz
                    /// megnézem a belső átlóit és az öszes eddigi szakaszt (utóbbiakkal lehet párhuzamos, és ugyan az is (szomszédosak a háromszögek))
                    /// "kódduplikéció"
                    for (int z=0; z<belsoAtlokTemp.size() && siker; z++){
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

                    for (int z=0; z<szakaszok.size() && siker; z++){
                        double res = 0, s = 0, t = 0;
                        //vec2 p0(sz1.p1.x,sz1.p2.y), p1(2.0f, 1.0f), q0(0.0f, 1.0f), q1(2.0f, 1.0f);
                        vec2 closest2[2];
                        bool Dret = false;
                        //DistanceSegments2(sz1.p1,sz1.p2,szakaszok[z].p1,szakaszok[z].p2,res,s,t,closest2,Dret);
                        //if (!Dret && res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) )){
                        if (metszikEgymast(sz1,szakaszok[z])){
                            if (DEBUG) cout<<"BAJOS1"<<endl;
                            if (DEBUG) cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
                            if (DEBUG) cout<<"i: "<<i<<", j: "<<j<<endl;
                            if (DEBUG) cout<<p1.x<<" "<<p1.y<<", "<<p3.x<<" "<<p3.y<<endl;
                            if (DEBUG) cout<<szakaszok[z].p1.x<<" "<<szakaszok[z].p1.y<<", "<<szakaszok[z].p2.x<<" "<<szakaszok[z].p2.y<<endl;
                            siker=false;
                            continue;
                        }
                        //if ()
                        //DistanceSegments2(sz2.p1,sz2.p2,szakaszok[z].p1,szakaszok[z].p2,res,s,t,closest2,Dret);
                        //if (!Dret && res<EPSZ && ((t>EPSZ && t<1.0f-EPSZ) || (s>EPSZ && s<1.0f-EPSZ) )){
                        if (metszikEgymast(sz2,szakaszok[z])){
                            if (DEBUG) cout<<"BAJOS2"<<endl;
                            if (DEBUG) cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
                            if (DEBUG) cout<<"i: "<<i<<", j: "<<j<<endl;
                            if (DEBUG) cout<<p2.x<<" "<<p2.y<<", "<<p3.x<<" "<<p3.y<<endl;
                            if (DEBUG) cout<<szakaszok[z].p1.x<<" "<<szakaszok[z].p1.y<<", "<<szakaszok[z].p2.x<<" "<<szakaszok[z].p2.y<<endl;
                            siker=false;
                            continue;
                        }
                        Haromszog haromszog(p1,p2,p3);
                        bool ures = true;
                        for (int zz=0; zz<halmaz.size() && ures; zz++){
                            if (DEBUG) cout<<"zz: "<<zz<<endl;
                            for (int zzz=0; zzz<halmaz[zz].size() && ures; zzz++){
                                if (DEBUG) cout<<"zzz: "<<zzz<<endl;
                                if ((zz==j && (i==zzz || tidx==zzz)) || (zz==j && zzz==k))
                                    continue;

                                if (DEBUG) cout<<"banan"<<endl;
                                if (haromszog.benneVanAPont(halmaz[zz][zzz])){
                                    ures = false;
                                }
                            }
                        }
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

                    /// eddig, és a következő is kicsit
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
                        belsoAtlokTemp.push_back(tempS.szakaszok[0]);   /// fontos különbség, mert az oldal így most már el lesz könyvelve
                        /// hogyha nem lenne, akkor a síkidom felhasznált két oldalából kétszer is leképeznénk ugyan azt a háromszöget
                        szakaszok.push_back(tempS.szakaszok[1]);
                        szakaszok.push_back(tempS.szakaszok[2]);
                        if (DEBUG) cout<<szakaszok.size()<<endl;
                        navMesh.push_back(tempS);
                        sikeresSzakaszok[j][i]=true;
                    }
                }
            }
        }
        /// minden oldal elvileg fel van használva, ekkor még sok hely kimarad, ez a síkidomok számától függ
        /// BTW oldalak szám - 2 + 2 * síkidomok száma.
        /// Egy négyzetnek csak két háromszöge van az egyik átlója mentén
        /// mert matek
        if (DEBUG2) cout<<"SZAKASZOK"<<endl;
        /// itt még azok a háromszögek nincsenek meg, amik nem a síkidomok oldalival dolgoznak
        // CSAK DEBUG !!!
        for (int i=0; i<szakaszok.size() && DEBUG; i++){ /// megnézem az összes szakaszt
            for (int j=0; j<szakaszok.size(); j++){ /// összehasonlítom mindegyikkel
                if (j==i)   /// önmagával nem
                    continue;
                /// metszéshez szükséges visszatérési értékek (függvény !!!)
                double res = 0, s = 0, t = 0;
                vec2 closest2[2];
                bool Dret = false;
                ///eddig, itt meg a metszés keresése
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
        // EDDIG DEBUG !!!
        /*
        if (DEBUG){
            double res = 0, s = 0, t = 0;
            vec2 closest2[2];
            bool Dret = false;
            DistanceSegments2(vec2(225,225),vec2(400,0),vec2(200,200),vec2(300,225),res,s,t,closest2,Dret);
            cout<<"BAJOS2"<<endl;
            cout<<res<<", s: "<<s<<", t:"<<t<<", "<<closest2[0].x<<" "<<closest2[1].y<<", "<<closest2[1].x<<" "<<closest2[1].y<<endl;
        }*/

        if (DEBUG) cout<<"navMesh.size() "<<navMesh.size()<<endl;

        clock_t t2 = clock();   /// IDŐMÉRŐ
        //fillNavMesh(); /// minden maradék helyet ki kell tölteni
        cout<<"fillNavMesh() time: "<<clock()-t2<<endl; /// időtartama

    }

    void fillNavMesh(){
        /// ki kell tölteni a megmaradt helyet
        set<Szakasz> szakaszokA, szakaszokB; /// milyen szakaszain vannak eddig, és azok hányszor szerepelnek
        if (DEBUG2) cout<<"navMesh.size(): "<<navMesh.size()<<endl; /// hány háromszögünk van eddig
        for (int i=0; i<navMesh.size(); i++){ /// számba veszem a háromszögek oldalait
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
        if (DEBUG2) cout<<"szakaszokA0.size(): "<<szakaszokA.size()<<endl; /// az egyszer legalább szereplő szakaszok
        if (DEBUG2) cout<<"szakaszokB.size(): "<<szakaszokB.size()<<endl; /// a kétsze is szereplő szakaszok
        for (int i=0; i<sikidomok.size(); i++){ /// minden síkidomra
            for (int j=0; j<sikidomok[i].szakaszok.size(); j++){ /// azon minden szakaszára is lefuttatom az előzőt
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
        if (DEBUG2) cout<<"szakaszokA1.size(): "<<szakaszokA.size()<<endl;
        if (DEBUG2) cout<<"szakaszokB.size(): "<<szakaszokB.size()<<endl; /// most már a síkidomok oldalai néhány szakaszt megdupláztak
        BAszakasz.assign(szakaszokB.begin(),szakaszokB.end()); /// ezek a végső szakaszok
        if (DEBUG2) cout<<"BAszakasz.size(): "<<BAszakasz.size()<<endl;

        vector<Szakasz> kivonandoSzakaszok(szakaszokB.begin(),szakaszokB.end()); /// megnézzük, hogy ezeket kivonva mely szakaszok szereplenek csak egyszer
        if (DEBUG2) cout<<"kivonandoSzakaszok.size(): "<<kivonandoSzakaszok.size()<<endl;
        for (int i=0; i<kivonandoSzakaszok.size(); i++){
            Szakasz temp(kivonandoSzakaszok[i].p2,kivonandoSzakaszok[i].p1);
            if (szakaszokA.find(kivonandoSzakaszok[i])!=szakaszokA.end())
                szakaszokA.erase(szakaszokA.find(kivonandoSzakaszok[i]));
            else if (szakaszokA.find(temp)!=szakaszokA.end())
                szakaszokA.erase(szakaszokA.find(temp));
        }
        if (DEBUG2) cout<<"szakaszokA2.size(): "<<szakaszokA.size()<<endl; /// ennyi szakasz maradt, melyek egykék
        /// azért fontos ez, mert ezek formálnak jobb esetben síkidomokat, melyeket fel kell osztani háromszögekre

        //while (szakaszokA.size()!=0){
        vector<vec2> csucsok; /// az összes csúcsot újra összeszámolom
        for (int i=0; i<sikidomok.size();i++){
            for (int j=0; j<sikidomok[i].szakaszok.size(); j++){
                csucsok.push_back(sikidomok[i].szakaszok[j].p1);
            }
        }
        if (true){
            /// megnézem a megmaradt szakaszokat
            vector<Szakasz> megmaradtSzakaszok(szakaszokA.begin(),szakaszokA.end());
            for (int i=0; i<megmaradtSzakaszok.size(); i++){
                for (int j=i+1; j<megmaradtSzakaszok.size(); j++){
                    /// összehasonlítom őket egymással
                    Sikidom sikidom; /// ez lesz az esteleges háromszög változója
                    Szakasz sz, torlesre; /// melyik az új szakasz, és kell-e törölni
                    vec2 kozos; /// melyik a közös pontja a két vizsgált szakasznak
                    bool siker = false; /// történt-e baj eddig? Nem.
                    /// A négy lehetséges szakasz
                    Szakasz sz11(megmaradtSzakaszok[i].p1,megmaradtSzakaszok[j].p1);
                    Szakasz sz12(megmaradtSzakaszok[i].p1,megmaradtSzakaszok[j].p2);
                    Szakasz sz21(megmaradtSzakaszok[i].p2,megmaradtSzakaszok[j].p1);
                    Szakasz sz22(megmaradtSzakaszok[i].p2,megmaradtSzakaszok[j].p2);
                    /// megvizsgálom azt, hogy melyik csúcsuk közös, abból jön az sz-be hogy melyik az új szakasz
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
                    else { /// ha egyik sem, akkor nincs közös pontjuk, és mehetünk akövetkezőre
                        continue;
                    }
                    /// fontos, hogy ne tartalmazzon a háromszög másik csúcsot
                    int csCnt = 0;
                    Haromszog hrsz(sz.p1,sz.p2,kozos);
                    for (int k=0; k<csucsok.size(); k++){
                        if (hrsz.benneVanAPont(csucsok[k]))
                            csCnt++;
                    }
                    if (csCnt>3)
                        siker=false;
                    /// kiküszöbölve az idegen csúcs tartalmazáse
                    if (siker){ /// ha minden rendben volt
                        ///akkor a síkidomnak megadom a három oldalát
                        sikidom.szakaszok.push_back(sz);
                        sikidom.szakaszok.push_back(Szakasz(sz.p2,kozos));
                        sikidom.szakaszok.push_back(Szakasz(kozos,sz.p1));
                        navMesh.push_back(sikidom); /// és hozzáadom a többihez
                        /// és kitörlöm a használt szakaszokat, ha vannak
                        if (szakaszokA.find(megmaradtSzakaszok[i])!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[i]));
                        if (szakaszokA.find(megmaradtSzakaszok[i].inv())!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[i].inv()));
                        if (szakaszokA.find(megmaradtSzakaszok[j])!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[j]));
                        if (szakaszokA.find(megmaradtSzakaszok[j].inv())!=szakaszokA.end())
                            szakaszokA.erase(szakaszokA.find(megmaradtSzakaszok[j].inv()));
                        szakaszokA.erase(szakaszokA.find(torlesre));
                    }
                }
            }
        }
        /// redukáltam a nem lefedett területeket
        if (DEBUG2) cout<<"szakaszokA3.size(): "<<szakaszokA.size()<<endl;
        /// viszont nem teljes a megoldás

        // NEM HASZNÁLT
        /*
        vector<Szakasz> sikidomOldalak; /// összegyűjtöm ebbe a síkidomok oldalait
        for (int i=0; i<sikidomok.size();i++){
            sikidomOldalak.insert(sikidomOldalak.end(),sikidomok[i].szakaszok.begin(),sikidomok[i].szakaszok.end());
        }
        */

        /// A maradék szakszokat síkidomokra bontom
        // a visszatérési érték az lényegtelen
        vector<Sikidom> belsoSikidomok = szakaszokSikidomokbaSzervezese(szakaszokA);

        // NEM HASZNÁLT, és rossz is talán
        /*
        if (false){
            vector<Szakasz> megmaradtSzakaszok(szakaszokA.begin(),szakaszokA.end());
            vector<Szakasz> ujSzakaszok;
            for (int i=0; i<megmaradtSzakaszok.size(); i++){
                for (int j=i+1; j<megmaradtSzakaszok.size(); j++){
                    Sikidom sikidom;
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
                    }
                    cout<<"H"<<endl;
                }
            }
        }
        */
        if (DEBUG2) cout<<"szakaszokA.size(): "<<szakaszokA.size()<<endl;
        //BAszakasz.clear();
        //BAszakasz.assign(szakaszokA.begin(),szakaszokA.end());
        if (DEBUG2) cout<<"BAszakasz.size(): "<<BAszakasz.size()<<endl;
        //}
    }


    vector<Sikidom> szakaszokSikidomokbaSzervezese(set<Szakasz> szakaszokA){
        /// síkidomokba kell szerveznem, ez átalakult mássá
        vector<Szakasz> megmaradtSzakaszok(szakaszokA.begin(),szakaszokA.end());
        /// csúcsokat összegyűjtöm
        vector<vec2> csucsok;
        /// és azok fokszámát
        vector<int> csucsokFokszama;
        vector<vector<int>> csucsokhozSzakaszok;
        for (int i=0; i<megmaradtSzakaszok.size(); i++){
            csucsok.push_back(megmaradtSzakaszok[i].p1);
            csucsok.push_back(megmaradtSzakaszok[i].p2);
        }
        set<vec2> tempCsucsok;
        copy(csucsok.begin(), csucsok.end(),inserter(tempCsucsok, tempCsucsok.end()));
        csucsok.clear();
        /// set-be oda és visza váltva kiszűrtem a duplikációt
        csucsok.assign(tempCsucsok.begin(),tempCsucsok.end());
        csucsokFokszama.resize(csucsok.size());
        /// megnézem a csúcsok fokszámát, és hogy mely szakaszokhoz tartoznak
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
        // DEBUG
        if (DEBUG2) cout<<"CSUCSOK FOKSZAMA: "<<endl;
        for (int i=0; i<csucsokFokszama.size() && DEBUG2; i++){
            cout<<csucsokFokszama[i]<<endl;
        }

        vector<Sikidom> ret; /// visszatérési érték, de nem hsaznált !!!
        vector<Szakasz> osszesSzakasz; /// újra kigyűjtöm az eddigi szakaszokat
        set<vec2> osszesCsucsSet; /// és csúcsot
        for (int i=0; i<navMesh.size(); i++){
            for (int j=0; j<navMesh[i].szakaszok.size(); j++){
                osszesSzakasz.push_back(navMesh[i].szakaszok[j]);
                osszesCsucsSet.insert(navMesh[i].szakaszok[j].p1);
            }
        }
        /// lista csúcsokból
        vector<vec2> osszesCsucs; osszesCsucs.assign(osszesCsucsSet.begin(),osszesCsucsSet.end());
        set<Szakasz> tempOsszesSzakasz;
        copy(osszesSzakasz.begin(), osszesSzakasz.end(),inserter(tempOsszesSzakasz, tempOsszesSzakasz.end()));
        for (int i=0; i<megmaradtSzakaszok.size(); i++){
            if (tempOsszesSzakasz.find(megmaradtSzakaszok[i]) != tempOsszesSzakasz.end())
                tempOsszesSzakasz.erase(tempOsszesSzakasz.find(megmaradtSzakaszok[i]));
            if (tempOsszesSzakasz.find(megmaradtSzakaszok[i].inv()) != tempOsszesSzakasz.end())
                tempOsszesSzakasz.erase(tempOsszesSzakasz.find(megmaradtSzakaszok[i].inv()));
        }
        /// lista a szakaszokból
        osszesSzakasz.clear();
        osszesSzakasz.assign(tempOsszesSzakasz.begin(),tempOsszesSzakasz.end());

        /// miket találok új háromszögeket
        vector<vector<int>> ujHaromszogekIdx;
        if (DEBUG2) cout<<"EDDIG RENDBEN"<<endl;
        /// tripla iterálás a csúcsokon
        for (int idx=0; idx<csucsok.size(); idx++){
            bool ujHaromszog = false;
            for (int i=0; i<csucsok.size(); i++){
                if (i==idx)
                    continue;
                /// emiatt csak 2 és fél iterálás, DE NINCSEN MÁR SOK
                for (int j=i+1; j<csucsok.size(); j++){
                    if (j==idx)
                        continue;
                    set<int> csucsIdxSet={idx,i,j};
                    vector<int> csucsIdx;
                    csucsIdx.assign(csucsIdxSet.begin(),csucsIdxSet.end());
                    /// csúcsok sorba vannak rendezve
                    bool siker = true;
                    /// nem létezhet ilyen háromszög az újak között
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
                    /// ha nincs olyan háromszög, akkor lesz, van 3 szakasz hozzá
                    Szakasz a(csucsok[idx],csucsok[i]), b(csucsok[i],csucsok[j]), c(csucsok[j],csucsok[idx]);
                    /// ezen 3 szakasz metsz másikokat?
                    for (int k=0; k<osszesSzakasz.size(); k++){
                        if (metszikEgymast(a,osszesSzakasz[k]) ||
                            metszikEgymast(b,osszesSzakasz[k]) ||
                            metszikEgymast(c,osszesSzakasz[k])){
                            siker = false;
                            break;
                        }
                    }
                    if (!siker)
                        continue;
                    /// és tartalmaz más pontot?
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
                    // NEM HASZNÁLT, talán rossz is
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
                    /// sikerült új háromszöget találni
                    ujHaromszog=true;
                    /// létre is hozom a háromszöget
                    Sikidom temp;
                    temp.szakaszok.push_back(a);
                    temp.szakaszok.push_back(b);
                    temp.szakaszok.push_back(c);
                    ujHaromszogekIdx.push_back(csucsIdx);   /// eltárolom az új háromszög csúcsainak indexét
                    navMesh.push_back(temp);    /// meg magát a háromszöget
                    /// meg az új szakaszokat nem elmetszésre
                    osszesSzakasz.push_back(a);
                    osszesSzakasz.push_back(b);
                    osszesSzakasz.push_back(c);
                }
            }
            /// ha nem találtam új háromszöget, akkor végeztünk, vagy más megközelítés kell
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

        /// ez ismétlés, újra ki kell számítani (egyre kevesebb eredméynt kell kiadnia, így egyre gyorsabb)
        /// ki kell számoli, hogy mik a megmaradt oldalak
        set<Szakasz> szakaszokB;
        set<Szakasz> szakaszokC;
        for (int i=0; i<navMesh.size(); i++){
            for (int j=0; j<navMesh[i].szakaszok.size(); j++){
                if (szakaszokB.find(navMesh[i].szakaszok[j])==szakaszokB.end() &&
                    szakaszokB.find(navMesh[i].szakaszok[j].inv())==szakaszokB.end()){
                    szakaszokB.insert(navMesh[i].szakaszok[j]);
                } else if (szakaszokC.find(navMesh[i].szakaszok[j])==szakaszokC.end() &&
                    szakaszokC.find(navMesh[i].szakaszok[j].inv())==szakaszokC.end()){
                    szakaszokC.insert(navMesh[i].szakaszok[j]);
                }
            }
        }
        if (DEBUG2) cout<<"szakaszokC.size(): "<<szakaszokC.size()<<endl;
        if (DEBUG2) cout<<"szakaszokB.size(): "<<szakaszokB.size()<<endl;
        for (int i=0; i<sikidomok.size(); i++){
            for (int j=0; j<sikidomok[i].szakaszok.size(); j++){
                if (szakaszokB.find(sikidomok[i].szakaszok[j])==szakaszokB.end() &&
                    szakaszokB.find(sikidomok[i].szakaszok[j].inv())==szakaszokB.end()){
                    szakaszokB.insert(sikidomok[i].szakaszok[j]);
                } else if (szakaszokC.find(sikidomok[i].szakaszok[j])==szakaszokC.end() &&
                    szakaszokC.find(sikidomok[i].szakaszok[j].inv())==szakaszokC.end()){
                    szakaszokC.insert(sikidomok[i].szakaszok[j]);
                }
            }
        }
        if (DEBUG2) cout<<"szakaszokC.size(): "<<szakaszokC.size()<<endl;
        if (DEBUG2) cout<<"szakaszokB.size(): "<<szakaszokB.size()<<endl;
        vector<Szakasz> tempSzakaszok; tempSzakaszok.assign(szakaszokC.begin(),szakaszokC.end());
        for (int i=0; i<tempSzakaszok.size(); i++){
            if (szakaszokB.find(tempSzakaszok[i])!=szakaszokB.end())
                szakaszokB.erase(szakaszokB.find(tempSzakaszok[i]));
            if (szakaszokB.find(tempSzakaszok[i].inv())!=szakaszokB.end())
                szakaszokB.erase(szakaszokB.find(tempSzakaszok[i].inv()));
        }
        if (DEBUG2) cout<<"szakaszokC.size(): "<<szakaszokC.size()<<endl;
        if (DEBUG2) cout<<"szakaszokB.size(): "<<szakaszokB.size()<<endl;

        BAszakasz.clear();
        BAszakasz.assign(szakaszokB.begin(),szakaszokB.end());

        while (true){
            int z = BAszakasz.size();
            if (DEBUG2) cout<<"BAszakasz.size(): PQ: "<<BAszakasz.size()<<endl;
            if (DEBUG2) cout<<"szakaszBA.size(): PQ: "<<szakaszBA.size()<<endl;
            if (z==0)
                break;
            /// BAszakasz-szal dolgozik, redukálja 0-ig
            navMeshFillBelsoAtlok();

            if (z==BAszakasz.size())
                break;
        }
        if (DEBUG2) cout<<"BAszakasz.size(): PQ: "<<BAszakasz.size()<<endl;
        if (DEBUG2) cout<<"szakaszBA.size(): PQ: "<<szakaszBA.size()<<endl;

        BAszakasz=szakaszBA;

        return ret;
    }

    void navMeshFillBelsoAtlok(){
        /// ha az eddig megoldási próbálkozások nem teljesek, akkor itt van ez
        /// nem teljes: nem észrevett hiba miatt, vagy anomália okán, de legfőképp számítási pontosság miatt a metszésnél
        /// ez ennek okán már nem háromszögeket, hanem csak szakaszokat kreál
        /// végén csak a szakaszok összességéből kreálok egy gráf hálót

        /// kellenek a csúcsok és a fokszámaik
        set<vec2> csucsokTemp;
        vector<int> csucsokFokszama;
        for (int i=0; i<BAszakasz.size(); i++){
            if (csucsokTemp.find(BAszakasz[i].p1) == csucsokTemp.end()){
                csucsokTemp.insert(BAszakasz[i].p1);
            }
            if (csucsokTemp.find(BAszakasz[i].p2) == csucsokTemp.end()){
                csucsokTemp.insert(BAszakasz[i].p2);
            }
        }
        csucsokFokszama.resize(csucsokTemp.size());
        vector<vec2> csucsok; csucsok.assign(csucsokTemp.begin(), csucsokTemp.end());
        for (int i=0; i<BAszakasz.size(); i++){
            for (int j=0; j<csucsok.size(); j++){
                if (csucsok[j]==BAszakasz[i].p1)
                    csucsokFokszama[j]++;
                else if (csucsok[j]==BAszakasz[i].p2)
                    csucsokFokszama[j]++;
                else
                    continue;
                //csucsokhozSzakaszok[j].push_back(i);
            }
        }

        /// megkeresem a legkisebb fokszámú csúcsot
        int idx = -1, minFokszam = INT_MAX;
        for (int i=0; i<csucsokFokszama.size(); i++){
            if (csucsokFokszama[i]<minFokszam &&csucsokFokszama[i]>0){
                idx=i;
                minFokszam=csucsokFokszama[i];
            }
        }
        /// ha nincs csúcs, akkor return
        if (idx==-1){
            cout<<"VEGE"<<endl;
            return;
        }

        /// legkisebb fokszámú csúcsból indulva megnézem, mit lehet onnan elérni
        set<HonnanPont> eddigiPontok;
        vector<HonnanPont> aktualisPontok;
        HonnanPont elsoPont(csucsok[idx]);
        HonnanPont vegsoPont;
        aktualisPontok.push_back(elsoPont);
        bool stop = false;
        if (DEBUG) cout<<"A"<<endl;
        /// aktuális ponotkat kifejtem, azokból lesznek új pontok
        /// aktuális pontok átkerülnek az eddigi-ekbe, és az aktuálisba meg szűrve az újak
        /// ha ugyan azon csúcshoz ér, akkor ott van egy kör és síkidomot generál belőle
        /// itt csak egy irányba lehet haladni
        while (!stop){

            if (DEBUG) cout<<"B"<<endl;
            vector<HonnanPont> ujPontok;

            /// ha nincs kifejtendő pont, vagy 1 a fokszáma az rossz
            /// törlése a pontnak/szaksznak és return, újrapróbálkozás
            if (aktualisPontok.size()==0 || minFokszam<2){
                for (int i=0; i<BAszakasz.size(); i++){
                    if (BAszakasz[i].p1==elsoPont.p){
                        BAszakasz.erase(BAszakasz.begin()+i);
                        i--;
                    } else if (BAszakasz[i].p2==elsoPont.p){
                        BAszakasz.erase(BAszakasz.begin()+i);
                        i--;
                    }
                }
                return;
            }

            /// kifejtem az aktuális pontokat
            for (int i=0; i<aktualisPontok.size(); i++){
                for (int j=0; j<BAszakasz.size(); j++){
                    /// eltárolom az új pontokba a felfedezett pontot, és hogy honnan jött
                    if (aktualisPontok[i].p==BAszakasz[j].p1 && aktualisPontok[i].honnan!=BAszakasz[j].p2 ){
                        ujPontok.push_back(HonnanPont(BAszakasz[j].p2,aktualisPontok[i].p));
                    }
                    if (aktualisPontok[i].p==BAszakasz[j].p2 && aktualisPontok[i].honnan!=BAszakasz[j].p1){
                        ujPontok.push_back(HonnanPont(BAszakasz[j].p1,aktualisPontok[i].p));
                    }
                }
            }
            if (DEBUG) cout<<"C"<<endl;
            /// megnézem, hogy véletlen nem szerepel-e már az eddigiPontok-ban az egyik aktuális
            for (int i=0; i<aktualisPontok.size(); i++){
                if (eddigiPontok.find(aktualisPontok[i])!=eddigiPontok.end()){
                    cout<<"AKTUALIS VEG"<<endl;
                    vegsoPont = aktualisPontok[i].p;
                    stop=true;
                    break;
                } else {
                    /// egyébként eltárolom
                    eddigiPontok.insert(aktualisPontok[i]);
                }
            }
            if (stop)
                break;
            if (DEBUG) cout<<"D"<<endl;
            aktualisPontok.clear();
            set<HonnanPont> aktualisTempSet;
            /// megnézem az új pontokat is, hogy van-e már bennük másodjára előforduló
            /// ha van, akkor megvan a kör
            for (int i=0; i<ujPontok.size(); i++){
                if (eddigiPontok.find(ujPontok[i])!=eddigiPontok.end()){
                    if (DEBUG) cout<<"UJ EDDIGI VEG"<<endl;
                    vegsoPont = ujPontok[i];
                    stop=true;
                    break;
                } else if (aktualisTempSet.find(ujPontok[i])!=aktualisTempSet.end()){
                    if (DEBUG) cout<<"UJ AKTUALIS VEG"<<endl;
                    vegsoPont = ujPontok[i];
                    stop=true;
                    break;
                } else {
                    /// ha nincs, akkor hozzáadom
                    aktualisTempSet.insert(ujPontok[i]);
                }
            }
            if (DEBUG) cout<<"E"<<endl;
            /// szűrt új pontok
            aktualisPontok.assign(aktualisTempSet.begin(), aktualisTempSet.end());
            if (DEBUG) cout<<"E2 "<<aktualisPontok.size()<<endl;

            /// ha megvagyunk akkor az újakat is hozzáadjuk, nem lesz benne a végső
            if (stop){
                eddigiPontok.insert(aktualisPontok.begin(), aktualisPontok.end());
                break;
            }
        }
        if (DEBUG) cout<<"F"<<endl;
        /// a végső csúcs nincs benne a felfedezett csúcsokban
        /// csak az elsőként felfedezett módja
        Sikidom temp;
        /// kigyűjtöm melyik a végső pontnak a felfedezett megfelelője
        HonnanPont vegsoPontA = *eddigiPontok.find(vegsoPont);
        // DEBUG
        vector<HonnanPont> tempH; tempH.assign(eddigiPontok.begin(),eddigiPontok.end());
        for(int i=0; i<tempH.size() && DEBUG; i++){
            cout<<tempH[i].p.x<<" "<<tempH[i].p.y<<", "<<tempH[i].honnan.x<<" "<<tempH[i].honnan.y<<endl;
        }
        /// kigyűjti a szakaszokat a vágső pont felfedezettjéből
        while (vegsoPontA.p!=elsoPont.p){
            if (eddigiPontok.find(HonnanPont(vegsoPontA.honnan))==eddigiPontok.end()){
                cout<<"BAJOS"<<endl;
            }
            HonnanPont ptemp = *eddigiPontok.find(HonnanPont(vegsoPontA.honnan));
            temp.szakaszok.push_back(Szakasz(vegsoPontA.p,vegsoPontA.honnan));
            vegsoPontA=ptemp;
        }

        if (DEBUG) cout<<"G"<<endl;
        vector<Szakasz> sikidomEleje;
        sikidomEleje.push_back(Szakasz(vegsoPont.honnan,vegsoPont.p));
        vegsoPontA = *eddigiPontok.find(HonnanPont(vegsoPont.honnan));
        /// és kigyűjtöm a végső pontból visszafejtve
        while (vegsoPontA.p!=elsoPont.p){
            if (eddigiPontok.find(HonnanPont(vegsoPontA.honnan))==eddigiPontok.end())
                if (DEBUG) cout<<"BAJOS"<<endl;
            HonnanPont ptemp = *eddigiPontok.find(HonnanPont(vegsoPontA.honnan));
            sikidomEleje.push_back(Szakasz(vegsoPontA.honnan,vegsoPontA.p));
            vegsoPontA=ptemp;
        }
        if (DEBUG) cout<<"H"<<endl;
        /// helyes sorrendbe rendezem a szakaszokat
        reverse(temp.szakaszok.begin(),temp.szakaszok.end());
        temp.szakaszok.insert(temp.szakaszok.end(),sikidomEleje.begin(),sikidomEleje.end());
        /// felhasznált szakaszok törlése
        set<Szakasz> batemp(BAszakasz.begin(),BAszakasz.end());
        for (int i=0; i<temp.szakaszok.size(); i++){
            if (batemp.find(temp.szakaszok[i])!=batemp.end()){
                batemp.erase(batemp.find(temp.szakaszok[i]));
            }
            if (batemp.find(temp.szakaszok[i].inv())!=batemp.end()){
                batemp.erase(batemp.find(temp.szakaszok[i].inv()));
            }
        }
        BAszakasz.clear();
        BAszakasz.assign(batemp.begin(), batemp.end());
        reverse(temp.szakaszok.begin(), temp.szakaszok.end());
        temp.belso=false;
        vector<Szakasz> belsok = temp.belsoAtlok();
        if (DEBUG) cout<<"SIKIDOM: "<<temp.szakaszok.size()<<", "<<belsok.size()<<endl;
        for (int i=0; i<temp.szakaszok.size() && DEBUG;i++){
            cout<<temp.szakaszok[i].p1.x<<" "<<temp.szakaszok[i].p1.y<<", "<<temp.szakaszok[i].p2.x<<" "<<temp.szakaszok[i].p2.y<<endl;
        }
        /// gyűjtöm a belső átlókat, mert azok lesznek végül a lényegesek, ezek összessége az, ami hiányzik a gráfhoz
        szakaszBA.insert(szakaszBA.end(),belsok.begin(),belsok.end());
    }

    /// DEBUG, de kiszámolja és megjeleníti a síkidomok átlóit
    void bakeAtloNavMesh(bool osszes=false) {
        belsoAtlok.clear();
        for (int i=0; i<sikidomok.size(); i++){
            vector<Szakasz> temp = sikidomok[i].belsoAtlok(osszes);
            belsoAtlok.push_back(temp);
        }
    }

    void bakeKulsoNavMesh(){
        for (int i=0; i<sikidomok.size(); i++){
            vector<Szakasz> temp = sikidomok[i].belsoAtlok(true);
            vector<Szakasz> temp2 = sikidomok[i].belsoAtlok();
            temp2.insert(temp2.end(),sikidomok[i].szakaszok.begin(),sikidomok[i].szakaszok.end());
            for (int i=0; i<temp.size(); i++){
                for (int j=0; j<temp2.size(); j++){
                    if (temp[i]==temp2[j]){
                        temp.erase(temp.begin()+i);
                        i--;
                        break;
                    }
                }
            }
            kulsoAtlok.push_back(temp);
        }
    }

    void bakeKulsoNavMesh2(bool osszes=false){
        for (int i=0; i<sikidomok.size(); i++){
            vector<Szakasz> temp = sikidomok[i].belsoAtlok(osszes);
            /*
            vector<Szakasz> temp2; //= sikidomok[i].belsoAtlok2();
            temp2.insert(temp2.end(),sikidomok[i].szakaszok.begin(),sikidomok[i].szakaszok.end());
            for (int i=0; i<temp.size(); i++){
                for (int j=0; j<temp2.size(); j++){
                    if (temp[i]==temp2[j]){
                        temp.erase(temp.begin()+i);
                        i--;
                        break;
                    }
                }
            }
            */
            kulsoAtlok.push_back(temp);
        }
    }

    /// pálya megjelenítése
    void draw(SDL_Renderer &renderer, Kamera kamera){
        /*
        boxRGBA(&renderer,kamera.valosLekepezese(vec2(posX,0)).x,kamera.valosLekepezese(vec2(0,posY)).y,
                kamera.valosLekepezese(vec2(posX+sizeX,0)).x,kamera.valosLekepezese(vec2(0,posY+sizeY)).y,100,100,100,255);
        */

        /*
        for (int i=0; i<sikidomok.size(); i++){
            sikidomok[i].draw(renderer,kamera);
        }
        */

        for (int i=0; i<navMesh.size(); i++){
            if (i+1!=sok%(navMesh.size()+1) && sok%(navMesh.size()+1)!=0)
                continue;
            navMesh[i].draw(renderer,kamera,true);
        }

        /*
        for (int i=0; i<kulsoAtlok.size(); i++){
            for (int j=0; j<kulsoAtlok[i].size(); j++){
                kulsoAtlok[i][j].draw(renderer,kamera,255,255,255);
            }
        }
        for (int i=0; i<belsoAtlok.size(); i++){
            for (int j=0; j<belsoAtlok[i].size(); j++){
                belsoAtlok[i][j].draw(renderer,kamera,10,20,255);
            }
        }
        for (int i=0; i<BAszakasz.size(); i++){
            BAszakasz[i].draw(renderer,kamera,0,0,0);
        }
        for (int i=0; i<joBelso.size(); i++){
            joBelso[i].draw(renderer,kamera,255,30,255);
        }
        */
    }
};


#endif // GEO_H_INCLUDED
