#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
#include <locale>
#include "level.h"
using namespace sf;

using surface = std::pair<Vector2f,Vector2f>;

Clock c;
unsigned screenx = 1280, screeny = 720, fps = 24, level = 0, curr = 0, wallBuild = 0, botmenu = 0, maxric = 69;
bool rclicking = 0, shot = 0, showMenu = 0, mainMenu = 1, targetPlace = 0, editlevelname = 0, playing = 0, shift = 0, ctrl = 0;
Vector2f initclick;
Vector2f two(2.f,2.f);
Vector2f d_vec;

Font font;
Text leveltext, reflectables_left;

std::vector<RectangleShape> levelselector;
std::vector<Text> selectorText;

std::vector<std::vector<VertexArray>> shots; 
Color cColor;
leveld clev;

Transformer reflectable(4);
Transformer noplacezone(4);
Transformer target(6);
Transformer beam(3);

VertexArray newWall(LinesStrip,2);

Texture backText, saveText, mainScreenText, buildText, delWalltext; 
Sprite goBack, save, mainScreen, buildwall, delWall;

RenderWindow window(VideoMode(screenx,screeny), "Ricochet", Style::Close);

View view(window.getDefaultView());

FloatRect bounds;
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
        leveltext.setPosition(Vector2f(225.f,25.f));
    else{
        leveltext.setPosition(Vector2f(75.f,25.f));
        playing = 1;
    }

    leveltext.setString(clev.name);
    shots = std::vector<std::vector<VertexArray>>(clev.beams.size());
    reflectables_left.setString(std::to_string(clev.reflectorsLeft()));    
}

void init(){//called once at startup
    levels.initLevels(screenx,screeny);    
    font.loadFromFile("resources/arial.ttf");
            
    initMenu();
    load();
    window.setFramerateLimit(fps);
    bounds.left = 0.f;
    bounds.top = 0.f;
    bounds.height = screeny;
    bounds.width = screenx;

    buildText.loadFromFile("resources/buildwall.png");
    //placeText.loadFromFile("resources/placeTarget.png");
    backText.loadFromFile("resources/goback.png");
    //delTargettext.loadFromFile("resources/delTarget.png");
    delWalltext.loadFromFile("resources/delWall.png");
    saveText.loadFromFile("resources/save.png");
    mainScreenText.loadFromFile("resources/logo.png");
    

    buildwall.setTexture(buildText);
    //placeTarget.setTexture(placeText);
    goBack.setTexture(backText);
    //delTarget.setTexture(delTargettext);
    delWall.setTexture(delWalltext);
    save.setTexture(saveText);
    mainScreen.setTexture(mainScreenText);
    mainScreen.setOrigin(mainScreen.getLocalBounds().width/2.f,mainScreen.getLocalBounds().height/2.f);

    goBack.setPosition(Vector2f(25.f,25.f));
    buildwall.setPosition(Vector2f(75.f,25.f));
    //placeTarget.setPosition(Vector2f(125.f,25.f));
    delWall.setPosition(Vector2f(125.f,25.f));
    //delTarget.setPosition(Vector2f(225.f,25.f));
    save.setPosition(Vector2f(175.f,25.f));
    mainScreen.setPosition(Vector2f(view.getCenter().x,100.f)); 

    leveltext.setFont(font);
    leveltext.setCharacterSize(20);
    leveltext.setFillColor(Color::White);
    reflectables_left.setFont(font);
    reflectables_left.setCharacterSize(20);
    reflectables_left.setFillColor(Color::White);
    reflectables_left.setOrigin(6.f,11.f);

    
    reflectable.setPosition(Vector2f(screenx-45.f,45.f));
    
    reflectables_left.setPosition(reflectable.getOrigin());
    
    noplacezone.setPosition(Vector2f(screenx-85.f,45.f));
    noplacezone.setFillColor(Color::Red);
    

    beam.setPosition(Vector2f(screenx-125.f,45.f));
    target.setPosition(Vector2f(screenx-165.f,45.f));
}

void render(){
    window.clear();

    clev.cleanObjects(bounds,showMenu);
    
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

    for(Transformer &npzone : clev.noplacezones){
        if(npzone.selected)
            npzone.setPosition(mousepos);
        if(npzone.rotating)
            npzone.setPivot(mousepos);
        if(npzone.resizing)
            npzone.sizeTo(mousepos);
        if(!level)
            npzone.drawDull(window);
        else
            npzone.drawDullShape(window);
    }

    for(unsigned i = 0; i < shots.size(); i++){        
        for(unsigned line = 0; line < shots[i].size(); line++){
            window.draw(shots[i][line],BlendAdd); 
        }
    }          

    for(unsigned i = 0; i < clev.walls.size(); i++){
        window.draw(clev.walls[i]);
        
    }

    if(!level){
        for(Transformer &i : clev.beams){
            if(i.selected)
                i.setPosition(mousepos);
            if(i.rotating)
                i.setPivot(mousepos);            
            i.draw(window);                   
        }
        for(Transformer &t : clev.targets){
            if(t.selected)
                t.setPosition(mousepos);
            t.draw(window);
        }
        reflectables_left.setString(std::to_string(clev.reflectors.size()));                 
    }else{
        reflectables_left.setString(std::to_string(clev.reflectorsLeft()));
        for(Transformer &i : clev.beams){            
            i.draw(window);            
        }
        for(Transformer &i : clev.targets){            
            i.drawShape(window);            
        }        
    }

    for(Transformer &r : clev.reflectors){
        if(r.selected)
            r.setPosition(mousepos);
        if(r.rotating){
            r.setPivot(mousepos);
        }
        
        r.draw(window);   
                       
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
        if(!level){
            window.draw(buildwall);            
            window.draw(delWall);            
            window.draw(save);
            beam.draw(window);
            target.draw(window);  
            reflectable.drawShape(window); 
            noplacezone.drawDullShape(window); 
        }else if(clev.reflectorsLeft()){
            reflectable.drawShape(window);           
        }
        window.draw(leveltext);
        window.draw(reflectables_left);
    }

    if(wallBuild == 2){
        newWall[1].position = mousepos;
        window.draw(newWall);
    }

    if(targetPlace){
        target.setPosition(mousepos);
        target.draw(window);
    } 

    window.display();
}

void calcShots(){
    Vector2f a1,a2,b1,b2;
    surface hitsurface;

    for(unsigned i = 0; i < clev.targets.size(); i++){
        clev.targets[i].setFillColor(Color::Black);
    }

    for(unsigned s = 0; s < clev.beams.size(); s++){
        shots[s].clear();
        
        for(unsigned c = 0; c < 3; c++){            

            Color beamcolor = clev.beams[s].getOutlineColor();

            
            if(!c){
                if(beamcolor.r == 255)
                    beamcolor = Color::Red;
                else
                    continue;
            }
            else if(c == 1){
                if(beamcolor.g == 255)
                    beamcolor = Color::Green;
                else
                    continue;
            }
            else if(c == 2){
                if(beamcolor.b == 255)
                    beamcolor = Color::Blue;
                else
                    continue;
            }

            a1 = clev.beams[s].getOrigin();
            a2 = clev.beams[s].getPivot();

            int hit = 69;
            bool done = 0;
            unsigned count = 0;

            while(!done && count != maxric){

                hitsurface = clev.firstCollision(a1,a2,hit,beamcolor);

                //std::cout << wallhit << std::endl;

                if(hit == 69){
                    std::cout << "no collision with anything !!!\n";
                    break;
                }

                if(hit < 0)
                    done = 1;

                
                b1 = hitsurface.first, b2 = hitsurface.second;
                
                Vector2f collision_v = calcIntersectVector(a1,a2,b1,b2);

                for(Transformer &target : clev.targets){
                    if(target.hit(a1,collision_v)){                    
                        target.setFillColor(target.getFillColor()+beamcolor);
                    }
                }

                VertexArray shot(TriangleStrip,4);
                //beamcolor.a = 120/(s+1)+80;


                d_vec = signsVector(a1,a2);
                            
                
                shot[0].position = a1 - d_vec;            
                shot[0].color = beamcolor;
                shot[1].position = collision_v - d_vec;
                shot[1].color = beamcolor;
                shot[2].position = a1 + d_vec;            

                shot[3].position = collision_v + d_vec;
                
                shot[2].color = beamcolor;
                shot[3].color = beamcolor;
                
                shots[s].push_back(shot);

                a2 = newPivot(a1,collision_v,b1,b2);

                a1 = collision_v;

                count++;
            }
        }
    }
}

void eventHandler(){
    Event e;
    while(window.pollEvent(e)){
        bool nomore = 0;
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
                    if(reflectable.isClicked(newpos)){
                        if(!level || clev.reflectorsLeft()){                            
                            reflectable.selected = 1;
                            clev.reflectors.push_back(reflectable);
                            reflectable.selected = 0;
                            showMenu = 0;                            
                            nomore = 1;
                        }
                        if(nomore)
                            continue;                            
                    }
                    if(goBack.getGlobalBounds().contains(newpos)){
                        mainMenu = 1;
                        //std::cout << "going back" << std::endl;
                    }
                    for(Transformer &r : clev.reflectors){
                        if(r.isClicked(newpos)){
                            if(shift)
                                r.toggleColor(newpos);
                            else{
                                r.selected = 1;
                                showMenu = 0;
                                nomore = 1;
                                break;   
                            }                     
                        }
                    }
                    if(nomore)
                        continue; 
                    if(!level){

                        if(buildwall.getGlobalBounds().contains(newpos)){
                            showMenu = 0;
                            //std::cout << "building wall" << std::endl;
                            wallBuild = 1;
                        }                        
                        if(delWall.getGlobalBounds().contains(newpos)){                                                             
                            //std::cout << "deleting wall" << std::endl;
                            if(clev.walls.size()>4)
                                clev.walls.pop_back();
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
                        
                        if(beam.isClicked(newpos)){                                
                            if(shift)
                                beam.toggleColor(newpos);
                            else{
                                beam.selected = 1;
                                clev.beams.push_back(beam);
                                shots = std::vector<std::vector<VertexArray>>(clev.beams.size());
                                beam.selected = 0;
                                showMenu = 0;
                                nomore = 1;
                            }
                        }else{                        
                            for(Transformer &b : clev.beams){
                                if(b.isClicked(newpos)){
                                    if(shift)
                                        b.toggleColor(newpos);
                                    else{
                                        b.selected = 1;
                                        showMenu = 0;                                                                          
                                    }
                                    nomore = 1;
                                    break;                                    
                                }
                            }
                        }
                        if(nomore)
                            continue;
                        if(target.isClicked(newpos)){
                            if(shift){
                                target.toggleColor(newpos);                                
                            }else{
                                target.selected = 1;
                                clev.targets.push_back(target);
                                target.selected = 0;
                                showMenu = 0;
                                nomore = 1;
                            }
                        }else{
                            for(Transformer &b : clev.targets){
                                if(b.isClicked(newpos)){
                                    if(shift)
                                        b.toggleColor(newpos);
                                    else{
                                        b.selected = 1;
                                        showMenu = 0;                                    
                                    }
                                    nomore = 1;
                                    break;                                    
                                }
                            }
                        }
                        if(nomore)
                            continue;
                        if(noplacezone.isClicked(newpos)){
                            if(shift){
                                noplacezone.toggleFillColor(newpos);                                
                            }else{
                                noplacezone.selected = 1;
                                clev.noplacezones.push_back(noplacezone);
                                noplacezone.selected = 0;
                                showMenu = 0;
                            }
                            nomore = 1;
                        }else{
                            for(Transformer &npzone : clev.noplacezones){
                                if(npzone.isClicked(newpos)){
                                    if(shift){
                                        npzone.toggleFillColor(newpos);                                        
                                    }else
                                    if(ctrl){
                                        npzone.resizing = 1;
                                        showMenu = 0;
                                    }
                                    else{
                                        npzone.selected = 1;
                                        showMenu = 0;                                    
                                    }
                                    nomore = 1;
                                    break;
                                }
                            }
                        }
                        if(nomore)
                            continue;
                        
                    }
                    
                    
                }
                

                            
            }
            if(Mouse::isButtonPressed(Mouse::Right)){
                bool donezo = 0;
                if(showMenu && !level){
                    for(Transformer &b : clev.beams){
                        if(b.isClicked(newpos)){                            
                            b.rotating = 1;                            
                            showMenu = 0;
                            donezo = 1;
                            break;
                        }
                    }
                    if(donezo)
                        continue;
                    for(Transformer &n : clev.noplacezones){
                        if(n.isClicked(newpos)){                            
                            n.rotating = 1;                            
                            showMenu = 0;
                            donezo = 1;
                            break;
                        }
                    }
                }
                if(donezo)
                    continue;
                for(Transformer &r : clev.reflectors){
                    if(r.isClicked(newpos)){                            
                        r.rotating = 1;                        
                        showMenu = 0;
                        break;
                    }
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
                case Keyboard::LShift:
                    shift = 1;
                    break;
                case Keyboard::LControl:
                    ctrl = 1;
                    break;
                default:
                    break;
            }
        }
        if(e.type == Event::KeyReleased){
            switch(e.key.code){
                case Keyboard::LShift:
                    shift = 0;
                    break;
                case Keyboard::LControl:
                    ctrl = 0;
                    break;
                default:
                    break;
            }
        }
        if(e.type == Event::MouseButtonReleased){
            if(!Mouse::isButtonPressed(Mouse::Right)){                
                for(Transformer &b : clev.beams){
                    if(b.rotating){                            
                        b.rotating = 0;
                        showMenu = 1;                        
                        break;
                    }
                }
                for(Transformer &r : clev.reflectors){
                    if(r.rotating){                            
                        r.rotating = 0;                        
                        showMenu = 1;
                        break;
                    }
                }
                for(Transformer &n : clev.noplacezones){
                    if(n.rotating){                            
                        n.rotating = 0;                        
                        showMenu = 1;
                        break;
                    }
                }                             
            }
            if(!Mouse::isButtonPressed(Mouse::Left)){
                for(Transformer &b : clev.beams){
                    if(b.selected){
                        b.selected = 0;
                        showMenu = 1;
                    }
                }
                for(Transformer &r : clev.reflectors){
                    if(r.selected){
                        r.selected = 0;
                        showMenu = 1;
                    }
                }
                for(Transformer &b : clev.targets){
                    if(b.selected){
                        b.selected = 0;
                        showMenu = 1;
                    }
                }
                for(Transformer &n : clev.noplacezones){
                    if(n.selected){                            
                        n.selected = 0;                        
                        showMenu = 1;
                        break;
                    }
                    if(n.resizing){
                        n.resizing = 0;
                        showMenu = 1;
                        break;
                    }
                }
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

        render(); 
        
        if(!mainMenu)      
            calcShots();

        
        curr++;
        if(curr==fps)
            curr = 0;
    }
    
    return 0;
}
