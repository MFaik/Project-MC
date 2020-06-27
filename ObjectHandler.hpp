#pragma once

#include "Object.hpp"

#include <map>

class ObjectHandler
{
private:
    std::map<int ,Object*> UIObjects;
    std::map<int ,Object*> GameObjects;
    int UICount = 0,GameObjectCount = 0;
public:
    ObjectHandler();
    ~ObjectHandler();
    void Update();
    void Update(const sf::Event &event);
    void Render(sf::RenderWindow &window);
    int AddObject(Object *const object, bool isUI);
    Object* GetGameObject(int id);
    Object* GetUIObject(int id);
    void DeleteGameObject(int id);
    void DeleteUIObject(int id);
};