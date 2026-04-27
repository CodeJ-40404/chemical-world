#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>//一个一个导入!
#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>
#include <vector> //依旧 
#include <string>
#include <thread> //多线程 
#include <chrono>
#include "json.hpp"
using json = nlohmann::json;
namespace fs = std::filesystem;

//本游戏内容会非常庞大，所以存档系统必须写……
//但是存档系统需要很多麻烦的东西，比如sqite和nlohmann-json,不过可以学嘛，总之需要把代码写的灵活一点，防止到时候根本不知道怎么改
//对了，不知道为什么，我的代码报错了，什么abort()
//实在不行考虑考虑……sqlite?

using namespace std;

#ifdef _WIN32
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#endif


#define BLACK 0
#define DARK_BLUE 1
#define DARK_GREEN 2
#define DARK_CYAN 3 //这个其实是青色的深色版本，别被名字骗了 
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

void showlogo() {
	setcolor(CYAN);
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
║                         v1.0  |  BUILDING THE FUTURE                       ║
║                                                                            ║
╚════════════════════════════════════════════════════════════════════════════╝)";

	delay(3000);

	cls();
	setcolor(YELLOW);
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
║    ██║     ███████║      .+++           +++..      ██║ █╗ ██║██████╔╝      ║
║    ██║     ██╔══██║        .+           +.         ██║███╗██║██╔══██╗      ║
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

// ======================== 存档数据结构 ========================

struct PlayerData {
    string name;
    int level;
    long long coins;
    int goldBars;           // 稀有货币
    int exp;
    time_t createdAt;
    time_t lastPlayed;
};

struct InventoryItem {
    string name;
    int quantity;
    string category;   // ore, ingot, chemical, fuel, part, etc.
    double value;      // 基础价值
};

struct Machine {
    string id;
    string type;       // 粉碎机、电解机、车床等
    int level;
    bool isRunning;
    int x, y;          // 工厂布局坐标
    map<string, int> inventory;  // 机器内部库存
    int powerConsumption;
};

struct Factory {
    string name;
    vector<Machine> machines;
    int totalPower;
    int usedPower;
    map<string, int> centralStorage;  // 中央仓库
};

struct ChemicalKnowledge {
    string formula;
    bool discovered;
    int discoveryTime;
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
    int miningRigs;           // 采矿设备数量
    long long extractedAmount;
};

// ======================== 存档管理器 ========================

class SaveManager {
private:
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

        cout << "\n* 请输入你的名字: ";
        cout << "\n*如果不输入，默认是程序猿";
        getline(cin, player.name);

        if (player.name.empty()) {
            player.name = "程序猿";
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
            {"水", 10, "chemical", 5},
            {"贫瘠黄铁矿石", 5, "ore", 10},
            {"粗煤炭", 3, "fuel", 15}
        };

        // 初始化工厂
        factory.name = player.name + "的工厂";
        factory.machines = {};
        factory.totalPower = 0;
        factory.usedPower = 0;

        // 初始化化学知识（已发现的配方）
        chemistry = {
            {"H2O", true, (int)time(nullptr)},
            {"Fe2O3", false, 0}
        };

        // 初始化任务
        quests = {
            {1, "第一桶金", 0, false},
            {2, "粉碎矿石", 0, false},
            {3, "冶炼金属", 0, false}
        };

        // 初始化星球
        planets = {
            {"地球", true, {"铁矿石", "煤炭", "石油"}, 0, 0},
            {"月球", false, {"氦-3", "钛矿","极重油"}, 0, 0},
            {"火星", false, {"镓矿", "稀土","氦-4"}, 0, 0}
        };

        cout << "\n★   欢迎，" << player.name << "！你的化学世界之旅开始了！\n";
        cout << "□正在创建存档...\n";

        saveGame("auto");

        cout << "√存档创建成功！\n";
        Sleep(1500);
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
        cout << "√游戏已保存到 " << filename << endl;
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
            cout << "√ 存档加载成功！欢迎回来，" << player.name << "！\n";
            cout << "⚪ 上次游戏时间: " << getTimeString(player.lastPlayed) << endl;
            cout << "(ﾉ*･ω･)ﾉ\n";
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
        cout << "金条是一种高级货币！和你在游戏里能制作的金条有很大差异！";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    }
};

// ======================== 主菜单 ========================

class Game {
private:
    SaveManager saveManager;

    void showMainMenu() {
        system("cls");
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
            system("cls");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
            cout << "\n══════════════ 游戏主界面 ══════════════\n";
            saveManager.showPlayerStats();
            cout << "\n  1. 查看库存\n";
            cout << "  2. 管理工厂\n";
            cout << "  3. 实验室\n";
            cout << "  4. 星际探索\n";
            cout << "  5. 保存游戏\n";
            cout << "  6. 返回主菜单\n";
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
                system("pause");
                break;
            }
            case 5: {
                string saveName;
                cout << "输入存档名称: ";
                getline(cin, saveName);
                if (saveManager.saveGame(saveName)) {
                    cout << "保存成功！\n";
                }
                system("pause");
                break;
            }
            case 6:{
                  return;
            }
            default: {
                cout << "功能开发中...\n";
                system("pause");
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

int main()
{
	SetConsoleTitle(L"化学·世界");
	//cppwelcome();
	//cls();
	//showlogo();
    Game game;
    game.run();
}