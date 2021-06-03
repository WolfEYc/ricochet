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
    bool selected, rotating;
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

    bool isClicked(Vector2f mousepos){
        return shape.getGlobalBounds().contains(mousepos);
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
    }

    //remember its not actually a circle
    float getRadius() const{
        return shape.getRadius();
    }

    //checks if point is in polygon
    bool contains(Vector2f point){
        return InsidePolygon(getPoints(),shape.getPointCount(),point);
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
        Vector2f top = getPoint(0), right = getPoint(1), left = getPoint(2);
        red.setPosition(midpoint(top,getOrigin()));
        blue.setPosition(midpoint(right,getOrigin()));
        green.setPosition(midpoint(left,getOrigin()));
    }

    //use this to change the angle
    void setPivot(Vector2f pivot){
        float angle = rotationAngle(getOrigin(),pivot);
        shape.setRotation(angle);   
        Vector2f top = getPoint(0), right = getPoint(1), left = getPoint(2);
        red.setPosition(midpoint(top,getOrigin()));
        blue.setPosition(midpoint(right,getOrigin()));
        green.setPosition(midpoint(left,getOrigin()));      
    }
    
    //toggles color in response to mousepress
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
    
};
