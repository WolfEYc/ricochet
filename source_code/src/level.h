#include <bits/stdc++.h>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "transformer.h"
using namespace sf;

Color darkred = Color(50,0,0);

//comment
namespace fs = std::filesystem;

using surface = std::pair<Vector2f,Vector2f>;

class leveld
{
public:
    std::string name;
    std::vector<Transformer> beams;
    std::vector<Transformer> targets;
    std::vector<VertexArray> walls;
    std::vector<Transformer> reflectors;
    std::vector<Transformer> noplacezones;
    float maxref;  

    leveld& operator=(leveld other) {
        name = other.name;
        beams = other.beams;
        targets = other.targets;
        walls = other.walls;
        reflectors = other.reflectors;
        noplacezones = other.noplacezones;
        maxref = other.maxref;
        return *this;
    }

    void saveLevel(){
        std::ofstream out("levels/"+name+".txt");
        out << "name\n" << name.substr(0,20);
        out << "\nbeams\n";
        Color c;
        for(Transformer b : beams){       
            c = b.getOutlineColor();
            out << std::to_string(c.r) <<","<<std::to_string(c.g)<<","<<std::to_string(c.b) << " ";   
            printVector2f(b.getOrigin(),out);
            out << " ";
            printVector2f(b.getPivot(),out);
            out << std::endl;
        }
        
        out << "walls\n";
        for(VertexArray wall: walls){
            printVector2f(wall[0].position,out);
            out << " ";
            printVector2f(wall[1].position,out);
            out << std::endl;
        }

        out << "targets\n";
        
        for(Transformer target : targets){
            c = target.getOutlineColor();
            out << std::to_string(c.r) <<","<<std::to_string(c.g)<<","<<std::to_string(c.b) << " ";
            printVector2f(target.getOrigin(),out);
            out << std::endl;
        }

        out << "reflectors\n" << reflectors.size();

        out << "\nnoplacezones\n";

        for(Transformer noplacezone : noplacezones){
            c = noplacezone.getFillColor();
            out << std::to_string(c.r) <<","<<std::to_string(c.g)<<","<<std::to_string(c.b) << " ";  
            printVector2f(noplacezone.getOrigin(),out);
            out << " ";
            printVector2f(noplacezone.getPivot(),out);
            out << " " << noplacezone.getRadius() << "\n";
        }        
    }

    surface firstCollision (Vector2f a1, Vector2f a2, int &prevhit, Color beamcolor){
        float closest_dist = 9999.f;
        int closest_index = 69;
        surface closest_reflector;

        for(unsigned i = 0; i < beams.size(); i ++){
            std::vector<surface> reflectablewalls = beams[i].getWalls();
            if(a1 == beams[i].getOrigin() || a2 == beams[i].getPivot())
                continue;

            for(unsigned r = 0; r < reflectablewalls.size(); r++){
                surface refable = reflectablewalls[r];

                Vector2f b1 = refable.first;
                Vector2f b2 = refable.second;

                if(collides(a1,a2,b1,b2)){
                    Vector2f ipoint = calcIntersectVector(a1,a2,b1,b2);
                    float dist_to_intersection = dist(ipoint - a1);
                    if(dist_to_intersection < closest_dist){
                        closest_dist = dist_to_intersection;
                        closest_index = -1;
                        closest_reflector = {b1,b2};
                    }
                }

            }
            
        }

        for(unsigned w = 0; w < walls.size(); w++){
            VertexArray wall = walls[w];

            Vector2f b1 = wall[0].position;
            Vector2f b2 = wall[1].position;

            if(collides(a1,a2,b1,b2)){        
                Vector2f ipoint = calcIntersectVector(a1,a2,b1,b2);
                
                float dist_to_intersection = dist(ipoint - a1);
                if(dist_to_intersection < closest_dist){
                    closest_dist = dist_to_intersection;
                    closest_index = -1;
                    closest_reflector = {b1,b2};
                }        
            }
        }

        for(unsigned i = 0; i < reflectors.size(); i++){            
            if(int(i) == prevhit)
                continue;

            Color refColor = reflectors[i].getOutlineColor();

            if((beamcolor.r == 255 && refColor.r != 255) ||
               (beamcolor.b == 255 && refColor.b != 255) ||
               (beamcolor.g == 255 && refColor.g != 255))
                continue;

            
               

            std::vector<surface> reflectablewalls = reflectors[i].getWalls();

            for(unsigned r = 0; r < reflectablewalls.size(); r++){
                surface refable = reflectablewalls[r];

                Vector2f b1 = refable.first;
                Vector2f b2 = refable.second;

                if(collides(a1,a2,b1,b2)){
                    Vector2f ipoint = calcIntersectVector(a1,a2,b1,b2);
                    float dist_to_intersection = dist(ipoint - a1);
                    if(dist_to_intersection < closest_dist){
                        closest_dist = dist_to_intersection;
                        closest_index = i;
                        closest_reflector = {b1,b2};
                    }
                }

            }
            
        }
        
        prevhit = closest_index;
        return closest_reflector;
    }

    unsigned reflectorsLeft(){
        return maxref - reflectors.size();
    }

    void cleanObjects(FloatRect bounds,bool &showMenu){
        for(unsigned i = 0; i < reflectors.size(); i++){
            if(reflectors[i].selected)
                continue;
            if(!bounds.contains(reflectors[i].getOrigin())){
                reflectors.erase(reflectors.begin()+i);
                showMenu = 1;
                return;
            }
            Color refColor = reflectors[i].getOutlineColor();
            
            for(Transformer noplace : noplacezones){
                Color noColor = noplace.getFillColor();

                if(((refColor.r == 255 && noColor.r == 255)||
                (refColor.g == 255 && noColor.g == 255)||
                (refColor.b == 255 && noColor.b == 255))&&
                noplace.contains(reflectors[i].getOrigin())){
                    reflectors.erase(reflectors.begin()+i);
                    showMenu = 1;
                    return;
                }
            }
            for(unsigned j = 0; j < reflectors.size(); j++){
                if(j==i || reflectors[j].selected)                
                    continue;
                float distance = dist(reflectors[j].getOrigin()-reflectors[i].getOrigin());
                if(distance < reflectors[i].getRadius()*2.f){
                    reflectors.erase(reflectors.begin()+i);
                    showMenu = 1;
                    return;
                }
            }
        }
        for(unsigned i = 0; i<beams.size(); i++){
            if(!bounds.contains(beams[i].getOrigin())){
                beams.erase(beams.begin()+i);
                showMenu = 1;
                return;
            }
        }
        for(unsigned i = 0; i<targets.size(); i++){
            if(!bounds.contains(targets[i].getOrigin())){
                targets.erase(targets.begin()+i);
                showMenu = 1;
                return;
            }
        }
        for(unsigned i = 0; i<noplacezones.size(); i++){
            if(!bounds.contains(noplacezones[i].getOrigin())){
                noplacezones.erase(noplacezones.begin()+i);
                showMenu = 1;
                return;
            }
        }
        
    }
};

class Levels {
private:
    std::vector<leveld> levels;
    std::vector<std::ifstream> levelfiles;
public:
    leveld getLevel(unsigned i){
        return levels[i];
    }
    
    size_t size(){
        return levels.size();
    }

    void clear(){
        levels.clear();
        levelfiles.clear();
    }

    void initLevels(unsigned sx, unsigned sy){
        VertexArray wall(LineStrip,2);

        leveld level1;

        level1.name = "custom";

        Color color = Color::Yellow;
        
        Vector2f origin(300.f,500.f);
        
        Vector2f pivot = origin;

        pivot.y-=10.f;
        pivot.x+=10.f;

        Transformer beam(3);
        beam.setPosition(origin);
        beam.setPosition(pivot);
        beam.setOutlineColor(color);

        level1.beams.push_back(beam);    

        VertexArray leftwall(LineStrip,2);
        leftwall[0].position = Vector2f(0.1f,0.1f);
        leftwall[1].position = Vector2f(0.2f,sy);

        VertexArray topwall(LineStrip,2);
        topwall[0].position = Vector2f(0.1f,0.1f);
        topwall[1].position = Vector2f(sx,0.2f);

        VertexArray botwall(LineStrip,2);
        botwall[0].position = Vector2f(0.1f,sy);
        botwall[1].position = Vector2f(sx,sy-0.2f);

        VertexArray rightwall(LineStrip,2);
        rightwall[0].position = Vector2f(sx-0.1f,0.1f);
        rightwall[1].position = Vector2f(sx-0.2f,sy);

        level1.walls.push_back(leftwall);
        level1.walls.push_back(rightwall);
        level1.walls.push_back(topwall);
        level1.walls.push_back(botwall);

        level1.maxref = 0;

        for(unsigned i = 0; i < level1.walls.size(); i++){
            level1.walls[i][0].color = Color::Red;
            level1.walls[i][1].color = Color::Red;
        }

        levels.push_back(level1);

        Transformer target(6);
        Transformer noplacezone(4);
       
        
        float first,second,third;
        char comma;
        std::string in;

        for(auto &file : fs::directory_iterator("levels"))
            levelfiles.push_back(std::ifstream(file.path()));

        for(unsigned i = 0; i < levelfiles.size(); i++){
            //each level (file)
            leveld l;
            l.walls = level1.walls;

            unsigned mode = 0;

            while(getline(levelfiles[i],in)){
                if(mode == 4){
                    l.name = in;
                    mode = 0;
                    continue;
                }
                if(in == "walls"){
                    mode = 1;
                    continue;
                }            
                if(in == "targets"){
                    mode = 2;
                    continue;
                }
                if(in == "beams"){
                    mode = 3;
                    continue;
                }
                if(in == "name"){
                    mode = 4;
                    continue;
                }
                if(in == "reflectors"){
                    mode = 5;
                    continue;
                }
                if(in == "noplacezones"){
                    mode = 6;
                    continue;
                }

                std::stringstream ss(in);

                if(mode == 1){                   
                    ss >> first >> comma >> second;

                    wall[0].position = Vector2f(first,second);

                    ss >> first >> comma >> second;

                    wall[1].position = Vector2f(first,second);

                    l.walls.push_back(wall);
                    
                    continue;
                }

                if(mode == 2){
                    ss >> first >> comma >> second >> comma >> third;

                    color = Color(first,second,third);

                    target.setOutlineColor(color);
                                       
                    ss >> first >> comma >> second;

                    target.setPosition(Vector2f(first,second));
                    l.targets.push_back(target);
                    continue;
                }

                if(mode == 3){
                    ss >> first >> comma >> second >> comma >> third;

                    color = Color(first,second,third);

                    ss >> first >> comma >> second;

                    origin = Vector2f(first,second);
        
                    ss >> first >> comma >> second;   

                    pivot = Vector2f(first,second);

                    beam.setPosition(origin);
                    beam.setPivot(pivot);
                    beam.setOutlineColor(color);

                    l.beams.push_back(beam);
                    continue;
                }

                if(mode == 5){
                    ss >> l.maxref;
                    continue;
                }

                if(mode == 6){
                    ss >> first >> comma >> second >> comma >> third;

                    color = Color(first,second,third);

                    ss >> first >> comma >> second;
                    origin = Vector2f(first,second);
                    ss >> first >> comma >> second;
                    pivot = Vector2f(first,second);
                    float size;
                    ss >> size;
                    
                    noplacezone.setPosition(origin);
                    noplacezone.setPivot(pivot);
                    noplacezone.setRadius(size);
                    noplacezone.setFillColor(color);
                    
                    l.noplacezones.push_back(noplacezone);
                    continue;
                }                          
            }
            levels.push_back(l);
        }
    }
};
