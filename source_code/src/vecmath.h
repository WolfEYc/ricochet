#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
using namespace sf;

float dist(Vector2f v){
    return sqrt(v.x*v.x+v.y*v.y);
}

float nonzero(float num){
    if(num < 0.0001f && num > -0.0001f)
        return 0.0001f;
    return num;
}

void printVector2f(Vector2f v){
    std::cout << v.x << "," << v.y << std::endl;
}

void printVector2f(Vector2f v, std::ofstream &out){
    out << v.x << "," << v.y;
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

Vector2f calcIntersectVector(Vector2f a1, Vector2f a2, Vector2f b1, Vector2f b2){
    float adX = nonzero(a2.x-a1.x);
    float adY = nonzero(a2.y-a1.y);
    float bdX = nonzero(b2.x-b1.x);
    float bdY = nonzero(b2.y-b1.y);

    
    float aM = (adY)/(adX);
    float aB = a1.y - (aM*a1.x);

    float bM = (bdY)/(bdX);
    float bB = b1.y - (bM*b1.x);

    float left = aM-bM;
    float right = bB-aB;
    float X = right/left;
    float Y = aM*X+aB;

    return Vector2f(X,Y);
}

Vector2f calcPerpIntersect(Vector2f a1, Vector2f b1, Vector2f b2){

    float bdX = nonzero(b2.x-b1.x);
    float bdY = nonzero(b2.y-b1.y);

    float bM = (bdY)/(bdX);
    
    float pM = -1.f/(bM); //perpendicular slope (sign flipped inverse)
    float pB = a1.y - (pM*a1.x); //y-mx = b

    Vector2f y_int(0.f,pB); //yint of perp line

    return calcIntersectVector(a1,y_int,b1,b2); //return intersection point
}

bool collides (Vector2f a1, Vector2f a2, Vector2f b1, Vector2f b2){
    Vector2f intersection_vector = calcIntersectVector(a1,a2,b1,b2);
    
    float d1 = dist(intersection_vector - b1);
    float d2 = dist(intersection_vector - b2);
    float furthest;

    if(d1 > d2)
        furthest = d1;
    else
        furthest = d2;

    float wall_length = dist(b2-b1);
    
    bool axneg, bxneg, ayneg, byneg;

    if((a2.x - a1.x) > 0.f)
        axneg = 0;
    else
        axneg = 1;
    
    if((intersection_vector.x - a1.x) > 0.f)
        bxneg = 0;
    else
        bxneg = 1;

    if(a2.y - a1.y > 0.f)
        ayneg = 0;
    else
        ayneg = 1;

    if((intersection_vector.y - a1.y) > 0.f)
        byneg = 0;
    else
        byneg = 1;
    


    return bxneg == axneg && byneg == ayneg && furthest < wall_length;
}

Vector2f newPivot(Vector2f a1, Vector2f ipoint, Vector2f walla, Vector2f wallb){

    float wdY = nonzero(wallb.y-walla.y);
    float wdX = nonzero(wallb.x-walla.x);
    
    float bM = (wdY)/(wdX);
    
    float pM = -1.f/(bM); //perpendicular slope (sign flipped inverse)
    float pB = ipoint.y - (pM*ipoint.x); //y-mx = b

    Vector2f y_int(0.f,pB); //yint of perp line
    
    Vector2f midpoint = calcPerpIntersect(a1, y_int, ipoint);

    return midpoint + midpoint - a1;
}

