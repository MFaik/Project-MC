#pragma once
#include "../Object.hpp"
#include "TextEditor.hpp"

#include <SFML/Graphics.hpp>

#include <dirent.h> 

class CLI : public Object
{
private:
    std::string m_commandHistoryStr;
    sf::Text m_commandHistoryText;
    sf::Text m_commandText;
    sf::Font m_font;

    std::string m_commandStr;

    int m_desiredLineCount;

    bool m_isListing;
    DIR *m_dr;
    struct dirent *m_de;

    sf::RectangleShape m_bgRect;

    TextEditor *m_textEditor;

    bool m_isTextEditor;
    std::string m_currentFile;

    void PushCommandHistory(std::string str);
    void RunCommand();
    void CompleteCommand(std::string& str);
public:
    CLI(const sf::Rect<float> bounds, std::string commandHistory,sf::Color bgColor,sf::Color textColor,sf::Font font,int characterSize);
    ~CLI();
    void Update() override;
    void Update(const sf::Event &event) override;
    void Render(sf::RenderWindow &window) override;
};

