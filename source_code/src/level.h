#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
using namespace sf;

struct leveld
{
    std::string name;
    Vector2f center;
    std::vector<RectangleShape> enemies;
    std::vector<VertexArray> walls;
};