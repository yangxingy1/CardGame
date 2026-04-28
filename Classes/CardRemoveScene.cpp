#include "CardRemoveScene.h"
#include "SimpleAudioEngine.h"
#include "Constant.h"

USING_NS_CC;

void CardRemove::initDeck()
{
    deck.clear();

    // 生成52张牌
    for (int s = 0; s < 4; ++s) 
    {
        for (int r = 1; r <= 13; ++r) 
        {
            Card::Config cfg;
            cfg._suit = static_cast<cardSuit>(s);
            cfg._color = (cfg._suit == cardSuit::Diamond || cfg._suit == cardSuit::Heart) ? cardColor::Red : cardColor::Black;
            cfg.cardRank = r;
            
            deck.push_back(cfg);
        }
    }

    // 使用随机数洗牌
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);
    
    CCLOG("Deck initialized and shuffled. Total cards: %d", static_cast<int>(deck.size()));
}

std::vector<Card*> CardRemove::updateCardsStatus() 
{
    // 记录这次新翻开的牌
    std::vector<Card*> newlyFlippedCards; 

    for (auto card : mountainCards) 
    {
        if (!card->isVisible()) 
            continue; 

        bool isCovered = false;
        cocos2d::Rect cardRect = card->getBoundingBox();

        // 检查是否有其他的牌压在它上面
        for (auto other : mountainCards) 
        {
            // other 可见，且 Z 轴层级比当前牌高
            if (other != card && other->isVisible() && other->getLocalZOrder() > card->getLocalZOrder()) 
            {
                // 矩形重叠检测
                if (cardRect.intersectsRect(other->getBoundingBox())) {
                    isCovered = true;
                    break; 
                }
            }
        }

        card->setIsCovered(isCovered);
        
        // 不再被遮挡，当前是扣住状态，则翻开
        if (!isCovered && !card->getIsFaceUp()) 
        {
            card->setFaceUp(true);
            newlyFlippedCards.emplace_back(card);
        }
    }
    return newlyFlippedCards;
}

Card* CardRemove::addCardToUpArea(Vec2 pos, int zOrder) 
{
    // 使用纯粹的抽牌函数
    Card* card = createCardFromDeck();
    if (card) {
        card->setPosition(pos);
        card->setFaceUp(false); // 初始全部扣住，由 updateCardsStatus 决定是否翻开
        this->addChild(card, zOrder);
        
        // 【关键】加入到山峰管理容器，用于遮挡检测
        mountainCards.push_back(card);
    }
    return card;
}

Card* CardRemove::createCardFromDeck() 
{
    if (deck.empty()) return nullptr;
    
    Card::Config cfg = deck.back();
    deck.pop_back();
    
    return Card::create(cfg);
}

void CardRemove::initUpArea() 
{
    Vec2 mountain1Start = Vec2(Constant::MOUNTAIN_START_X, Constant::MOUNTAIN_START_Y);
    spawnMountain(mountain1Start);

}

void CardRemove::initDownArea() 
{

    // 初始化消除栈首张牌 -> 从 deck 中抽 1 张
    Card* firstWasteCard = createCardFromDeck();
    if (firstWasteCard) 
    {
        firstWasteCard->setPosition(Vec2(Constant::WASTE_START_X, Constant::WASTE_START_Y)); 
        firstWasteCard->setFaceUp(true);
        this->addChild(firstWasteCard, 100);
        wasteCards.push(firstWasteCard);
    }

    // 额外牌堆起始位置
    Vec2 drawPileBasePos = Vec2(Constant::EXTRA_START_X, Constant::EXTRA_START_Y);
    
    // 初始化额外牌堆 -> 从 deck 中抽 20 张
    for (int i = 0; i < 38; ++i) {
        Card* card = createCardFromDeck();
        if (card) {
            card->setFaceUp(false); 
            // 坐标：向右方偏移 10 像素 
            card->setPosition(drawPileBasePos + Vec2(i * Constant::EXTRA_OFFSET, 0));
            this->addChild(card, i + 1); 
            pileCards.push_back(card);
        }
    }

    CCLOG("Pile Area initialized. Remaining cards in deck: %d", static_cast<int>(deck.size()));
}

void CardRemove::drawCardToWaste() 
{
    if (pileCards.empty()) 
    return;

    // 取出额外牌堆最上方的一张
    Card* card = pileCards.back();
    pileCards.pop_back();

    // 创建记录
    Record record;
    record.moveType = 0;
    record.moveCard = card;
    record.position = card->getPosition();
    record.originalZOrder = card->getLocalZOrder();
    undoStack.push(record);

    // 处理当前的栈顶牌：将其隐藏
    if (!wasteCards.empty()) 
        wasteCards.top()->setVisible(false);

    // 目标位置 (右侧消除栈位置)
    Vec2 wastePos = Vec2(Constant::WASTE_START_X, Constant::WASTE_START_Y);
    
    // 执行动画与翻牌
    auto move = MoveTo::create(0.2f, wastePos);
    auto flip = CallFunc::create([card]() {
        card->setFaceUp(true); // 到达位置后翻开
    });
    
    card->setLocalZOrder(500 + (int)wasteCards.size()); // 确保在最上层
    card->runAction(Sequence::create(move, flip, nullptr));

    // 压入栈
    wasteCards.push(card);
    checkWinOrLoss();
}

void CardRemove::undoMove() 
{
    resultLabel->setVisible(false); // 只要一回退，立刻隐藏提示字
    // 如果没有历史记录，或者只有初始发的那张废牌，就不执行
    if (undoStack.empty()) 
        return; 

    // 拿出上一步的操作记录
    Record record = undoStack.top();
    undoStack.pop();

    Card* card = record.moveCard;
    
    // 停止当前牌的飞行动画
    card->stopAllActions();
    
    // 从废牌栈顶弹出这张牌
    wasteCards.pop();
    
    // 让废牌栈的前一张牌重新显现
    if (!wasteCards.empty()) 
        wasteCards.top()->setVisible(true);

    // 将这张牌瞬间飞回原来的位置
    card->runAction(MoveTo::create(0.15f, record.position));
    card->setLocalZOrder(record.originalZOrder);

    // 根据操作类型，逆转状态
    if (record.moveType == 0) 
    {
        // 额外牌堆: 扣过去 + 塞回
        card->setFaceUp(false);
        pileCards.push_back(card);
    } 
    else if (record.moveType == 1) 
    {
        // 主牌堆: 塞回 + 扣回因此翻开的牌
        mountainCards.push_back(card);
        
        // 把当时因为它而被翻开的下层牌，重新扣过去！
        for (Card* flippedCard : record.foldCards) 
            flippedCard->setFaceUp(false);
        
        // updateCardsStatus();
    }
}

void CardRemove::spawnMountain(Vec2 startPos) 
{
    // 间距计算：[182] [80] [182(中空)] [80] [182]
    // 左右两列地基的中心距 = 80 + 182 + 80 + 182 = 524
    float columnDist = 524.0f;
    int baseZ = 10;

    // --- 1. 地基层 c---c (2列 * 4层 = 8张) ---
    for (int i = 0; i < 4; ++i) {
        float offsetY = i * (Constant::CARD_WIDTH / 2); // 每层向上覆盖半张卡
        // 左列 (Z-Order 随层数增加，保证上方覆盖下方)
        addCardToUpArea(startPos + Vec2(0, offsetY), baseZ - i);
        // 右列
        addCardToUpArea(startPos + Vec2(columnDist, offsetY), baseZ - i);
    }

    // --- 2. 金字塔层 (向下延伸，覆盖一半高度) ---
    // 金字塔逻辑起点：位于地基第4层下方半张牌的位置
    float pyramidY = startPos.y  - Constant::CARD_HEIGHT * 0.5f; 
    int pyramidZ = baseZ + 10;

    // c-c-c (3张)
    float gap3 = columnDist / 2.0f;
    addCardToUpArea(Vec2(startPos.x, pyramidY), pyramidZ);
    addCardToUpArea(Vec2(startPos.x + gap3, pyramidY), pyramidZ);
    addCardToUpArea(Vec2(startPos.x + columnDist, pyramidY), pyramidZ);

    // -c-c- (2张)
    float row2Y = pyramidY - Constant::CARD_HEIGHT * 0.5f;
    addCardToUpArea(Vec2(startPos.x + gap3 * 0.5f, row2Y), pyramidZ + 1);
    addCardToUpArea(Vec2(startPos.x + gap3 * 1.5f, row2Y), pyramidZ + 1);

    // --c-- (1张)
    float row3Y = row2Y - Constant::CARD_HEIGHT * 0.5f;
    addCardToUpArea(Vec2(startPos.x + gap3, row3Y), pyramidZ + 2);
}

void CardRemove::checkWinOrLoss()
{
    // 1. 判断胜利：山峰被清空
    if (mountainCards.empty()) 
    {
        resultLabel->setString("YOU WIN !");
        resultLabel->setVisible(true);
        return;
    }

    // 2. 判断失败：牌堆空了，且无牌可消
    if (pileCards.empty()) 
    {
        bool hasPlayableMove = false;
        
        if (!wasteCards.empty()) 
        {
            Card* activeCard = wasteCards.top();
            
            // 遍历山上所有可见且未被遮挡的牌
            for (auto card : mountainCards) {
                if (card->isVisible() && !card->getIsCovered()) {
                    if (card->canMatch(activeCard)) {
                        hasPlayableMove = true;
                        break; // 只要找到一步可走的棋，就还没输
                    }
                }
            }
        }

        if (!hasPlayableMove) 
        {
            resultLabel->setString("GAME OVER");
            resultLabel->setVisible(true);
        }
    }
}

Scene* CardRemove::createScene()
{
    return CardRemove::create();
}

bool CardRemove::init()
{
    if(!Scene::init())
        return false;
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    // 初始化所有区域
    initDeck();
    initUpArea();
    initDownArea();
    updateCardsStatus();

    // 注册触摸事件
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(CardRemove::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


    // 小标题
    auto label = Label::createWithTTF("Card Game", "fonts/Marker Felt.ttf", 48);
    label->setPosition(Vec2(origin.x + visibleSize.width/2, 
                            origin.y + visibleSize.height - label->getContentSize().height));
    this->addChild(label, 1);

    // 胜负显示
    resultLabel = Label::createWithTTF("", "fonts/Marker Felt.ttf", 64);
    resultLabel->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    resultLabel->setTextColor(Color4B::YELLOW);
    resultLabel->enableOutline(Color4B::BLACK, 3);
    resultLabel->setVisible(false);
    this->addChild(resultLabel, 2000); 

    // 背景图
    auto sprite = Sprite::create("CardRemove.jpg");
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

    // 添加到场景中，放在最底层（Z-Order 设置为 -1）
    this->addChild(sprite, -1);

    
    return true;
}

void CardRemove::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}

bool CardRemove::onTouchBegan(Touch* touch, Event* event) 
{
    Vec2 location = touch->getLocation();

    // -----------------------检查消除栈-------------------------
    if (!wasteCards.empty()) 
    {
        Card* topWaste = wasteCards.top();
        if (topWaste->getBoundingBox().containsPoint(location)) 
        {
            if (!undoStack.empty()) 
            {
                CCLOG("Clicked Waste Stack! Executing Undo...");
                undoMove();
                return true; // 拦截触摸
            } 
            else
                CCLOG("No moves to undo.");
        }
    }

    // ------------------------检查额外牌堆-----------------------
    // 只有额外牌堆不为空时才检测点击
    if (!pileCards.empty()) 
    {
        Card* topCard = pileCards.back();
        // 检查点击位置是否在最上方那张牌的范围内
        if (topCard->getBoundingBox().containsPoint(location)) 
        {
            drawCardToWaste();
            return true;
        }
    }

    // ------------------------检查主牌区-------------------------
    for (auto it = mountainCards.rbegin(); it != mountainCards.rend(); ++it) 
    {
        Card* card = *it;

        // 条件 1: 牌必须没被消除 
        // 条件 2: 牌必须没有被遮挡 
        // 条件 3: 点击坐标在牌的包围盒内
        if (card->isVisible() && !card->getIsCovered() && card->getBoundingBox().containsPoint(location)) 
        {
            
            // 获取当前废牌栈顶部的牌作为 基准牌
            if (!wasteCards.empty()) 
            {
                Card* activeCard = wasteCards.top();
                
                if (card->canMatch(activeCard)) 
                {
                    
                    CCLOG("Match Success!");

                    // 创建记录
                    Record record;
                    record.moveType = 1;
                    record.moveCard = card;
                    record.position = card->getPosition();
                    record.originalZOrder = card->getLocalZOrder();
                    // 从山峰数组中移除这张牌 
                    mountainCards.erase(std::next(it).base());

                    // 隐藏旧的基准牌（为了渲染性能）
                    activeCard->setVisible(false);

                    // 创建动画，飞向右下角的消除栈
                    Vec2 wastePos = Vec2(Constant::WASTE_START_X, Constant::WASTE_START_Y);
                    auto move = MoveTo::create(0.2f, wastePos);
                    
                    // 将层级拔高，确保飞越屏幕时不会被其他牌挡住
                    card->setLocalZOrder(500 + (int)wasteCards.size()); 
                    card->runAction(move);

                    // 将这张牌压入消除栈，成为新的基准牌
                    wasteCards.push(card);

                    // 重新计算山峰中剩余卡牌的遮挡关系，并翻开下层牌
                    record.foldCards = updateCardsStatus();
                    undoStack.push(record);
                    checkWinOrLoss();
                    return true; // 成功匹配，拦截触摸
                    
                } 
                else 
                {
                    // 点中了牌，但数字不匹配
                    CCLOG("Not Match! Can't remove.");
                    
                    // (可选) 以后这里可以给 card 加个左右抖动的动作，提示玩家点错了
                    // auto shake = Sequence::create(MoveBy::create(0.05, Vec2(5, 0)), MoveBy::create(0.1, Vec2(-10, 0)), MoveBy::create(0.05, Vec2(5, 0)), nullptr);
                    // card->runAction(shake);
                    
                    return true; 
                }
            }
        }
    }

    return false;
}