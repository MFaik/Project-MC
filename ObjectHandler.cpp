#include "ObjectHandler.hpp"

#include "TextEditor.hpp"
sf::Font codeFont;
ObjectHandler::ObjectHandler()
{
    
    codeFont.loadFromFile("Assets/Fonts/basis33.ttf");
    TextEditor *textEditor = new TextEditor(sf::Rect<float>{0,0,800,600},sf::Color::Black,sf::Color::Yellow,codeFont,24);
    AddObject(textEditor,true);
}

ObjectHandler::~ObjectHandler()
{
    for(auto it : UIObjects)
    {
        delete it.second;
    }
    UIObjects.clear();

    for(auto it : GameObjects)
    {
        delete it.second;
    }
    GameObjects.clear();
}

void ObjectHandler::Update()
{
    for(auto it : UIObjects)
    {
        it.second->Update();
    }

    for(auto it : GameObjects)
    {
        it.second->Update();
    }
}

void ObjectHandler::Update(const sf::Event &event)
{
    for(auto it : UIObjects)
    {
        it.second->Update(event);
    }

    for(auto it : GameObjects)
    {
        it.second->Update(event);
    }
}

void ObjectHandler::Render(sf::RenderWindow &window)
{
    for(auto it : UIObjects)
    {
        it.second->Render(window);
    }

    for(auto it : GameObjects)
    {
        it.second->Render(window);
    }
}

int ObjectHandler::AddObject(Object *const object, bool isUI)
{
    if(isUI)
    {
        UIObjects[UICount++] = object;
    }
    else
    {
        GameObjects[GameObjectCount++] = object;
    }
    
}

Object* ObjectHandler::GetObject(int id, bool isUI)
{
    if(isUI)
    {
        if(UIObjects.find(id) != UIObjects.end())
            return UIObjects[UICount];
    }
    else
    {
        if(GameObjects.find(id) != GameObjects.end())
            return GameObjects[UICount];
    }
}

void ObjectHandler::DeleteObject(int id, bool isUI)
{
    if(isUI)
    {
        if(UIObjects.find(id) != UIObjects.end())
            delete UIObjects[UICount];
        UIObjects.erase(id);
    }
    else
    {
        if(GameObjects.find(id) != GameObjects.end())
            delete GameObjects[UICount];
        GameObjects.erase(id);
    }
}