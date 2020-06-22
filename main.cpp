#include "ObjectHandler.hpp"

#include <SFML/Graphics.hpp>

ObjectHandler Handler;
int main()
{
    sf::RenderWindow window(sf::VideoMode(800,600),"Text Editor >:(");

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
            Handler.Update(event);
        }

        Handler.Update();
        
        window.clear(sf::Color::Black);

        Handler.Render(window);

        window.display();
    }
}