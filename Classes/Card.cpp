#include "Card.h"

USING_NS_CC;

// 初始化静态变量
const std::vector<std::string> Card::suits = {"club", "diamond", "heart", "spade"};
const std::vector<std::string> Card::colors = {"red", "black"};
const std::vector<std::string> Card::numbers = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

// 创建对象
Card* Card::create(Config config)
{
    Card* pRet = new(std::nothrow) Card();
    if(pRet && pRet->init(config))
    {
        // 引擎管理内存
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return nullptr;
}

bool Card::init(Config config)
{
    _config = config;
    // 初始化背景
    _isFaceUp = false;      // 初始定义为盖住
    _isCovered = true;
    if(!Sprite::initWithFile("res/card_general.png"))
        return false;
    // 获取背景大小
    Size size = this->getContentSize();

    // 左上角 : 花色
    auto suitIcon = Sprite::create(getSuitPath());
    suitIcon->setPosition(Vec2(size.width * 0.2f, size.height * 0.85f));
    _suitIcon = suitIcon;
    this->addChild(suitIcon);

    // 右上角 : 小数字
    auto smallNum = Sprite::create(getNumberPath("small"));
    smallNum->setPosition(Vec2(size.width * 0.8f, size.height * 0.85f));
    _smallNum = smallNum;
    this->addChild(smallNum);

    // 中心 : 大数字
    auto bigNum = Sprite::create(getNumberPath("big"));
    bigNum->setPosition(Vec2(size.width * 0.5f, size.height * 0.4f));
    _bigNum = bigNum;
    this->addChild(bigNum);

    return true;
}

std::string Card::getSuitPath()
{
    std::string result = "res/suits/";
    result += suits[static_cast<int>(getConfig()._suit)];
    result += ".png";
    return result;
}

std::string Card::getNumberPath(std::string size)
{
    if(size != "small" && size != "big")
        return "";
    std::string result = "res/number/";
    result += size;
    result += "_";
    result += colors[static_cast<int>(getConfig()._color)];
    result += "_";
    result += numbers[getConfig().cardRank - 1];
    result += ".png";
    return result;
}

void Card::setFaceUp(bool faceUp)
{
    setIsFaceUp(faceUp);

    if(_suitIcon)
        _suitIcon->setVisible(faceUp);
    if(_smallNum)
        _smallNum->setVisible(faceUp);
    if(_bigNum)
        _bigNum->setVisible(faceUp);

    // 切换背景
    if(faceUp)
        this->setTexture("res/card_general.png");
    else
        this->setTexture("res/back.jpg");
}

bool Card::canMatch(Card* other)
{
    if(!other)
        return false;
    int diff = std::abs(this->getConfig().cardRank - other->getConfig().cardRank);
    return diff == 1;
}