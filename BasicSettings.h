#pragma once//通用基础信息
#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <math.h>
#include <algorithm>
#include <sstream>
#include <tchar.h>
#include <random>
#include <ctime>
#include <cstdlib>

#define pi 3.1415926

extern const int ballR ;//球半径
extern const int baffleWidth;//挡板厚度

extern const int WindowWidth ;
extern const int WindowHeight ;

extern const int WallWidth;//墙壁宽度
extern const int WallHeight;//墙壁高度

extern const int MapWidth;//地图宽度
extern const int MapHeight;//地图高度

extern const std::wstring set_postfix;//配置文件后缀
extern const std::wstring set_prefix;//配置文件前缀

extern const std::wstring lastgame_prefix;//残局文件前缀
extern const std::wstring lastgame_postfix;//残局文件后缀

extern const float g;//重力加速度

class gameSettings {
public:
	int basicV = 1;//基础球速
	int seed = -1;//种子
	int gameLevel = 1;//初始关卡
};
enum brickType {  Normal, Durable , Indestructible, No };//#,@,* 三种砖与没有砖的占位

struct CollisionInfo {//碰撞信息
	bool collided = false;//是否碰撞
	bool in = false;//球心是否在矩形内
	float collisionX = -1;//碰撞点x坐标
	float collisionY = -1;//碰撞点y坐标
};

CollisionInfo* collide(int ball_x, int ball_y, int r, int rect_x, int rect_y, int rect_width, int rect_height);//判断球与矩形的碰撞

std::vector<std::vector<int>>*  getRandType(int level,int seed,int x,int y);//根据关卡等级返回随机砖块类型

void adjustHeight(int aimWidth, LPCTSTR text);