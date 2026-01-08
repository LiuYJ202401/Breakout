#pragma once//游戏管理器
#include "BasicSettings.h"
#include"aGame.h"
#include "Button.h"
class GameManager
{private:
	IMAGE& img;
	ExMessage* m=new ExMessage;

	int state = 0;//状态参数，0为主菜单，1为自定义配置界面，2为残局设定加载界面，3为游戏内界面，4为暂停保存残局界面，5为胜利界面，6为失败界面
	bool ifrunning = true;//游戏运行中
	bool check = true;//是否发生场景转换
	bool ifLastgame = false;//是否加载残局
	std::vector<Button*> buttons;

	std::vector<std::wstring> allSettingsName;
	std::wstring settings = L"default";//配置文件名
	gameSettings set;
	std::wstring lastgamename=L"";//残局文件名



	bool newgame = true;//是创建一局游戏还是继续游戏
	bool nextlevel = false;//是否进入下一关
	aGame* startAGame() ;//依据配置创建新一局游戏，返回创建的游戏类的指针
	aGame* startAGame(std::wstring last);//依据残局创建新一局游戏，返回创建的游戏类的指针
	aGame* thisgame;//指向当前游戏的指针
	aGame* snapshot;//游戏状态快照（以便实现重玩本关功能）

	void Menudraw();//主菜单绘制
	void MenuCheck();//监测在主页面的操作并做出反映

	void Settings() ;//调用配置的类的函数
	void SetDraw() ;//进入自定义配置界面绘制
	void create();//创建新配置

	void Lastgame();//调用残局的类的函数
	void LastgameDraw() ;//进入残局设定加载界面绘制
	bool createLastgame(bool ready = false);//创建新残局//返回是否创建成功


	void Stop() ;//进入暂停界面
	void StopDraw() ;//进入暂停界面绘制
	void Start() ;//进入游戏状态与游戏进行
	//游戏界面的绘制直接由aGame的绘制函数完成

	void Win();//胜利界面
	void WinDraw();//胜利界面绘制
	void Lose();//失败界面
	void LoseDraw();//失败界面绘制

public:
	GameManager();//默认构造函数
	GameManager(IMAGE& img);
	void draw();//整体的绘制函数//根据不同状态调用不同的绘制函数
	void run();//游戏运行，检查输入信号，切换界面
	bool ifRunning() { return ifrunning; };//游戏是否运行中//退出游戏结束程序
};

