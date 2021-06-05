#include <SFML/Graphics.hpp>
#include <vector>
#include "vecmath.h"
using namespace sf;

using surface = std::pair<Vector2f,Vector2f>;

Color r(255,0,0);
Color g(0,255,0);
Color b(0,0,255);
Color black(0,0,0);
Color white(255,255,255);

class Transformer{
private:
    CircleShape shape,red,green,blue;
public:
    bool selected, rotating, resizing;
    Transformer(unsigned points){
        shape.setPointCount(points);
        shape.setRadius(15.f);
        shape.setOrigin(15.f,15.f);
        shape.setOutlineColor(Color::White);
        shape.setOutlineThickness(3.f);
        shape.setFillColor(Color::Black);
        shape.rotate(0.0004f);
        selected = 0;
        rotating = 0;
        resizing = 0;
        red.setRadius(3.6f);
        red.setOrigin(3.6f,3.6f);
        red.setOutlineThickness(2.f);
        red.setFillColor(black);
        red.setOutlineColor(r);
        green = red;
        blue = red;
        green.setOutlineColor(g);
        blue.setOutlineColor(b);
    }

    void setFillColor(Color c){
        shape.setFillColor(c);
    }

    Color getFillColor(){
        return shape.getFillColor();
    }

    //checks if point is in polygon
    bool contains(Vector2f point){
        Vector2f farpoint(9999.f,9999.f);        
        unsigned count = 0;
        
        std::vector<surface> ws = getWalls();
        for(surface s : ws){
            if(segIntersectSeg({point,farpoint},s)){
                count++;
            }
        }
        return (count % 2);
    }
    

    bool isClicked(Vector2f mousepos){
        return Transformer::contains(mousepos);
    }

    Color getOutlineColor(){
        return shape.getOutlineColor();
    }

    Vector2f getOrigin(){
        return shape.getPosition();
    }

    Vector2f getPivot(){
        return getPoint(0);
    }

    Vector2f getPoint(unsigned i){
        return shape.getTransform().transformPoint(shape.getPoint(i));
    }

    std::vector<Vector2f> getPoints(){
        std::vector<Vector2f> pts;
        for(unsigned i = 0; i < shape.getPointCount(); i++){
            pts.push_back(shape.getPoint(i));
        }
        return pts;
    }

    std::vector<surface> getWalls(){
        std::vector<surface> ws;
        for(unsigned i = 0; i < shape.getPointCount(); i++){
            ws.push_back({getPoint(i),getPoint((i+1)%shape.getPointCount())});
        }
        return ws;
    }

    //really only use for checking if beam goes thru target
    bool hit(Vector2f a1, Vector2f a2){
        std::vector<surface> ws = getWalls();
        for(surface s : ws){
            if(segIntersectSeg({a1,a2},s)){
                return true;
            }
        }
        return false;
    }

    //yeah it do thing
    void setRadius(float size){
        shape.setRadius(size);
        shape.setOrigin(getRadius(),getRadius());
        setPosition(shape.getPosition());
    }

    //remember its not actually a circle
    float getRadius() const{
        return shape.getRadius();
    }

    void setOutlineColor(Color c){ 
        shape.setOutlineColor(c);       
        if(c.r == 255)
            red.setFillColor(r);
        if(c.b == 255)
            blue.setFillColor(b);
        if(c.g == 255)
            green.setFillColor(g);
    }

    void setPosition(Vector2f newpos){
        shape.setPosition(newpos);
        Vector2f top, right, left;
        if(shape.getPointCount()>4)
            top = getPoint(0), right = getPoint(2), left = getPoint(4);
        else
            top = getPoint(0), right = getPoint(1), left = getPoint(2);

        red.setPosition(midpoint(top,getOrigin()));
        blue.setPosition(midpoint(right,getOrigin()));
        green.setPosition(midpoint(left,getOrigin()));
    }

    //use this to change the angle
    void setPivot(Vector2f pivot){
        float angle = rotationAngle(getOrigin(),pivot);
        shape.setRotation(angle);   
        setPosition(getOrigin());     
    }
    
    //toggles Fill color in response to mousepress
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

    //toggles Fill color in response to mousepress
    void toggleFillColor(Vector2f mousepos){
        if(red.getGlobalBounds().contains(mousepos)){
            if(red.getFillColor() == black){
                red.setFillColor(r);
                shape.setFillColor(shape.getFillColor()+r);
            }else{
                red.setFillColor(black);
                Color outcol = shape.getFillColor();
                outcol.r=0;
                shape.setFillColor(outcol);
            }            
        }else
        if(green.getGlobalBounds().contains(mousepos)){
            if(green.getFillColor() == black){
                green.setFillColor(g);
                shape.setFillColor(shape.getFillColor()+g);
            }else{
                green.setFillColor(black);
                Color outcol = shape.getFillColor();
                outcol.g=0;
                shape.setFillColor(outcol);
            }
            
        }else
        if(blue.getGlobalBounds().contains(mousepos)){
            if(blue.getFillColor() == black){
                blue.setFillColor(b);
                shape.setFillColor(shape.getFillColor()+b);
            }else{
                blue.setFillColor(black);
                Color outcol = shape.getFillColor();
                outcol.b=0;
                shape.setFillColor(outcol);                
            }            
        } 
    }

    //Draws with color editing visible
    void draw(RenderWindow &window){
        window.draw(shape);
        window.draw(red);
        window.draw(green);
        window.draw(blue);
    }

    //Draws without color editing visible
    void drawShape(RenderWindow &window){
        window.draw(shape);
    }
    
    void sizeTo(Vector2f mousepos){
        float distance = dist(mousepos - getOrigin());
        setRadius(distance);
    }

    void drawDull(RenderWindow &window){
        drawDullShape(window);
        window.draw(red);
        window.draw(green);
        window.draw(blue);
    }

    void drawDullShape(RenderWindow &window){
        Color fillcol = shape.getFillColor();
        shape.setOutlineThickness(0.f);
        Color original = fillcol;
        fillcol.r = std::clamp(fillcol.r/8,0,255);
        fillcol.b = std::clamp(fillcol.b/8,0,255);
        fillcol.g = std::clamp(fillcol.g/8,0,255);
        shape.setFillColor(fillcol);
        window.draw(shape);
        shape.setFillColor(original);
    }
};
