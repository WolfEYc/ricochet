#include <SFML/Graphics.hpp>
#include "transformer.h"
using namespace sf;

Color r(255,0,0);
Color g(0,255,0);
Color b(0,0,255);
Color black(0,0,0);
Color white(255,255,255);

class Beam : public Transformer{
private:
    CircleShape red,green,blue;
public:
    Beam() : Transformer(){
        red.setRadius(3.6f);
        red.setOrigin(3.6f,3.6f);
        red.setOutlineThickness(2.f);
        red.setFillColor(black);
        red.setOutlineColor(r);
        green = red;
        blue = red;
        green.setOutlineColor(g);
        blue.setOutlineColor(b);
        shape.setPointCount(3);
        setColor(Color::Black);
    }

    Beam(float points) : Transformer(){
        red.setRadius(3.6f);
        red.setOrigin(3.6f,3.6f);
        red.setOutlineThickness(2.f);
        red.setFillColor(black);
        red.setOutlineColor(r);
        green = red;
        blue = red;
        green.setOutlineColor(g);
        blue.setOutlineColor(b);
        shape.setPointCount(points);
        setColor(Color::Black);
    }

    Beam(Vector2f origin, Vector2f pivot, Color beamcolor) : Beam() {
        setPosition(origin);
        setPivot(pivot);
        setColor(beamcolor);
    }

    void setColor(Color c){
        Transformer::setColor(c);
        if(c.r == 255)
            red.setFillColor(r);
        if(c.b == 255)
            blue.setFillColor(b);
        if(c.g == 255)
            green.setFillColor(g);
    }

    void setPosition(Vector2f newpos) override{
        Transformer::setPosition(newpos);
        Vector2f top = getPoint(0), right = getPoint(1), left = getPoint(2);
        red.setPosition(midpoint(top,getOrigin()));
        blue.setPosition(midpoint(right,getOrigin()));
        green.setPosition(midpoint(left,getOrigin()));
    }

    void setPivot(Vector2f pivot) override{
        Transformer::setPivot(pivot);
        Vector2f top = getPoint(0), right = getPoint(1), left = getPoint(2);
        red.setPosition(midpoint(top,getOrigin()));
        blue.setPosition(midpoint(right,getOrigin()));
        green.setPosition(midpoint(left,getOrigin()));      
    }
    
    void toggleColor(Vector2f mousepos){
        if(red.getGlobalBounds().contains(mousepos)){
            if(red.getFillColor() == black){
                red.setFillColor(r);
                shape.setOutlineColor(shape.getOutlineColor()+r);
            }else{
                red.setFillColor(black);
                Color outcol = shape.getOutlineColor();
                outcol.r=0;
                shape.setOutlineColor(outcol);
            }            
        }else
        if(green.getGlobalBounds().contains(mousepos)){
            if(green.getFillColor() == black){
                green.setFillColor(g);
                shape.setOutlineColor(shape.getOutlineColor()+g);
            }else{
                green.setFillColor(black);
                Color outcol = shape.getOutlineColor();
                outcol.g=0;
                shape.setOutlineColor(outcol);
            }
            
        }else
        if(blue.getGlobalBounds().contains(mousepos)){
            if(blue.getFillColor() == black){
                blue.setFillColor(b);
                shape.setOutlineColor(shape.getOutlineColor()+b);
            }else{
                blue.setFillColor(black);
                Color outcol = shape.getOutlineColor();
                outcol.b=0;
                shape.setOutlineColor(outcol);                
            }            
        } 
    }

    void draw(RenderWindow &window) override{
        window.draw(shape);
        window.draw(red);
        window.draw(green);
        window.draw(blue);
    }

    void drawShape(RenderWindow &window){
        window.draw(shape);
    }

    Vector2f getPoint(unsigned i){
        return shape.getTransform().transformPoint(shape.getPoint(i));
    }
};
