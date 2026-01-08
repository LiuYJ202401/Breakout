#pragma once//单局游戏
#include "Ball.h"
#include "Baffle.h"
#include "Brick.h"
#include "Map.h"
#include "Button.h"

class aGame
{
public:
	ExMessage* m = new ExMessage;
	aGame(gameSettings set, int x, int y,int gameLevel);
	aGame(gameSettings set);//从配置创建游戏
	aGame(std::wstring lastgamename);//从残局创建游戏
	aGame(aGame* ori);//复制构造函数
	~aGame() {
		delete ball;
		delete baffle;
		delete map;
		delete msg;
	}
	void gameDraw(std::wstring setname) ;//绘制一局游戏内的物品
	void gameRun() ;//一局游戏运行
	void SetMapBrick(brickType t, int x, int y) { map->setBrick(t, x, y); }//修改地图砖块
	bool ifend() { return ifEnd; }
	bool ifwin() { return ifWin; }
	void displayInfo(std::wstring setname);//绘制游戏信息（血量、关卡等）
	void earnScore() { scores++; }//得分
	int  getScore() { return scores; }//获取分数
	void nextLevel();//进入下一关
	gameSettings settings;//三样默认或自定义设置

	Ball* ball;//小球
	Baffle* baffle;//挡板
	Map* map;//砖块地图，可以用于加载残局
	Button stop=Button(WindowWidth/10*9,WindowHeight/10*9,WindowWidth/20,WindowHeight/20);
	Button restart=Button(WindowWidth/10*9,WindowHeight/20*15,WindowWidth/20,WindowHeight/20);

	bool ifstop() { return ifStop; }
	bool ifrestart() { return ifRestart; }
	void nostop() { ifStop = false; }
	int getblood() { return blood; }

	void serialize(std::ofstream& o) {
		o.write(reinterpret_cast<const char*>(&settings), sizeof(settings));
		baffle->serialize(o);
		ball->serialize(o);
		
		o.write(reinterpret_cast<const char*>(&scores), sizeof(scores));
		o.write(reinterpret_cast<const char*>(&blood), sizeof(blood));
		o.write(reinterpret_cast<const char*>(&level), sizeof(level));
		map->serialize(o);
	}
	bool deserialize(std::ifstream& in) {
		if (!in.read(reinterpret_cast<char*>(&settings), sizeof(settings))) {
			return false;
		}
		if (!baffle) baffle = new Baffle();
		if (!ball) ball = new Ball();
		if (!map) map = new Map();

		if (!baffle->deserialize(in)) return false;
		if (!ball->deserialize(in, baffle)) return false;

		if (!in.read(reinterpret_cast<char*>(&scores), sizeof(scores))) return false;
		if (!in.read(reinterpret_cast<char*>(&blood), sizeof(blood))) return false;
		if (!in.read(reinterpret_cast<char*>(&level), sizeof(level))) return false;
		if (!map->deserialize(in)) return false;

		return true;
	}


private:
	ExMessage* msg = new ExMessage;
	int scores;//分数
	int blood;//血量
	bool ifEnd = false;
	bool ifWin = false;//是否在通关状态
	bool ifStop = false;
	bool ifRestart = false;
	int level;//关卡
	int DefaultXBlockNum = 18;//横向格子数
	int DefaultYBlockNum = 9;//纵向格子数
	int displayX = WallWidth * 2 + MapWidth + 20;//信息显示区左上角x坐标
};

