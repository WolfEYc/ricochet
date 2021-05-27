#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
#include <experimental/filesystem>
#include <locale>
#include "vecmath.h"
#include "level.h"
using namespace sf;
namespace fs = std::experimental::filesystem;

Clock c;

unsigned screenx = 600, screeny = 600, fps = 24, level = 0, maxric = 8, curr = 0, wallBuild = 0, botmenu = 0;
bool rclicking = 0, shot = 0, showMenu = 0, mainMenu = 1, targetPlace = 0, editlevelname = 0;

Font font;
std::string levelname;
Text leveltext;

std::vector<VertexArray> walls;
std::vector<RectangleShape> enemies, levelselector;
std::vector<Text> selectorText;
std::vector<bool> whoisHit;
std::vector<VertexArray> shots(maxric); 

CircleShape pivot, gun;
RectangleShape enemy(Vector2f(10.f,20.f));
VertexArray newWall(LinesStrip,2);

Texture buildText, placeText, backText, delWalltext, delTargettext, saveText, mainScreenText;
Sprite buildwall, placeTarget, goBack, delWall, delTarget, save, mainScreen;

RenderWindow window(VideoMode(screenx,screeny), "Ricochet", Style::Close);
View view(window.getDefaultView());

std::vector<leveld> levels;
std::vector<std::ifstream> levelfiles;

void initLevels(){
    VertexArray wall(LineStrip,2);

    leveld level1;

    level1.name = "custom";

    level1.center = Vector2f(300.f,500.f);

    VertexArray leftwall(LineStrip,2);
    leftwall[0].position = Vector2f(0.1f,0.1f);
    leftwall[1].position = Vector2f(0.2f,screeny);

    VertexArray topwall(LineStrip,2);
    topwall[0].position = Vector2f(0.1f,0.1f);
    topwall[1].position = Vector2f(screenx,0.2f);

    VertexArray botwall(LineStrip,2);
    botwall[0].position = Vector2f(0.1f,screeny);
    botwall[1].position = Vector2f(screenx,screeny-0.2f);

    VertexArray rightwall(LineStrip,2);
    rightwall[0].position = Vector2f(screenx-0.1f,0.1f);
    rightwall[1].position = Vector2f(screenx-0.2f,screeny);

    level1.walls.push_back(leftwall);
    level1.walls.push_back(rightwall);
    level1.walls.push_back(topwall);
    level1.walls.push_back(botwall);

    for(unsigned i = 0; i < level1.walls.size(); i++){
        level1.walls[i][0].color = Color::Red;
        level1.walls[i][1].color = Color::Red;
    }

    levels.push_back(level1);

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
            if(in == "center"){
                mode = 3;
                continue;
            }
            if(in == "name"){
                mode = 4;
                continue;
            }

            

            std::stringstream ss(in);

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
            if(mode == 2){
                enemy.setPosition(pt);
                l.enemies.push_back(enemy);
                continue;
            }
            if(mode == 3){
                l.center = pt;
            }            
        }
        levels.push_back(l);

    }
    
    unsigned x = 0, y = 0;
    RectangleShape levelrect;
    levelrect.setFillColor(Color::Blue);
    levelrect.setSize(Vector2f(150.f,50.f));
    levelrect.setOutlineThickness(3.f);
    levelrect.setOutlineColor(Color::White);


    Text ltext;
    ltext.setFont(font);
    ltext.setCharacterSize(20);
    

    for(unsigned i = 0; i < levels.size(); i++, x++){
        if(x == 3){
            x = 0;
            y++;
        }
        if(i)
            levelrect.setFillColor(Color::Red);
            

        ltext.setString(levels[i].name.substr(0,20));
        ltext.setPosition(x*175+60.f,y*100+260.f);
        selectorText.push_back(ltext);

        levelrect.setPosition(Vector2f(x*175+50.f,y*100+250.f));
        levelselector.push_back(levelrect);
    }

    botmenu = 600 - y*100 + 200;    

}

void load(){
    walls = levels[level].walls;
    enemies = levels[level].enemies;
    gun.setPosition(levels[level].center);
    levelname = levels[level].name;
    if(!level)
        leveltext.setPosition(Vector2f(325.f,25.f));
    else
        leveltext.setPosition(Vector2f(75.f,25.f));

    leveltext.setString(levelname);
    shots = std::vector<VertexArray>(maxric);
    Vector2f pivpos = gun.getPosition();
    pivpos.y-=10.f;
    pivpos.x+=10.f;
    pivot.setPosition(pivpos);
}

void init(){ //called once at startup
    font.loadFromFile("resources/arial.ttf");
    enemy.setFillColor(Color::Green);
    enemy.setOrigin(5.f,10.f);
    initLevels();
    load();
    window.setFramerateLimit(fps);
    gun.setRadius(5.f);
    gun.setOrigin(5.f,5.f);
    gun.setFillColor(Color::Red);
    pivot.setRadius(5.f);
    pivot.setFillColor(Color::Blue);
    pivot.setOrigin(5.f,5.f);
    

    buildText.loadFromFile("resources/buildwall.png");
    placeText.loadFromFile("resources/placeTarget.png");
    backText.loadFromFile("resources/goback.png");
    delTargettext.loadFromFile("resources/delTarget.png");
    delWalltext.loadFromFile("resources/delWall.png");
    saveText.loadFromFile("resources/save.png");
    mainScreenText.loadFromFile("resources/logo.png");
    
    buildwall.setTexture(buildText);
    placeTarget.setTexture(placeText);
    goBack.setTexture(backText);
    delTarget.setTexture(delTargettext);
    delWall.setTexture(delWalltext);
    save.setTexture(saveText);
    mainScreen.setTexture(mainScreenText);

    goBack.setPosition(Vector2f(25.f,25.f));
    buildwall.setPosition(Vector2f(75.f,25.f));
    placeTarget.setPosition(Vector2f(125.f,25.f));
    delWall.setPosition(Vector2f(175.f,25.f));
    delTarget.setPosition(Vector2f(225.f,25.f));
    save.setPosition(Vector2f(275.f,25.f));
    mainScreen.setPosition(Vector2f(100.f,100.f)); 

    leveltext.setFont(font);
    leveltext.setCharacterSize(20);
    leveltext.setFillColor(Color::White);
    
}

void render(){
    window.clear();

    if(mainMenu){
        window.setView(view);
        window.draw(mainScreen);
        for(unsigned i = 0; i < levelselector.size(); i++){
            window.draw(levelselector[i]);
            window.draw(selectorText[i]);
        }
        window.display();
        return;
    }

    Vector2i windowoffset = window.getPosition();
    windowoffset.y+=30;
    Vector2f mousepos = Vector2f(Mouse::getPosition()-windowoffset);

    if(!shot){        
        for(unsigned line = 0; line < maxric; line++){
            window.draw(shots[line]);
        }        
    }else{        
        for(unsigned line = 0; line < curr; line++){
            window.draw(shots[line]);
        }       
    }

    for(unsigned i = 0; i < walls.size(); i++){
        window.draw(walls[i]);
    }

    for(unsigned i = 0; i < enemies.size(); i++){
        window.draw(enemies[i]);
    }

    window.draw(gun);
    window.draw(pivot);

    

    if(showMenu){
        if(editlevelname){
            static sf::Time text_effect_time;
            static bool show_cursor;

            text_effect_time += c.restart();

            if (text_effect_time >= sf::seconds(0.5f))
            {
                show_cursor = !show_cursor;
                text_effect_time = sf::Time::Zero;
            }

            leveltext.setString(levelname + (show_cursor ? '_' : ' '));
        }else{
            leveltext.setString(levelname);
        }


        window.draw(goBack);
        if(level == 0){
            window.draw(buildwall);
            window.draw(placeTarget);
            window.draw(delWall);
            window.draw(delTarget);
            window.draw(save);                        
        }
        window.draw(leveltext);
    }

    if(wallBuild == 2){
        newWall[1].position = mousepos;
        window.draw(newWall);
    }

    if(targetPlace){
        enemy.setPosition(mousepos);
        window.draw(enemy);
    }

    window.display();
}

unsigned firstCollision (Vector2f a1, Vector2f a2, unsigned prevwall){
    float closest_intersection = 9999.f;
    unsigned closest_wall = 69;

    for(unsigned w = 0; w < walls.size(); w++){

        if(w == prevwall)
            continue;

        VertexArray wall = walls[w];

        Vector2f b1 = wall[0].position;
        Vector2f b2 = wall[1].position;

        if(collides(a1,a2,b1,b2)){        
            Vector2f ipoint = calcIntersectVector(a1,a2,b1,b2);
            
            float dist_to_intersection = dist(ipoint - a1);
            if(dist_to_intersection < closest_intersection){
                closest_intersection = dist_to_intersection;
                closest_wall = w;
            }        
        }
    }
    return closest_wall;
}

void calcShot(){

    Vector2f a1 = gun.getPosition();
    Vector2f a2 = pivot.getPosition();

    std::vector<VertexArray> thisshots;
    whoisHit = std::vector<bool> (enemies.size(), 0);

    for(unsigned i = 0; i < enemies.size(); i++){
        enemies[i].setFillColor(Color::Green);
    }

    unsigned wallhit = 69;
    for(unsigned i = 0; i < maxric; i++){

        wallhit = firstCollision(a1,a2,wallhit);

        //std::cout << wallhit << std::endl;

        if(wallhit == 69)
            break;

        Vector2f b1 = walls[wallhit][0].position;
        Vector2f b2 = walls[wallhit][1].position;

        Vector2f collision_v = calcIntersectVector(a1,a2,b1,b2);

        for(unsigned i = 0; i < enemies.size(); i++){
            if(segmentIntersectsRectangle(enemies[i].getGlobalBounds(),a1,collision_v)){
                whoisHit[i] = 1;
                enemies[i].setFillColor(Color::Yellow);        
            }
        }

        VertexArray shot(LineStrip,2);
        shot[0].position = a1;
        shot[0].color = Color(155,135,12);
        shot[1].position = collision_v;
        shot[1].color = Color(155,135,12);

        shots[i] = shot;

        a2 = newPivot(a1,collision_v,b1,b2);

        //std::cout << "new piv: ";
        //printVector2f(a2);

        //std::cout << "collision pt: ";
        a1 = collision_v;
        //printVector2f(a1);

    }
}

void saveLevel(){
    std::ofstream out("levels/"+levelname+".txt");
    out << "name\n" << levelname.substr(0,20);
    out << "\ncenter\n";
    printVector2f(gun.getPosition(),out);
    out << "\nwalls\n";
    for(unsigned i = 4; i < walls.size(); i++){
        printVector2f(walls[i][0].position,out);
        out << " ";
        printVector2f(walls[i][1].position,out);
        out << std::endl;
    }

    out << "targets\n";
    
    for(unsigned i = 0; i < enemies.size(); i++){
        printVector2f(enemies[i].getPosition(),out);
        out << std::endl;
    }

    levels.clear();
    levelselector.clear();
    levelfiles.clear();
    selectorText.clear();

    initLevels();
}

int main(){
    init();
    
    while(window.isOpen()){

        Event e;
        while(window.pollEvent(e)){
            if (e.type == Event::Closed)
                window.close();

            if(e.type == Event::MouseWheelScrolled){
                if(mainMenu){
                    if(e.mouseWheelScroll.delta == 1){
                        if(view.getCenter().y>300.f){
                            Vector2f viewcenter = view.getCenter();
                            viewcenter.y-=20;
                            view.setCenter(viewcenter);
                        }
                    }else if(view.getCenter().y<botmenu+300.f){
                        Vector2f viewcenter = view.getCenter();
                        viewcenter.y+=20;
                        view.setCenter(viewcenter);
                    }
                }
            }
            if(e.type == Event::MouseButtonPressed){ // do shooty stuff
                Vector2i windowoffset = window.getPosition();
                windowoffset.y+=30;
                Vector2f newpos = window.mapPixelToCoords((Mouse::getPosition()-windowoffset));
                
                if(Mouse::isButtonPressed(Mouse::Left)){
                    if(mainMenu){
                        for(unsigned i = 0; i < levelselector.size(); i++){
                            if(levelselector[i].getGlobalBounds().contains(newpos)){
                                //clicked on a level
                                level = i;
                                load();
                                view = window.getDefaultView();
                                window.setView(view);
                                mainMenu = 0;
                            }
                        }
                    }else
                    if(targetPlace){
                        enemy.setPosition(newpos);
                        enemies.push_back(enemy);
                        targetPlace = 0;
                        showMenu = 1;
                    }else
                    if(wallBuild){
                        wallBuild++;
                        if(wallBuild==2){
                            newWall[0].position = newpos;                                                     
                        }else if(wallBuild == 3){
                            newWall[1].position = newpos;
                            walls.push_back(newWall);
                            showMenu = 1;
                            wallBuild = 0;
                        }else{
                            showMenu = 1;
                            wallBuild = 0;
                        }
                    }else
                    if(showMenu){
                        if(goBack.getGlobalBounds().contains(newpos)){
                            mainMenu = 1;
                            //std::cout << "going back" << std::endl;
                        }
                        if(level==0){
                            if(buildwall.getGlobalBounds().contains(newpos)){
                                showMenu = 0;
                                //std::cout << "building wall" << std::endl;
                                wallBuild = 1;
                            }
                            if(placeTarget.getGlobalBounds().contains(newpos)){
                                showMenu = 0;                                
                                //std::cout << "placing target" << std::endl;
                                targetPlace = 1;
                            }
                            if(delWall.getGlobalBounds().contains(newpos)){                                                             
                                //std::cout << "deleting wall" << std::endl;
                                if(walls.size()>4)
                                    walls.pop_back();
                            }
                            if(delTarget.getGlobalBounds().contains(newpos)){                                                             
                                //std::cout << "deleting Target" << std::endl;
                                if(!enemies.empty())
                                    enemies.pop_back();
                            }
                            if(save.getGlobalBounds().contains(newpos)){
                                saveLevel();
                                //std::cout << "saving level" << std::endl;                                
                            }
                            if(leveltext.getGlobalBounds().contains(newpos)){
                                editlevelname = 1;
                                //std::cout << "editing level" << std::endl;
                            }
                        }
                    }else //cant change pivot if menu is open
                    if(abs(newpos.x-gun.getPosition().x) > 0.12f && abs(newpos.y-gun.getPosition().y) > 0.12f
                        && dist(newpos-gun.getPosition()) > 10.f )
                        pivot.setPosition(Vector2f(Mouse::getPosition()-windowoffset)); 
                }
                if(Mouse::isButtonPressed(Mouse::Right)){
                    if(showMenu && level == 0){
                        gun.setPosition(newpos);
                    }
                }
            }
            if(e.type == Event::KeyPressed){
                switch(e.key.code){
                    case Keyboard::Space:
                        if(shot == 0){
                            shot = 1;
                            calcShot();
                        }
                        break;
                    case Keyboard::Tab:
                        if(!mainMenu && !wallBuild && !targetPlace)                        
                            showMenu = !showMenu;
                        
                        break;
                    case Keyboard::BackSpace:
                        if(editlevelname && !levelname.empty())
                            levelname.pop_back();
                        break;
                    case Keyboard::Enter:
                        editlevelname = 0;
                        break;
                }
            }
            if(e.type == Event::MouseButtonReleased){
                if(!Mouse::isButtonPressed(Mouse::Right)){
                    rclicking = 0;
                }
            }
            if (editlevelname && e.type == sf::Event::TextEntered) {
                if (std::isprint(e.text.unicode) && e.text.unicode!=' ')
                    levelname += e.text.unicode;
            }
        }
        if(shot){
            curr++;
            if(curr == maxric){
                shot = 0;
                curr = 0;
            }  
        }
        render();
    }
    return 0;
}
