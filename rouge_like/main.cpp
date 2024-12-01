# include <graphics.h>
# include <iostream>
# include <string>

int idx_current_anim = 0;
const int PLAYER_ANIM_NUM = 6;
const int PLAYER_SPEED = 10;

bool is_move_up = false;
bool is_move_down = false;
bool is_move_left = false;
bool is_move_right = false;

IMAGE image_player_left[PLAYER_ANIM_NUM];
IMAGE image_player_right[PLAYER_ANIM_NUM];

POINT pose_player = {640, 360};

#pragma comment(lib, "MSIMG32.LIB")
inline void putimage_alpha(int x, int y, IMAGE* image) {
	int w = image->getwidth();
	int h = image->getheight();
	AlphaBlend(
		GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(image), 0, 0, w, h,
		{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}


void load_image() {
	for (int i = 0; i < PLAYER_ANIM_NUM; i++) {
		std::wstring path = L"img/paimon_left_" + std::to_wstring(i) + L".png";
		loadimage(&image_player_left[i], path.c_str());
	}

	for (int i = 0; i < PLAYER_ANIM_NUM; i++) {
		std::wstring path = L"img/paimon_right_" + std::to_wstring(i) + L".png";
		loadimage(&image_player_right[i], path.c_str());
	}
}

int main() {
	initgraph(1280, 720);

	bool is_running = true;

	ExMessage msg;
	IMAGE image_background;

	loadimage(&image_background, _T("img/background.png"));
	load_image();
	BeginBatchDraw();

	while (is_running) {
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg)) {
			if (msg.message == WM_KEYDOWN) {
				switch (msg.vkcode) {
				case VK_UP:
					is_move_up = true;
					break;
				case VK_DOWN:
					is_move_down = true;
					break;
				case VK_LEFT:
					is_move_left = true;
					break;
				case VK_RIGHT:
					is_move_right = true;
					break;
				case 87:
					is_move_up = true;
					break;
				case 83:
					is_move_down = true;
					break;
				case 65:
					is_move_left = true;
					break;
				case 68:
					is_move_right = true;
					break;
				}
			}
			else if (msg.message == WM_KEYUP) {
				switch (msg.vkcode) {
				case VK_UP:
					is_move_up = false;
					break;
				case VK_DOWN:
					is_move_down = false;
					break;
				case VK_LEFT:
					is_move_left = false;
					break;
				case VK_RIGHT:
					is_move_right = false;
					break;
				case 87:
					is_move_up = false;
					break;
				case 83:
					is_move_down = false;
					break;
				case 65:
					is_move_left = false;
					break;
				case 68:
					is_move_right = false;
					break;
				}
			}
		}
		if (is_move_up) pose_player.y -= PLAYER_SPEED;
		if (is_move_down) pose_player.y += PLAYER_SPEED;
		if (is_move_left) pose_player.x -= PLAYER_SPEED;
		if (is_move_right) pose_player.x += PLAYER_SPEED;

		static int counter = 0;
		if (++counter % 5 == 0) 
			idx_current_anim++;

		idx_current_anim %= PLAYER_ANIM_NUM;


		cleardevice();

		putimage(0, 0, &image_background);
		putimage_alpha(pose_player.x, pose_player.y, &image_player_left[idx_current_anim]);

		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;

		if (delta_time < 1000 / 165) {
			Sleep(1000 / 165 - delta_time);
		}

	}

	EndBatchDraw();
	return 0;
} // main 