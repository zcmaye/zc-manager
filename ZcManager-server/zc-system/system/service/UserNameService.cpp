#include "UserNameService.h"

UserNameService::UserNameService()
    :m_chineseNames({
    "小可爱",
    "柠衬酸",
    "未如愿",
    "碎面具",
    "又不是星星发什么光",
    "柚香",
    "敢勾我夫我必让她哭",
    "句迷人诗",
    "蔚蓝°",
    "___向日葵╮微笑",
    "可乐丶不渴",
    "ˇ起颩るㄝ",
    "爱你如初",
    "今世我陪你白发苍苍",
    "心亡泪凉っ",
    "漓殇う",
    "顏夕の未歌",
    "自娱自乐自我闹っ",
    "给劳资TMD滚！",
    "萌妹子不装萌>。ヘ",
    "猫贪余温",
    "莫道红颜依何处",
    "中二癌晚期少年",
    "情话是骗子说傻子听的",
      "此籹子不需要谁来怜惜",
    "超级无敌小机智",
    "我不上你的当",
    "化蝶灬飞",
    "踏花游湖",
    "久孤",
    "余生长醉",
    "中毒的爱情",
    "肆无忌惮゛戒情戒爱づ",
    "九耀星璇",
    "不及眉间朱砂尽",
    "不规则的美╮",
    "范二姑涼歡樂多°",
    "巴黎铁塔上盛开的繁华*",
    "你的她貌美如狗~",
    "珊瑚是深海的記憶ㄟ"})
{

}

UserNameService::~UserNameService()
{}

void UserNameService::setType(Type type)
{
    m_type = type;
}

UserNameService::Type UserNameService::type() const
{
    return m_type ;
}

std::string UserNameService::createUserName()
{
    switch (m_type)
    {
    case UserNameService::Chinese:
        return createChineseUserName();
    case UserNameService::English:
        return createEnglishUserName();
    default:
        break;
    }
    return std::string("xxx");
}

std::string UserNameService::createChineseUserName()
{
    std::mt19937 gen(m_rd());
	return m_chineseNames[gen() % m_chineseNames.size()];
}

std::string UserNameService::createEnglishUserName()
{
    std::mt19937 gen(m_rd());
	//gen() % 3;  //0 1 2
	//gen() % (8 - 3) + 3;  //0 1 2 3 4  3 4 5 6 7
#define R(min,max) (gen() % (max - min) + min)

    std::string name;
    auto len = R(3,8);
    for (int i = 0; i < len; i++) {
        auto choose = R(0,3);
        if(choose == 0)
			name.push_back(R(0,26) + 'A');
        else if(choose == 1)
			name.push_back(R(0,26) + 'a');
        else
			name.push_back(R(0,10) + '0');
    }
#undef R
    return name;

}

