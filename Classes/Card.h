#ifndef _YXY_CARD_H_
#define _YXY_CARD_H_

#include "cocos2d.h"
#include <vector>

enum class cardSuit{ Club, Diamond, Heart, Spade };         // 梅花 方片 红心 黑桃
enum class cardColor{ Red, Black };                         // 红色 黑色


class Card : public cocos2d::Sprite
{
private:   
    const static std::vector<std::string> suits;
    const static std::vector<std::string> colors;
    const static std::vector<std::string> numbers;

    // 三个组成部分
    cocos2d::Sprite* _suitIcon;
    cocos2d::Sprite* _smallNum;
    cocos2d::Sprite* _bigNum;

    std::string getSuitPath();
    std::string getNumberPath(std::string size);

public: 
    struct Config
    {
        cardSuit _suit;
        cardColor _color;
        int cardRank;       // 1-13
    };

    static Card* create(Config config);
    bool init(Config config);

    void setFaceUp(bool faceUp);                            // 是否正面朝上
    bool canMatch(Card* other);

    // 游戏逻辑相关
    CC_SYNTHESIZE_READONLY(Config, _config, Config);        // 生成只读属性 + get
    CC_SYNTHESIZE(bool, _isFaceUp, IsFaceUp);               // 生成私有属性 + get + set
    CC_SYNTHESIZE(bool, _isCovered, IsCovered);             // 是否被上层牌遮挡

};

#endif  // _YXY_CARD_H_