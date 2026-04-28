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
    cocos2d::Label* resultLabel;

    // ---------------------牌堆处理----------------------
    void initDeck();                                            // 洗牌
    Card* createCardFromDeck();                                 // 拿取洗好的牌
    void initUpArea();                                          // 初始化上半区
    void initDownArea();                                        // 初始化下半区

    // ---------------------动作处理----------------------
    void drawCardToWaste();                                     // 翻开额外牌最上面的一张   放入消除栈
    void undoMove();                                            // 回退

    // ---------------------辅助函数----------------------
    void spawnMountain(cocos2d::Vec2 startPos);                 // 生成山峰区
    std::vector<Card*> updateCardsStatus();                     // 更新覆盖状态 
    Card* addCardToUpArea(cocos2d::Vec2 pos, int zOrder);       // 辅助函数：从 deck 中安全抽牌并添加到场景
    void checkWinOrLoss();                                      // 胜负判定

public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    void menuCloseCallback(cocos2d::Ref* pSender);

    CREATE_FUNC(CardRemove);

    // 触摸监听
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
};

#endif  // _YXY_CARDREMOVE_SCENE_H_