#include "CardRemoveScene.h"
#include "SimpleAudioEngine.h"
#include "Card.h"

USING_NS_CC;

Scene* CardRemove::createScene()
{
    return CardRemove::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading : %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames\n");
}

bool CardRemove::init()
{
    if(!Scene::init())
        return false;
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 牌 1：黑桃 A (黑色)
    Card::Config config1;
    config1._suit = cardSuit::Spade;
    config1._color = cardColor::Black;
    config1.cardRank = 1;

    auto card1 = Card::create(config1);
    if (card1) {
        // 放在屏幕左边
        card1->setPosition(Vec2(visibleSize.width * 0.3f, visibleSize.height * 0.5f));
        // card1->setFaceUp(true); // 如果你之前实现了 setFaceUp，记得调用让它翻面
        this->addChild(card1);
    }

    // 牌 2：红心 K (红色)
    Card::Config config2;
    config2._suit = cardSuit::Heart;
    config2._color = cardColor::Red;
    config2.cardRank = 13;

    auto card2 = Card::create(config2);
    if (card2) {
        // 放在屏幕中间
        card2->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));
        this->addChild(card2);
    }

    // 牌 3：梅花 5 (黑色)
    Card::Config config3;
    config3._suit = cardSuit::Club;
    config3._color = cardColor::Black;
    config3.cardRank = 5;

    auto card3 = Card::create(config3);
    if (card3) {
        // 放在屏幕右边
        card3->setPosition(Vec2(visibleSize.width * 0.7f, visibleSize.height * 0.5f));
        card3->setFaceUp(false);
        this->addChild(card3);
    }

    // 关闭按钮
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(CardRemove::menuCloseCallback, this)
    );
    if(closeItem == nullptr || closeItem->getContentSize().width <= 0 || closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // 菜单
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // 窗口标题
    auto label = Label::createWithTTF("Card Game", "fonts/Marker Felt.ttf", 24);
    if(label == nullptr)
        problemLoading("'fonts/Marker Felt.ttf'");
    else
    {
        label->setPosition(Vec2(origin.x + visibleSize.width/2, 
                                origin.y + visibleSize.height - label->getContentSize().height));
        this->addChild(label, 1);
    }

    // 背景图
    auto sprite = Sprite::create("CardRemove.jpg");
    if(sprite == nullptr)
        problemLoading("'CardRemove.jpg'");
    else
    {
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
        // 设置在屏幕正中心
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
        
        // 计算缩放比例 -> 铺满
        // 屏幕宽度 / 图片宽度 = 倍数
        float scaleX = visibleSize.width / sprite->getContentSize().width;
        float scaleY = visibleSize.height / sprite->getContentSize().height;
        
        // 应用缩放
        sprite->setScaleX(scaleX);
        sprite->setScaleY(scaleY);

        // 3. 添加到场景中，放在最底层（Z-Order 设置为 -1）
        this->addChild(sprite, -1);
    }
    
    return true;
}

void CardRemove::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}