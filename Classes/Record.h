#ifndef _YXY_RECORD_H_
#define _YXY_RECORD_H_

#include "cocos2d.h"
#include "Card.h"
#include <stack>

struct Record
{
    int moveType;                       // 0 -> 从额外牌堆翻牌  1 -> 从山峰翻牌
    Card* moveCard;                     // 移动的牌
    cocos2d::Vec2 position;             // 牌原位置
    int originalZOrder;                 // 遮挡层级
    std::vector<Card*> foldCards;       // 哪些牌因此被翻开
};


#endif  // _YXY_RECORD_H_