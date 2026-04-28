class Constant
{
private:
    Constant(){}
    ~Constant(){}

    Constant(const Constant&) = delete;
    Constant& operator=(const Constant&) = delete;

    static Constant constant;

public:
    static const float DOWN_AREA;
    static const float CARD_WIDTH;
    static const float CARD_HEIGHT;
    static const int MOUNTAIN_START_X = 278;
    static const int MOUNTAIN_START_Y = 1539;
    static const int WASTE_START_X = 830;
    static const int WASTE_START_Y = 290;
    static const int EXTRA_START_X = 250;
    static const int EXTRA_START_Y = 290;
    static const float EXTRA_OFFSET;
    static Constant& getInstant()
    {   
        return constant;    
    }

};