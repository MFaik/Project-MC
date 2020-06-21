#pragma once

#include <SFML/Graphics.hpp>

class Object
{
protected:
    sf::Rect<float> m_bounds;
public:
    Object(const sf::Rect<float> &bounds);
    virtual ~Object() = 0;
    virtual void Update() = 0;
    virtual void Update(const sf::Event &event) = 0;
    virtual void Render(sf::RenderWindow &window) = 0;

};