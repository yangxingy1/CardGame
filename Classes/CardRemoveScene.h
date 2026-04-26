#ifndef _YXY_CARDREMOVE_SCENE_H_
#define _YXY_CARDREMOVE_SCENE_H_

#include "cocos2d.h"

class CardRemove : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    void menuCloseCallback(cocos2d::Ref* pSender);

    CREATE_FUNC(CardRemove);
};

#endif  // _YXY_CARDREMOVE_SCENE_H_