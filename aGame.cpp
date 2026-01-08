#include "aGame.h"


aGame::aGame(std::wstring lastgame):baffle(),ball(),map(),ifEnd(false),ifWin(false){//从残局创建
	int dotCount = std::count(lastgame.begin(), lastgame.end(), L'.');
	while (dotCount > 1) {
		lastgame.resize(lastgame.size()-lastgame_postfix.size());
	}
	if (dotCount == 0) lastgame += lastgame_postfix;
	std::wstring fullname = lastgame_prefix + lastgame;
	std::cout << "尝试读取残局文件"; std::wcout << (fullname); std::cout << "...\n";
	std::ifstream in(fullname, std::ios::in | std::ios::binary);
	if (in.is_open()) {
		if (!this->deserialize(in)) std::wcout << L"Failed to open " << lastgame << std::endl;

	}
	else { std::cout << "无法读取残局文件";std::wcout << (fullname); }
	in.close();
	stop.setString(L"暂停");
	restart.setString(L"重玩本关");
};

aGame::aGame(gameSettings set, int x, int y, int gameLevel) :ifWin(false), ifEnd(false), DefaultXBlockNum(x), DefaultYBlockNum(y) {
	this->settings = set;
	this->baffle = new Baffle();
	this->ball = new Ball();
	this->map = new Map(set.seed, DefaultXBlockNum, DefaultYBlockNum, set.gameLevel);
	this->scores = 0;
	this->blood = 3;
	this->level = gameLevel;
	ball->setBall(level, set.basicV);
	ball->linkBaffle(baffle);
	baffle->adjust(level);
	stop.setString(L"暂停");
	restart.setString(L"重玩本关");
}


aGame::aGame(gameSettings set) :ifWin(false), ifEnd(false) {
	this->settings = set;
	this->baffle = new Baffle();
	this->ball = new Ball();
	this->map = new Map(set.seed, DefaultXBlockNum, DefaultYBlockNum, set.gameLevel);
	this->scores = 0;
	this->blood = 3;
	this->level = set.gameLevel;
	ball->setBall(level, set.basicV);
	ball->linkBaffle(baffle);
	baffle->adjust(level);
	stop.setString(L"暂停");
	restart.setString(L"重玩本关");
}

aGame::aGame(aGame* ori) {
	this->settings = ori->settings;
	this->ball = new Ball();
	this->baffle = new Baffle();
	this->map = new Map(ori->map);
	this->stop = ori->stop;
	this->scores = ori->scores;
	this->blood = ori->blood;
	this->level = ori->level;
	this->DefaultXBlockNum = ori->DefaultXBlockNum;
	this->DefaultYBlockNum = ori->DefaultYBlockNum;
	ball->setBall(level, settings.basicV);
	ball->linkBaffle(baffle);
	baffle->adjust(level);
	stop.setString(L"暂停");
	restart.setString(L"重玩本关");
}

void aGame::gameRun() {
	ball->ballMove();//处理球的移动
	baffle->baffleMove();//处理挡板的移动
	ball->ballRun(map,this);//处理球的逻辑（与挡板、墙壁、砖块的交互）
	if (ball->fall()) {//若球掉落
		blood--;//扣血
		if (blood <= 0) {
			ifEnd = true;
		}
		else {
			ball->setBall(level, this->settings.basicV);//重置球状态
			ball->linkBaffle(baffle);//重新将球放回挡板上
		}
	}
	if (map->is_empty()) {//若砖块被清空，赢。
		ifWin = true;
	}
	if (GetAsyncKeyState('C') & 0x8000) {//调试用，清图
		map->clear();
	}
	if (GetAsyncKeyState('B') & 0x8000) {//调试用，加血
		blood++;
	}
	if (peekmessage(m, EX_MOUSE)) {//点击暂停
		if (m->message == WM_LBUTTONDOWN) {
			int x = m->x; int y = m->y;
			if (stop.ifIn(x, y)) { ifStop = true; }
			if (restart.ifIn(x, y)) { ifRestart = true; }
		}
	}
}

void aGame::gameDraw(std::wstring setname) {
	baffle->baffleDraw();
	ball->ballDraw();
	map->mapDraw();
	this->displayInfo(setname);
	stop.draw();
	restart.draw();
}

void aGame::displayInfo(std::wstring setname) {//显示血量、分数,etc.
	settextcolor(WHITE);
	int displayY = WindowHeight / 16;
	std::basic_ostringstream<TCHAR> oss;//建立字符串流
	oss << _T("score: ") << scores;
	adjustHeight(WindowWidth * 4 / 18, (oss.str()).c_str());//调整字号，使得字符串不至于超出屏幕
	outtextxy(displayX, displayY, (oss.str()).c_str());//输出分数
	oss.str(_T(""));//清空字符串流
	oss << _T("blood: ") << blood;
	adjustHeight(WindowWidth * 4 / 18, (oss.str()).c_str());
	displayY += textheight((oss.str()).c_str());
	outtextxy(displayX, displayY, (oss.str()).c_str());
	oss.str(_T(""));
	oss << _T("level: ") << level;
	adjustHeight(WindowWidth * 4 / 18, (oss.str()).c_str());
	displayY += textheight((oss.str()).c_str());
	outtextxy(displayX, displayY, (oss.str()).c_str());
	oss.str(_T(""));
	oss << _T("config:") ;
	if(textwidth((oss.str()).c_str()) > WindowWidth * 4 / 18 ) adjustHeight(WindowWidth * 4 / 18, (oss.str()).c_str());
	displayY += textheight((oss.str()).c_str());
	outtextxy(displayX, displayY, (oss.str()).c_str());
	oss.str(_T(""));
	oss << setname;
	if (textwidth((oss.str()).c_str()) > WindowWidth * 4 / 18) adjustHeight(WindowWidth * 4 / 18, (oss.str()).c_str());
	displayY += textheight((oss.str()).c_str());
	outtextxy(displayX, displayY, (oss.str()).c_str());
	return;
}

void aGame::nextLevel() {//进入下一关
	level++;
	blood = 3;//重置游戏状态
	ifWin = false;
	delete baffle;
	baffle = new Baffle();//重置挡板
	ball->setBall(level, this->settings.basicV);//重置球状态
	ball->linkBaffle(baffle);//重新将球放回挡板上
	baffle->adjust(level);//调整挡板长度和速度
	int tempXBlockNum = map->getXBlockNum();
	int tempYBlockNum = map->getYBlockNum();
	delete map;
	map = new Map(settings.seed,tempXBlockNum,tempYBlockNum,level);//创建新地图
}