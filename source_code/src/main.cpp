#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
#include <locale>
#include "vecmath.h"
#include "level.h"
using namespace sf;

using surface = std::pair<Vector2f,Vector2f>;

Clock c;

unsigned screenx = 1000, screeny = 1000, fps = 24, level = 0, curr = 0, wallBuild = 0, botmenu = 0, selection = 0;
bool rclicking = 0, shot = 0, showMenu = 0, mainMenu = 1, targetPlace = 0, editlevelname = 0, playing = 0;

Font font;

Text leveltext;


std::vector<RectangleShape> levelselector;
std::vector<Text> selectorText;
std::vector<bool> whoisHit;
std::vector<std::vector<VertexArray>> shots; 
Color cColor;
leveld clev;
RectangleShape target;
VertexArray newWall(LinesStrip,2);

Texture buildText, placeText, backText, delWalltext, delTargettext, saveText, mainScreenText;
Sprite buildwall, placeTarget, goBack, delWall, delTarget, save, mainScreen;
CircleShape red, green, blue;

RenderWindow window(VideoMode(screenx,screeny), "Ricochet", Style::Close);
View view(window.getDefaultView());

Levels levels;

void initMenu(){
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
        if(x == 5){
            x = 0;
            y++;
        }
        if(i)
            levelrect.setFillColor(Color::Red);
            

        ltext.setString(levels.getLevel(i).name.substr(0,20));
        ltext.setPosition(x*175+60.f,y*100+260.f);
        selectorText.push_back(ltext);

        levelrect.setPosition(Vector2f(x*175+50.f,y*100+250.f));
        levelselector.push_back(levelrect);
    }

    botmenu = screeny - y*100 + 200;
}

void load(){
    clev = levels.getLevel(level);
    
    
    if(!level)
        leveltext.setPosition(Vector2f(325.f,25.f));
    else{
        leveltext.setPosition(Vector2f(75.f,25.f));
        playing = 1;
    }

    leveltext.setString(clev.name);
    shots = std::vector<std::vector<VertexArray>>(clev.gunsNpivots.size());    
}

void init(){ //called once at startup
    levels.initLevels(screenx,screeny);
    font.loadFromFile("resources/arial.ttf");
            
    target.setSize(Vector2f(20.f,20.f));
    target.setOrigin(10.f,10.f);
    target.setOutlineThickness(3.f);
    
    initMenu();
    load();
    window.setFramerateLimit(fps);
        

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
    mainScreen.setOrigin(mainScreen.getLocalBounds().width/2.f,mainScreen.getLocalBounds().height/2.f);

    goBack.setPosition(Vector2f(25.f,25.f));
    buildwall.setPosition(Vector2f(75.f,25.f));
    placeTarget.setPosition(Vector2f(125.f,25.f));
    delWall.setPosition(Vector2f(175.f,25.f));
    delTarget.setPosition(Vector2f(225.f,25.f));
    save.setPosition(Vector2f(275.f,25.f));
    mainScreen.setPosition(Vector2f(view.getCenter().x,100.f)); 

    leveltext.setFont(font);
    leveltext.setCharacterSize(20);
    leveltext.setFillColor(Color::White);

    red.setRadius(10.f);
    red.setOrigin(10.f,10.f);
    red.setOutlineThickness(3.f);
    red.setOutlineColor(Color::Red);

    green = red;
    blue = red;
    
    green.setOutlineColor(Color::Green);
    blue.setOutlineColor(Color::Blue);

    red.setPosition(325.f,25.f);
    green.setPosition(345.f,25.f);
    blue.setPosition(365.f,25.f);
    
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

    
    for(unsigned i = 0; i < shots.size(); i++){        
        for(unsigned line = 0; line < shots[i].size(); line++){
            window.draw(shots[i][line]);
        } 
    }       

    for(unsigned i = 0; i < clev.walls.size(); i++){
        window.draw(clev.walls[i]);
    }

    for(unsigned i = 0; i < clev.targets.size(); i++){
        window.draw(clev.targets[i]);
    }

    for(auto i : clev.gunsNpivots){
        window.draw(i.first);
        window.draw(i.second);
    }

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

            leveltext.setString(clev.name + (show_cursor ? '_' : ' '));
        }else{
            leveltext.setString(clev.name);
        }


        window.draw(goBack);
        if(level == 0){
            window.draw(buildwall);
            window.draw(placeTarget);
            window.draw(delWall);
            window.draw(delTarget);
            window.draw(save);
            window.draw(red);
            window.draw(green);
            window.draw(blue);                        
        }
        window.draw(leveltext);
    }

    if(wallBuild == 2){
        newWall[1].position = mousepos;
        window.draw(newWall);
    }

    if(targetPlace){
        target.setPosition(mousepos);
        window.draw(target);
    }

    window.display();
}

void calcShots(){
    Vector2f a1,a2,b1,b2;
    surface hitsurface;

    for(unsigned s = 0; s < shots.size(); s++){
        a1 = clev.getGun(s).getPosition();
        a2 = clev.getPivot(s).getPosition();

        shots[s].clear();

        for(unsigned i = 0; i < clev.targets.size(); i++){
            clev.targets[i].setFillColor(Color::Black);
        }
        
        unsigned hit = 69;
        bool done = 0;

        while(!done){

            hitsurface = clev.firstCollision(a1,a2,hit);

            //std::cout << wallhit << std::endl;

            if(hit == 69){
                std::cout << "no collision with anything !!!\n";
                break;
            }

            if(hit < 0)
                done = 1;

            
            b1 = hitsurface.first, b2 = hitsurface.second;
            

            Vector2f collision_v = calcIntersectVector(a1,a2,b1,b2);

            Color beamcolor = clev.getGun(s).getFillColor();

            for(RectangleShape target : clev.targets){
                if(segmentIntersectsRectangle(target.getGlobalBounds(),a1,collision_v)){                    
                    target.setFillColor(beamcolor);
                }
            }

            VertexArray shot(LineStrip,2);
            shot[0].position = a1;
            shot[0].color = beamcolor;
            shot[1].position = collision_v;
            shot[1].color = beamcolor;

            shots[s].push_back(shot);

            a2 = newPivot(a1,collision_v,b1,b2);

            //std::cout << "new piv: ";
            //printVector2f(a2);

            //std::cout << "collision pt: ";
            a1 = collision_v;
            //printVector2f(a1);

        }

    }
}

void eventHandler(){
    Event e;
    while(window.pollEvent(e)){
        if (e.type == Event::Closed)
            window.close();

        if(e.type == Event::MouseWheelScrolled && mainMenu){
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
                    target.setPosition(newpos);
                    clev.targets.push_back(target);
                    targetPlace = 0;
                    showMenu = 1;
                }else
                if(wallBuild){
                    wallBuild++;
                    if(wallBuild==2){
                        newWall[0].position = newpos;                                                     
                    }else if(wallBuild == 3){
                        newWall[1].position = newpos;
                        clev.walls.push_back(newWall);
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
                            if(clev.walls.size()>4)
                                clev.walls.pop_back();
                        }
                        if(delTarget.getGlobalBounds().contains(newpos)){                                                             
                            //std::cout << "deleting Target" << std::endl;
                            if(!clev.targets.empty())
                                clev.targets.pop_back();
                        }
                        if(save.getGlobalBounds().contains(newpos)){
                            clev.saveLevel();
                            levels.clear();
                            levelselector.clear();    
                            selectorText.clear();
                            levels.initLevels(screenx,screeny);
                            //std::cout << "saving level" << std::endl;                                
                        }
                        if(leveltext.getGlobalBounds().contains(newpos)){
                            editlevelname = 1;
                            //std::cout << "editing level" << std::endl;
                        }
                    }
                }else //cant change pivot if menu is open
                if(abs(newpos.x-clev.getGun(selection).getPosition().x) > 0.12f && abs(newpos.y-clev.getGun(selection).getPosition().y) > 0.12f
                    && dist(newpos-clev.getGun(selection).getPosition()) > 10.f)
                    clev.getPivot(selection).setPosition(Vector2f(Mouse::getPosition()-windowoffset)); 
            }
            if(Mouse::isButtonPressed(Mouse::Right)){
                if(showMenu && level == 0){
                    clev.getGun(selection).setPosition(newpos);
                }
            }
        }
        if(e.type == Event::KeyPressed){
            switch(e.key.code){
                case Keyboard::Tab:
                    if(!mainMenu && !wallBuild && !targetPlace)                        
                        showMenu = !showMenu;
                    
                    break;
                case Keyboard::BackSpace:
                    if(editlevelname && !clev.name.empty())
                        clev.name.pop_back();
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
                clev.name += e.text.unicode;
        }
    }
}

int main(){
    init();
    
    while(window.isOpen()){

        eventHandler();        
        calcShots();
        render();
    }
    return 0;
}
