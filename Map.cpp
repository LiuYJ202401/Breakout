#include "Map.h"
#include "Brick.h"
#include "Ball.h"
#include <vector>
Map::Map(int seed,int xBlockNum , int yBlockNum ,int level) :ifempty(false) {//地图（默认为9*18），砖的类型随gamelevel变化。
	this->xBlockNum = xBlockNum;
	this->yBlockNum = yBlockNum;
	this->BlockWidth = MapWidth / (xBlockNum);
	this->BlockHeight = MapHeight / yBlockNum;
	this->BrickHeight = BlockHeight * 9 / 10;
	this->BrickWidth = BlockWidth * 9 / 10;
	this->BrickWidth = BlockWidth * 9 / 10;
	int cur_x = WallWidth + BlockWidth / 20, cur_y = WallHeight + BlockHeight / 10;//砖块初始位置
	vector<vector<int>>* type = getRandType(level, seed, xBlockNum, yBlockNum);
	for (int x = 0; x < xBlockNum; x++) {
		this->bricks.push_back(std::vector<Brick>());
		for (int y = 0; y < yBlockNum; y++) {
			this->bricks[x].push_back(Brick(cur_x, cur_y, BrickHeight, BrickWidth, (*type)[x][y]));//生成砖块
			cur_y += BlockHeight;
		}
		cur_x += BlockWidth;
		cur_y = WallHeight + BlockHeight / 10;
	}
	delete type;
}

Map::Map(Map* ori) {//复制构造函数
	this->xBlockNum = ori->xBlockNum;
	this->yBlockNum = ori->yBlockNum;
	this->BlockWidth = ori->BlockWidth;
	this->BlockHeight = ori->BlockHeight;
	this->BrickHeight = ori->BrickHeight;
	this->BrickWidth = ori->BrickWidth;
	this->ifempty = ori->ifempty;
	for (int x = 0; x < xBlockNum; x++) {
		this->bricks.push_back(std::vector<Brick>());
		for (int y = 0; y < yBlockNum; y++) {
			this->bricks[x].push_back(ori->bricks[x][y]);//复制砖块
		}
	}
}

bool Map::is_empty() {//检测是否所有可以被打掉的砖都被打掉了
	ifempty = checkEmpty();
	return ifempty;
}

bool Map::checkEmpty() {
	for (int x = 0; x < xBlockNum; x++) {
		for (int y = 0; y < yBlockNum; y++) {
			if (bricks[x][y].Type() <= 1) {//若为普通砖或耐久砖
				return false;
			}
		}
	}
	return true;
}


void Map::mapDraw() {
	setfillcolor(WHITE);
	fillrectangle(0, 0, WallWidth, WindowHeight);//左墙
	fillrectangle(WallWidth + MapWidth, 0, WallWidth * 2 + MapWidth, WindowHeight);//右墙
	fillrectangle(0, 0, MapWidth + WallWidth, WallHeight);//天花板
	fillrectangle(0, WindowHeight - WallHeight, MapWidth + WallWidth, WindowHeight);//地板
	for (int x = 0; x < xBlockNum; x++) {
		for (int y = 0; y < yBlockNum; y++) {
			bricks[x][y].brickDraw();
		}
	}
	return;
}

void Map::check(Ball* ball,aGame* game) {//判断球与砖的碰撞并处理
	for (int i = 0; i < xBlockNum; i++){
		for (int j = 0; j < yBlockNum; j++  ) {
			bricks[i][j].check(ball,game);
		}
	}
}

void Map::clear() {//清图，调试用。
	for (int x = 0; x < xBlockNum; x++) {
		for (int y = 0; y < yBlockNum; y++) {
			if(bricks[x][y].Type()<=1) bricks[x][y].destroy();
		}
	}
}
