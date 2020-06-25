#include "CLI.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <direct.h>
#include <sys/stat.h>

extern std::string g_CurrentSaveFile;

CLI::CLI(const sf::Rect<float> bounds, std::string commandHistory,sf::Color bgColor,sf::Color textColor,sf::Font font,int characterSize)
 : Object(bounds) , m_commandHistoryStr(commandHistory) ,m_font(font) ,m_isTextEditor(false)
{

    m_textEditor = new TextEditor(sf::Rect<float>(bounds.left,bounds.top,bounds.width,bounds.height-characterSize),bgColor,textColor,m_font,characterSize);

    auto glyph = font.getGlyph('(',characterSize,false);
    int characterDecenderHeight = glyph.bounds.top+glyph.bounds.height;

    m_commandHistoryText.setPosition(m_bounds.left,m_bounds.top-characterDecenderHeight);
    m_commandHistoryText.setFillColor(textColor);
    m_commandHistoryText.setFont(m_font);
    m_commandHistoryText.setCharacterSize(characterSize);
    m_commandHistoryText.setString("");

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
    m_commandText.setString(">");
    
    m_bgRect.setPosition(m_bounds.left,m_bounds.top);
    m_bgRect.setSize(sf::Vector2f(m_bounds.width,m_bounds.height-characterSize));
    m_bgRect.setFillColor(bgColor);
}
CLI::~CLI()
{
    delete m_textEditor;
}
//--------------------------------------------------------------------------------------------
//Update
void CLI::Update()
{}

void CLI::Update(const sf::Event &event)
{
    if(m_isTextEditor)
    {
        m_textEditor->Update(event);
        if(event.type == sf::Event::TextEntered)
        {
            if(event.text.unicode == 19)//ctrl-s
            {
                std::ofstream ofs("Assets/Saves/" + g_CurrentSaveFile + "/codes/" + m_currentFile,std::ios::out | std::ios::trunc);
                ofs << m_textEditor->getText();
                ofs.close();
            }
            else if(event.text.unicode == 17)//ctrl-q
            {
                m_isTextEditor = false;
                m_commandText.setString(">");
            }
        }
    }
    else 
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
            else if(isalnum(event.text.unicode) || event.text.unicode == ' ' || event.text.unicode == '.' || event.text.unicode == '_' || event.text.unicode == '-')
            {
                m_commandStr += tolower(event.text.unicode);
            }
            if(!m_isTextEditor)
                m_commandText.setString(">" + m_commandStr);
        }
    }
}
//--------------------------------------------------------------------------------------------
//Render
void CLI::Render(sf::RenderWindow &window)
{
    window.draw(m_bgRect);
    if(!m_isTextEditor)
    {
        window.draw(m_commandHistoryText);
    }
    else
    {
        m_textEditor->Render(window);
    }
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

    strPos = endPos;
    while(strPos < m_commandStr.size() && isspace(m_commandStr[strPos]))
        strPos++;
    endPos = m_commandStr.size();

    std::string secondArg(m_commandStr.substr(strPos,endPos-strPos));

    PushCommandHistory(m_commandStr);

    m_commandStr = "";

    if(firstArg == "help")
    {
        PushCommandHistory("");
        PushCommandHistory("--------- here to help you ---------");
        PushCommandHistory("help        Displays this information");
        PushCommandHistory("manual      Opens the coding manual (external pdf)");
        PushCommandHistory("list        Lists the code database");
        PushCommandHistory("stop        Stops the running code");
        PushCommandHistory("run    <fn> Runs the code inside <fn>");
        PushCommandHistory("create <fn> Creates a file named <fn>");
        PushCommandHistory("edit   <fn> Opens <fn> for editing");
        PushCommandHistory("delete <fn> Deletes <fn>");
    }
    else if(firstArg == "manual")
    {
        system("cd Assets && Manual.pdf");
    }
    else if(firstArg == "list")
    {
        PushCommandHistory("will get you the list of the codes in the future");
    }
    else if(firstArg == "stop")
    {
        PushCommandHistory("will stop in the future");
    }
    else if(firstArg == "create")
    {
        if(secondArg.size() <= 0)
        {
            PushCommandHistory("File name cannot be empty");
            return;
        }
        std::string name("Assets/Saves/" + g_CurrentSaveFile + "/codes/" + secondArg);
        struct stat buffer;   
        if(stat (name.c_str(), &buffer) == 0)//check if file exists
        {
            PushCommandHistory(secondArg + ": File already exist");
            return;
        }
        else
        {
            m_textEditor->setText("");
        }
        
        mkdir("Assets/Saves");
        mkdir(("Assets/Saves/" + g_CurrentSaveFile).c_str());
        mkdir(("Assets/Saves/" + g_CurrentSaveFile + "/codes").c_str());
        std::ofstream ofs("Assets/Saves/" + g_CurrentSaveFile + "/codes/" + secondArg,std::ios::out | std::ios::trunc);
        ofs.close();

        PushCommandHistory(secondArg + ": Successfully created");
    }
    else if(firstArg == "edit")
    {
        if(secondArg.size() <= 0)
        {
            PushCommandHistory("File name cannot be empty");
            return;
        }
        std::ifstream ifs("Assets/Saves/" + g_CurrentSaveFile + "/codes/" + secondArg);
        if(ifs.is_open())
        {
            std::string text((std::istreambuf_iterator<char>(ifs) ),(std::istreambuf_iterator<char>()));
            m_textEditor->setText(text);
        }
        else
        {
            PushCommandHistory(secondArg + ": File doesn't exist");
            return;
        }
        
        m_isTextEditor = true;
        m_currentFile = secondArg;
        m_commandText.setString("             ctrl+Q to quit without saving     ctrl+S to save");
    }
    else if(firstArg == "delete")
    {
        if(remove(("Assets/Saves/" + g_CurrentSaveFile + "/codes/" + secondArg).c_str()))
            PushCommandHistory(secondArg + ": File doesn't exist");
        else
            PushCommandHistory(secondArg + ": File seccessfully deleted");
    }
    else if(firstArg == "run")
    {
        if(secondArg.size() <= 0)
        {
            PushCommandHistory("File name cannot be empty");
            return;
        }
        PushCommandHistory("will run " + secondArg + " in the future");
    }
    else
    {
        PushCommandHistory(firstArg + ": Command not found");
    }

}