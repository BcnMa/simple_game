# include <graphics.h>
# include <iostream>

char current_piece = 'o';

char board[3][3] = {
	{ '-', '-', '-' },
	{ '-', '-', '-' },
	{ '-', '-', '-' }
};

bool check_win(char player) {
	if (board[0][0] == player && board[0][1] == player && board[0][2] == player)
		return true;
	if (board[1][0] == player && board[1][1] == player && board[1][2] == player)
		return true;
	if (board[2][0] == player && board[2][1] == player && board[2][2] == player)
		return true;
	if (board[0][0] == player && board[1][0] == player && board[2][0] == player)
		return true;
	if (board[0][1] == player && board[1][1] == player && board[2][1] == player)
		return true;
	if (board[0][2] == player && board[1][2] == player && board[2][2] == player)
		return true;
	if (board[0][0] == player && board[1][1] == player && board[2][2] == player)
		return true;
	if (board[0][2] == player && board[1][1] == player && board[2][0] == player)
		return true;

	return false;
}

bool check_draw() {
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			if (board[i][j] == '-') {
				return false;
			}
		}
	}
	return true;
}

void draw_board() {
	setlinestyle(PS_SOLID, 5);
	line(200, 0, 200, 600);
	line(400, 0, 400, 600);
	line(0, 200, 600, 200);
	line(0, 400, 600, 400);
}

void draw_piece() {
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			if (board[i][j] == 'o') {
				circle(j * 200 + 100, i * 200 + 100, 100);
			}
			else if (board[i][j] == 'x') {
				line(j * 200, i * 200, j * 200 + 200, i * 200 + 200);
				line(j * 200, i * 200 + 200, j * 200 + 200, i * 200);
			}
		}
	}

}

void draw_text() {
	static TCHAR str[64];
	_stprintf_s(str, _T("当前玩家棋子类型: %c"), current_piece);

	settextcolor(RGB(255, 0, 0));
	outtextxy(0, 0, str);
}

void restart_game() {
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			board[i][j] = '-';
		}
	}

	current_piece = 'o';
}

int main() {
	initgraph(600, 600);

	bool is_running = true;
	int x = 300;
	int y = 300;
	int index_x, index_y;



	ExMessage msg;
	BeginBatchDraw();
	while (is_running) {
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN) {
				x = msg.x;
				y = msg.y;

				index_x = y / 200;
				index_y = x / 200;

				if (board[index_x][index_y] == '-') {
					board[index_x][index_y] = current_piece;

					if (current_piece == 'o') {
						current_piece = 'x';
					}
					else {
						current_piece = 'o';
					}
				}
			}
		}
		cleardevice();

		draw_board();
		draw_piece();
		draw_text();

		FlushBatchDraw();

		if (check_win('x')) {
			MessageBox(GetHWnd(), _T("x 玩家获胜"), _T("游戏结束"), MB_OK);
			is_running = false;
		}

		if (check_win('o')) {
			MessageBox(GetHWnd(), _T("o 玩家获胜"), _T("游戏结束"), MB_OK);
			is_running = false;
		}

		if (check_draw() && is_running) {
			MessageBox(GetHWnd(), _T("平局"), _T("游戏结束"), MB_OK);
			is_running = false;
		}

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;

		if (!is_running) {
			MessageBox(GetHWnd(), _T("重新开始游戏？"), _T("此回合结束"), MB_OK);
			restart_game();
			is_running = true;
		}

		if (delta_time < 1000 / 165) {
			Sleep(1000 / 165 - delta_time);
		}
	}

	EndBatchDraw();
} // main