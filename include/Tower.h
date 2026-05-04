#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Enemy.h"

// bullet struct
struct Bullet {
    sf::Vector2f pos;
    sf::Vector2f vel;
    bool alive = true;
};

class Tower {
public:
    //function that initializes the tower's stats
    Tower(sf::Vector2f pos, float range, float fireRate, float damage);

    /*  function that updates things like:
        - tick cooldown
        - bullets                           */
    void update(float dt, std::vector<Enemy>& enemies);

    //function that draws the tower into the window
    void draw(sf::RenderWindow& window);

private:
    sf::Vector2f position;
    float        range;
    float        fireRate;    
    float        damage;
    float        cooldown;

    sf::CircleShape    shape;
    sf::CircleShape    rangeCircle;
    std::vector<Bullet> bullets;

    
    Enemy* findTarget(std::vector<Enemy>& enemies);
};