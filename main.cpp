#include "ObjectHandler.hpp"

#include <SFML/Graphics.hpp>

ObjectHandler g_Handler;
std::string g_CurrentSaveFile = "0w0";

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
            g_Handler.Update(event);
        }

        g_Handler.Update();
        
        window.clear(sf::Color::Black);

        g_Handler.Render(window);

        window.display();
    }
}