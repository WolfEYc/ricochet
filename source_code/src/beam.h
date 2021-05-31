#include <SFML/Graphics.hpp>
#include "vecmath.h"
using namespace sf;

Color r(255,0,0);
Color g(0,255,0);
Color b(0,0,255);
Color black(0,0,0);
Color white(255,255,255);

class Beam{
private:
    CircleShape red,green,blue,tri;
public:
    bool selected;
    Beam(){
        red.setRadius(3.6f);
        red.setOrigin(3.6f,3.6f);
        red.setOutlineThickness(2.f);
        red.setFillColor(black);
        red.setOutlineColor(r);
        green = red;
        blue = red;
        green.setOutlineColor(g);
        blue.setOutlineColor(b);
        tri.setRadius(15.f);
        tri.setPointCount(3);
        tri.setOrigin(15.f,15.f);
        tri.setOutlineColor(black);
        tri.setOutlineThickness(2.f);
        tri.setFillColor(black);
        tri.rotate(0.0001f);
        selected = 0;
    }
    
    Beam(Vector2f origin, float rotation) : Beam(){
        setPosition(origin);
        rotate(rotation);
    }

    Beam(Vector2f origin, Vector2f pivot, Color beamcolor) : Beam() {
        setPosition(origin);
        setPivot(pivot);
        setColor(beamcolor);
    }

    void setColor(Color c){
        tri.setOutlineColor(c);
        if(c.r == 255)
            red.setFillColor(r);
        if(c.b == 255)
            blue.setFillColor(b);
        if(c.g == 255)
            green.setFillColor(g);
    }

    void setPosition(Vector2f newpos){
        tri.setPosition(newpos);
        red.setPosition(newpos.x,newpos.y-6.f);
        green.setPosition(newpos.x-5.f,newpos.y+4.f);
        blue.setPosition(newpos.x+5.f,newpos.y+4.f);
    }

    void setPivot(Vector2f pivot){
        
        std::cout << "origin: ";
        printVector2f(getOrigin());
        std::cout << "init: ";
        printVector2f(getPivot());
        std::cout << "curr: ";
        printVector2f(pivot);

        float angle = rotationAngle(getOrigin(),getPivot(),pivot);
        std::cout << angle << std::endl;
        rotate(angle);
    }

    void rotate(float angle){
        tri.rotate(angle);
        Vector2f top = getPoint(0), right = getPoint(1), left = getPoint(2);
        red.setPosition(midpoint(top,getOrigin()));
        blue.setPosition(midpoint(right,getOrigin()));
        green.setPosition(midpoint(left,getOrigin()));
    }

    bool isClicked(Vector2f mousepos){
        return tri.getGlobalBounds().contains(mousepos);
    }

    Color getColor(){
        return tri.getOutlineColor();
    }

    Vector2f getOrigin(){
        return tri.getPosition();
    }

    Vector2f getPivot(){
        return getPoint(0);
    }
    
    void toggleColor(Vector2f mousepos){
        if(red.getGlobalBounds().contains(mousepos)){
            if(red.getFillColor() == black){
                red.setFillColor(r);
                tri.setOutlineColor(tri.getOutlineColor()+r);
            }else{
                red.setFillColor(black);
                tri.setOutlineColor(tri.getOutlineColor()-r);
            }            
        }else
        if(green.getGlobalBounds().contains(mousepos)){
            if(green.getFillColor() == black){
                green.setFillColor(g);
                tri.setOutlineColor(tri.getOutlineColor()+g);
            }else{
                green.setFillColor(black);
                tri.setOutlineColor(tri.getOutlineColor()-g);
            }
            
        }else
        if(blue.getGlobalBounds().contains(mousepos)){
            if(blue.getFillColor() == black){
                blue.setFillColor(b);
                tri.setOutlineColor(tri.getOutlineColor()+b);
            }else{
                blue.setFillColor(black);
                tri.setOutlineColor(tri.getOutlineColor()-b);
            }            
        }
         
    }

    void draw(RenderWindow &window){
        
        window.draw(tri);
        window.draw(red);
        window.draw(green);
        window.draw(blue);
        
    }

    Vector2f getPoint(unsigned i){
        return tri.getTransform().transformPoint(tri.getPoint(i));
    }
};
