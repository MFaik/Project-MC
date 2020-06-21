#pragma once

#include "Object.hpp"

#include <list>
#include <string>
#include <deque>

struct Cursor
{
    std::list<std::string>::iterator lineIt;
    int x,y;
};
struct HistoryPart
{
    Cursor cursor;
    std::deque<char> goBackStr;
};
class TextEditor : public Object
{
private:
    std::list<std::string> m_Text;
    int m_characterHeight,m_characterWidth,m_characterDecenderHeight;
    sf::Color m_textColor,m_textColorInverted;

    int m_renderStartLine = 0,m_renderStartPos = 0;
    Cursor m_cursor,m_selectStart,m_selectEnd,m_selectLil,m_selectBig;
    bool m_selecting;

    bool m_historyReadOnly = false;
    std::deque<HistoryPart*> m_History;
    int m_maxHistoryLimit;
    int m_lastCharType;

    sf::Texture m_fontTexture;
    sf::Sprite m_fontSprite;
    sf::Glyph m_charGlyphs[128];

    sf::RectangleShape m_bgRect;
    sf::RectangleShape m_selectbgRect;

    void CharWriter(char a);
    int GetCharType(char a); 
    void CursorMover(sf::Vector2i moveWay,int nextXPos);
    void DeSelect(int i);
    void DeleteSelection();
    void CopySelection();
    void PasteSelection();
    void PushHistory();
    //bool IsCursorInside(Cursor &mainCursor,Cursor &strCursor,Cursor &endCursor);
public:
    TextEditor(const sf::Rect<float> &bounds,sf::Color bgColor,sf::Color textColor,sf::Font font,int characterSize);
    ~TextEditor();
    std::string getText();
    std::list<std::string>& getTextList();
    void setText(const std::string &str);
    void Update() override;
    void Update(const sf::Event &event) override;
    void Render(sf::RenderWindow &window) override;
};