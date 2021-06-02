#include <SFML/Graphics.hpp>
#include <vector>
#include "vecmath.h"
using namespace sf;

using surface = std::pair<Vector2f,Vector2f>;

class Transformer{
protected:
    CircleShape shape;
public:
    bool selected, rotating;
    Transformer(){
        shape.setRadius(15.f);
        shape.setOrigin(15.f,15.f);
        shape.setOutlineColor(Color::White);
        shape.setOutlineThickness(2.f);
        shape.setFillColor(Color::Black);
        shape.rotate(0.0004f);
        selected = 0;
        rotating = 0;        
    }

    Transformer(float points) : Transformer(){
        shape.setPointCount(points);
    }

    Transformer(Vector2f origin, Vector2f pivot, Color beamcolor) : Transformer() {
        setPosition(origin);
        setPivot(pivot);
        setColor(beamcolor);
    }

    virtual void setColor(Color c){
        shape.setOutlineColor(c);        
    }

    virtual void setPosition(Vector2f newpos){
        shape.setPosition(newpos);       
    }

    virtual void setPivot(Vector2f pivot){
        float angle = rotationAngle(getOrigin(),pivot);
        shape.setRotation(angle);    
    }

    bool isClicked(Vector2f mousepos){
        return shape.getGlobalBounds().contains(mousepos);
    }

    Color getColor(){
        return shape.getOutlineColor();
    }

    Vector2f getOrigin(){
        return shape.getPosition();
    }

    Vector2f getPivot(){
        return getPoint(0);
    }

    virtual void draw(RenderWindow &window){
        window.draw(shape);
    }

    Vector2f getPoint(unsigned i){
        return shape.getTransform().transformPoint(shape.getPoint(i));
    }

    std::vector<surface> getWalls(){
        std::vector<surface> ws;
        unsigned i;
        for(i = 0; i < shape.getPointCount()-1; i++){
            ws.push_back({getPoint(i),getPoint(i+1)});
        }
        ws.push_back({getPoint(0),getPoint(shape.getPointCount()-1)});
        return ws;
    }

    
};
