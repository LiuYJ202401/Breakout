#include "BasicSettings.h"


const int ballR = 10;//球半径
const int baffleWidth = 40;//挡板厚度

const int WindowWidth = 1080;
const int WindowHeight = 640;


const int WallWidth = WindowWidth / 27;//墙壁宽度(WindowWidth * 40 / 1080)
const int WallHeight = WindowHeight / 16;//墙壁高度(WindowHeight * 40 / 640)
const int MapWidth = WindowWidth * 70 / 108;
const int MapHeight = WindowHeight * 20 / 64;

const std::wstring set_postfix = L".config";
const std::wstring set_prefix = L".\\config\\";

const std::wstring lastgame_prefix = L".\\endgames\\";
const std::wstring lastgame_postfix = L".end";


CollisionInfo* collide(int ball_x, int ball_y, int r, int rect_x, int rect_y, int rect_width, int rect_height) {//判断球与矩形的碰撞
	CollisionInfo* info = new CollisionInfo;
	if (ball_x <= rect_x - r || ball_x >= rect_x+rect_width + r || ball_y <= rect_y - r || ball_y >= rect_y+rect_height + r) return info;//初步检测
	int clostestX = std::clamp(ball_x, rect_x, rect_x + rect_width);//找到球心到矩形最近的点
	int clostestY = std::clamp(ball_y, rect_y, rect_y + rect_height);
	int dist_squared = (ball_x - clostestX) * (ball_x - clostestX) + (ball_y - clostestY) * (ball_y - clostestY);//计算距离的平方
	info->collided = dist_squared <= r * r;//判断是否碰撞
	if (info->collided) {
		info->collisionX = clostestX;//记录碰撞点x坐标
		info->collisionY = clostestY;//记录碰撞点y坐标
		if (clostestX == ball_x && clostestY == ball_y) info->in = true;//球心在矩形内
	}
	return info;
}

std::vector<std::vector<int>>* getRandType(int level,int seed,int x,int y) {
	std::vector<std::vector<int>>* type = new std::vector<std::vector<int>>;
	int sd;
	if (seed == -1) {//获取随机种子
		sd = static_cast<int>(std::time(nullptr));
	}
	else {//使用指定种子
		sd = seed+level-1;
	}
	std::mt19937 gen;
	gen.seed(sd);//使用梅森旋转算法生成随机数
	std::uniform_int_distribution<> dis(1, 1000);//生成1到1000的均匀分布随机整数
	int bar0 = 600 - level * 20; // 普通砖块概率
	int bar1 = 900 - level * 10; // 耐久砖块概率
	for (int i = 0; i < x; i++) {
		(*type).push_back(std::vector<int>());
		for (int j = 0; j < y; j++) {
			int randNum = dis(gen);//获取随机数
			if (randNum <= bar0) {
				(*type)[i].push_back(0); // 普通砖
			}
			else if (randNum <= bar1) {
				(*type)[i].push_back(1); // 耐久砖
			}
			else {
				if (level > 2) (*type)[i].push_back(2); // 坚不可摧砖
				else (*type)[i].push_back(0);
			}
		}
	}
	return type;
}

void adjustHeight(int aimWidth, LPCTSTR text) {//用二分查找寻找合适的字号高度，使得text显示出来的宽度尽可能接近aimWidth
	int min = 5, max = 500;//二分查找的上下界
	while (min < max - 1) {
		int mid = (min + max) / 2;
		settextstyle(mid, 0, _T("Consolas"));
		if (textwidth(text) < aimWidth) {
			min = mid;
		}
		else if (textwidth(text) > aimWidth) {
			max = mid;
		}
		else break;
	}
	settextstyle(min, 0, _T("Consolas"));
	return;
}