#include "CLI.hpp"

#include <iostream>
#include <string>

CLI::CLI(const sf::Rect<float> &bounds, std::string commandHistory,sf::Color bgColor,sf::Color textColor,sf::Font font,int characterSize)
 : Object(bounds) , m_commandHistoryStr(commandHistory) ,m_font(font)
{
    auto glyph = font.getGlyph('(',characterSize,false);
    int characterDecenderHeight = glyph.bounds.top+glyph.bounds.height;

    m_commandHistoryText.setPosition(m_bounds.left,m_bounds.top-characterDecenderHeight);
    m_commandHistoryText.setFillColor(textColor);
    m_commandHistoryText.setFont(m_font);
    m_commandHistoryText.setCharacterSize(characterSize);

    int lineCount = std::count(m_commandHistoryStr.begin(), m_commandHistoryStr.end(), '\n');
    int desiredLineCount = m_bounds.height/characterSize - 2;
    if(lineCount < desiredLineCount)
    {
        int padLineCount =  desiredLineCount - lineCount;
        m_commandHistoryStr.insert(0,padLineCount,'\n');
    }
    else if(lineCount > desiredLineCount)
    {
        int deletedLineCount = 0;
        for(int i = 0;deletedLineCount < lineCount-desiredLineCount;i++)
        {
            if(m_commandHistoryStr[i] == '\n')
            {
                m_commandHistoryStr.erase(m_commandHistoryStr.begin()+i);
                deletedLineCount++;
            }
            else
                m_commandHistoryStr.erase(m_commandHistoryStr.begin()+i);
        }
    }
    

    m_commandHistoryText.setString(m_commandHistoryStr);

    m_commandText.setPosition(m_bounds.left,m_bounds.top+m_bounds.height-m_commandHistoryText.getCharacterSize()-characterDecenderHeight);
    m_commandText.setFillColor(textColor);
    m_commandText.setFont(m_font);
    m_commandText.setCharacterSize(characterSize);
    
    m_bgRect.setPosition(m_bounds.left,m_bounds.top);
    m_bgRect.setSize(sf::Vector2f(m_bounds.width,m_bounds.height-characterSize));
    m_bgRect.setFillColor(bgColor);

    textEditor = new TextEditor(sf::Rect<float>(bounds.left,bounds.top,bounds.width,bounds.height-characterSize),bgColor,textColor,font,characterSize);
}
CLI::~CLI()
{
    delete textEditor;
}
//--------------------------------------------------------------------------------------------
//Update
void CLI::Update()
{}

void CLI::Update(const sf::Event &event)
{
    if(event.type == sf::Event::TextEntered)
    {
        if(event.text.unicode == 13)//Enter
        {
            if(m_commandStr.size() > 0)
                RunCommand();
        }
        else if(event.text.unicode == 9)//Tab
        {
            
        }
        else if(event.text.unicode == 8)//Back Space
        {
            if(m_commandStr.size() > 0)
                m_commandStr.erase(m_commandStr.size()-1,1);
        }
        else if(isalnum(event.text.unicode) || event.text.unicode == ' ')
        {
            m_commandStr += tolower(event.text.unicode);
        }
        std::cout << event.text.unicode;
        m_commandText.setString(m_commandStr);
    }
    
}
//--------------------------------------------------------------------------------------------
//Render
void CLI::Render(sf::RenderWindow &window)
{
    window.draw(m_bgRect);

    window.draw(m_commandHistoryText);
    
    window.draw(m_commandText);
}
//--------------------------------------------------------------------------------------------
//PushCommandHistory
void CLI::PushCommandHistory(std::string str)
{
    auto it = m_commandHistoryStr.find('\n');
    m_commandHistoryStr.erase(m_commandHistoryStr.begin(),m_commandHistoryStr.begin()+it+1);
    m_commandHistoryStr += "\n";
    m_commandHistoryStr += str;
    m_commandHistoryText.setString(m_commandHistoryStr);
}
//--------------------------------------------------------------------------------------------
//RunCommand
void CLI::RunCommand()
{

    int strPos = 0,endPos = 0;
    while(strPos < m_commandStr.size() && isspace(m_commandStr[strPos]))
        strPos++;
    endPos = strPos;
    while(endPos < m_commandStr.size() && isalpha(m_commandStr[endPos]))
        endPos++;
    std::string firstArg(m_commandStr.substr(strPos,endPos-strPos));

    while(strPos < m_commandStr.size() && isspace(m_commandStr[strPos]))
        strPos++;
    endPos = m_commandStr.size()-1;

    std::string secondArg(m_commandStr.substr(strPos,endPos-strPos));

    if(firstArg == "help")
    {
        PushCommandHistory("");
        PushCommandHistory("-------- here to help you --------");
        PushCommandHistory("help       Displays this information");
        PushCommandHistory("manual     Opens the coding manual (external pdf)");
        PushCommandHistory("list       Lists the code Database");
        PushCommandHistory("stop       Stops the current code");
        PushCommandHistory("run  <fn>  Runs the code inside <fn>");
        PushCommandHistory("edit <fn>  Edits <fn>. If <fn> doesn't exist creats it");
    }
    else if(firstArg == "manual")
    {
        system("cd Assets && Manual.pdf");
    }
    else if(firstArg == "list")
    {
        PushCommandHistory("got you the list of the codes ;)");
    }
    else if(firstArg == "stop")
    {
        PushCommandHistory("successfully stopped");
    }
    else if(firstArg == "edit")
    {
        PushCommandHistory("gonna edit" + secondArg);
    }
    else
    {
        PushCommandHistory(firstArg + ": command not found");
    }

    m_commandStr = "";

}