# include <graphics.h>
# include <iostream>

int simple_circle () {
	initgraph(1280, 720);

	int x = 640;
	int y = 360;

	BeginBatchDraw();

	while (true) {
		ExMessage msg;
		while (peekmessage(&msg)) {
			if (msg.message == WM_MOUSEMOVE) {
				x = msg.x;
				y = msg.y;

			}
		}
		cleardevice();
		solidcircle(x, y, 100);
		FlushBatchDraw();
	}

	EndBatchDraw();
	return 0;
}