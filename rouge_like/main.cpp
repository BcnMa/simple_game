# include <graphics.h>
# include <iostream>
# include <string>
# include <vector>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

# pragma comment(lib, "winmm.lib")
# pragma comment(lib, "MSIMG32.LIB")

inline void putimage_alpha(int x, int y, IMAGE* image) {
	int w = image->getwidth();
	int h = image->getheight();
	AlphaBlend(
		GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(image), 0, 0, w, h,
		{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

enum class SpawnEdge {
	Up = 0,
	Down,
	Left,
	Right,
};

class Animation {
private:
	int timer = 0;
	int idx_frame = 0;
	int interval_ms = 0;
	std::vector<IMAGE*> frame_list;

public:
	Animation(LPCTSTR path, int num, int interval) {
		interval_ms = interval;

		TCHAR path_file[256];
		for (int i = 0; i < num; i++) {
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}

	~Animation() {
		for (int i = 0; i < frame_list.size(); i++) {
			delete frame_list[i];
		}
	}

	void play(int x, int y, int delta) {
		timer += delta;
		if (timer >= interval_ms) {
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}

		putimage_alpha(x, y, frame_list[idx_frame]);
	}
}; // class Animation


class Player {
private:
	IMAGE image_shadow;
	Animation * anim_left, * anim_right;

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
	bool facing_left = false;	

	const int PLAYER_SPEED = 3;
	const int PLAYER_WIDTH = 80;
	const int PLAYER_HEIGHT = 80;
	const int PLAYER_SHADOW_WIDTH = 32;

	POINT player_pose = { WINDOW_WIDTH / 2 - PLAYER_WIDTH / 2, WINDOW_HEIGHT / 2 - PLAYER_HEIGHT / 2 };

public:
	Player() {
		loadimage(&image_shadow, _T("img/paimon_shadow.png"));
		anim_left = new Animation(_T("img/paimon_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/paimon_right_%d.png"), 6, 45);
	}

	~Player() {
		delete anim_left;
		delete anim_right;
	}

	void reset() {
		player_pose.x = WINDOW_WIDTH / 2 - PLAYER_WIDTH / 2;
		player_pose.y = WINDOW_HEIGHT / 2 - PLAYER_HEIGHT / 2;
		is_move_up = false;
		is_move_down = false;
		is_move_left = false;
		is_move_right = false;
		facing_left = false;
	}
	void process_event(const ExMessage& msg) {
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

	void move() {
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);

		if (len_dir != 0) {

			double nor_x = dir_x / len_dir;
			double nor_y = dir_y / len_dir;
			player_pose.x += int(PLAYER_SPEED * nor_x);
			player_pose.y += int(PLAYER_SPEED * nor_y);
		}
		player_pose.x = min(max(player_pose.x, 0), WINDOW_WIDTH - PLAYER_WIDTH);
		player_pose.y = min(max(player_pose.y, 0), WINDOW_HEIGHT - PLAYER_HEIGHT);

		if (dir_x < 0) facing_left = true;
		else if (dir_x > 0) facing_left = false;
	}

	void draw(int delta) {
		int pose_shadow_x = player_pose.x + (PLAYER_WIDTH - PLAYER_SHADOW_WIDTH) / 2;
		int pose_shadow_y = player_pose.y + PLAYER_HEIGHT - 8;
		putimage_alpha(pose_shadow_x, pose_shadow_y, &image_shadow);

		if (facing_left) anim_left->play(player_pose.x, player_pose.y, delta);
		else anim_right->play(player_pose.x, player_pose.y, delta);
	}

	const POINT& get_position() const {
		return player_pose;
	}

	const int get_width() const {
		return PLAYER_WIDTH;
	}

	const int get_height() const {
		return PLAYER_HEIGHT;
	}

	void set_position(const POINT& pose) {
		player_pose = pose;
	}

}; // class Player


class Bullet {
private:
	POINT position = { 0, 0 };
	const int RADIUS = 5;

public:
	Bullet() = default;
	~Bullet() = default;

	void move(int x, int y) {
		position.x = x;
		position.y = y;
	}

	void draw() const {
		setlinecolor(RGB(255, 0, 0));
		setfillcolor(RGB(255, 0, 0));
		fillcircle(position.x, position.y, RADIUS);
	}

	const POINT get_position() const {
		return position;
	}

	void set_position(const POINT& pose) {
		position = pose;
	}
}; // class Bullet


class Enemy {
private:
	IMAGE image_shadow;
	Animation* anim_left, * anim_right;

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
	bool facing_left = false;
	bool alive = true;

	const int ENEMY_SPEED = 4;
	const int ENEMY_WIDTH = 96;
	const int ENEMY_HEIGHT = 96;
	const int ENEMY_SHADOW_WIDTH = 32;

	POINT enemy_pose = { 0, 0 };

public:
	Enemy() {
		loadimage(&image_shadow, _T("img/bee_shadow.png"));
		anim_left = new Animation(_T("img/bee_left_%d.png"), 4, 45);
		anim_right = new Animation(_T("img/bee_right_%d.png"), 4, 45);

		SpawnEdge edge = (SpawnEdge)(rand() % 4);
		switch (edge) {
		case SpawnEdge::Up:
			enemy_pose.x = rand() % (WINDOW_WIDTH - ENEMY_WIDTH);
			enemy_pose.y = -ENEMY_HEIGHT;
			break;
		case SpawnEdge::Down:
			enemy_pose.x = rand() % (WINDOW_WIDTH - ENEMY_WIDTH);
			enemy_pose.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			enemy_pose.x = -ENEMY_WIDTH;
			enemy_pose.y = rand() % (WINDOW_HEIGHT - ENEMY_HEIGHT);
			break;
		case SpawnEdge::Right:
			enemy_pose.x = WINDOW_WIDTH;
			enemy_pose.y = rand() % (WINDOW_HEIGHT - ENEMY_HEIGHT);
			break;
		default:
			break;
		}
	}

	~Enemy() {
		delete anim_left;
		delete anim_right;
	}

	bool check_bullet_collision(const Bullet& bullet) {
		bool is_overlap_x = bullet.get_position().x >= enemy_pose.x && bullet.get_position().x <= enemy_pose.x + ENEMY_WIDTH;
		bool is_overlap_y = bullet.get_position().y >= enemy_pose.y && bullet.get_position().y <= enemy_pose.y + ENEMY_HEIGHT;
		return is_overlap_x && is_overlap_y;
	}

	bool check_player_collision(const Player& player) {
		bool is_overlap_x = enemy_pose.x + ENEMY_WIDTH / 2 >= player.get_position().x && enemy_pose.x + ENEMY_WIDTH / 2 <= player.get_position().x + player.get_width();
		bool is_overlap_y = enemy_pose.y + ENEMY_HEIGHT >= player.get_position().y && enemy_pose.y + ENEMY_HEIGHT / 2 <= player.get_position().y + player.get_height();
		return is_overlap_x && is_overlap_y;
	}

	void move(const Player& player) {
		const POINT& player_position = player.get_position();
		int dir_x = player_position.x - enemy_pose.x;
		int dir_y = player_position.y - enemy_pose.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0) {

			double nor_x = dir_x / len_dir;
			double nor_y = dir_y / len_dir;
			enemy_pose.x += int(ENEMY_SPEED * nor_x);
			enemy_pose.y += int(ENEMY_SPEED * nor_y);
		}
		enemy_pose.x = min(max(enemy_pose.x, 0), WINDOW_WIDTH - ENEMY_WIDTH);
		enemy_pose.y = min(max(enemy_pose.y, 0), WINDOW_HEIGHT - ENEMY_HEIGHT);

		if (dir_x < 0) facing_left = true;
		else if (dir_x > 0) facing_left = false;
	}

	void draw(int delta) {
		int pose_shadow_x = enemy_pose.x + (ENEMY_WIDTH - ENEMY_SHADOW_WIDTH) / 2;
		int pose_shadow_y = enemy_pose.y + ENEMY_HEIGHT - 8;
		putimage_alpha(pose_shadow_x, pose_shadow_y, &image_shadow);

		if (facing_left) anim_left->play(enemy_pose.x, enemy_pose.y, delta);
		else anim_right->play(enemy_pose.x, enemy_pose.y, delta);

	}

	void hurt() {
		alive = false;
	}

	bool check_alive() {
		return alive;
	}
			
}; // class Enemy

void generate_enemy(std::vector<Enemy*>& enemy_list) {
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0) {
		enemy_list.push_back(new Enemy());
	}
}

void update_bullets(std::vector<Bullet>& bullet_list, const Player& player) {
	const double RADIUS_SPEED = 0.0045;
	const double TANGENT_SPEED = 0.0055;
	double delta_degree = 2 * 3.14159 / bullet_list.size();

	POINT player_pose = player.get_position();
	double radius = 100 + 25 * sin(GetTickCount() * RADIUS_SPEED);
	for (int i = 0; i < bullet_list.size(); i++) {
		double radian = GetTickCount() * TANGENT_SPEED + delta_degree * i;
		POINT pose = { 
			player_pose.x + player.get_width() / 2  + int(radius * cos(radian)), 
			player_pose.y + player.get_height() / 2 + int(radius * sin(radian))
		};
		bullet_list[i].set_position(pose);
	}
}

void draw_socres(int scores) {
	static TCHAR text[64];
	_stprintf_s(text, _T("当前得分： %d"), scores);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(0, 255, 0));
	outtextxy(590, 10, text);
}

int main() {
	ExMessage msg;
	IMAGE image_background;
	int scores = 0;

	initgraph(1280, 720);
	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);

	mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);

	loadimage(&image_background, _T("img/background.png"));

	bool is_running = true;
	BeginBatchDraw();
	
	Player paimon;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(3);

	while (is_running) {
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg)) {
			paimon.process_event(msg);
		}
		paimon.move();
		generate_enemy(enemy_list);
		update_bullets(bullet_list, paimon);

		for (Enemy* enemy : enemy_list) {
			enemy->move(paimon);
		}

		for (Enemy* enemy : enemy_list) {
			if (enemy->check_player_collision(paimon)) {
				MessageBox(GetHWnd(), _T("扣1看派蒙喷水"), _T("你死了，菜逼！"), MB_OK);
				is_running = false;
			}
		}

		if (!is_running) {
			for (Enemy* enemy : enemy_list) {
				delete enemy; 
			}
			enemy_list.clear();
			MessageBox(GetHWnd(), _T("好吧，喷不出来"), _T("重开吧！"), MB_OK);
			paimon.reset();
			scores = 0;
			is_running = true;
		}


		for (Enemy* enemy : enemy_list) {
			for (const Bullet& bullet : bullet_list) {
				if (enemy->check_bullet_collision(bullet)) {
					mciSendString(_T("play hit from 0"), NULL, 0, NULL);
					enemy->hurt();
				}
			}
		}

		for (int i = 0; i < enemy_list.size(); i++) {
			Enemy* enemy = enemy_list[i];
			if (!enemy->check_alive()) {
				std::swap(enemy_list[i], enemy_list.back());
				enemy_list.pop_back();
				delete enemy;
				scores++;
			}
		}
		cleardevice();
		putimage(0, 0, &image_background);
		paimon.draw(1000 / 165);
		for (Enemy* enemy : enemy_list) {
			enemy->draw(1000 / 165);
		}
		for (const Bullet bullet : bullet_list) {
			bullet.draw();
		}
		draw_socres(scores);
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