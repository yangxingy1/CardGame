#ifndef _YXY_CARDREMOVE_SCENE_H_
#define _YXY_CARDREMOVE_SCENE_H_

#include "cocos2d.h"
#include "Card.h"
#include "Record.h"
#include <vector>
#include <stack>
#include <random>
#include <algorithm>

class CardRemove : public cocos2d::Scene
{
private:
    // 区域位置
    const float PILE_AREA = 580.0f;
    const float CARD_WIDTH = 182.0f;
    const float CARD_HEIGHT = 282.0f;
    // 所有牌的信息
    std::vector<Card::Config> deck;
    // 额外牌堆
    std::vector<Card*> pileCards;
    // 消除栈
    std::stack<Card*> wasteCards;
    // 主牌区所有牌
    std::vector<Card*> mountainCards;
    // 历史记录
    std::stack<Record> undoStack;

    void spawnMountain(cocos2d::Vec2 startPos); 
    // 更新覆盖状态 返回翻开的牌 -> 历史记录 -> 回退功能
    std::vector<Card*> updateCardsStatus();

    // 辅助工具：从 deck 中安全抽牌并添加到场景
    Card* addCardToUpArea(cocos2d::Vec2 pos, int zOrder);

    // ---------------------牌堆处理----------------------
    // 洗牌
    void initDeck();
    // 拿取洗好的牌
    Card* createCardFromDeck();
    // 初始化上半区
    void initUpArea(); 
    // 初始化下半区
    void initDownArea();

    // ---------------------动作处理----------------------
    void drawCardToWaste();         // 翻开额外牌最上面的一张   放入消除栈
    void undoMove();                // 回退

public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    void menuCloseCallback(cocos2d::Ref* pSender);

    CREATE_FUNC(CardRemove);

    // 触摸监听
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
};

#endif  // _YXY_CARDREMOVE_SCENE_H_