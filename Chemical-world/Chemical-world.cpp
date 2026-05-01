#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>//一个一个导入!
#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <map>
#include <iomanip>
#include <vector> //依旧 
#include <string>
#include <thread> //多线程 
#include <chrono>
#include "json.hpp"
#include "cfs.h"
#include <functional>

using json = nlohmann::json;
namespace fs = std::filesystem;

//本游戏内容会非常庞大，所以存档系统必须写……
//但是存档系统需要很多麻烦的东西，比如sqite和nlohmann json,不过可以学嘛，总之需要把代码写的灵活一点，防止根本不知道怎么改
//……
//实在不行考虑考虑……sqlite?

using namespace std;

#ifdef _WIN32
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#endif


#define BLACK 0
#define DARK_BLUE 1
#define DARK_GREEN 2
#define DARK_CYAN 3 
#define DARK_RED 4
#define DARK_PURPLE 5
#define DARK_YELLOW 6
#define RESET 7
#define GREY 8
#define BLUE 9
#define GREEN 10
#define CYAN 11 //说是青色，其实是……淡浅绿色 
#define RED 12
#define PURPLE 13
#define YELLOW 14
#define LIGHT_WHITE 15



string gettimeString(time_t t) {
    char buffer[80];
    struct tm* timeinfo = localtime(&t);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return string(buffer);
}

// 控制台特效类
class ConsoleEffect {
public:
    static void setColor(int textColor, int bgColor = 0) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textColor + bgColor * 16);
    }

    static void gotoxy(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    static void hideCursor() {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    }

    static void showCursor() {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
        cursorInfo.bVisible = true;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    }

    static void clearScreen() {
        system("cls");
    }
};


// 炫酷动画类
class CoolAnimation {
public:
    // 矩阵数字雨
    static void matrixRain(int duration = 3) {
        ConsoleEffect::hideCursor();
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        int width = csbi.dwSize.X;
        int height = csbi.dwSize.Y;

        vector<int> columns(width, 0);

        for (int t = 0; t < duration * 15; t++) {
            ConsoleEffect::clearScreen();
            for (int i = 0; i < width; i++) {
                if (rand() % 8 == 0) {
                    columns[i] = rand() % height;
                }
                if (columns[i] > 0) {
                    for (int j = 0; j < 3 && columns[i] - j >= 0; j++) {
                        ConsoleEffect::gotoxy(i, columns[i] - j);
                        if (j == 0) ConsoleEffect::setColor(10); // 亮绿
                        else ConsoleEffect::setColor(2); // 暗绿
                        cout << char(rand() % 2 + '0');
                    }
                    columns[i]--;
                }
            }
            Sleep(80);
        }
        ConsoleEffect::clearScreen();
    }

    // 彩虹文字
    static void rainbowText(const string& text, int delay = 30) {
        int colors[] = { 12, 10, 14, 11, 13, 9 };
        for (size_t i = 0; i < text.length(); i++) {
            ConsoleEffect::setColor(colors[i % 6]);
            cout << text[i];
            Sleep(delay);
        }
        ConsoleEffect::setColor(7);
    }

    // 烟花特效
    static void firework() {
        Sleep(1500);
        ConsoleEffect::clearScreen();
        for (int i = 0; i < 80; i++) {
            int x = rand() % 80;
            int y = rand() % 20;
            ConsoleEffect::gotoxy(x, y);
            ConsoleEffect::setColor(rand() % 15 + 1);
            cout << "*";
            Sleep(25);
        }
        Sleep(1500);
		system("cls");
    }

};




//按钮生成系统
//这里会去写生成按钮的全家桶，代码量和技术难度恐怕会炸
//悲
//有没有朋友帮帮忙
//求求了
struct Button{
    //预留
};

class ButtonG {
public:
    //生成按钮
    void generateButton() {
        //预留
    }
};


// ======================== 编码转换助手 ========================

class EncodingHelper {
public:
    // GBK 转 UTF-8（保存时用）
    static string gbkToUtf8(const string& gbkStr) {
        if (gbkStr.empty()) return "";

        int len = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, NULL, 0);
        if (len == 0) return gbkStr;

        wstring wstr(len, L'\0');
        MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, &wstr[0], len);

        len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        if (len == 0) return gbkStr;

        string utf8Str(len, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], len, NULL, NULL);

        // 去掉末尾的 '\0'
        if (!utf8Str.empty() && utf8Str.back() == '\0') {
            utf8Str.pop_back();
        }

        return utf8Str;
    }

    // UTF-8 转 GBK（显示用）
    static string utf8ToGbk(const string& utf8Str) {
        if (utf8Str.empty()) return "";

        int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
        if (len == 0) return utf8Str;

        wstring wstr(len, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstr[0], len);

        len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        if (len == 0) return utf8Str;

        string gbkStr(len, '\0');
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &gbkStr[0], len, NULL, NULL);

        if (!gbkStr.empty() && gbkStr.back() == '\0') {
            gbkStr.pop_back();
        }

        return gbkStr;
    }

    // 清理字符串，确保是有效的 UTF-8
    static string sanitizeUtf8(const string& str) {
        string result;
        result.reserve(str.length());

        for (size_t i = 0; i < str.length(); ) {
            unsigned char c = str[i];

            // ASCII 直接保留
            if (c < 0x80) {
                if (c >= 0x20 || c == '\n' || c == '\r' || c == '\t') {
                    result += c;
                }
                i++;
            }
            // 2字节 UTF-8
            else if ((c & 0xE0) == 0xC0 && i + 1 < str.length()) {
                if ((str[i + 1] & 0xC0) == 0x80) {
                    result += str.substr(i, 2);
                    i += 2;
                }
                else {
                    result += '?';
                    i++;
                }
            }
            // 3字节 UTF-8
            else if ((c & 0xF0) == 0xE0 && i + 2 < str.length()) {
                if ((str[i + 1] & 0xC0) == 0x80 && (str[i + 2] & 0xC0) == 0x80) {
                    result += str.substr(i, 3);
                    i += 3;
                }
                else {
                    result += '?';
                    i++;
                }
            }
            // 4字节 UTF-8
            else if ((c & 0xF8) == 0xF0 && i + 3 < str.length()) {
                if ((str[i + 1] & 0xC0) == 0x80 && (str[i + 2] & 0xC0) == 0x80 && (str[i + 3] & 0xC0) == 0x80) {
                    result += str.substr(i, 4);
                    i += 4;
                }
                else {
                    result += '?';
                    i++;
                }
            }
            // 非法字节
            else {
                result += '?';
                i++;
            }
        }

        return result;
    }
};

//随机数系统 v1.0.2 (本贾尼你个老头子坏得很)

inline int randint(int min, int max) {
    static random_device rd;
    static mt19937 gen(rd() + time(0));
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

inline void cls() {
	system("cls");
}
inline void delay(int ms) {
	Sleep(ms);
}
inline void setcolor(int color) {
#ifdef _WIN32
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, color);
#endif
}

inline void typewriter(const string& text, int ms = 30) {
	for (char c : text) {
		cout << c << flush;
		delay(ms);
	}
}

//开场动画，帅到爆炸
//这里的代码写得有点乱，主要是为了展示一些炫酷的动画效果，后续版本可能会重构一下
void cppwelcome() {
	setcolor(DARK_GREEN);
	typewriter("#include<bits/stdc++.h>\n", 40);
	delay(100);
	typewriter("using namespace std;\n", 40);
	delay(150);
	typewriter("int main(){\n", 40);
	delay(88);
	typewriter("	cout<<''hello wrold'';\n", 40);
	delay(200);
	typewriter("	cout<<''Hello,player!'';\n", 40);
	delay(100);
	typewriter("	string c='' WELCOME TO CHEMICAL-WORLD!'';\n",40);
	delay(88);
	typewriter("	//infinite、creative！;\n", 40);
	delay(150);
	typewriter("	return 0;\n", 40);
	delay(300);
	typewriter("}\n",40);
	delay(2000);
	setcolor(RESET);
}

//帅炸了，4个连续的LOGO，直接炸屏，后续版本可能会增加一些动画效果，比如闪烁、渐变等等，反正就是要炫酷
//TODO:增加一些动画效果，比如闪烁、渐变等等，反正就是要炫酷
void showlogo() {
	setcolor(CYAN);
    //ascii文字LOGO,还不错
	cout << R"( ╔════════════════════════════════════════════════════════════════════════════╗
 ║                                                                            ║
 ║     ██████╗██╗  ██╗███████╗███╗   ███╗██╗ ██████╗ █████╗ ██╗               ║
 ║    ██╔════╝██║  ██║██╔════╝████╗ ████║██║██╔════╝██╔══██╗██║               ║
 ║    ██║     ███████║█████╗  ██╔████╔██║██║██║     ███████║██║               ║
 ║    ██║     ██╔══██║██╔══╝  ██║╚██╔╝██║██║██║     ██╔══██║██║               ║
 ║    ╚██████╗██║  ██║███████╗██║ ╚═╝ ██║██║╚██████╗██║  ██║███████╗          ║ 
 ║     ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝          ║
 ║                                                                            ║
 ║                        ██╗    ██╗ ██████╗ ██████╗ ██ ╗                     ║
 ║                        ██║    ██║██╔═══██╗██╔══██╗██║                      ║
 ║                        ██║ █╗ ██║██║   ██║██████╔╝██║                      ║
 ║                        ██║███╗██║██║   ██║██╔══██╗██║                      ║
 ║                        ╚███╔███╔╝╚██████╔╝██║  ██║███████╗                 ║ 
 ║                         ╚══╝╚══╝  ╚═════╝ ╚═╝  ╚═╝╚══════╝                 ║
 ║                                                                            ║
 ║                     ╔══════════════════════════════════════╗               ║
 ║                     ║   从 原 子 到 星 辰 ， 一 手 掌 控   ║               ║
 ║                     ╚══════════════════════════════════════╝               ║
 ║                                                                            ║
 ╚════════════════════════════════════════════════════════════════════════════╝)";
	delay(5000);
    
	cls();
	setcolor(RED);
    //有种虫洞的感觉，太帅了，就和本人和看到这个注释的你一样帅炸了（如果你是女的，那就是太漂亮了）
    //谄媚这一块
	cout << R"(╔════════════════════════════════════════════════════════════════════════════╗
║                                                                            ║
║                                    .&@&&&&@,.                              ║
║                              ,&@@@(/////(#@@@#.                            ║
║                           .@@@/(/////////////#@@.                          ║
║                         .@@(/////////&@&//////(@@.                         ║
║                        @@/,,,,,,,,,,,,,,,,,,,,,*@@                         ║
║                      .@#,,,,,,,,,,,,,,,,,,,,,,,,(@.                        ║
║                     .@/,,,,,,,,,,,,,,,,,,,,,,,,,,,@                        ║
║                     @&,,,,,,,,,,,,,,,,,,,,,,,,,,,,&@                       ║
║                    @@,,,,,,,,,,,,,,,,,,,,,,,,,,,,,%@                       ║
║      ██████╗       @&,,,,,,,,,,,,,,,,,,,,,,,,,,,,,&@     ██████╗           ║
║     ██╔════╝       @@,,,,,,,,,,,,,,,,,,,,,,,,,,,,,%@    ██╔═████╗          ║
║     ██║            @&,,,,,,,,,,,,,,,,,,,,,,,,,,,,&@    ██║██╔██║           ║
║     ██║            &@,,,,,,,,,,,,,,,,,,,,,,,,,,,%@     ████╔╝██║           ║
║     ╚██████╗       .@/,,,,,,,,,,,,,,,,,,,,,,,,,#@      ╚██████╔╝           ║
║      ╚═════╝        (@,,,,,,,,,,,,,,,,,,,,,,,,,&@        ╚═════╝           ║
║                      %@,,,,,,,,,,,,,,,,,,,,,,,@&                           ║
║                       @&,,,,,,,,,,,,,,,,,,,,,%@                            ║
║                        @@*,,,,,,,,,,,,,,,,,#@&                             ║
║                         .@@/,,,,,,,,,,,,,#@@                               ║
║                           ,@@@(,,,,,,(&@@&                                 ║
║                              ,#@@@@@@&*                                    ║
║                                                                            ║
║                    ╔══════════════════════════════════════╗                ║
║                    ║  工 业  |  化 学  |  星 际  |  无 限 ║                ║
║                    ╚══════════════════════════════════════╝                ║
║                                                                            ║
╚════════════════════════════════════════════════════════════════════════════╝)";
	delay(3000);

	cls();
	setcolor(GREEN);
	//还有jobs的名言，太经典了，必须要放在这里
	cout << R"(╔════════════════════════════════════════════════════════════════════════════╗
║                                                                            ║
║                                    ╔╗                                      ║
║                                   ╔╝╚╗                                     ║
║                                  ╔╝  ╚╗                                    ║
║                                 ╔╝    ╚╗                                   ║
║    ██████╗██╗  ██╗███████╗     ╚╗    ╔╝     ██╗    ██╗ ██████╗ ██████╗     ║
║   ██╔════╝██║  ██║██╔════╝      ╚╗  ╔╝      ██║    ██║██╔═══██╗██╔══██╗    ║
║   ██║     ███████║█████╗         ╚╗╔╝       ██║ █╗ ██║██║   ██║██████╔╝    ║
║   ██║     ██╔══██║██╔══╝          ╚╝        ██║███╗██║██║   ██║██╔══██╗    ║
║   ╚██████╗██║  ██║███████╗                  ╚███╔███╔╝╚██████╔╝██║  ██║    ║
║    ╚═════╝╚═╝  ╚═╝╚══════╝                   ╚══╝╚══╝  ╚═════╝ ╚═╝  ╚═╝    ║
║                                                                            ║
║   ┌─────────────────────────────────────────────────────────────────────┐  ║
║   │                                                                     │  ║
║   │   "The only way to do great work is to love what you do."           │  ║
║   │                                          — Steve Jobs               │  ║
║   │                                                                     │  ║
║   └─────────────────────────────────────────────────────────────────────┘  ║
║                                                                            ║
║                         v0.4  |  BUILDING THE FUTURE                       ║
║                                                                            ║
╚════════════════════════════════════════════════════════════════════════════╝)";

	delay(3000);

	cls();
	setcolor(YELLOW);
	//最后一个LOGO，感觉有点像元素周期表，这个算不了什么，但也很帅
	cout << R"(╔════════════════════════════════════════════════════════════════════════════╗
║                                                                            ║
║                               .....                                        ║
║                            .+*********+.                                   ║
║                          .+**+++***+++**+.                                 ║
║                        .+++***********+++.                                 ║
║                       .++******+ +******++..                               ║
║                      .+++******   ******++++.                              ║
║     ██████╗██╗  ██╗   .+++****       ****+++..    ██╗    ██╗ ██████╗       ║
║    ██╔════╝██║  ██║    .++**+         +**+++.     ██║    ██║██╔══██╗       ║
║    ██║     ███████║      .+++           +++..      ██║ █╗ ██║██║   ██║      ║
║    ██║     ██╔══██║        .+           +.         ██║███╗██║██║   ██║      ║ ║
║    ╚██████╗██║  ██║          .         .           ╚███╔███╔╝██║  ██║      ║
║     ╚═════╝╚═╝  ╚═╝                                   ╚══╝╚══╝ ╚═╝  ╚═╝    ║
║                                                                            ║
║                          ┌─────────────────────┐                           ║
║                          │   H    He           │                           ║
║                          │   Li   Be    B   C  │                           ║
║                          │   N    O     F   Ne │                           ║
║                          │   Na   Mg    Al  Si │                           ║
║                          │   P    S     Cl  Ar │                           ║
║                          └─────────────────────┘                           ║
║                                                                            ║
║                    ╔══════════════════════════════════════╗                ║
║                    ║    元 素  |  工 业  |  宇 宙         ║                ║
║                    ╚══════════════════════════════════════╝                ║
║                                                                            ║
╚════════════════════════════════════════════════════════════════════════════╝)";
	delay(3000);
	cls();
	setcolor(RESET);
}
//异常处理，防止游戏崩溃，增加稳定性，后续版本可能会增加一些日志记录功能，记录玩家的操作和游戏的状态，方便调试和优化
//不写注释我自己都快看不懂了
//TODO:增加一些日志记录功能，记录玩家的操作和游戏的状态，方便调试和优化
void setupExceptionHandler() {
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    _set_abort_behavior(0, _WRITE_ABORT_MSG);
}

// ======================== 存档数据结构 ========================
struct InventoryItem {
    string name;
    long long quantity;
    string category;   // ore, ingot, chemical, fuel, part, etc.
    double value;      // 基础价值
};

struct PlayerData {
    string name;
    long long level;
    long long coins;
    long long goldBars;
    long long exp;
    time_t createdAt;
    time_t lastPlayed;
    //我去，差点没加上inventory
    //inventory以及一些功能
	//inventory是一个vector，里面是InventoryItem结构体，包含物品名称、数量、类别（矿石、零件、化学品等）和基础价值（用于交易和计算利润）
    
    vector<InventoryItem> inventory;

    
    void additem(const string& itemName, long long quantity, const string& category = "misc", double value = 0) {
        for (auto& item : inventory) {
            if (item.name == itemName) {
                item.quantity += quantity;
                return;
            }
        }
        inventory.push_back({ itemName, quantity, category, value });
    }

    bool hasitem(const string& itemName, long long quantity = 1) {
        for (auto& item : inventory) {
            if (item.name == itemName && item.quantity >= quantity) {
                return true;
            }
        }
        return false;
    }
};



struct Machine {
    string id;
    string type;       // 粉碎机、电解机、车床等
    int level;
    bool isRunning;
    int x, y;          // 工厂布局坐标
    map<string, int> inventory;  // 机器内部库存
    long long powerConsumption;
};

struct Factory {
    string name;
    vector<Machine> machines;
    long long totalPower;
    long long usedPower;
    map<string, int> centralStorage;  // 中央仓库

    
};


struct ShopOffer {
    string name;
    long long price;
    int stock;
	string itemCategory;   // ore, ingot, chemical, fuel, part, etc.
	string description;
};

struct SHOP {
	vector<ShopOffer> items;
	//这里可能会有一些特殊的商店，比如黑市、科技商店等等，后续版本可能会增加一些特殊的商店，提供一些独特的物品和服务
};

struct ChemicalKnowledge {
    string formula;
    bool discovered;
    long long discoveryTime;
};

struct Quest {
    int id;
    string name;
    int progress;
    bool completed;
};

struct Planet {
    string name;
    bool unlocked;
    vector<string> resources;
    long long miningRigs;           // 采矿设备数量
    long long extractedAmount;
};

PlayerData player;
Factory factory;
//物品操作
void addItem(const string& itemName, long long quantity, const string& category = "misc", double value = 0) {
    for (auto& item : player.inventory) {
        if (item.name == itemName) {
            item.quantity += quantity;
            return;
        }
    }
    player.inventory.push_back({ itemName, quantity, category, value });
}

bool hasItem(const string& itemName, long long quantity = 1) {
    for (auto& item : player.inventory) {
        if (item.name == itemName && item.quantity >= quantity) {
            return true;
        }
    }
    return false;
}

int getItemCount(const string& itemName) {
    for (auto& item : player.inventory) {
        if (item.name == itemName) {
            return item.quantity;
        }
    }
    return false;
}

int removeItem(const string& itemName, long long quantity) {
    for (auto& item : player.inventory) {
        if (item.name == itemName) {
            if (item.quantity >= quantity) {
                item.quantity -= quantity;
                typewriter("消耗了 " + itemName + " x " + to_string(quantity));
                return quantity;
            }
            else {
                int removed = item.quantity;
                item.quantity = 0;
                typewriter("注意! " + item.name + "的数量不足!\n" + "需要: " + to_string(quantity) + " 实际: " + to_string(removed), 40);
                return removed;
            }
        }
    }
    typewriter("注意! " + itemName + "不存在", 40);
    return false;
}

//硬币、金条操作
void addCoins(long long amount) {
    player.coins += amount;
    typewriter("进账" + to_string(amount) + "个" + "硬币", 40);
}

bool removeCoins(long long amount) {
    if (player.coins >= amount) {
        player.coins -= amount;
        typewriter("支出" + to_string(amount) + "个" + "硬币", 40);
        return true;

    }
    else {
        typewriter("注意! 你的硬币不足!\n需要: " + to_string(amount) + " 实际: " + to_string(player.coins), 40);
        return false;
    }
}

void addGoldBars(long long amount) {
    player.goldBars += amount;
    typewriter("进账" + to_string(amount) + "个" + "金条", 40);
}

bool removeGoldBars(long long amount) {
    if (player.goldBars >= amount) {
        player.goldBars -= amount;
        typewriter("支出" + to_string(amount) + "个" + "金条", 40);
        return true;
    }
    else {
        typewriter("注意! 你的金条不足!\n需要: " + to_string(amount) + " 实际: " + to_string(player.goldBars), 40);
        return false;
    }
}

//经验操作
void addExp(long long amount) {
    player.exp += amount;
    typewriter("获得了 " + to_string(amount) + " 点经验", 40);
    //升级系统，n级别所需经验= 100 * n+当前等级*20
    long long requiredExp = 100 * player.level + player.level * 20;
    if (player.exp >= requiredExp) {
        player.level++;
        player.exp -= requiredExp;
        typewriter("恭喜你升级了！当前等级: " + to_string(player.level), 40);
    }
}

//对机器的操作，添加机器到工厂里，要写的东西稍微会多一点
void addMachine(const string& name, const string& type, int x = 0, int y = 0) {
    Machine newMachine;
    newMachine.id = name;
    newMachine.type = type;
    newMachine.level = 1;
    newMachine.isRunning = false;
    newMachine.x = x;
    newMachine.y = y;
    newMachine.powerConsumption = 100 * newMachine.level; // 简单的功率消耗计算
    factory.machines.push_back(newMachine);
    typewriter("wow!添加了 " + type + " 到工厂！", 40);
}


//===========教程===========

enum TutorialEvent {
    OPEN_SHOP,
    BUY_MACHINE,
    CONNECT_POWER,
    USE_MACHINE,
    COMPLETE_QUEST,
    UNLOCK_PLANET //这个我瞎写的，没啥用
};

struct TutorialStep {
    string id;
    string title;
    string description;
    string triggerEvent;
    string requiredItem;
    string targetMachine;
    bool isCompleted;
    function<bool(PlayerData&, Factory&)> condition;
};

class TutorialManager {
private:
    vector<TutorialStep> steps;
    map<string, bool> completedFlags;
    int currentStepIndex;
    bool tutorialActive;

    void showStep(const TutorialStep& step) {
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        cout << "\n══════════════ 新 手 教 程 ══════════════\n";
        cout << "||" << step.title << string(50 - step.title.length(), ' ') << "|| \n";
        cout << "═════════════════════════════════════\n";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
        cout << "\n" << step.description << "\n" << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        cout << "\n [按任意键继续]";
        cin.get();
		cin.get(); //防止屑残留,太屑了
    }
public:
    PlayerData player;
    Factory factory;

    //这里要加入一个拆除机器，拆除机器会返还一部分资源，返还的资源数量取决于机器的等级和类型，尤其是珍贵的硬币，LOL
    //但是我们先放着，等后续版本再说吧
    //咕咕咕

    
	//这里可能会添加一些暂时没啥用处的函数
	//解锁新配方、解锁新行星等等，反正就是一些会在后续版本里用到的函数，先放着
    //咕咕咕
    
	//包含特定物品的礼包，新手专属！
    //新手包:富集粗黄铁矿，水，硬币，锡粉,氢氧化钠，粗硫粉
    void giveStarterPack() {
        addItem("water", 20, "chemical", 5);
        addItem("raw rich pyrite", 10, "ore", 10);
        addItem("raw coal", 5, "fuel", 15);
		addItem("tin powder", 5, "ore", 20);
		addItem("sodium hydroxide", 5, "chemical", 25);
		addItem("raw sulfur powder", 5, "ore", 20);
        addCoins(100);
        typewriter("你获得了新手礼包！里面有一些水、矿石、燃料和硬币，可以帮助你快速开始游戏！加油！", 40);
	}

    //农作物包，这里包含一些可以提取出化学物质的作物
    //包含:土豆，玉米，甘蔗，棉花，橡胶树苗
    void givefarmerPack() {
		addItem("potato", 10, "crop", 5);
		addItem("corn", 10, "crop", 5);
		addItem("sugarcane", 10, "crop", 5);
		addItem("cotton", 10, "crop", 5);
		addItem("rubber tree sapling", 5, "crop", 10);
        typewriter("你获得了农作物礼包！里面有一些玉米、土豆、甘蔗、棉花和橡胶树苗，可以帮助你提取出各种化学物质！", 40);
	}

	//工具包,包含一些可以提高生产效率的工具，品质一般，但是耐用
	//包含:铁锹，钒钢扳手，不锈钢螺丝刀，化学单元(一种储存化学物品的东西)，石英玻璃瓶，聚四氟乙烯烧杯
    void giveToolPack() {
		addItem("shovel", 1, "tool", 20);
		addItem("vanadium steel wrench", 1, "tool", 20);
		addItem("stainless steel screwdriver", 1, "tool", 20);
		addItem("chemical unit", 1, "tool", 20);
		addItem("quglass bottle", 1, "tool", 20);
		addItem("polytetrafluoroethylene beaker", 1, "tool", 20);
		typewriter("别走！你还需呀一些工具来提高生产效率！这个工具包里有一些铁锹、扳手、螺丝刀、化学单元、玻璃瓶和烧杯，可以帮助你更高效地生产和提取化学物质！", 40);
    }

    //新手快被物品塞满了！但这还不够！给点化学品吧
	//包含:硫酸，盐酸，硝酸，氟，氯，甲烷
    void giveChemistryPack() {
		addItem("sulfuric acid", 10, "chemical", 20);
		addItem("hydrochloric acid", 10, "chemical", 20);
		addItem("nitric acid", 10, "chemical", 20);
		addItem("fluorine", 10, "chemical", 20);
		addItem("chlorine", 10, "chemical", 20);
		addItem("methane", 10, "chemical", 20);
        typewriter("你获得了化学品礼包！里面有一些硫酸、盐酸、硝酸、氟、氯和甲烷，可以帮助你进行各种化学实验！", 40);
    }
	//随机的给萌新的话，可以是一些提示、建议或者鼓励的话，增加一些人情味
    void randomWordToNoob() {
		int r=randint(1, 5);
        switch (r) {
        case 1:
                typewriter("记住，化学世界充满了无限的可能性！勇敢地去探索吧！", 40);
				break;
        case 2:
			    typewriter("不要害怕失败，每一次失败都是通往成功的必经之路！", 40);
                break;
        case 3:
			    typewriter("保持好奇心，化学世界有很多有趣的东西等着你去发现！", 40);
                break;
		case 4:
            typewriter("和其他玩家交流，分享你的发现和经验，化学世界是一个大家庭！", 40);
			typewriter("加入我们的社区(等等吧)，参加活动，结交朋友，一起在化学世界中冒险！(抱歉，没有联机……)", 40);
			break;
        case 5:
			typewriter("玩得开心！化学世界是为了让你享受化学的乐趣而设计的！", 40);
        }
    }
    
    //这里突然想起来一个事情，是不是要加一个显示库存，必须帅一点，UI必须好看一点，666666666666666
	//感觉可以加入一个ASCII艺术的库存界面，显示玩家的物品和数量，甚至可以根据物品的类别用不同的颜色显示，增加一些视觉效果
    void displayInventory() {
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        cout << "\n══════════════ 你 的 库 存 ══════════════\n";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
        cout << "\n物品名称           数量           类别           价值\n";
        cout << "-------------------------------------------------------------\n";
        for (const auto& item : player.inventory) {
            cout << item.name << string(20 - item.name.length(), ' ')
                << item.quantity << string(15 - to_string(item.quantity).length(), ' ')
                << item.category << string(15 - item.category.length(), ' ')
                << item.value << "\n";
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        cout << "\n [按任意键继续]";
		cin.get();
    }

    TutorialManager() {
        tutorialActive = true;
        currentStepIndex = 0;
        initSteps();
    }

    void initSteps() {
        steps = {
            {"first_sale","第一桶金","完成你的第一笔销售！去卖掉一些东西吧!\n提示:考虑考虑水？量大管饱!","OPEN_SHOP","","",false,nullptr},
            {"buy_electrolyzer","购买电解机","现在你有了一些钱，可以购买你的第一台机器了！去商店买一台电解机吧！\n提示:电解机可以把水分解成氢气和氧气！","BUY_MACHINE","","电解机",false,nullptr},
            {"power_up","工厂通电","对了，你的工厂需要电力才能运行！去商店买个电池箱，给你的机器供电吧！\n提示:电池箱可以储存电力并为机器供电！","CONNECT_POWER","","",false,nullptr},
            {"electrolysis_salt","电解食盐","wow! 你已经有了电解机了！现在试着把食盐电解成氢气、氯气,还可以……弄些氢氧化钠！\n提示:食盐的化学式是NaCl！","USE_MACHINE","盐","电解机",false,
            [](PlayerData& p,Factory& f)->bool {
                return p.hasitem("sodium") && p.hasitem("cloride");
            }},
            {"sodium_is_so_explosive","活泼的钠","你成功电解出了钠！现在试着把它放在水里看看会发生什么吧！\n提示:钠+水=氢气+氢氧化钠,你需要很多！","CHEMICAL_REACTION ","钠","水",false,nullptr}
        };

    }
	//检查事件触发，判断是否满足条件，完成步骤，给予奖励，增加一些成就感
    void checkEvent(const string& event, const string& itemName, PlayerData& player, Factory& factory) {
        if (!tutorialActive) return;
        for (auto& step : steps) {
            if (step.isCompleted) continue;

            if (step.triggerEvent == event) {
                if (step.requiredItem.empty() || step.requiredItem == itemName) {
                    showStep(step);
                    step.isCompleted = true;
                    completedFlags[step.id] = true;

                }
            }
        }
    }
    
    //根据步骤ID给予奖励，奖励可以是经验、金币、物品等
    void giveReward(const string& stepId, PlayerData& player) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        cout << "\ngood job! 奖励给你:\n";
        if (stepId == "first_sale") {
            player.coins += 50;
            cout << "+50 coins!\n";
        }
        else if (stepId == "buy_electrolyzer") {
            player.coins += 100;
            cout << "+100 coins!\n";
        }
        else if (stepId == "power_up") {
            player.additem("raw coal", 1, "item", 15);
            cout << "+1 粗煤炭\n";
        }
        else if (stepId == "electrolysis_salt") {
            player.exp += 100;
            cout << "+100 exp\n";
        }
        else if (stepId == "sodium_is_so_explosive") {
            player.exp += 200;
            cout << "+200 exp\n";
        }

        delay(2000);
    }
	//检查步骤是否完成
    bool isStepCompleted(const string& stepId) {
		auto it = completedFlags.find(stepId);
		return it != completedFlags.end() && it->second;
    }

	//跳过教程，直接完成所有步骤，给予所有奖励
    void skipTutorial() {
        tutorialActive = false;
        for (auto& step : steps) {
            step.isCompleted = true;
            completedFlags[step.id] = true;
        }
        cout << "教程已跳过！祝你在化学世界玩得开心！\n";
        cout << "看来你很有自信！\n";
        delay(2000);
    }
    
    bool isActive() {
        return tutorialActive;
	}

};



vector<ShopOffer> shopItems;


// ======================== 存档管理器 ========================
//💾没有存档的游戏不是好游戏
//存档管理器负责保存和加载游戏数据，管理玩家的进度和状态，以及提供一些与存档相关的功能
class SaveManager {
private:
    TutorialManager tutorial;
    string saveDirectory = "saves/";
    string currentSaveFile = "";
    PlayerData player;
    Factory factory;
    vector<InventoryItem> inventory;
    vector<ChemicalKnowledge> chemistry;
    vector<Quest> quests;
    vector<Planet> planets;
    
    // 辅助函数：创建存档目录
    void ensureSaveDirectory() {
        if (!fs::exists(saveDirectory)) {
            fs::create_directory(saveDirectory);
        }
    }
    // 辅助函数：获取当前时间字符串
    string getTimeString(time_t t) {
        char buffer[80];
        struct tm* timeinfo = localtime(&t);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return string(buffer);
    }

    string prepareString(const string& raw) {
        string utf8 = EncodingHelper::gbkToUtf8(raw);
        return EncodingHelper::sanitizeUtf8(utf8);
    }

    string displayString(const string& utf8Str) {
        return EncodingHelper::utf8ToGbk(utf8Str);
    }


public:

    SaveManager() {
        ensureSaveDirectory();
    }

    

    // ========== 玩家数据操作 ==========
    void createNewGame() {
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
        cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                      新 游 戏 向 导                           ║
╚═══════════════════════════════════════════════════════════════╝
)";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);

        cout << "\n* 请输入你的名字(为了避免一些奇怪的BUG，请不要输入A-Z,a-z,0-9,_,-以外的字符！敬请谅解……): ";
        cout << "\n*如果不输入，默认是Chemist";
        cout << "\n!!!非上述的字符会被过滤掉！";
        string rawName;
        getline(cin, rawName);
        string cleanName;
        for (char c : rawName) {
            if ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_' || c=='-') {
                cleanName += c;
            }
        }
        if (cleanName.empty()) {
            player.name = "Chemist";
        }
        else {
            player.name = cleanName;
        }

        // 初始化玩家数据
        player.level = 1;
        player.coins = 100;
        player.goldBars = 0;
        player.exp = 0;
        player.createdAt = time(nullptr);
        player.lastPlayed = time(nullptr);

        // 初始化库存（新手包）
        inventory = {
            {"water", 10, "chemical", 5},
            {"raw rich pyrite", 5, "ore", 10},
            {"raw coal", 3, "fuel", 15}
        };

        // 初始化工厂
        factory.name = player.name + "'s Factory";
        factory.machines = {};
        factory.totalPower = 0;
        factory.usedPower = 0;

        // 初始化化学知识（已发现的配方）
        chemistry = {
            {"H2O", true, (int)time(nullptr)},
        };

        // 初始化任务
        quests = {
            {1, "First_sale", 0, false},
            {2, "ores go GRRR", 0, false},
            {3, "a hundred bucks", 0, false}
        };

        // 初始化星球
        planets = {
            {"Earth", true, {"pyrite", "coal", "oil"}, 0, 0}
        };

        cout << "\nhi!   欢迎，" << player.name << "！你的化学世界之旅开始了！\n";
        cout << "hi!正在创建存档...\n";

        saveGame("auto");

        cout << "wow! 存档创建成功！\n";
        Sleep(1500);
        cls();
        cout << "欢迎！" << player.name << "！准备好开始你的化学世界之旅吧！\n";
        cout<<"你需要新手教程吗? (y/n): ";
        char choice;
        cin >> choice;
		cin.ignore();
        if (choice == 'n' || choice == 'N') {
            tutorial.skipTutorial();
        }
        else {
			typewriter("好的！让我们开始新手教程吧！", 40);
            typewriter("教程会一步步引导你了解游戏的基本操作和机制。", 40);
        }

    }

	void triggerEvent(const string& event, const string& itemName = "") {
        if (tutorial.isActive()) {
			tutorial.checkEvent(event, itemName, player, factory);
        }
    }

    TutorialManager& getTutorial() {
        return tutorial;
	}

    

    // 保存游戏
    bool saveGame(const string& slotName) {
        json saveData;

        // 更新时间戳
        player.lastPlayed = time(nullptr);

        // === 玩家数据 ===
        saveData["player"]["name"] = player.name;
        saveData["player"]["level"] = player.level;
        saveData["player"]["coins"] = player.coins;
        saveData["player"]["goldBars"] = player.goldBars;
        saveData["player"]["exp"] = player.exp;
        saveData["player"]["createdAt"] = player.createdAt;
        saveData["player"]["lastPlayed"] = player.lastPlayed;

        // === 库存数据 ===
        for (const auto& item : inventory) {
            json itemJson;
            itemJson["name"] = item.name;
            itemJson["quantity"] = item.quantity;
            itemJson["category"] = item.category;
            itemJson["value"] = item.value;
            saveData["inventory"].push_back(itemJson);
        }

        // === 工厂数据 ===
        saveData["factory"]["name"] = factory.name;
        saveData["factory"]["totalPower"] = factory.totalPower;
        saveData["factory"]["usedPower"] = factory.usedPower;

        for (const auto& machine : factory.machines) {
            json machineJson;
            machineJson["id"] = machine.id;
            machineJson["type"] = machine.type;
            machineJson["level"] = machine.level;
            machineJson["isRunning"] = machine.isRunning;
            machineJson["x"] = machine.x;
            machineJson["y"] = machine.y;
            machineJson["powerConsumption"] = machine.powerConsumption;

            // 机器内部库存
            for (const auto& [item, qty] : machine.inventory) {
                machineJson["inventory"][item] = qty;
            }
            saveData["factory"]["machines"].push_back(machineJson);
        }

        // 中央仓库
        for (const auto& [item, qty] : factory.centralStorage) {
            saveData["factory"]["centralStorage"][item] = qty;
        }

        // === 化学知识 ===
        for (const auto& chem : chemistry) {
            json chemJson;
            chemJson["formula"] = chem.formula;
            chemJson["discovered"] = chem.discovered;
            chemJson["discoveryTime"] = chem.discoveryTime;
            saveData["chemistry"].push_back(chemJson);
        }

		// === 商店数据 ===
		for (const auto& item : shopItems) {
            json itemJson;
            itemJson["name"] = item.name;
            itemJson["itemCategory"] = item.itemCategory;
            itemJson["price"] = item.price;
            itemJson["stock"] = item.stock;
            saveData["shopItems"].push_back(itemJson);
        }

        // === 任务系统 ===
        for (const auto& quest : quests) {
            json questJson;
            questJson["id"] = quest.id;
            questJson["name"] = quest.name;
            questJson["progress"] = quest.progress;
            questJson["completed"] = quest.completed;
            saveData["quests"].push_back(questJson);
        }

        // === 星球数据 ===
        for (const auto& planet : planets) {
            json planetJson;
            planetJson["name"] = planet.name;
            planetJson["unlocked"] = planet.unlocked;
            planetJson["resources"] = planet.resources;
            planetJson["miningRigs"] = planet.miningRigs;
            planetJson["extractedAmount"] = planet.extractedAmount;
            saveData["planets"].push_back(planetJson);
        }

        // === 元数据 ===
        saveData["metadata"]["version"] = "1.0";
        saveData["metadata"]["saveTime"] = time(nullptr);

        // 写入文件
        string filename = saveDirectory + slotName + ".json";
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "X 无法保存游戏！" << endl;
            return false;
        }

        file << saveData.dump(4);  
        file.close();

        currentSaveFile = slotName;
        cout << "wow! 游戏已保存到 " << filename << endl;
        return true;
    }

    // 加载游戏
    bool loadGame(const string& slotName) {
        string filename = saveDirectory + slotName + ".json";
        ifstream file(filename);

        if (!file.is_open()) {
            cerr << "X 找不到存档文件！" << endl;
            return false;
        }

        json saveData;
        file >> saveData;
        file.close();

        try {
            // === 加载玩家数据 ===
            player.name = saveData["player"]["name"];
            player.level = saveData["player"]["level"];
            player.coins = saveData["player"]["coins"];
            player.goldBars = saveData["player"]["goldBars"];
            player.exp = saveData["player"]["exp"];
            player.createdAt = saveData["player"]["createdAt"];
            player.lastPlayed = saveData["player"]["lastPlayed"];

            // === 加载库存 ===
            inventory.clear();
            for (const auto& item : saveData["inventory"]) {
                inventory.push_back({
                    item["name"],
                    item["quantity"],
                    item["category"],
                    item["value"]
                    });
            }

            // === 加载工厂 ===
            factory.name = saveData["factory"]["name"];
            factory.totalPower = saveData["factory"]["totalPower"];
            factory.usedPower = saveData["factory"]["usedPower"];

            factory.machines.clear();
            for (const auto& machineJson : saveData["factory"]["machines"]) {
                Machine m;
                m.id = machineJson["id"];
                m.type = machineJson["type"];
                m.level = machineJson["level"];
                m.isRunning = machineJson["isRunning"];
                m.x = machineJson["x"];
                m.y = machineJson["y"];
                m.powerConsumption = machineJson["powerConsumption"];

                if (machineJson.contains("inventory")) {
                    for (auto& [item, qty] : machineJson["inventory"].items()) {
                        m.inventory[item] = qty;
                    }
                }
                factory.machines.push_back(m);
            }

            // 中央仓库
            factory.centralStorage.clear();
            if (saveData["factory"].contains("centralStorage")) {
                for (auto& [item, qty] : saveData["factory"]["centralStorage"].items()) {
                    factory.centralStorage[item] = qty;
                }
            }

            // === 加载化学知识 ===
            chemistry.clear();
            for (const auto& chem : saveData["chemistry"]) {
                chemistry.push_back({
                    chem["formula"],
                    chem["discovered"],
                    chem["discoveryTime"]
                    });
            }

            // === 加载任务 ===
            quests.clear();
            for (const auto& quest : saveData["quests"]) {
                quests.push_back({
                    quest["id"],
                    quest["name"],
                    quest["progress"],
                    quest["completed"]
                    });
            }

            // === 加载星球 ===
            planets.clear();
            for (const auto& planet : saveData["planets"]) {
                Planet p;
                p.name = planet["name"];
                p.unlocked = planet["unlocked"];
                p.resources = planet["resources"].get<vector<string>>();
                p.miningRigs = planet["miningRigs"];
                p.extractedAmount = planet["extractedAmount"];
                planets.push_back(p);
            }

            currentSaveFile = slotName;
            cout << " wow! 存档加载成功！欢迎回来，" << player.name << "！\n";
            cout << " hi! 上次游戏时间: " << getTimeString(player.lastPlayed) << endl;
            return true;

        }
        catch (const exception& e) {
            cerr << "X 存档损坏！错误: " << e.what() << endl;
            return false;
        }
    }
    
    // 列出所有存档
    void listSaves() {
        ensureSaveDirectory();

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
        cout << "║                        存 档 列 表                            ║\n";
        cout << "╚═══════════════════════════════════════════════════════════════╝\n";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);

        int index = 1;
        vector<string> saveFiles;

        for (const auto& entry : fs::directory_iterator(saveDirectory)) {
            if (entry.path().extension() == ".json") {
                saveFiles.push_back(entry.path().stem().string());
                cout << index++ << ". " << entry.path().stem().string() << endl;
            }
        }

        if (saveFiles.empty()) {
            cout << "暂无存档，请先开始新游戏！\n";
        }

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    }

    // 获取玩家数据（供其他模块使用）
    PlayerData& getPlayerData() { return player; }
    vector<InventoryItem>& getInventory() { return inventory; }
    Factory& getFactory() { return factory; }

    // 添加物品到库存
    void addItem(const string& name, int quantity, const string& category, double value) {
        for (auto& item : inventory) {
            if (item.name == name) {
                item.quantity += quantity;
                return;
            }
        }
        inventory.push_back({ name, quantity, category, value });
    }

    // 显示玩家状态（调试用）
    void showPlayerStats() {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        cout << "\n══════════════ 玩家状态 ══════════════\n";
        cout << " 名字: " << player.name << endl;
        cout << " 等级: " << player.level << endl;
        cout << " 硬币: " << player.coins << endl;
        cout << " 金条: " << player.goldBars << endl;
        cout << " 经验: " << player.exp << endl;
        cout << " 库存物品: " << inventory.size() << " 种\n";
        cout << " 机器数量: " << factory.machines.size() << endl;
        cout << "═════════════════════════════════════\n";
        cout << "金条是一种高级货币!事实上，你可以制作它们！";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        delay(2000);
        cls();
    }

    void handleShopAction(const string& action, const string& itemName) {
        if (action == "buy") {
            if (itemName == "electrolyzer") {
                if (player.coins >= 100) {
                    player.coins -= 100;
                    addItem("electrolyzer", 1, "machine", 0);
                    cout << "购买成功！你现在有了电解机！\n";
                    triggerEvent("BUY_MACHINE", "electrolyzer");
                }
                else {
                    cout << "你的钱不够！\n";
                }
            }
            else if (itemName == "battery box") {
                if (player.coins >= 150) {
                    player.coins -= 150;
                    addItem("battery box", 1, "machine", 0);
                    cout << "购买成功！你现在有了电池箱！\n";
                    triggerEvent("BUY_MACHINE", "battery box");
                }
                else {
                    cout << "你的钱不够！\n";
                }
            }
            else {
                cout << "商店里没有这个物品！\n";
            }
        }
	}

    void handleMachineUse(const string& machineType, const string& inputItem) {
        if (machineType == "electrolyzer" && inputItem == "salt") {
            if (player.hasitem("salt", 1)) {
                player.coins += 20; // 卖掉产物赚点钱
                player.additem("sodium", 1, "chemical", 10);
                player.additem("chloride", 1, "chemical", 10);
                player.additem("sodium hydroxide", 1, "chemical", 15);
                player.coins += 20; // 卖掉产物赚点钱
                cout << "你成功电解了食盐，获得了钠、氯和氢氧化钠！\n";
                triggerEvent("USE_MACHINE", "salt");
            }
            else {
                cout << "你没有足够的食盐！\n";
            }
        }
	}
    //这里先写个简单的占位
	//但是以后可以根据不同的化学反应写更多的逻辑，甚至可以有一些随机事件，比如偶尔会有个小爆炸什么的
	//LoL，钠和水的反应确实很剧烈，所以可以考虑在这个反应里加点特效什么的
	//当然了，这些都是后续可以考虑的内容，现在先把基本的逻辑写出来
    void handleChemicalReaction(const string& item1, const string& item2) {
        if ((item1 == "sodium" && item2 == "water") || (item1 == "water" && item2 == "sodium")) {
            if (player.hasitem("sodium", 1) && player.hasitem("water", 1)) {
                player.additem("hydrogen", 1, "chemical", 5);
                player.additem("sodium hydroxide", 1, "chemical", 15);
                player.coins += 30; // 卖掉产物赚点钱
                cout << "你把钠放在水里了！发生了剧烈的反应，产生了氢气和氢氧化钠！\n";
                triggerEvent("CHEMICAL_REACTION", "sodium");
            }
            else {
                cout << "你没有足够的钠或水！\n";
            }
        }
    }

    void interactwithmachines() {
        cout << "你想用哪个机器？(输入机器名称，或者输入exit退出): ";
        string machineName;
        getline(cin, machineName);
        if (machineName == "exit") {
            return;
        }
        else if (machineName == "electrolyzer") {
            cout << "你想放什么东西进去电解机？(输入物品名称，或者输入exit退出): ";
            string inputItem;
            getline(cin, inputItem);
            if (inputItem == "exit") {
                return;
            }
            handleMachineUse("electrolyzer", inputItem);
        }else if(machineName=="化学反应釜") {
            cout << "你想放什么东西进去化学反应釜？(输入物品名称，或者输入exit退出): ";
            string inputItem;
            getline(cin, inputItem);
            if (inputItem == "exit") {
                return;
            }
            handleChemicalReaction(inputItem, "water"); // 示例，实际可以根据需求调整
        }
        else {
            cout << "没有这个机器！\n";
        }
	}

};

//物品分析系统
class ItemAnalyzer {
public:
    //物品多如牛毛，这里需要加一个tags识别器，方便后续的管理（tags有富集、贫瘠、不稳定、易燃、腐蚀性等）
	//需要先打个非常大的标签库，后续可以根据需要添加更多的标签，这样就可以更好地管理和分类物品了
    //希望兄弟有兴趣能帮我写一下，因为这里我不是很擅长，磕一个，orz
    //预留区域
};


//商店系统
enum category {
    machine,
    chemical,
    tool,
    pack,
    ore
};
struct ShopItem {
    string name;
    string description;
    int price;
    category itemCategory; // 例如 "machine", "chemical", "tool" 等
};

struct SpecialOffer {
    ShopOffer offer;
    time_t startTime;
    time_t endTime;
};

//礼包，玩家可以购买，礼包里有一些物品
struct Packs{
    string name;
    string description;
    long long price;
	vector<ShopItem> contents; // 礼包内容
};

vector<Packs> shopPacks;
class Shop {
public:
	SaveManager saveManager;
    PlayerData player;
    //展示系统商店
    void showShop() {
        cls();
        CoolAnimation::rainbowText( R"(          _____                    _____                   _______                   _____          
         /\    \                  /\    \                 /::\    \                 /\    \         
        /::\    \                /::\____\               /::::\    \               /::\    \        
       /::::\    \              /:::/    /              /::::::\    \             /::::\    \       
      /::::::\    \            /:::/    /              /::::::::\    \           /::::::\    \      
     /:::/\:::\    \          /:::/    /              /:::/~~\:::\    \         /:::/\:::\    \     
    /:::/__\:::\    \        /:::/____/              /:::/    \:::\    \       /:::/__\:::\    \    
    \:::\   \:::\    \      /::::\    \             /:::/    / \:::\    \     /::::\   \:::\    \   
  ___\:::\   \:::\    \    /::::::\    \   _____   /:::/____/   \:::\____\   /::::::\   \:::\    \  
 /\   \:::\   \:::\____\/:::/  \:::\    /::\____\|:::|    |     |:::|    | /:::/\:::\   \:::\____\ 
/::\   \:::\   \:::\    \  /:::/  \:::\    /:::/    / \:::\    \   /:::/    / \::/    \:::\  /:::|    |
\:::\   \:::\   \::/    /\::/    \:::\  /:::/    /   \:::\    /:::/    /   \/_____/\:::\/:::/    / 
 \:::\   \:::\   \/____/  \/____/ \:::\/:::/    /     \:::\    /:::/    /             \::::::/    /  
   \:::\   \:::\____\               \::::/    /       \:::\__/:::/    /               \::::/    /   
    \:::\  /:::/    /               /:::/    /         \::::::::/    /                 \::/____/    
     \:::\/:::/    /               /:::/    /           \::::::/    /                                
      \::::::/    /               /:::/    /             \::::/    /                   ~~          
       \::::/    /               /:::/    /               \::/____/                                 
        \::/    /                \::/    /                 ~~                                       
         \/____/                  \/____/                )",0);
        cout << "\n\n\n";
		setcolor(CYAN);
        typewriter( "欢迎来到系统商店，当前时间:");
		time_t now = time(nullptr);
		CoolAnimation::rainbowText(gettimeString(now)+"\n\n", 5);
        typewriter("这里有许多有趣的物品，快来看看吧！\n\n\n");
		setcolor(RESET);
	}

    //一个灵活的函数，添加商品,并自动把物品分类展示
    void addTh(category itemC, const string& name, const string& description, long long price) {
		ShopOffer newItem;
        newItem.name = name;
		newItem.description = description;
        newItem.price = price;
		newItem.itemCategory = itemC;
		shopItems.push_back(newItem);
		//分类展示
        setcolor(GREEN);
		cout <<"分类 |  ";
        cout << (itemC == machine ? "机器" : itemC == chemical ? "化学品" : itemC==tool ?"工具": "矿物") << " | ";
		cout << "名称: " << name << " | ";
		cout << "价格: " << price << " coins\n";
		cout << "\n描述: " << description << "\n\n";
        //上架时间
        setcolor(CYAN);
        time_t now = time(nullptr);
        typewriter("上架时间:" + gettimeString(now), 0);
        cout << "\n\n";
        setcolor(RESET);
    }

    //随机生成特别商品,使用随机数抽取，这里先预设一些
    string getRandomItem() {
        //大马士革钢板，不锈钢板，硝化甘油，极富集闪锌矿，贫瘠钻石矿，高十六烷值汽油,等等
        vector<string> items = { "damascus steel plate","stainless steel plate","nitroglycerin","extra rich zinc ore","barren diamond ore","high octane gasoline" };
        vector<long long> price = { 200,400,300,250,800,80 };
        long long index = rand() % items.size();
        
        return price[index];
        return items[index];
    }
    
    

    void generateSpecialOffers() {
		int numOffers = 5; // 每次生成5个特别商品
		for (int i = 0; i < numOffers; i++) {
            category itemC = static_cast<category>(rand() % 3); // 随机分类
            string itemName = getRandomItem(); // 随机商品名称
            string description = "这是一个特别商品"; // 简单描述
            long long price = (rand() % 400) + 100; // 随机价格在100-499之间
            addTh(itemC, itemName, description, price);
        }
    }

    //包含特定物品的礼包
    // 快速发展礼包
    //包含:富集粗黄铁矿，富集粗闪锌矿，镓锭，富集粗磁铁矿，贫瘠钻石矿，工业酒精，水，锡粉,氢氧化钠，粗硫粉，生橡胶末，高密度聚乙烯板，甲烷,复合耐热合金板
    
    void BuyQuickStartPack() {
		addItem("enriched raw pyrite", 150, "ore", 10);
		addItem("enriched raw zinc ore", 50, "ore", 10);
		addItem("gallium ingot", 20, "chemical", 20);
		addItem("enriched raw magnetite", 50, "ore", 10);
		addItem("barren diamond ore", 3, "ore", 800);
		addItem("industrial alcohol", 100, "chemical", 5);
		addItem("water", 2000, "chemical", 100);
		addItem("tin powder", 10, "chemical", 8);
		addItem("sodium hydroxide", 10, "chemical", 15);
		addItem("raw sulfur powder", 10, "chemical", 20);
		addItem("raw rubber powder", 10, "chemical", 20);
		addItem("high density polyethylene sheet", 5, "tool", 25);
		addItem("methane", 10, "chemical", 5);
		addItem("composite heat resistant alloy sheet", 5, "tool", 800);
        addCoins(-2000);
        CoolAnimation::rainbowText("你购买了初级发育礼包！里面有一些水、矿石、燃料、材料、工具，价格实惠!(2000 coins)可以帮助你快速发展！加油！", 40);
    }

    //高分子-塑料礼包
	//聚苯并咪唑板，聚醚醚酮板，聚四氟乙烯板，聚乙烯板，聚丙烯板，聚氨酯板，聚甲醛板，聚碳酸酯板，聚酰胺板，聚甲基丙烯酸甲酯板,等等
    void BuyPolymerPack() {
		addItem("polybenzimidazole sheet", 25, "tool", 300);//聚苯并咪唑板，耐高温，机械性能优异，但价格有点贵
		addItem("polyether ether ketone sheet", 25, "tool", 300);//聚醚醚酮板，耐高温，机械性能优异，但价格有点贵.2
		addItem("polytetrafluoroethylene sheet", 25, "tool", 450);//聚四氟乙烯板，耐高温，化学性能优异，但价格有点贵.3
		addItem("polyethylene sheet", 50, "tool", 250);//聚乙烯板，耐化学性能优异，价格不算贵
		addItem("polypropylene sheet", 50, "tool", 250);//聚丙烯板，耐化学性能优异，价格不算贵.2
		addItem("polyurethane sheet", 50, "tool", 250);//聚氨酯板，耐磨性能优异，价格不算贵.3
		addItem("polyoxymethylene sheet", 50, "tool", 250);//聚甲醛板，机械性能优异，价格不算贵.4
		addItem("polycarbonate sheet", 50, "tool", 250);//聚碳酸酯板，机械性能优异，价格不算贵.5
		addItem("polyamide sheet", 50, "tool", 250);//聚酰胺板，机械性能优异，价格不算贵.6
		addItem("polymethyl methacrylate sheet", 50, "tool", 300);//聚甲基丙烯酸甲酯板，机械性能优异，价格不算贵.7
		CoolAnimation::rainbowText("你购买了高分子-塑料礼包！里面有各种各样的塑料板材，可以用来制作机器的外壳或者其他工具！(3000 coins)", 40);
		CoolAnimation::rainbowText("你是个塑料和高分子的狂热爱好者", 40);
		addCoins(-3000);
    }

	//能源礼包
	//高十六烷值汽油，高辛烷值汽油，高纯度液化天然气，液氨，氢气，生物柴油，极重油,轻燃油，石脑油，芳烃混合燃油
    void BuyEnergyPack() {
		addItem("high octane gasoline", 100, "fuel", 100);//高辛烷值汽油，俗称98号汽油，适合高性能发动机
		addItem("high cetane diesel", 100, "fuel", 100);//高十六烷值柴油，适合柴油发动机，俗称0号柴油
		addItem("high purity liquefied natural gas", 100, "fuel", 150);//高纯度液化天然气，适合燃气发动机，呜呼呼
		addItem("liquid ammonia", 100, "fuel", 150);//液氨，适合氨燃料电池，有点臭……
		addItem("hydrogen", 2000, "fuel", 200);//氢气，适合氢燃料电池，清洁能源！到处都是
		addItem("biodiesel", 100, "fuel", 150);//生物柴油，适合柴油发动机，由植物油或者动物脂肪制成的，好油啊，呕
		addItem("extra heavy oil", 100, "fuel", 150);//极重油，适合重型柴油发动机，但是环保组织会抗议
		addItem("light fuel oil", 100, "fuel", 150);//轻燃油，适合一些小型发动机，但是环保组织还是会抗议
		addItem("naphtha", 100, "fuel", 150);//石脑油，适合一些特殊的化学反应或者燃烧过程，那这个当燃料?挂眼科吧
		addItem("aromatic mixed fuel", 100, "fuel", 150);//芳烃混合燃油，适合一些特殊的化学反应或者燃烧过程，那这个当燃料?还是挂眼科吧
		addCoins(-1500);
        CoolAnimation::rainbowText("你购买了能源礼包！里面有各种各样的燃料，可以用来驱动你的机器和设备！(1500 coins)", 40);
        CoolAnimation::rainbowText("让你的发电机轰鸣吧！呜呜呜~", 40);
    }

    //元素礼包
	//含有大量的元素！不包括那些特别昂贵的，人造的
    //这里包含的东西特别多
    void BuyElementPack() {
        setcolor(RED);
        typewriter("警告!前方有巨量元素将要涌入你的仓库!", 40);
		addItem("hydrogen", 1000, "chemical", 5);//氢，宇宙中最丰富的元素,轻飘飘！
		addItem("helium", 1000, "chemical", 10);//氦,很简单的东西
		addItem("lithium", 100, "chemical", 20);//锂，电池的核心元素
		addItem("beryllium", 100, "chemical", 20);//铍，你可能不熟悉
		addItem("boron", 100, "chemical", 20);//硼，死谷
		addItem("carbon", 4000, "chemical", 50);//碳，你就是这个
		addItem("nitrogen", 1000, "chemical", 10);//氮，豆豆        
		addItem("oxygen", 8000, "chemical", 80);//氧,你需要这个
		addItem("fluorine", 100, "chemical", 100);//氟
		addItem("neon", 5, "chemical", 50);//氖，虽然很贵，但也不算特别昂贵的元素
		addItem("sodium", 500, "chemical", 10);//钠，这个不能直接吃！
		addItem("magnesium", 100, "chemical", 20);//镁，一听名字就很美
		addItem("aluminum", 400, "chemical", 200);//地壳中含量第三的元素,和氧是真爱
		addItem("silicon", 1000, "chemical", 200); //硅,加利福利亚!🍎
		addItem("phosphorus", 100, "chemical", 20);//磷，肥料的核心元素
		addItem("sulfur", 800, "chemical", 20);//硫，火山的核心元素,恶魔一般的物质!
		addItem("chlorine", 100, "chemical", 20);//氯，消毒剂的核心元素,和钠是真爱
		addItem("argon", 5, "chemical", 50);//氩，这东西是个稀有气体
		addItem("potassium", 100, "chemical", 20);//钾，肥料的核心元素.2
		addItem("calcium", 100, "chemical", 20);//钙，骨骼的核心元素,骨头汤不能让你补钙，望周知
		addItem("titanium", 100, "chemical", 200);//钛，炒鸡坚固的金属元素
		addItem("iron", 1500, "chemical", 50);//铁,这东西需求量大到离谱 
		addItem("nickel", 100, "chemical", 20);//镍，电池的核心元素.2
		addItem("copper", 1500, "chemical", 40);//铜,电线来啦
		addItem("zinc", 300, "chemical", 20);//锌，电池的核心元素.3,你也需要锌，所以你=电池
		addItem("gallium", 5, "chemical", 80);//镓，这东西是铝和锌的儿子，自己弄不太推荐，你还是买一点吧
		addItem("germanium", 5, "chemical", 80);//锗，和硅很像的元素
		addItem("arsenic", 100, "chemical", 20);//砷，有毒！
		addItem("selenium", 100, "chemical", 20);//硒，虽然有毒，但你必须有，不然会得病
		addItem("bromine", 100, "chemical", 20);//溴，有毒的液体元素,臭！
		addItem("krypton", 5, "chemical", 50);//氪，这东西也是个稀有气体
		addItem("xenon", 5, "chemical", 50);//氙，这东西也是个稀有气体
		addItem("radon", 5, "chemical", 30);//氡，这东西也是个稀有气体,而且有放射性，注意安全！
		addItem("cesium", 5, "chemical", 80);//铯，这东西是个非常活泼的金属元素，自己弄不太推荐，你还是买一点吧
        //富集稀土,直接包含稀土元素
		addItem("rich rare earth", 400, "chemical", 80);
		//钫太贵了，就不放了,这东西你能造出来瞬间就成大佬了
		addItem("radium", 15, "chemical", 80);//镭，这东西是个放射性元素，谋害居里夫人!
        CoolAnimation::rainbowText("你购买了元素礼包！里面有大量的元素,买这个礼包可见你是多么深爱着元素周期表，比较常见的都在这里(1500 coins)", 40);
		CoolAnimation::rainbowText("小心那些狡猾的有毒物质，门捷列夫", 40);
		addCoins(-1500);
    }

    //机器礼包，超级昂贵，祝君好运吧
    void BuyMachinePack() {
		CoolAnimation::rainbowText("尊敬的大主顾，您购买了最昂贵的礼包之一!您真的是个土豪啊！", 40);
		addItem("large industrial electrolyzer", 2, "machine", 3000);//大型工业电解机，电老虎
        addItem("large industrial centrifuge", 2, "machine", 1500);//大型工业离心机,分离一切!
        addItem("giant chemical reactor",1,"machine",2000);//巨型化学反应釜，能进行复杂的化学反应,这东西相当庞大!
		addItem("sixteen thread lathe", 1, "machine", 5000);//16线程车床,能加工各种复杂的零件,不用手搓啦
		addItem("five axis milling machine", 1, "machine", 5000);//五轴铣床,就问你高不高级
		addItem("giant industrial 3D printer", 1, "machine", 3000);//巨型工业3D打印机，别问我原理
		addItem("basic electric arc furnace", 2, "machine", 2000);//基础电弧炉,好热!吞电量大到离谱!
        addItem("basic industrial electric blasting furnace", 1, "machine", 8000);//基础工业电力高炉,简单粗暴！虽然会吃光你的电力
        addItem("multifunctional distillation chamber",1,"machine",5000);//多功能蒸馏室，酿杯酒，石油化工的必备
        //一共多少钱?
		// 3000*2 + 1500*2 + 2000 + 5000 + 5000 + 3000 + 2000*2 + 8000 + 5000 = 49000 coins!这……
		addCoins(-50000);
		CoolAnimation::rainbowText("你购买了机器礼包！里面有各种各样的机器，可以用来生产和加工各种物品！(50000 coins)", 40);
        CoolAnimation::rainbowText("整整5w!你如果能笑着买下来，那肯定是个土豪了", 40);
    }

	//工具礼包，包含一些常用的工具
    void BuyToolPack() {
		addItem("high precision scale", 1, "tool", 200);//高精度天平，锱铢必较
		addItem("high precision electric thermometer", 1, "tool", 50);//高精度电子温度计，hot or cold!
		addItem("high precision pressure gauge", 1, "tool", 50);//高精度压力表，压力山大
		addItem("high precision flow meter", 1, "tool", 100);//高精度流量计，嘿，看看你的液体阀门吧!
		addItem("high precision pH meter", 1, "tool", 40);//高精度pH计，这个可以循环使用！别问我原理
		addItem("high precision conductivity meter", 1, "tool", 40);//高精度电导率计，滋滋滋
		addItem("high precision viscometer", 1, "tool", 50);//高精度粘度计，看看你的液体有多稠吧!别粘住手了
		addItem("high precision spectrophotometer", 1, "tool", 500);//高精度分光光度计，能分析物质的成分和浓度,这个东西相当高级了!我也不知道原理
		addItem("high precision gas chromatograph", 1, "tool", 500);//高精度气相色谱仪，能分析气体的成分和浓度!
		addItem("high precision mass spectrometer", 1, "tool", 500);//高精度质谱仪，能分析物质的分子量和结构,你集齐了三个很奇怪的仪器
        addItem("electric high strength heat resistant alloy wrench", 1, "tool", 100);//电动高强度耐热合金扳手,grrrrr……只需要充电!
		addItem("electric titanium alloy screwdriver", 1, "tool", 100);//电动钛合金螺丝刀,超级拧螺丝帮手
		addItem("Tungsten steel hammer", 1, "tool", 100);//钨钢锤子,好沉!
        addItem("Damascus steel knife", 1, "tool", 100);//大马士革钢刀，这可是叙利亚的好货！
        addItem("basic atomic clock",1,"tool",200);//初级原子钟，没什么用，但是你的一些机器需要这个，买吧
		addCoins(-2500);
		CoolAnimation::rainbowText("你购买了工具礼包！里面有各种各样的工具，可以用来分析和处理各种物品！(2500 coins)", 40);
		CoolAnimation::rainbowText("看看这些精妙的工具!", 40);
    }

    //矿产礼包!,还有你最喜欢的石油和天然气!
    void BuyOrePack() {
		addItem("rich raw pyrite", 100, "ore", 100);//富集粗黄铁矿，铁的含量高达70%，是炼铁的好原料!
		addItem("rich raw zinc ore", 100, "ore", 100);//富集粗闪锌矿，锌的含量高达50%，是炼锌的好原料! 对了，你需要镓吗？这是个来源
		addItem("enriched raw magnetite", 100, "ore", 100);//富集粗磁铁矿，铁的含量高达60%,是炼铁的好原料!对了，你需要钛吗？这是个来源，同时你还可以获得磁铁!
		addItem("barren diamond ore", 10, "ore", 800);//贫瘠钻石矿,钻石的含量大约在0.1%左右,这可是宝贝，可以做奢侈品！(即将添加的功能)
		addItem("crude oil", 3000, "ore", 1000);//原油，石油的来源，可以炼制成各种燃料和化学品!这简直是黑金!
		addItem("natural gas", 3000, "ore", 800);//天然气，清洁能源的来源，可以炼制成各种燃料和化学品!这简直是蓝金! 当然啦，你也可以直接把它烧掉，虽然不太环保……
		addItem("rich raw coal", 3000, "ore", 500);//富集粗煤炭，虽然不太环保
		addItem("rich bauxite",1000,"ore",400);//富集铝土矿，铝的含量高达30%，是炼铝的好原料!对了，你需要镓吗？这是个来源.2
		addItem("rich rare earth", 1000, "ore", 800);//富集稀土，虽然不算特别昂贵的矿石，但也是个宝贝，可以提取出各种稀土元素!中国的稀土资源非常丰富，这个矿石可以说是中国的宝藏了!
		addItem("rich uranium ore", 100, "ore", 1000);//富集铀矿，这是核能的来源，可以用来发电或者制造核武器!（当然了，核武器这个功能暂时不会添加的，毕竟我们是和平主义者🕊）
		addItem("rich thorium ore", 100, "ore", 800);//富集钍矿，钍的含量高达10%，是核能的潜在来源，可以用来发电!虽然目前钍能还不太成熟，但未来可能会成为核能的重要发展方向!
		addItem("rich lithium mica", 100, "ore", 500);//富集锂云母，锂的含量高达5%
		addItem("rich potassium feldspar", 100, "ore", 500);//富集钾长石，钾的含量高达10%，是肥料的好原料!不要用金克拉了
		addItem("rich sodium feldspar", 100, "ore", 500);//富集钠长石，钠的含量高达10%，咸咸的(bushi)
		addItem("rich calcium feldspar", 100, "ore", 500);//富集钙长石，钙的含量高达10%
		addItem("rich sulfur ore", 100, "ore", 500);//富集硫矿，硫的含量高达10%，这是臭名昭著的一种矿石，害死不少矿工了！小心处理！
		addItem("rich phosphorus ore", 100, "ore", 500);//富集磷矿，磷的含量高达10%，是肥料的好原料!不要用金克拉了.2
		addItem("rich potassium ore", 100, "ore", 500);//富集钾矿，钾的含量高达10%，是肥料的好原料!不要用金克拉了.3,金坷垃被黑的最惨的一集
        //多少钱啊……我的天，5k
		addCoins(-5000);
		CoolAnimation::rainbowText("你购买了矿产礼包！里面有各种各样的矿石，可以用来提取各种元素和资源！(5000 coins)", 40);
		CoolAnimation::rainbowText("看看这些宝藏吧！小心处理哦!", 40);
    }

	//加入礼包，和addTh()类似，不过是添加一个礼包
    void addPacks(string name, int quantity, string category, long long price) {
        Packs newPack;
		newPack.name = name;
		newPack.description = "这是一个礼包，里面有很多东西！";
		newPack.price = price;
		newPack.contents = {}; // 这里可以根据需要添加礼包内容
		shopPacks.push_back(newPack);


    }

    //存储商店信息
    void saveShopData() {
        json shopData;
        for (const auto& item : shopItems) {
            json itemJson;
            itemJson["name"] = item.name;
            itemJson["description"] = item.description;
            itemJson["price"] = item.price;
            itemJson["itemCategory"] = item.itemCategory;
            shopData["shopItems"].push_back(itemJson);
        }
        ofstream file("shop_data.json");
        if (file.is_open()) {
            file << shopData.dump(4);
            file.close();
        }
	}
    //写两个预留函数，防止到时候需要
    //上架一个包
    void typePack(){}

    //手动上架一个
    void typeTh(string ttpTh,category tpTh, string tpnm, string desc, long long price) {
        typewriter(">输入你想上架的商品吧!");
        typewriter("注意哦!商品的类型只有tool、machine、chemical、ore和pack!", 40);
        getline(cin, ttpTh);
        getline(cin, tpnm);
        getline(cin, desc);
        cin >> price;
        bool isWrt=false;
        while (isWrt == false) {
            if (ttpTh == "tool" || ttpTh == "工具") {
                tpTh = tool;
            }
            else if (ttpTh == "machine" || ttpTh == "机器") {
                tpTh = machine;
            }
            else if (ttpTh == "chemical" || ttpTh == "化学品") {
                tpTh == chemical;
            }
            else if (ttpTh == "pack" || ttpTh == "包") {
                tpTh == pack;
                //预留
            }
            else if (ttpTh == "ore" || ttpTh == "矿物") {
                tpTh == ore;
            }
            else {
                continue;
            }
        }
        
        cout << "\n\n\n";
        addTh(tpTh, tpnm, desc, price);
    }
    //现在你以为啥也没得玩，到时候你就会见证奇迹!!!!!!!!!!!!!!!!!!!!!!!!!!!
};

//======指令系统=====
//已经写代码写到崩溃了
class Command {
private:
    //这里预留
};




// ======================== 主菜单 ========================     LoL

class Start {
private:
    SaveManager saveManager;

    void showMainMenu() {
        cls();
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
        cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                   化 学 世 界 - 主 菜 单                      ║
╚═══════════════════════════════════════════════════════════════╝
)";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        cout << "\n  1. 新游戏\n";
        cout << "  2. 加载存档\n";
        cout << "  3. 查看存档列表\n";
        cout << "  4. 退出\n";
        cout << "\n  请选择: ";
    }

    void gameLoop() {
        int choice;
        while (true) {
            cls();
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
            cout << "\n══════════════ 游戏主界面 ══════════════\n";
            saveManager.showPlayerStats();
            cout << "\n  1. 查看库存\n";
            cout << "  2. 开始游戏!!!\n";
            cout << "  3. 保存游戏\n";
            cout << "  4. 返回主菜单\n";
            cout << "  请选择: ";

            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1: {
                // 查看库存（示例）
                cout << "\n当前库存:\n";
                for (const auto& item : saveManager.getInventory()) {
                    cout << "  - " << item.name << " x" << item.quantity << " (" << item.category << ")\n";
                }
                cout << "\n非常抱歉的通知……由于技术原因，这里只能使用英文来展示，orz";
                system("pause");
                break;
            }
            case 2: {
				//这里可以加一个判定，教程没完事就继续，完事了就正常进入游戏
				//但问题是现在游戏内容还没有，所以先暂时这样吧
                cout << "\n游戏内容正在开发中，敬请期待！\n";
                system("pause");
				break;
            }
            case 3: {
                string saveName;
                cout << "输入存档名称: ";
                getline(cin, saveName);
                if (saveManager.saveGame(saveName)) {
                    cout << "保存成功！\n";
                }
                system("pause");
                break;
            }
            case 4:{
                  return;
            }
            }
        }
    }
    
public:
    void run() {
        int choice;
        while (true) {
            showMainMenu();
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                saveManager.createNewGame();
                gameLoop();
                break;

            case 2: {
                string saveName;
                cout << "输入存档名称: ";
                getline(cin, saveName);
                if (saveManager.loadGame(saveName)) {
                    gameLoop();
                }
                else {
                    cout << "加载失败！\n";
                    system("pause");
                }
                break;
            }

            case 3:
                saveManager.listSaves();
                system("pause");
                break;

            case 4:
                cout << "感谢游玩！\n";
                return;

            default:
                cout << "无效选择！\n";
                system("pause");
            }
        }
    }
};


//主函数，尽可能精简!
int main()
{
	SetConsoleTitle(L"化学·世界");
    Shop shop;
    shop.showShop();
    //addTh()一个测试商品
    shop.addTh(tool, "test", "hello", 7891);
    shop.addTh(tool, "nailong", "xinbabbabalubiya", 9178);
    
	//cppwelcome();
	//cls();
	//showlogo();
    // 为了调试方便这里可以注释掉
    
    //下面的那一部分有调试代码
    /*
    try {

        // 测试存档目录是否可创建
        if (!fs::exists("saves")) {
            if (!fs::create_directory("saves")) {
                cerr << "X 无法创建 saves 目录！" << endl;
                system("pause");
                return 1;
            }
            cout << " wow! 创建 saves 目录成功" << endl;
        }
        Start start;
        start.run();

    }
    catch (const exception& e) {
        cerr << "X 致命错误: " << e.what() << endl;
        system("pause");
        return 1;
    }
    catch (...) {
        cerr << "X 未知错误！" << endl;
        system("pause");
        return 1;
    }*/
	//撒花，游戏成功运行了！虽然现在内容还很少，但这是一个好的开始！后续版本会逐渐添加更多的内容和功能，敬请期待！谢谢大家的支持！
    //(ﾉ*･ω･)ﾉ (花x114514)
    //(👉ﾟヮﾟ)👉👈(ﾟヮﾟ👈)
    //放个apple别偷吃
	//  ,--./,-.
	// / #      \
    //|          |
	// \        /
	//  `._,._,'
	//(( •̀ ω •́ )✧)
    return 0;
}