#include <stdio.h>
#include <easyx.h>
#include <conio.h>
#include <time.h>

const int MAX = 10;

enum ENV {
	WIN_WIDTH = 480,   //backgroud width
	WIN_HEIGHT = 700,  //backgroud height
	BULLET_NUM = 30, //bullet number
	SHIP_SPEED = 5,  //ship speed
	BULLET_SPEED = 3,//bullet speed
	BIG,			//big enemies
	SMALL,			//smalll enemies
	ENEMY_NUM = 5,    //enemy number
	ENEMY_SPEED = 3   //enemy speed
};

struct ship {   //ally and enemy
	int x;
	int y;     //ship location
	bool live; //check live(include bullet)
	int hp;    //health
	int type;   //enemy type
	int width;
	int height;
};

ship ally;
ship bullet[BULLET_NUM];
ship enemy[ENEMY_NUM];

IMAGE background;
IMAGE img_ally[2];
IMAGE img_enemy[2][2];
IMAGE img_bullet[2];

//timer
bool Timer(int ms, int id)
{
	static DWORD t[MAX];         //t[10]
	if (clock() - t[id] > ms)
	{
		t[id] = clock();
		return true;
	}
	return false;
}

//initial and load image
void initial()
{
	ally.x = WIN_WIDTH / 2 - 51;
	ally.y = WIN_HEIGHT - 126;
	ally.live = true;

	initgraph(WIN_WIDTH, WIN_HEIGHT);

	loadimage(&background, L"image\\bk.png", 480, 700);
	loadimage(&img_ally[0], L"image\\me1.jpg", 102, 126, NOTSRCERASE);
	loadimage(&img_ally[1], L"image\\me2.jpg", 102, 126, SRCINVERT);
	loadimage(&img_enemy[0][0], L"image\\enemyPlane1.jpg", 69, 99, NOTSRCERASE);
	loadimage(&img_enemy[0][1], L"image\\enemyPlane2.jpg", 69, 99, SRCINVERT);
	loadimage(&img_enemy[1][0], L"image\\enemyPlane1.jpg", 80, 110, NOTSRCERASE);
	loadimage(&img_enemy[1][1], L"image\\enemyPlane2.jpg", 80, 110, SRCINVERT);
	loadimage(&img_bullet[0], L"image\\bullet1.jpg", 5, 11, NOTSRCERASE);
	loadimage(&img_bullet[1], L"image\\bullet2.jpg", 5, 11, SRCINVERT);
}

//put images
void put()
{
	putimage(0, 0, &background);
	putimage(ally.x, ally.y, &img_ally[0], NOTSRCERASE);
	putimage(ally.x, ally.y, &img_ally[1], SRCINVERT);
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].live) {
			if (enemy[i].type == SMALL)
			{
				putimage(enemy[i].x, enemy[i].y, &img_enemy[0][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_enemy[0][1], SRCINVERT);
			}
			else
			{
				putimage(enemy[i].x, enemy[i].y, &img_enemy[1][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_enemy[1][1], SRCINVERT);
			}
		}
	}
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].live)
		{
			putimage(bullet[i].x, bullet[i].y, &img_bullet[0], NOTSRCERASE);
			putimage(bullet[i].x, bullet[i].y, &img_bullet[1], SRCINVERT);
		}
	}
}

//create ally bullet
void createbullet()
{
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (!bullet[i].live)
		{
			bullet[i].x = ally.x + 48.5;
			bullet[i].y = ally.y - 11;
			bullet[i].live = true;
			break;
		}
	}
}

//create enemies
bool check_collision_rect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
	return x1 < x2 + w2 &&
		x1 + w1 > x2 &&
		y1 < y2 + h2 &&
		y1 + h1 > y2;
}

void createenemy() {
	for (int i = 0; i < ENEMY_NUM; i++) {
		if (!enemy[i].live) {
			int new_x, new_y = 0;
			int new_width, new_height;
			int new_hp;
			int new_type;

			const int MAX_PLACEMENT_ATTEMPTS = 10;
			bool position_found = false;

			for (int attempt = 0; attempt < MAX_PLACEMENT_ATTEMPTS; ++attempt) {
				new_type = rand() % 2 == 0 ? SMALL : BIG;
				if (new_type == BIG) {
					new_hp = 3;
					new_width = 80;
					new_height = 110;
				}
				else {
					new_hp = 1;
					new_width = 69;
					new_height = 99;
				}

				// Generate a random x position
				if (WIN_WIDTH - new_width <= 0) {
					new_x = 0;
				}
				else {
					new_x = rand() % (WIN_WIDTH - new_width + 1);
				}


				// Check for overlap with existing live enemies
				bool overlaps = false;
				for (int j = 0; j < ENEMY_NUM; ++j) {
					if (i == j || !enemy[j].live) {
						continue;
					}
					if (check_collision_rect(new_x, new_y, new_width, new_height,
						enemy[j].x, enemy[j].y, enemy[j].width, enemy[j].height)) {
						overlaps = true;
						break; // Overlaps with an existing enemy
					}
				}

				if (!overlaps) {
					position_found = true;
					break; // Found a good position
				}
			}

			enemy[i].x = new_x;
			enemy[i].y = new_y;
			enemy[i].live = true;
			enemy[i].type = new_type;
			enemy[i].hp = new_hp;
			enemy[i].width = new_width;
			enemy[i].height = new_height;

			break;
		}
	}
}

//ally bullet move
void bulletmove()
{
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].live)
		{
			bullet[i].y -= BULLET_SPEED;
		}
		if (bullet[i].y < 0)
		{
			bullet[i].live = false;
		}
	}
}

//enemy move
void enemymove()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].live)
		{
			enemy[i].y += ENEMY_SPEED;
		}
		if (enemy[i].y > WIN_HEIGHT)
		{
			enemy[i].live = false;
		}
	}
}

//move ally
void move()
{
	if (_kbhit())
	{
		switch (_getch())
		{
		case 'w':ally.y -= SHIP_SPEED; break;
		case 's':ally.y += SHIP_SPEED; break;
		case 'a':ally.x -= SHIP_SPEED; break;
		case 'd':ally.x += SHIP_SPEED; break;
		case 'j':createbullet(); break;
		}
	}
}

void collision()
{
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].live)
		{
			for (int j = 0; j < ENEMY_NUM; j++)
			{
				if (enemy[j].live && bullet[i].x < enemy[j].x + enemy[j].width && bullet[i].x + 5>enemy[j].x && bullet[i].y<enemy[j].y + enemy[j].height && bullet[i].y + 11>enemy[j].y)
				{
					bullet[i].live = false;
					enemy[j].hp--;
					if (enemy[j].hp == 0)
					{
						enemy[j].live = false;
					}
					break;
				}
			}
		}
	}
}

//windows api move ally
void win_move()
{
	/*
	if (GetAsyncKeyState('W') & 0x8000) { ally.y -= SHIP_SPEED; }
	if (GetAsyncKeyState('S') & 0x8000) { ally.y += SHIP_SPEED; }
	if (GetAsyncKeyState('A') & 0x8000) { ally.x -= SHIP_SPEED; }
	if (GetAsyncKeyState('D') & 0x8000) { ally.x += SHIP_SPEED; }
	if (GetAsyncKeyState('J') & 0x8000) { createbullet(); Sleep(50); }
	*/
	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
	{
		ally.y -= SHIP_SPEED;
	}
	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
	{
		ally.y += SHIP_SPEED;
	}
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
	{
		ally.x -= SHIP_SPEED;
	}
	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
	{
		ally.x += SHIP_SPEED;
	}
	if (GetAsyncKeyState('J') & 0x8000)
	{
		createbullet();
	}
}


/*int main()
{
	initial();
	while (1)
	{
		put();
		FlushBatchDraw();

		//win_move();
		move();
		bulletmove();

		if (Timer(500, 0))
		{
			createenemy();
		}
		if (Timer(30, 1))
		{
			enemymove();
		}
		EndBatchDraw();
	}

	return 0;
}*/

int main() {
	srand((unsigned int)time(NULL));
	initial();
	while (1) {
		put();
		FlushBatchDraw();

		win_move();  // Use smoother movement
		bulletmove();
		enemymove();
		collision();  // Add collision detection

		if (Timer(500, 0)) {
			createenemy();
		}

		if (GetAsyncKeyState(VK_ESCAPE)) {
			break;  // Exit game on ESC key
		}

		Sleep(16);  // Control FPS to ~60
		EndBatchDraw();
	}

	closegraph();  // Close graphics window
	return 0;
}
