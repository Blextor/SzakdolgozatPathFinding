#include "common.h"
#include "vec3.h"


/// https://www.geometrictools.com/GTE/Samples/Distance/DistanceSegments3/DistanceSegments3Console.cpp
/// metszépontot számít, kissé pontatlan talán
void DistanceSegments3Console(
    vec3 const& P0, vec3 const& P1,
    vec3 const& Q0, vec3 const& Q1,
    double& sqrDistance, double& s, double& t, vec3 closest[2])
{
    double const SMALL_NUM = 0.00000001;
    vec3   u = P1 - P0;
    vec3   v = Q1 - Q0;
    vec3   w = P0 - Q0;
    double    a = dotprod(u, u);         // always >= 0
    double    b = dotprod(u, v);
    double    c = dotprod(v, v);         // always >= 0
    double    d = dotprod(u, w);
    double    e = dotprod(v, w);
    double    D = a*c - b*b;        // always >= 0
    double    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
    double    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

    // compute the line parameters of the two closest points
    if (D < SMALL_NUM) { // the lines are almost parallel
        sN = 0.0;         // force using point P0 on segment S1
        sD = 1.0;         // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                 // get the closest points on the infinite lines
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
            sN = 0.0;
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
        tN = 0.0;
        // recompute sc for this edge
        if (-d < 0.0)
            sN = 0.0;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    }
    else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
        tN = tD;
        // recompute sc for this edge
        if ((-d + b) < 0.0)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d + b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (std::fabs(sN) < SMALL_NUM ? 0.0 : sN / sD);
    tc = (std::fabs(tN) < SMALL_NUM ? 0.0 : tN / tD);

    // get the difference of the two closest points
    s = sc;
    t = tc;
    closest[0] = P0 * (1.0 - sc) + P1 * sc;
    closest[1] = Q0 * (1.0 - tc) + Q1 * tc;
    vec3 diff = closest[0] - closest[1];
    sqrDistance = dotprod(diff, diff);
}

void DistanceSegments2(
    vec2  P0, vec2  P1,
    vec2  Q0, vec2  Q1,
    double& sqrDistance, double& s, double& t, vec2 closest[2], bool& Dret)
{
    double const SMALL_NUM = 0.00000001;
    vec2   u = P1 - P0;
    vec2   v = Q1 - Q0;
    vec2   w = P0 - Q0;
    double  a = u.dot(u, u);         // always >= 0
    double  b = u.dot(u, v);
    double  c = u.dot(v, v);         // always >= 0
    double  d = u.dot(u, w);
    double  e = u.dot(v, w);
    double D = a*c - b*b;        // always >= 0
    double  sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
    double  tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

    // compute the line parameters of the two closest points
    if (D < SMALL_NUM) { // the lines are almost parallel
        Dret=true;
        sN = 0.0;         // force using point P0 on segment S1
        sD = 1.0;         // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                 // get the closest points on the infinite lines
        Dret=false;
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
            sN = 0.0;
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
        tN = 0.0;
        // recompute sc for this edge
        if (-d < 0.0)
            sN = 0.0;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    }
    else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
        tN = tD;
        // recompute sc for this edge
        if ((-d + b) < 0.0)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d + b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (std::fabs(sN) < SMALL_NUM ? 0.0 : sN / sD);
    tc = (std::fabs(tN) < SMALL_NUM ? 0.0 : tN / tD);

    // get the difference of the two closest points
    s = sc;
    t = tc;
    closest[0] = P0 * (1.0 - sc) + P1 * sc;
    closest[1] = Q0 * (1.0 - tc) + Q1 * tc;
    vec2 diff = closest[0] - closest[1];
    sqrDistance = diff.length();
}

bool operator== (const vec2& lhs,const vec2& rhs) {
    return (fabs(lhs.x-rhs.x)<EPSZ && fabs(lhs.y-rhs.y)<EPSZ);
}

bool operator< (const vec2& lhs,const vec2& rhs) {
    if (lhs.x==rhs.x)
        return lhs.y<rhs.y;
    return lhs.x<rhs.x;
}
