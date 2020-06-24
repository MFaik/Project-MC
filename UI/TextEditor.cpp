#include "TextEditor.hpp"

#include <SFML/Graphics.hpp>

#include <cmath>
#include <string>
#include <iostream>

TextEditor::TextEditor(const sf::Rect<float> &bounds,sf::Color bgColor,sf::Color textColor,sf::Font font,int characterSize)
 : Object(bounds) , m_characterHeight(characterSize) , m_textColor(textColor)
{
    m_Text.push_back("");
    m_cursor.lineIt = m_Text.begin();
    m_cursor.x = 0;
    m_cursor.y = 0;
    m_selectEnd = m_cursor;
    m_selectStart = m_cursor;
    m_selectLil = m_cursor;
    m_selectBig = m_cursor;

    m_maxHistoryLimit = 50;



    PushHistory();

    m_characterWidth = 0;
    m_characterDecenderHeight = 0;
    for(int i = 32;i < 128;i++)
    {
        m_charGlyphs[i] = font.getGlyph((char)i,m_characterHeight,false);
        if(m_charGlyphs[i].bounds.width > m_characterWidth)
            m_characterWidth = m_charGlyphs[i].bounds.width;
        if(m_charGlyphs[i].bounds.top+m_charGlyphs[i].bounds.height > m_characterDecenderHeight)
            m_characterDecenderHeight = m_charGlyphs[i].bounds.top+m_charGlyphs[i].bounds.height;
    }

    m_renderStartLine = 0;
    m_renderedChars = bounds.width/m_characterWidth;
    m_renderedLines = bounds.height/m_characterHeight;
    m_renderStartIterator = m_Text.begin();

    m_fontSprite.setColor(textColor);
    m_fontTexture = font.getTexture(m_characterHeight);
    m_fontSprite.setTexture(m_fontTexture);
    
    m_textColorInverted = sf::Color(255-textColor.r,255-textColor.g,255-textColor.b);

    m_bgRect.setFillColor(bgColor);
    m_bgRect.setSize(sf::Vector2f(bounds.width,bounds.height));
    m_bgRect.setPosition(bounds.left,bounds.top);

    m_selectbgRect.setFillColor(sf::Color(255-bgColor.r,255-bgColor.g,255-bgColor.b));
}

TextEditor::~TextEditor()
{
}
//--------------------------------------------------------------------------------------------
//Get Text
std::string TextEditor::getText()
{
    std::string ret = "";
    for(std::string str : m_Text)
    {
        ret += str + "\n";
    }
    ret += "\0";
    return ret;
}
std::list<std::string>& TextEditor::getTextList()
{
    return m_Text;
}
//--------------------------------------------------------------------------------------------
//Set Text
void TextEditor::setText(const std::string&str)
{
    m_Text.clear();
    m_Text.push_back("");
    auto it = m_Text.begin();
    for(char i : str)
    {
        if(i != '\n')
        {
           *it += i;
        }else
        {
           m_Text.push_back("");
           it++;
        }
    }
}
void TextEditor::Update()
{
}
//--------------------------------------------------------------------------------------------
//Update
void TextEditor::Update(const sf::Event &event)
{
    if(event.type == sf::Event::TextEntered)
    {
        if(event.text.unicode == 26)//ctrl-z
        {
            if(m_History.size() <= 1)
                return;
            m_historyReadOnly = true;
            DeSelect(0);
            for(char i : m_History.back()->goBackStr)
            {
                CharWriter(i);
            }
            delete m_History.back();
            m_History.pop_back();
            m_historyReadOnly = false;
            m_lastCharType = -1;
        }
        else if(event.text.unicode == 24)//ctrl-x
        {
            CopySelection();
            DeleteSelection();
        }
        else if(event.text.unicode == 22)//ctrl-v
        {
            PasteSelection();
        }
        else if(event.text.unicode == 3)//ctrl-c
        {
            CopySelection();
        }
        else if(event.text.unicode <= 127)//chars
        {
            if(GetCharType(event.text.unicode) != m_lastCharType)
            {
                m_lastCharType = GetCharType(event.text.unicode);
                m_History.back()->cursor = m_cursor;
                PushHistory();
            }
            CharWriter(event.text.unicode);
        }
    }
    else if(event.type == sf::Event::KeyPressed)
    {
        if(event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)
        {
            if(m_History.size() > 0)
                if(m_History.back()->goBackStr.size() > 0)
                    PushHistory();
            
            if(!m_selecting)
            {
                if(event.key.code == sf::Keyboard::Right && (m_selectBig.x != m_selectLil.x || m_selectBig.y != m_selectLil.y))
                {
                    DeSelect(1);
                    return;
                }
                else if(event.key.code == sf::Keyboard::Left && (m_selectBig.x != m_selectLil.x || m_selectBig.y != m_selectLil.y))
                {
                    DeSelect(-1);
                    return;
                }
                else
                    DeSelect(0);
            }
            
            if(event.key.code == sf::Keyboard::Right)
            {
                int charType = GetCharType((*m_cursor.lineIt)[m_cursor.x]);
                CursorMover(sf::Vector2i(1,0),0);
                if(event.key.control)
                {
                    while(m_cursor.x < m_cursor.lineIt->size() && GetCharType((*m_cursor.lineIt)[m_cursor.x]) == charType)
                        CursorMover(sf::Vector2i(1,0),0);
                }
            }
            else if(event.key.code == sf::Keyboard::Left)
            {
                int charType = GetCharType((*m_cursor.lineIt)[m_cursor.x-1]);
                CursorMover(sf::Vector2i(-1,0),0);
                if(event.key.control)
                {
                    while( m_cursor.x > 0 && GetCharType((*m_cursor.lineIt)[m_cursor.x-1]) == charType)
                        CursorMover(sf::Vector2i(-1,0),0);
                }
            }
            else if(event.key.code == sf::Keyboard::Up)
            {
                CursorMover(sf::Vector2i(0,-1),0);
            }
            else if(event.key.code == sf::Keyboard::Down)
            {
                CursorMover(sf::Vector2i(0,1),0);
            }
        }
        else if(event.key.code == sf::Keyboard::Delete)
        {
            CharWriter((char)7);
        }
        else if(event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift)
        {
            m_selecting = true;
            m_selectLil = m_cursor;
            m_selectBig = m_cursor;
            m_selectStart = m_cursor;
            m_selectEnd = m_cursor;
        }
    }
    else if(event.type == sf::Event::KeyReleased)
    {
        if(event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift)
        {
            m_selecting = false;
        }
    }
}
//--------------------------------------------------------------------------------------------
//Render
void TextEditor::Render(sf::RenderWindow &window)
{
    window.draw(m_bgRect);
    
    if(m_selectLil.x != m_selectBig.x || m_selectLil.y != m_selectBig.y)
    {
        if(m_selectLil.y == m_selectBig.y)
        {
            m_selectbgRect.setPosition(m_bounds.left+m_selectLil.x*m_characterWidth,
                                       m_bounds.top+m_selectLil.y*m_characterHeight + m_characterDecenderHeight - m_renderStartLine*m_characterHeight);
            m_selectbgRect.setSize(sf::Vector2f((m_selectBig.x-m_selectLil.x)*m_characterWidth,m_characterHeight));
            window.draw(m_selectbgRect);
        }
        else
        {
            if(m_selectLil.y >= m_renderStartLine && m_selectLil.y < m_renderStartLine+m_renderedLines)
            {
                m_selectbgRect.setPosition(m_bounds.left+m_selectLil.x*m_characterWidth,
                                           m_bounds.top+m_selectLil.y*m_characterHeight + m_characterDecenderHeight - m_renderStartLine*m_characterHeight);
                m_selectbgRect.setSize(sf::Vector2f((m_selectLil.lineIt->size()-m_selectLil.x)*m_characterWidth + 10,m_characterHeight));
                window.draw(m_selectbgRect);
            }
            auto it = m_selectLil.lineIt;
            it++;
            for(int y = m_selectLil.y+1;y < m_selectBig.y;y++)
            {
                if(y >= m_renderStartLine && y < m_renderStartLine+m_renderedLines)
                {
                    m_selectbgRect.setPosition(m_bounds.left,m_bounds.top+y*m_characterHeight + m_characterDecenderHeight - m_renderStartLine*m_characterHeight);
                    m_selectbgRect.setSize(sf::Vector2f(it->size()*m_characterWidth + 10,m_characterHeight));
                    window.draw(m_selectbgRect);
                }
                it++;
            }
            if(m_selectBig.y >= m_renderStartLine && m_selectBig.y < m_renderStartLine+m_renderedLines)
            {
                m_selectbgRect.setPosition(m_bounds.left,m_bounds.top+m_selectBig.y*m_characterHeight + m_characterDecenderHeight - m_renderStartLine*m_characterHeight);
                m_selectbgRect.setSize(sf::Vector2f((m_selectBig.x)*m_characterWidth,m_characterHeight));
                window.draw(m_selectbgRect);
            }
        }
    }
    std::string lineNumber = std::to_string((int)m_Text.size()); 
    int numberDigits = std::max((int)lineNumber.size(),3);
    m_selectbgRect.setPosition(m_bounds.left+m_cursor.x*m_characterWidth+(numberDigits+1)*m_characterWidth,
                               m_bounds.top+(m_cursor.y-m_renderStartLine)*m_characterHeight+m_characterDecenderHeight);
    m_selectbgRect.setSize(sf::Vector2f(3,m_characterHeight));
    window.draw(m_selectbgRect);

    m_fontSprite.setColor(m_textColor);
    auto it = m_renderStartIterator;
    for(int i = m_renderStartLine;i < std::min(m_renderStartLine+m_renderedLines,(int)m_Text.size());i++)
    {
        std::string currentLineNumber = std::to_string(i+1);
        for(int x = 0;x < currentLineNumber.size();x++)
        {
            m_fontSprite.setTextureRect(m_charGlyphs[currentLineNumber[currentLineNumber.size()-x-1]].textureRect);
            m_fontSprite.setPosition(
m_bounds.left+(numberDigits-x-1)*m_characterWidth+(m_characterWidth - m_charGlyphs[currentLineNumber[currentLineNumber.size()-x-1]].bounds.width)/2,
m_bounds.top  + (i-m_renderStartLine+1)*m_characterHeight + m_charGlyphs[currentLineNumber[currentLineNumber.size()-x-1]].bounds.top - m_characterDecenderHeight
            );
            window.draw(m_fontSprite);
        }
        for(int x = 0;x < it->size();x++)
        {
            m_fontSprite.setTextureRect(m_charGlyphs[(*it)[x]].textureRect);
            m_fontSprite.setPosition(m_bounds.left+x*m_characterWidth+(m_characterWidth - m_charGlyphs[(*it)[x]].bounds.width)/2+(numberDigits+1)*m_characterWidth,
                                     m_bounds.top  + (i-m_renderStartLine+1)*m_characterHeight + m_charGlyphs[(*it)[x]].bounds.top - m_characterDecenderHeight);
            
            if(m_selectBig.y < i || (m_selectBig.y == i && m_selectBig.x <= x))
                m_fontSprite.setColor(m_textColor);
            else if(m_selectLil.y < i || (m_selectLil.y == i && m_selectLil.x <= x))
                m_fontSprite.setColor(m_textColorInverted);
            
            window.draw(m_fontSprite);
        }
        it++;
    }
}
//--------------------------------------------------------------------------------------------
//CharWriter
void TextEditor::CharWriter(char a)
{

    if(m_cursor.x > m_cursor.lineIt->size())
        m_cursor.x = m_cursor.lineIt->size();
    
    if(m_selectLil.x != m_selectBig.x || m_selectLil.y != m_selectBig.y)
    {
        DeleteSelection();
        if(a == 8 || a == 7)
            return;
    }
    DeSelect(0);
    //Back Space
    if(a == 8)
    {
        bool onlySpaces = m_cursor.x > 0;
        int spaces = 0;
        if(onlySpaces)
            for(int i = 0;i < m_cursor.x;i++)
            {
                if((*m_cursor.lineIt)[i] != ' ')
                {
                    onlySpaces = false;
                    break;
                }
                spaces++;
            }
        if(!onlySpaces)
        {
            if(m_cursor.x > 0)
            {
                if(!m_historyReadOnly)
                    m_History.back()->goBackStr.push_front((*m_cursor.lineIt)[m_cursor.x-1]);
                m_cursor.lineIt->erase(--m_cursor.x,1);
            }
            else if(m_cursor.y > 0)
            {
                if(!m_historyReadOnly)
                    m_History.back()->goBackStr.push_front('\n');
                auto tmp = m_cursor.lineIt;
                CursorMover(sf::Vector2i{0,-1},1);
                *m_cursor.lineIt += *tmp;
                m_Text.erase(tmp);
            }
        }
        else
        {
            int spacesToDelete = std::min(spaces,4);
            (*m_cursor.lineIt)[m_cursor.x-spacesToDelete] = '-';
            for(int i = 0;i < spacesToDelete;i++)
            {
                CharWriter(8);
            }
        }
    }
    //Delete
    else if(a == 7)
    {
        if(m_cursor.x < m_cursor.lineIt->size())
        {
            if(!m_historyReadOnly)
                m_History.back()->goBackStr.push_back((*m_cursor.lineIt)[m_cursor.x]);
            m_cursor.lineIt->erase(m_cursor.x,1);
            
        }
        else if(m_cursor.y < m_Text.size() - 1)
        {
            if(!m_historyReadOnly)
                m_History.back()->goBackStr.push_back('\n');
            auto tmp = m_cursor.lineIt;
            tmp++;
            *m_cursor.lineIt += *tmp;
            m_Text.erase(tmp);
        }
    }
    //New Line
    else if(a == 13)
    {
        if(!m_historyReadOnly)
            m_History.back()->goBackStr.push_front(8);
        auto it = m_cursor.lineIt;
        it++;
        m_Text.insert(it,m_cursor.lineIt->substr(m_cursor.x));
        *m_cursor.lineIt = m_cursor.lineIt->substr(0,m_cursor.x);
        CursorMover(sf::Vector2i{0,1},-1);
    }
    //Ctrl + BackSpace
    else if(a == 127)
    {
        if(m_cursor.x <= 0)
            CharWriter(8);
        else
        {
            int charType = GetCharType((*m_cursor.lineIt)[m_cursor.x-1]);
            while(m_cursor.x > 0 && GetCharType((*m_cursor.lineIt)[m_cursor.x-1]) == charType)
            {
                CharWriter(8);
            }
        }
    }
    //Tab
    else if(a == 9)
    {
        for(int i = 0;i < 4;i++)
            CharWriter(' ');
    }
    //Written Chars
    else if(a >= 32)
    {
        if(m_cursor.lineIt->size() >= m_renderedChars-3)
            return;
        m_cursor.lineIt->insert(m_cursor.x,1,a);
        CursorMover(sf::Vector2i(1,0),0);
        if(!m_historyReadOnly)
            m_History.back()->goBackStr.push_front(8);
    }

    DeSelect(0);
}
//--------------------------------------------------------------------------------------------
//GetCharType
int TextEditor::GetCharType(char a)
{
    if(isalpha(a))
        return 1;
    else if(isdigit(a))
        return 2;
    else if(a == ' ')
        return 3;
    else if(a == 8)
        return 4;
    else
        return 5;
}
//--------------------------------------------------------------------------------------------
//CursorMover
void TextEditor::CursorMover(sf::Vector2i moveWay,int nextXPos)
{
    if(moveWay.x == 1)
    {
        if(m_cursor.x < m_cursor.lineIt->size())
            m_cursor.x++;
        else
            CursorMover(sf::Vector2i{0,1},-1);
    }
    else if(moveWay.x == -1)
    {
        if(m_cursor.x > 0)
            m_cursor.x--;
        else
            CursorMover(sf::Vector2i{0,-1},1);
    }
    else if(moveWay.y == 1)
    {
        if(m_cursor.y < m_Text.size()-1)
        {
            m_cursor.y++;
            m_cursor.lineIt++;
            if(nextXPos == -1)m_cursor.x = 0;
            if(nextXPos == 1)m_cursor.x = m_cursor.lineIt->size();
        }
        else
            m_cursor.x = m_cursor.lineIt->size();
    }
    else if(moveWay.y == -1)
    {
        if(m_cursor.y > 0)
        {
            m_cursor.y--;
            m_cursor.lineIt--;
            if(nextXPos == -1)m_cursor.x = 0;
            if(nextXPos == 1)m_cursor.x = m_cursor.lineIt->size();
        }
        else
            m_cursor.x = 0;
    }

    if(m_selectEnd.x > m_selectEnd.lineIt->size())
        m_selectEnd.x = m_selectEnd.lineIt->size();
    if(m_selectStart.x > m_selectStart.lineIt->size())
        m_selectStart.x = m_selectStart.lineIt->size();
    if(m_cursor.x > m_cursor.lineIt->size())
        m_cursor.x = m_cursor.lineIt->size();

    if(m_selecting)
        m_selectEnd = m_cursor;
    if(m_selectEnd.y < m_selectStart.y)
    {
        m_selectLil = m_selectEnd;
        m_selectBig = m_selectStart;
    }
    else if(m_selectEnd.y > m_selectStart.y)
    {
        m_selectLil = m_selectStart;
        m_selectBig = m_selectEnd;
    }
    else
    {
        m_selectLil = (m_selectEnd.x < m_selectStart.x) ? m_selectEnd : m_selectStart;
        m_selectBig = (m_selectEnd.x > m_selectStart.x) ? m_selectEnd : m_selectStart;
    }
    
    if(m_cursor.y >= m_renderStartLine+m_renderedLines)
    {
        m_renderStartLine = m_cursor.y-m_renderedLines+1;
        m_renderStartIterator = m_cursor.lineIt;
        for(int i = 0;i < m_renderedLines-1;i++)
            m_renderStartIterator--;
    }
    else if(m_cursor.y < m_renderStartLine)
    {
        m_renderStartLine = m_cursor.y;
        m_renderStartIterator = m_cursor.lineIt;
    }
}
//--------------------------------------------------------------------------------------------
//Selection
void TextEditor::DeSelect(int i)
{
    if(m_selectBig.x == m_selectLil.x && m_selectLil.y == m_selectBig.y)
        return;
    if(i == -1)
        m_cursor = m_selectLil;
    else if(i == 1)
        m_cursor = m_selectBig; 
    m_selecting = false;
    m_selectStart = m_selectEnd;
    m_selectBig = m_selectStart;
    m_selectLil = m_selectBig;

    CursorMover(sf::Vector2i(0,0),0);
}

void TextEditor::DeleteSelection()
{
    PushHistory();
    auto it = m_selectLil.lineIt;
    for(int i = m_selectLil.y;i <= m_selectBig.y;i++)
    {
        if(i != m_selectLil.y && i != m_selectBig.y)
            m_Text.erase(it);
        else if(m_selectLil.y == m_selectBig.y)
        {
            std::string tmp = it->substr(0,m_selectLil.x);
            tmp += it->substr(m_selectBig.x);
            *it = tmp;
        }
        else if(i == m_selectLil.y)
            *it = it->substr(0,m_selectLil.x);
        else if(i == m_selectBig.y)
            *it = it->substr(m_selectBig.x);
        it++;
    }
    DeSelect(-1);
}
void TextEditor::CopySelection()
{
    auto it = m_selectLil.lineIt;
    std::string ret = "";
    for(int i = m_selectLil.y;i <= m_selectBig.y;i++)
    {
        if(i != m_selectLil.y)
            ret+= '\n';
        int x = 0;
        if(i == m_selectLil.y)
            x = m_selectLil.x;
        int endx = it->size();
        if(i == m_selectBig.y)
            endx = m_selectBig.x;
        ret += it->substr(x,endx-x);
        it++;
    }
    sf::Clipboard::setString(ret);
}

void TextEditor::PasteSelection()
{
    std::string pasteString = sf::Clipboard::getString();
    if(pasteString == "")
        return;

    if(m_selectLil.x != m_selectBig.x || m_selectLil.y != m_selectBig.y)
        DeleteSelection();
    DeSelect(-1);

    PushHistory();

    for(char i : pasteString)
        CharWriter(i);

}
//--------------------------------------------------------------------------------------------
//Push History
void TextEditor::PushHistory()
{
    if(m_History.size() > 0)
        m_History.back()->cursor = m_cursor;
    m_History.push_back(new HistoryPart);
    if(m_History.size() > m_maxHistoryLimit)
    {
        delete m_History.front();
        m_History.pop_front();
    }
}