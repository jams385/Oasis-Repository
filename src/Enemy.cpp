#include "Enemy.h" 

//initializes an enemy
Enemy::Enemy(sf::Vector2f startPos, float speed, float hp)
    : position(startPos), speed(speed), hp(hp), alive(true)
{
    shape.setRadius(18.f);
    shape.setOrigin(18.f, 18.f);
    shape.setFillColor(sf::Color(210, 180, 140));
    shape.setPosition(position);
}

//updates enemy position
void Enemy::update(float dt) {
    position.x += speed * dt;
    shape.setPosition(position);
}

//renders enemy and removes once dead
void Enemy::draw(sf::RenderWindow& window) {
    
    if (!alive){ 
        return;
    }

    window.draw(shape);
}

//checks if enemy is alive
bool Enemy::isAlive() const { 
    return alive; 
}

/*  Gets the current position of the enemy.
    is used to for targeting.   
*/
sf::Vector2f Enemy::getPosition() const { 
    return position; 
}
  

// Takes away the hp of an enemy
void Enemy::takeDamage(float amount) {
    hp -= amount;
    if (hp <= 0) alive = false;
}