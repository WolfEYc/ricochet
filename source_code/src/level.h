#include <bits/stdc++.h>
#include <filesystem>
#include <SFML/Graphics.hpp>
using namespace sf;

//comment
namespace fs = std::filesystem;

using surface = std::pair<Vector2f,Vector2f>;

class leveld
{
public:
    std::string name;
    std::vector<std::pair<CircleShape,CircleShape>> gunsNpivots;
    std::vector<RectangleShape> targets;
    std::vector<VertexArray> walls;
    std::vector<RectangleShape> reflectors;  

    CircleShape& getGun(unsigned i){
        return gunsNpivots[i].first;
    }

    CircleShape& getPivot(unsigned i){
        return gunsNpivots[i].second;
    }

    leveld& operator=(leveld other) {
        name = other.name;
        gunsNpivots = other.gunsNpivots;
        targets = other.targets;
        walls = other.walls;
        return *this;
    }

    void saveLevel(){
        std::ofstream out("levels/"+name+".txt");
        out << "name\n" << name.substr(0,20);
        out << "\nbeams\n";
        Color c;
        for(unsigned i = 0; i < gunsNpivots.size(); i++){        
            c = getGun(i).getFillColor();
            out << c.r <<","<<c.g<<","<<c.b << " ";        
            printVector2f(getGun(i).getPosition(),out);
            out << " ";
            printVector2f(getPivot(i).getPosition(),out);
            out << std::endl;
        }
        
        out << "\nwalls\n";
        for(VertexArray wall: walls){
            printVector2f(wall[0].position,out);
            out << " ";
            printVector2f(wall[1].position,out);
            out << std::endl;
        }

        out << "targets\n";
        
        for(RectangleShape target : targets){
            printVector2f(target.getPosition(),out);
            out << std::endl;
        }    
    }

    surface firstCollision (Vector2f a1, Vector2f a2, int &prevhit){
        float closest_dist = 9999.f;
        int closest_index = 69;
        surface closest_reflector;

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

        for(unsigned r = 0; r < reflectors.size(); r++){

            if(r == prevhit)
                continue;

            RectangleShape reflector = reflectors[r];
            
            for(unsigned p = 0; p < 4; p++){
                Vector2f b1 = reflector.getTransform().transformPoint(reflector.getPoint(p));

                Vector2f b2;
                if(p==3)
                    b2 = reflector.getTransform().transformPoint(reflector.getPoint(0));
                else
                    b2 = reflector.getTransform().transformPoint(reflector.getPoint(p+1));
            
                if(collides(a1,a2,b1,b2)){
                    Vector2f ipoint = calcIntersectVector(a1,a2,b1,b2);
                    float dist_to_intersection = dist(ipoint - a1);

                    if(dist_to_intersection < closest_dist){
                        closest_dist = dist_to_intersection;
                        closest_index = r;
                        closest_reflector = {b1,b2};                    
                    } 

                }
            }
        }

        prevhit = closest_index;
        return closest_reflector;
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
    
    size_t size (){
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
        CircleShape legun;
        legun.setRadius(5.f);
        legun.setPosition(Vector2f(300.f,500.f));
        legun.setFillColor(color);
        legun.setOrigin(5.f,5.f);
        CircleShape lepivot = legun;

        Vector2f pivpos = legun.getPosition();
        pivpos.y-=10.f;
        pivpos.x+=10.f;
        lepivot.setPosition(pivpos);

        level1.gunsNpivots.push_back({legun,lepivot});    

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

        for(unsigned i = 0; i < level1.walls.size(); i++){
            level1.walls[i][0].color = Color::Red;
            level1.walls[i][1].color = Color::Red;
        }

        levels.push_back(level1);

        RectangleShape target;        
        target.setSize(Vector2f(20.f,20.f));
        target.setOrigin(10.f,10.f);
        target.setOutlineThickness(3.f);

        for(auto &file : fs::directory_iterator("levels"))
            levelfiles.push_back(std::ifstream(file.path()));

        for(unsigned i = 0; i < levelfiles.size(); i++){
            //each level (file)
            leveld l;

            l.walls = level1.walls;

            std::string in;
            unsigned mode = 0;
            unsigned it = 0;
            while(levelfiles[i] >> in){
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

                std::stringstream ss(in);

                if(mode == 3){                    

                    float first,second,third;
                    char comma;

                    ss >> first >> comma >> second >> comma >> third;

                    color = Color(first,second,third);

                    ss >> first >> comma >> second;


                    legun.setPosition(Vector2f(first,second));
                    legun.setFillColor(color);
                    
                    lepivot = legun;  

                    ss >> first >> comma >> second;           
                    lepivot.setPosition(first,second);

                    l.gunsNpivots.push_back({legun,lepivot});
                    continue;
                }

                if(mode == 2){

                    float first,second,third;
                    char comma;

                    ss >> first >> comma >> second >> comma >> third;

                    color = Color(first,second,third);

                    target.setOutlineColor(color);
                                       
                    ss >> first >> comma >> second;

                    target.setPosition(first,second);
                    l.targets.push_back(target);
                    continue;
                }               

                float first,second;
                char comma;

                ss >> first >> comma >> second;
                Vector2f pt(first,second);

                if(mode == 1){
                    wall[it].position = pt;
                    
                    if(it)
                        l.walls.push_back(wall);

                    it = !it;
                    continue;
                }
                
                        
            }
            levels.push_back(l);
        }
    }

};