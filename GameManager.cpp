#include"GameManager.h"
GameManager::GameManager(IMAGE&img):img(img) {
	std::ifstream o(set_prefix + settings + set_postfix, std::ios::in | std::ios::binary);
	if (o.is_open()) {
		o.read((char*)&set, sizeof(set));
		o.close();
	}
	else {
		std::ofstream o(set_prefix + settings + set_postfix, std::ios::out | std::ios::binary);
		o.write((char*)&set, sizeof(set)); 
		o.close();
	}
}

void GameManager::draw() {
	cleardevice();
	switch (state) {
	case 0:Menudraw(); break;
	case 1:SetDraw(); break;
	case 2:LastgameDraw(); break;
	case 3:thisgame->gameDraw(settings); break;
	case 4:StopDraw(); break;
	case 5:WinDraw(); break;
	case 6:LoseDraw(); break;
	}
};
void GameManager::run() {
	switch (state) {
	case 0:MenuCheck(); break;
	case 1:Settings(); break;
	case 2:Lastgame(); break;
	case 3:Start(); break;
	case 4:Stop(); break;
	case 5:Win(); break;
	case 6:Lose(); break;
	}
	
};

void GameManager::MenuCheck() {
	if (check) {//是否初始化
		while (!buttons.empty()) {
			delete buttons.back();
			buttons.pop_back();
		}
		check = false;
		//初始化按钮
		Button* begin = new Button(WindowWidth / 2, WindowHeight / 4, WindowWidth / 3, WindowHeight / 12);
		begin->setString(_T("开始游戏"));
		begin->setid(3);
		buttons.push_back(begin);

		Button* setting = new Button(WindowWidth / 5, WindowHeight / 2, WindowWidth / 5, WindowHeight / 12);
		setting->setString(_T("当前配置：") + settings);
		setting->setid(1);
		buttons.push_back(setting);

		Button* lastgame = new Button(WindowWidth / 5*4, WindowHeight / 2, WindowWidth / 5, WindowHeight / 12);
		if(ifLastgame) { lastgame->setString(lastgamename); }
		else { lastgame->setString(_T("无残局")); }
		
		lastgame->setid(2);
		buttons.push_back(lastgame);

		Button* exit = new Button(WindowWidth / 2, WindowHeight / 4*3, WindowWidth / 3, WindowHeight / 12);
		exit->setString(_T("退出游戏"));
		exit->setid(-1);
		buttons.push_back(exit);
	}
	if (peekmessage(m,EX_MOUSE)) {
		if (m->message == WM_LBUTTONDOWN) {
			int x = m->x; int y = m->y;
			for (Button* i : buttons) {//检查按钮触发
				if (i->ifIn(x, y)) {
					switch (i->uid()) {
					case 3: { check = true; state = 3; Start(); break; }
					case 1: { check = true; state = 1; Settings(); break; }
					case -1: { ifrunning = false;return; break; }
					case 2: { check = true; state = 2; Lastgame(); break; }
					}
					break;
				}
			}
		}
	}
	
}
void GameManager::Menudraw() {
	for (auto i : buttons) {
		i->draw();
	}
}
void GameManager::SetDraw() {
	for (auto i : buttons) {
		i->draw();
	}
}
void GameManager::LastgameDraw() {
	for (auto i : buttons) {
		i->draw();
	}
}
void GameManager::Settings() {//配置界面
	if (check) {//是否初始化
		while (!buttons.empty()) {
			delete buttons.back();
			buttons.pop_back();
		}
		check = false;
		//初始化按钮
		Button* menu = new Button(WindowWidth / 10, WindowHeight / 10, WindowWidth / 30, WindowHeight / 30);
		menu->setString(_T("返回"));
		menu->setid(0);
		buttons.push_back(menu);

		Button* create = new Button(WindowWidth / 10 * 9, WindowHeight / 10 * 9, WindowWidth / 25, WindowHeight / 30);
		create->setString(_T("创建新配置"));
		create->setid(10);
		buttons.push_back(create);

		allSettingsName.clear();
		int i = 0;
		for (const auto& entry : std::filesystem::directory_iterator(set_prefix)) {//可选择的配置文件的按钮
			if (entry.is_regular_file()) {
				Button* set = new Button(WindowWidth / 2, WindowHeight / 10 *(2+ i), WindowWidth / 5, WindowHeight / 25);
				set->setString(entry.path().filename());
				set->setid(-1-i);
				buttons.push_back(set);

				allSettingsName.push_back(entry.path().filename());
				//pass//低矮处消隐//似乎也没必要
				i++;
			}
		}
		
	}
	if (peekmessage(m, EX_MOUSE)) {
		if (m->message == WM_LBUTTONDOWN) {
			int x = m->x; int y = m->y;
			for (Button* i : buttons) {//检查按钮触发
				if (i->ifIn(x, y)) {//在按钮范围内
					if (i->uid() < 0) {
						std::wstring nextconfig = i->Word();
						settings = nextconfig.substr(0,nextconfig.length()-7);//修改当前配置文件

						std::ifstream ss(set_prefix + nextconfig, std::ios::in | std::ios::binary);
						ss.read((char*)&set, sizeof(set));
						ss.close();

						check = true; state = 0;//返回主菜单
					}//检查配置文件按钮
					else {
						switch (i->uid()) {
						case 0: { check = true; state = 0; break; }
						case 10: { create(); break; }
						}
					}
				break;
				}
				
			}
		}
		else if (WM_MOUSEWHEEL) {
			for (auto& i : buttons) {
				if (i->uid() < 0) { i->moveY(m->wheel/10); }
			}
		}//滚动滑轮
	}
}


void GameManager::create(){
	wchar_t s[20]; // 稍微增大缓冲区

	// 配置名称
	int result = InputBox(s, 20,
		L"创建新配置\n"
		L"要求：\n"
		L"只能包含字母、数字或下划线\n");

	if (result == 0) {
		return; // 用户点击取消
	}
	if (wcslen(s) == 0) {
		MessageBox(GetHWnd(), L"配置名称不能为空", L"输入错误", MB_OK | MB_ICONERROR);
		return;
	}
	for (size_t i = 0; i < wcslen(s); i++) {
		wchar_t c = s[i];
		if (!((c >= L'a' && c <= L'z') ||
			(c >= L'A' && c <= L'Z') ||
			(c >= L'0' && c <= L'9') ||
			c == L'_')) {
			MessageBox(GetHWnd(),
				L"配置名称只能包含字母、数字或下划线\n"
				L"请不要使用空格、中文或其他特殊字符",
				L"输入错误", MB_OK | MB_ICONERROR);
			return;
		}
	}
	std::wstring configName = s;
	std::wstring fullPath = set_prefix + configName + set_postfix;

	if (std::filesystem::exists(fullPath)) {
		int choice = MessageBox(GetHWnd(),
			L"该配置名称已存在，是否覆盖？",
			L"确认覆盖", MB_YESNO | MB_ICONWARNING);

		if (choice != IDYES) {
			return;
		}
	}


	wchar_t n[256],sd[256],level[256];
	int basicV = InputBox(n, 256, L"请输入一个1到10的正整数，表示球运动的基础速度");
	if (basicV == 0) return; // 用户取消
	if (wcslen(n) == 0) {
		MessageBox(GetHWnd(), L"基础速度不能为空", L"输入错误", MB_OK | MB_ICONERROR);
		return;
	}
	basicV = 0;
	try {
		basicV = std::stoi(n);
	}
	catch (const std::exception&) {
		MessageBox(GetHWnd(), L"基础速度必须是1-10之间的整数", L"输入错误", MB_OK | MB_ICONERROR);
		return;
	}
	if (basicV < 1 || basicV > 10) {
		MessageBox(GetHWnd(), L"基础速度必须在1-10之间", L"输入错误", MB_OK | MB_ICONERROR);
		return;
	}
	int seed = InputBox(sd, 256, L"请输入一个整数，作为随机数种子");
	if (seed == 0) return; // 用户取消

	if (wcslen(sd) == 0) {
		MessageBox(GetHWnd(), L"随机数种子不能为空", L"输入错误", MB_OK | MB_ICONERROR);
		return;
	}
	seed = 0;
	try {
		seed = std::stoi(sd);
	}
	catch (const std::exception&) {
		MessageBox(GetHWnd(), L"随机数种子必须是有效的整数", L"输入错误", MB_OK | MB_ICONERROR);
		return;
	}

	int gamelevel = InputBox(level, 256, L"请输入一个正整数，作为初始关卡");
	if (gamelevel == 0) return; // 用户取消
	if (wcslen(level) == 0) {
		MessageBox(GetHWnd(), L"初始关卡不能为空", L"输入错误", MB_OK | MB_ICONERROR);
		return;
	}
	gamelevel = 0;
	try {
		gamelevel = std::stoi(level);
	}
	catch (const std::exception&) {
		MessageBox(GetHWnd(), L"初始关卡必须是正整数", L"输入错误", MB_OK | MB_ICONERROR);
		return;
	}
	if (gamelevel < 1) {
		MessageBox(GetHWnd(), L"初始关卡必须是正整数（>=1）", L"输入错误", MB_OK | MB_ICONERROR);
		return;
	}


	std::wstring confirmMsg = L"请确认以下配置信息：\n\n";
	confirmMsg += L"配置名称: " + configName + L"\n";
	confirmMsg += L"基础速度: " + std::to_wstring(basicV) + L"\n";
	confirmMsg += L"随机数种子: " + std::to_wstring(seed) + L"\n";
	confirmMsg += L"初始关卡: " + std::to_wstring(gamelevel) + L"\n\n";
	confirmMsg += L"确认创建此配置吗？";

	int confirm = MessageBox(GetHWnd(), confirmMsg.c_str(), L"确认配置", MB_YESNO | MB_ICONQUESTION);

	if (confirm != IDYES) {
		return;
	}

	gameSettings a;
	a.basicV = basicV;
	a.seed = seed;
	a.gameLevel = gamelevel;

	std::ofstream o(fullPath, std::ios::out | std::ios::binary);
	o.write((char*)&a, sizeof(a));
	o.close();

	settings = configName;
	set = a;
	check = true;
}

void GameManager::Start() {
	if (check) {
		check = false;
		if (nextlevel) {
			nextlevel = false;
			thisgame->nextLevel();
			delete snapshot;
			snapshot = new aGame(thisgame);
		}
		if (newgame) {
			newgame = false;
			if (ifLastgame) { thisgame = startAGame(lastgamename); }
			else { thisgame = startAGame(); }
			delete snapshot;
			snapshot = new aGame(thisgame);
		}
	}
	//进行游戏
	if (thisgame->ifend()) {//游戏结束，移动到失败界面
		state = 6;
		check = true;
		newgame = true;
	}
	else if (thisgame->ifwin()) {//若胜利
		state = 5;
		check = true;
		nextlevel = true;
	}
	else if (thisgame->ifrestart()) {//若重新开始本关
		delete thisgame;
		thisgame = new aGame(snapshot);
	}
	else {
		thisgame->gameRun();
		if ((GetAsyncKeyState('P') & 0x8000)||thisgame->ifstop()) {
			state = 4;
			check = true;
		}
		if (GetAsyncKeyState('R') & 0x8000) {//重玩本关
			delete thisgame;
			thisgame = new aGame(snapshot);
		}
	}
}

aGame* GameManager::startAGame() {
	aGame* game = new aGame(set);

	return game;
}
aGame* GameManager::startAGame(std::wstring) {
	aGame* game = new aGame(lastgamename);

	return game;
}

void GameManager::Stop() {
	if (check) {
		while (!buttons.empty()) {
			delete buttons.back();
			buttons.pop_back();
		}
		check = false;
		Button* resume = new Button(WindowWidth / 5, WindowHeight / 2, WindowWidth / 5, WindowHeight / 12);//绘制按钮
		resume->setString(L"恢复");
		resume->setid(0);
		buttons.push_back(resume);

		Button* lastgame = new Button(WindowWidth / 5 * 4, WindowHeight / 2, WindowWidth / 5, WindowHeight / 12);
		lastgame->setString(L"保存残局");
		lastgame->setid(1);
		buttons.push_back(lastgame);
	}
	if (peekmessage(m, EX_MOUSE)) {
		if (m->message == WM_LBUTTONDOWN) {
			int x = m->x; int y = m->y;
			for (Button* i : buttons) {//检查按钮触发
				if (i->ifIn(x, y)) {
					switch (i->uid()) {
					case 0: { check = true; state = 3; thisgame->nostop(); Start(); break; }
					case 1: { if (createLastgame(true)) { check = true; state = 3; thisgame->nostop(); Start(); }; break; }//保存残局信息后继续游戏
					}
					break;
				}
			}
		}
	}
}

void GameManager::StopDraw() {
	settextcolor(WHITE);//绘制“暂停”
	settextstyle(WindowHeight / 12, 0, _T("Consolas"));
	outtextxy((WindowWidth-textwidth(L"已暂停")) / 2, WindowHeight / 6, L"已暂停");
	for (auto i : buttons) {
		i->draw();
	}
}

void GameManager::Win() {
	if (check) {
		while (!buttons.empty()) {
			delete buttons.back();
			buttons.pop_back();
		}
		check = false;
		Button* next = new Button(WindowWidth / 2, WindowHeight / 2, WindowWidth / 3, WindowHeight / 12);
		next->setString(L"下一关");
		next->setid(0);
		buttons.push_back(next);
	}
	if (peekmessage(m, EX_MOUSE)) {
		if (m->message == WM_LBUTTONDOWN) {
			int x = m->x; int y = m->y;
			for (Button* i : buttons) {//检查按钮触发
				if (i->ifIn(x, y)) {
					switch (i->uid()) {
					case 0: { check = true; state = 3; break; }
					}
					break;
				}
			}
		}
	}
}

void GameManager::WinDraw() {
	settextcolor(WHITE);//绘制“胜利”
	settextstyle(WindowHeight / 12, 0, _T("Consolas"));
	outtextxy((WindowWidth - textwidth(L"你过关")) / 2, WindowHeight / 6, L"你过关！");
	std::basic_ostringstream<TCHAR> oss;//建立字符串流
	oss << _T("当前分数: ") << thisgame->getScore();//输出分数
	std::basic_string<TCHAR> str = oss.str();
	LPCTSTR word = (str).c_str();
	outtextxy((WindowWidth - textwidth(word)) / 2, WindowHeight / 3, word);
	for (auto i : buttons) {
		i->draw();
	}
}

void GameManager::Lose() {
	if (check) {
		while (!buttons.empty()) {
			delete buttons.back();
			buttons.pop_back();
		}
		check = false;
		Button* menu = new Button(WindowWidth / 2, WindowHeight / 2, WindowWidth / 3, WindowHeight / 12);
		menu->setString(L"返回主菜单");
		menu->setid(0);
		buttons.push_back(menu);
	}
	if (peekmessage(m, EX_MOUSE)) {
		if (m->message == WM_LBUTTONDOWN) {
			int x = m->x; int y = m->y;
			for (Button* i : buttons) {//检查按钮触发
				if (i->ifIn(x, y)) {
					switch (i->uid()) {
					case 0: { check = true; state = 0; break; }
					}
					break;
				}
			}
		}
	}
}

void GameManager::LoseDraw() {
	settextcolor(WHITE);//绘制“GameOver”
	settextstyle(WindowHeight / 12, 0, _T("Consolas"));
	outtextxy((WindowWidth - textwidth(L"该罚")) / 2, WindowHeight / 6, L"该罚！");
	std::basic_ostringstream<TCHAR> oss;//建立字符串流
	oss << _T("分数: ") << thisgame->getScore();//输出分数
	std::basic_string<TCHAR> str = oss.str();
	LPCTSTR word = (str).c_str();
	outtextxy((WindowWidth - textwidth(word)) / 2, WindowHeight / 3, word);
	for (auto i : buttons) {
		i->draw();
	}
}



void GameManager::Lastgame() {
	if (check) {
		while (!buttons.empty()) {
			delete buttons.back();
			buttons.pop_back();
		}
		check = false;
		//初始化按钮
		Button* menu = new Button(WindowWidth / 10, WindowHeight / 10, WindowWidth / 30, WindowHeight / 30);
		menu->setString(_T("返回"));
		menu->setid(0);
		buttons.push_back(menu);

		Button* create = new Button(WindowWidth / 10 * 9, WindowHeight / 10 * 9, WindowWidth / 25, WindowHeight / 30);
		create->setString(_T("创建新残局"));
		create->setid(10);
		buttons.push_back(create);

		Button* nocreate = new Button(WindowWidth / 10 * 9, WindowHeight / 10 * 7, WindowWidth / 25, WindowHeight / 30);
		nocreate->setString(_T("不加载残局"));
		nocreate->setid(1);
		buttons.push_back(nocreate);


		allSettingsName.clear();
		int i = 0;
		for (const auto& entry : std::filesystem::directory_iterator(lastgame_prefix)) {//与“可选择的配置文件的按钮”是相同配置方法
			if (entry.is_regular_file()) {
				Button* set = new Button(WindowWidth / 2, WindowHeight / 10 * (2 + i), WindowWidth / 5, WindowHeight / 25);
				set->setString(entry.path().filename());
				set->setid(-1 - i);
				buttons.push_back(set);

				allSettingsName.push_back(entry.path().filename());
				i++;
			}
		}
	}
	//残局查看与选择
	
	if (peekmessage(m, EX_MOUSE)) {
		if (m->message == WM_LBUTTONDOWN) {
			int x = m->x; int y = m->y;
			for (Button* i : buttons) {//检查按钮触发
				if (i->ifIn(x, y)) {//在按钮范围内
					if (i->uid() < 0) {
						lastgamename = i->Word();
						//在创建游戏时，依据残局文件创建游戏
						ifLastgame = true;
						check = true; state = 0;//返回主菜单
					}//检查配置文件按钮
					else {
						switch (i->uid()) {
						case 0: { check = true; state = 0; break; }
						case 1: { check = true; state = 0; ifLastgame = false;lastgamename=L""; break; }
						case 10: { this->createLastgame(); break; }
						}
					}
					break;
				}

			}
		}
		else if (WM_MOUSEWHEEL) {
			for (auto& i : buttons) {
				if (i->uid() < 0) { i->moveY(m->wheel / 10); }
			}
		}//滚动滑轮
	}
	//确认残局信息
}
	

//TODO如何创建残局？
bool GameManager::createLastgame(bool ready) {
	std::wstring name;
	wchar_t s[30];
	int result = InputBox(s, 30,
		L"保存当前游戏为残局，请填写残局名称\n"
		L"要求：\n"
		L"只能包含字母、数字或下划线\n");
	if (result == 0) {
		return false; // 用户取消
	}
	if (wcslen(s) == 0) {
		MessageBox(GetHWnd(), L"残局名称不能为空", L"输入错误", MB_OK | MB_ICONERROR);
		return false;
	}
	// 验证名称格式
	for (size_t i = 0; i < wcslen(s); i++) {
		wchar_t c = s[i];
		if (!((c >= L'a' && c <= L'z') ||
			(c >= L'A' && c <= L'Z') ||
			(c >= L'0' && c <= L'9') ||
			c == L'_')) {
			MessageBox(GetHWnd(),
				L"残局名称只能包含字母、数字或下划线\n"
				L"请不要使用空格、中文或其他特殊字符",
				L"输入错误", MB_OK | MB_ICONERROR);
			return false;
		}
	}
	name = s;
	std::wstring fullPath = lastgame_prefix + name + lastgame_postfix;
	if (std::filesystem::exists(fullPath)) {
		int choice = MessageBox(GetHWnd(),
			L"该残局名称已存在，是否覆盖？",
			L"确认覆盖", MB_YESNO | MB_ICONWARNING);
		if (choice != IDYES) {
			return false;
		}
	}
	std::ofstream o(fullPath, std::ios::binary);
	if (!o.is_open()) {
		MessageBox(GetHWnd(), L"无法创建残局文件", L"错误", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!ready) {//创建新残局
		int x, y, gameLevel;
		{
		wchar_t bufferX[30] = { 0 };
		int resultX = InputBox(bufferX, 30,
			L"请输入横向的砖块数\n"
			L"要求：不小于8，不大于20的整数\n"
			L"当前输入：");
		if (bufferX[0] == L'\0') {
			return false; // 用户取消
		}
		try {
			x = std::stoi(bufferX);
			if (x < 8 || x > 20) {
				MessageBox(GetHWnd(), L"横向砖块数必须在8到20之间", L"输入错误", MB_OK | MB_ICONERROR);
				return false;
			}
		}
		catch (...) {
			MessageBox(GetHWnd(), L"请输入有效的整数", L"输入错误", MB_OK | MB_ICONERROR);
			return false;
		}
		wchar_t bufferY[30] = { 0 };
		int resultY = InputBox(bufferY, 30,
			L"请输入纵向的砖块数\n"
			L"要求：不小于8，不大于20的整数\n"
			L"当前输入：");
		if (bufferY[0] == L'\0') {
			return false; // 用户取消
		}
		try {
			y = std::stoi(bufferY);
			if (y < 8 || y > 20) {
				MessageBox(GetHWnd(), L"纵向砖块数必须在8到20之间", L"输入错误", MB_OK | MB_ICONERROR);
				return false;
			}
		}
		catch (...) {
			MessageBox(GetHWnd(), L"请输入有效的整数", L"输入错误", MB_OK | MB_ICONERROR);
			return false;
		}

		wchar_t bufferK[30] = { 0 };
		int resultK = InputBox(bufferK, 30,
			L"请输入当前游戏等级\n"
			L"请输入1-10之间的整数：");
		if (bufferK[0] == L'\0') {
			return false; // 用户取消
		}
		try {
			gameLevel = std::stoi(bufferK);
			if (gameLevel < 1 || gameLevel > 10) {
				MessageBox(GetHWnd(), L"游戏等级必须在1到10之间", L"输入错误", MB_OK | MB_ICONERROR);
				return false;
			}
		}
		catch (...) {
			MessageBox(GetHWnd(), L"请输入有效的整数", L"输入错误", MB_OK | MB_ICONERROR);
			return false;
		}

		// 确认配置
		std::wstring confirmMsg = L"配置信息：\n"
			L"横向砖块数：" + std::to_wstring(x) + L"\n" +
			L"纵向砖块数：" + std::to_wstring(y) + L"\n" +
			L"游戏等级：" + std::to_wstring(gameLevel) + L"\n\n" +
			L"是否采用当前配置创建残局？";

		int confirm = MessageBox(GetHWnd(), confirmMsg.c_str(), L"确认配置", MB_YESNO | MB_ICONQUESTION);
		if (confirm != IDYES) {
			return false;
		}
	}
		aGame* game = new aGame(set,x,y,gameLevel);
		bool ifcontinue = true;
		brickType t = Normal;
		while (ifcontinue) {
			cleardevice();
			game->gameDraw(settings);
			SetWorkingImage(); // 切换回屏幕
			putimage(0, 0, &img); // 将内存图像输出到屏幕
			SetWorkingImage(&img); // 切换回内存缓冲区
			int xx, yy;//要修改的位置
			wchar_t choiceBuffer[10] = { 0 };
			switch (t) {
			case 0: InputBox(choiceBuffer, 10,
				L"当前砖块类型："
				L"普通砖块 (白色)"
				L"\n"
				L"请选择操作:\n"
				L"1. 修改砖块类型\n"
				L"2. 切换当前砖块类型\n"
				L"3. 完成编辑\n"
				L"请输入数字 1-3:"); break;
			case 1: InputBox(choiceBuffer, 10,
				L"当前砖块类型："
				L"耐久砖块 (金色)"
				L"\n"
				L"请选择操作:\n"
				L"1. 修改砖块类型\n"
				L"2. 切换当前砖块类型\n"
				L"3. 完成编辑\n"
				L"请输入数字 1-3:"); break;
			case 2: InputBox(choiceBuffer, 10,
				L"当前砖块类型："
				L"坚不可摧 (灰色)"
				L"\n"
				L"请选择操作:\n"
				L"1. 修改砖块类型\n"
				L"2. 切换当前砖块类型\n"
				L"3. 完成编辑\n"
				L"请输入数字 1-3:"); break;
			case 3: InputBox(choiceBuffer, 10,
				L"当前砖块类型："
				L"无砖块，取消放置"
				L"\n"
				L"请选择操作:\n"
				L"1. 修改砖块类型\n"
				L"2. 切换当前砖块类型\n"
				L"3. 完成编辑\n"
				L"请输入数字 1-3:"); break;
			}
			if (choiceBuffer[0] == L'\0') {
				continue; // 用户取消，继续循环
			}
			int choice = 0;

			try {
				choice = std::stoi(choiceBuffer);
				if (choice >= 1 && choice <= 3) {
				}
				else {
					MessageBox(GetHWnd(), L"请输入1-3之间的数字", L"输入错误", MB_OK | MB_ICONERROR);
				}
			}
			catch (...) {
				MessageBox(GetHWnd(), L"请输入有效的数字", L"输入错误", MB_OK | MB_ICONERROR);
				continue;
			}
			
			switch (choice) {
			case 1: { // 修改指定位置的砖块类型
				int row = 0, col = 0;
				bool validInput = false;

				// 分别输入行和列
				do {
					std::wstring rowPrompt = L"请输入砖块所在的行号\n范围: 1-" + std::to_wstring(y) + L"\n当前输入：";
					wchar_t rowBuffer[30] = { 0 };
					InputBox(rowBuffer, 30, rowPrompt.c_str());
					if (rowBuffer[0] == L'\0') {
						break; // 用户取消
					}
					try {
						row = std::stoi(rowBuffer);
						if (row < 1 || row > y) {
							std::wstring errorMsg = L"行号必须在1到" + std::to_wstring(y) + L"之间";
							MessageBox(GetHWnd(), errorMsg.c_str(), L"输入错误", MB_OK | MB_ICONERROR);
							continue;
						}
					}
					catch (...) {
						MessageBox(GetHWnd(), L"请输入有效的整数", L"输入错误", MB_OK | MB_ICONERROR);
						continue;
					}

					std::wstring colPrompt = L"请输入砖块所在的列号\n范围: 1-" + std::to_wstring(x) + L"\n当前输入：";
					wchar_t colBuffer[30] = { 0 };
					InputBox(colBuffer, 30, colPrompt.c_str());
					if (colBuffer[0] == L'\0') {
						break; // 用户取消
					}
					try {
						col = std::stoi(colBuffer);
						if (col < 1 || col > x) {
							std::wstring errorMsg = L"列号必须在1到" + std::to_wstring(x) + L"之间";
							MessageBox(GetHWnd(), errorMsg.c_str(), L"输入错误", MB_OK | MB_ICONERROR);
							continue;
						}
					}
					catch (...) {
						MessageBox(GetHWnd(), L"请输入有效的整数", L"输入错误", MB_OK | MB_ICONERROR);
						continue;
					}

					validInput = true;
				} while (!validInput);

				if (validInput) {
					xx = col - 1; // 转换为0-based索引
					yy = row - 1; // 转换为0-based索引
					game->SetMapBrick(t, xx, yy);
				}
				break;
			}

			case 2: { // 切换当前砖块类型
				wchar_t typeBuffer[10] = { 0 };
				InputBox(typeBuffer, 10,
					L"请选择砖块类型:\n"
					L"0: 普通砖块 (白色)\n"
					L"1: 耐久砖块 (金色)\n"
					L"2: 坚不可摧 (灰色)\n"
					L"3: 无砖块，取消放置\n"
					L"请输入数字 0-3:");

				if (typeBuffer[0] == L'\0') {
					continue;
				}

				int newType = std::stoi(typeBuffer);
				if (newType >= 0 && newType <= 3) {
					t = (brickType)newType;
				}
				else {
					MessageBox(GetHWnd(), L"请输入有效的类型数字 (0-3)", L"错误", MB_OK | MB_ICONERROR);
				}
				break;
			}

			case 3: // 完成编辑
				ifcontinue = false;
				break;

			default:
				MessageBox(GetHWnd(), L"请输入有效的选项 (1-3)", L"错误", MB_OK | MB_ICONERROR);
				break;
			}
			Sleep(10);
		}
		game->serialize(o);
		o.close();
	}
	else {//保存暂停的残局
		thisgame->serialize(o);
		o.close();
	}
	lastgamename = name + L".end";
	ifLastgame = true;
	return true;
}


