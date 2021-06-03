#include <SFML/Graphics.hpp>
#include "transformer.h"
using namespace sf;

Color darkred = Color(50,0,0);
Color lightred = Color(100,0,0);

class zone : public Transformer{
public:
    zone() : Transformer(4){
        setFillColor(darkred);        
    }
    zone(float size, Vector2f origin, Vector2f pivot) : Transformer(origin,pivot,lightred){
        shape.setPointCount(4);
        setFillColor(darkred);  
        shape.setRadius(size);           
    }

    void resize(float size){
        shape.setRadius(size);
    }

    float getSize(){
        return shape.getRadius();
    }

    bool contains(Vector2f point){
        return InsidePolygon(getPoints(),4,point);
    }
};