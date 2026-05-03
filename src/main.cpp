#include <SFML/Graphics.hpp>

struct Bullet
{
    sf::CircleShape shape;
    sf::Vector2f direction;
    float speed;
};

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(1280, 720), "My window");

    //initializes blue cirlce
    sf::CircleShape tower(50.f);
    tower.setFillColor(sf::Color::Blue);
    tower.setOrigin(tower.getRadius(), tower.getRadius());     
    tower.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);

    //initializes red circle
    sf::CircleShape enemy(30.f);
    enemy.setFillColor(sf::Color::Red);
    enemy.setOrigin(enemy.getRadius(), enemy.getRadius());
    enemy.setPosition(window.getSize().x / 4.f, window.getSize().y / 4.f);

    std::vector<Bullet> bullets; //container

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed){
                window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space){
                
                sf::Vector2f dir = enemy.getPosition() - tower.getPosition();
                float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                dir /= len;

                Bullet b;
                b.shape = sf::CircleShape(5.f);
                b.shape.setFillColor(sf::Color::Cyan);
                b.shape.setOrigin(5.f, 5.f);
                b.shape.setPosition(tower.getPosition());
                b.direction = dir;
                b.speed = 4.f;

                bullets.push_back(b);
            }

        }

        for (auto& b : bullets)
            b.shape.move(b.direction * b.speed);

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draws the actual shapes
        window.draw(tower);
        window.draw(enemy);

        for(auto& b : bullets)
            window.draw(b.shape);

        window.display();
    }

    return 0;
}