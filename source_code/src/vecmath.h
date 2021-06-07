#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
using namespace sf;

#define PI 3.14159

#define TWOPI PI*2.f;

using surface = std::pair<Vector2f,Vector2f>;

using l_equation = std::pair<float,float>;

float dist(Vector2f v){
    return sqrt(v.x*v.x+v.y*v.y);
}

float dotprod(Vector2f first, Vector2f second){
    return first.x*second.x+first.y*second.y;
}

float nonzero(float num){
    if(num < 0.00001f && num > -0.00001f)
        return 0.0001f;
    return num;
}

float rads2degs(float rads){
    return rads * 180.f/PI;
}

float degs2rads(float degs){
    return degs * PI/180.f;
}

void printVector2f(Vector2f v){
    std::cout << v.x << "," << v.y << std::endl;
}

void printVector2f(Vector2f v, std::ofstream &out){
    out << v.x << "," << v.y;
}

bool sameSigns(Vector2f p1, Vector2f p2){
    return ((p1.x < 0) == (p2.x < 0)) && ((p1.y < 0) == (p2.y < 0));
}

l_equation eqFrom2Pts(Vector2f p1, Vector2f p2){
    float dX = nonzero(p2.x-p1.x);
    float dY = nonzero(p2.y-p1.y);

    float M = dY/dX;
    //y-mx = b
    float b = p1.y-M*p1.x;
    return {M,b};
}

Vector2f calcIntersectVector(Vector2f a1, Vector2f a2, Vector2f b1, Vector2f b2){
    
    l_equation a = eqFrom2Pts(a1,a2);
    l_equation b = eqFrom2Pts(b1,b2);
    
    float aM = a.first;
    float aB = a.second;

    float bM = b.first;
    float bB = b.second;

    float left = aM-bM;
    float right = bB-aB;
    float X = right/left;
    float Y = aM*X+aB;

    return Vector2f(X,Y);
}

bool segmentIntersectsRectangle(const sf::FloatRect& rect, const sf::Vector2f& a_p1, const sf::Vector2f& a_p2)
{
    // Find min and max X for the segment
    auto minX = std::min(a_p1.x, a_p2.x);
    auto maxX = std::max(a_p1.x, a_p2.x);

    // Find the intersection of the segment's and rectangle's x-projections
    if (maxX > rect.left + rect.width) {
        maxX = rect.left + rect.width;
    }

    if (minX < rect.left) {
        minX = rect.left;
    }

    // If Y-projections do not intersect then there's no intersection
    if (minX > maxX) { return false; }

    // Find corresponding min and max Y for min and max X we found before
    auto minY = a_p1.y;
    auto maxY = a_p2.y;

    auto dx = a_p2.x - a_p1.x;
    if (std::abs(dx) > 0.0000001f) {
        auto k = (a_p2.y - a_p1.y) / dx;
        auto b = a_p1.y - k * a_p1.x;
        minY = k * minX + b;
        maxY = k * maxX + b;
    }

    if (minY > maxY) {
        std::swap(minY, maxY);
    }

    // Find the intersection of the segment's and rectangle's y-projections
    if (maxY > rect.top + rect.height) {
        maxY = rect.top + rect.height;
    }

    if (minY < rect.top) {
        minY = rect.top;
    }

    // If Y-projections do not intersect then there's no intersection
    if (minY > maxY) { return false; }
    return true;
}

bool collides (Vector2f origin, Vector2f pivot, Vector2f b1, Vector2f b2){
    Vector2f intersection_vector = calcIntersectVector(origin,pivot,b1,b2);
    
    float d1 = dist(intersection_vector - b1);
    float d2 = dist(intersection_vector - b2);
    float furthest;

    if(d1 > d2)
        furthest = d1;
    else
        furthest = d2;

    float wall_length = dist(b2-b1);
    
    return sameSigns(intersection_vector-origin,pivot-origin) && furthest < wall_length;
}

float rotationAngle(Vector2f origin, Vector2f currPos){
    return rads2degs(atan2(currPos.y-origin.y,currPos.x-origin.x)) + 90.f;
}

Vector2f pivotfromangle(float mirrorangle, float shotangle, Vector2f rotationPoint){
    float pivotangle = (mirrorangle*2.f-shotangle);
    float X = 5.f*cos(degs2rads(pivotangle-90.f))+rotationPoint.x;
    float Y = 5.f*sin(degs2rads(pivotangle-90.f))+rotationPoint.y;

    return Vector2f(X,Y);
}

Vector2f newPivot(Vector2f a1, Vector2f ipoint, Vector2f walla, Vector2f wallb){
    
    float shotangle = rotationAngle(a1,ipoint);
    
    float mirrorangle = rotationAngle(walla,wallb);
     
     /*
    if(walla.x < wallb.x)
        mirrorangle = rotationAngle(walla,wallb);
    else
        mirrorangle = rotationAngle(wallb,walla);
        */
    

    return pivotfromangle(mirrorangle,shotangle,ipoint);
  
}

Vector2f midpoint(Vector2f p1, Vector2f p2){
    Vector2f sum = p1+p2;
    return Vector2f(sum.x*0.5,sum.y*0.5);
}

bool segIntersectSeg(surface a, surface b){
    Vector2f intersection_vector = calcIntersectVector(a.first,a.second,b.first,b.second);
    
    float d1 = dist(intersection_vector - b.first);
    float d2 = dist(intersection_vector - b.second);
    float bfurthest = std::max(d1,d2);

    d1 = dist(intersection_vector - a.first);
    d2 = dist(intersection_vector - a.second);
    float afurthest = std::max(d1,d2);
    
    float b_length = dist(b.second-b.first);
    float a_length = dist(a.second-a.first);
        
    return afurthest < a_length && bfurthest < b_length;
}

Vector2f signsVector(Vector2f origin, Vector2f pivot){
    Vector2f d_vec = pivot - origin;
    d_vec.x = d_vec.x > 0 ? 1 : -1;
    d_vec.y = d_vec.y > 0 ? -1 : 1;
    return d_vec;
}

Vector2f signVector(Vector2f signs, Vector2f point){
    point.x*=signs.x;
    point.y*=signs.y;
    return point;
}