#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <stdlib.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

// dividing window to sections
#define SCREEN_WIDTH	1260 // px
#define SCREEN_HEIGHT	720 // px
#define SCOREBOARD_WIDTH 300 // px
#define GAME_WIDTH 960 // px
#define GRADE_SQUARE_SIZE 100 // px
#define MENU_HEIGHT 300 // px
#define MENU_WIDTH 400 // px

#define FRAME_DELAY 16 // limiting fps to 60

#define PLAYER_START_X 425 //px
#define PLAYER_START_Y -335 //px
#define PLAYER_SPEED 10 // px per click
#define MAX_PLAYER_HEALTH 3

#define POINTS_LOST_ON_HIT 5
#define POINTS_GAIN_ON_HIT 2
#define POINTS_GAIN_ON_COLLECTING_HEART 2
#define POINTS_GAIN_ON_HIT_MOVING 4

#define TOP -1500 // px
#define BOTTOM 770 // px
#define LEFT -860 // px
#define RIGHT 1770 // px

//player move direction
#define BACKWARD 1
#define NOT_BACKWARD 0

// game options
#define MAX_ENEMIES_AMOUNT 100
#define MAX_BULLETS_PER_SHIP 300
#define BASE_ENEMIES_AMOUNT 5
#define MAX_HEARTS_ON_GROUND 50
#define AIRSTRIKE_SIZE 10

// bullet direction
#define UP -1
#define DOWN 1
#define LEFT_MOVE -1
#define RIGHT_MOVE 1

#define BULLET_SPEED 5 // px per 1/60 s
#define MOVING_ENEMY_SPEED 1 // px per 1/60 s
#define DISTANCE_TILL_PLAYER_BULLET_DISAPPEAR 1500 // px
#define DISTANCE_TILL_ENEMY_BULLET_DISAPPEAR 920 // px

// hitboxes and sprites parameters
#define BULLET_WIDHT 8 // px
#define BULLET_HEIGHT 16 // px
#define BIG_BULLET_WIDTH 16 //px
#define BIG_BULLET_HEIGHT 32 //px
#define BASIC_ENEMY_WIDTH 75 // px
#define BASIC_ENEMY_HEIGHT 72 // px
#define MOVING_ENEMY_WIDTH 40 // px
#define MOVING_ENEMY_HEIGHT 40 // px
#define PLAYER_WIDTH 73 // px
#define PLAYER_HEIGHT 68 // px
#define HEART_WIDTH 50 // px
#define HEART_HEIGHT 50 // px
#define ROUND_BULLET_WIDTH 8 // px
#define ROUND_BULLET_HEIGHT 8 // px
#define ROUND_ENEMY_WIDTH 100 // px
#define ROUND_ENEMY_HEIGHT 76 // px
#define AIRSTRIKE_WIDTH 50 // px 
#define AIRSTRIKE_HEIGHT 50 // px

// enemie shoot frequency
#define SHOOT 3
#define NO_SHOOT 4
#define SHOOT_SPEED 1 // s
#define MOVING_SHOOT 3
#define MOVING_NO_SHOOT 4
#define MOVING_SHOOT_SPEED 4 //s

// invincibility lenght alfter being shoot
#define INVINCIBLE_ON 5
#define INVINCIBLE_OFF 6
#define INVINCIBILITY_TIME 4 // s

// bonus points lenght after killing enemy
#define BONUS_TIME 5 // s

// airstrike after picking up heart
#define AIRSTRIKE_PREPARE_ON 3
#define AIRSTRIKE_PREPARE_OFF 4
#define AIRSTRIKE_PREPARE_TIME 5 // s
#define AIRSTRIKE_FIRE_ON 3
#define AIRSTRIKE_FIRE_OFF 4
#define AIRSTRIKE_FIRE_TIME 2 // s

//displaying diffrent menu
#define OPEN_MENU 0
#define CLOSE_MENU 1
#define OPEN_LEVEL_MENU 0
#define CLOSE_LEVEL_MENU 1
#define OPEN_GAME_OVER 0
#define CLOSE_GAME_OVER 1

//enemy type
#define COMMON 1
#define MOVING 2
#define ROUND 3

struct game_bullet
{
	SDL_Surface* sprite;
	SDL_Rect* hitbox;
	int x;
	int y;
	int directionY;
	int directionX;
};

struct game_heart
{
	SDL_Surface* sprite;
	SDL_Rect* hitbox;
	int x;
	int y;
};

struct game_player
{
	SDL_Surface* sprite;
	SDL_Rect* hitbox;
	int x;
	int y;
	int score;
	int level;
	int health;
	game_bullet* bulletsTable[MAX_BULLETS_PER_SHIP];
	SDL_Surface* hearts[MAX_PLAYER_HEALTH];
	game_heart* heartsTable[MAX_HEARTS_ON_GROUND];
	SDL_Surface* grade;
};

struct game_enemy
{
	SDL_Surface* sprite;
	SDL_Rect* hitbox;
	int x;
	int y;
	int direction;
	int distance;
	int type;
	game_bullet* bulletsTable[MAX_BULLETS_PER_SHIP];
};

struct game_airstrike
{
	SDL_Surface* sprite;
	SDL_Rect* hitbox;
	int x;
	int y;
};

struct game_enemies
{
	int amount;
	game_enemy* enemiesTable[MAX_ENEMIES_AMOUNT];
	game_airstrike* airstrikeTable[AIRSTRIKE_SIZE];
};

int RandomNumberInRange(int range_min, int range_max)
{
	return ((double)rand() / RAND_MAX) * (range_max - range_min) + range_min;
}

void FreeAllSurfaces(SDL_Surface* charset, SDL_Surface* screen, SDL_Texture* scrtex, SDL_Renderer* renderer, SDL_Window* window, game_player* player, game_enemies* enemies)
{
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_FreeSurface(player->sprite);
	for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
	{
		if (player->bulletsTable[i] != NULL)
		{
			SDL_FreeSurface(player->bulletsTable[i]->sprite);
			free(player->bulletsTable[i]->hitbox);
			free(player->bulletsTable[i]);
		}
	}
	for (int i = 0; i < MAX_HEARTS_ON_GROUND; i++)
	{
		if (player->heartsTable[i] != NULL)
		{
			SDL_FreeSurface(player->heartsTable[i]->sprite);
			free(player->heartsTable[i]->hitbox);
			free(player->heartsTable[i]);
		}
	}
	for (int j = 0; j < MAX_ENEMIES_AMOUNT; j++)
	{
		if (enemies->enemiesTable[j] != NULL)
		{
			for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
			{
				if (enemies->enemiesTable[j]->bulletsTable[i] != NULL)
				{
					SDL_FreeSurface(enemies->enemiesTable[j]->bulletsTable[i]->sprite);
					free(enemies->enemiesTable[j]->bulletsTable[i]->hitbox);
					free(enemies->enemiesTable[j]->bulletsTable[i]);
				}
			}
			SDL_FreeSurface(enemies->enemiesTable[j]->sprite);
			free(enemies->enemiesTable[j]->hitbox);
			free(enemies->enemiesTable[j]);
		}
	}
	for (int i = 0; i < AIRSTRIKE_SIZE; i++)
	{
		SDL_FreeSurface(enemies->airstrikeTable[i]->sprite);
		free(enemies->airstrikeTable[i]->hitbox);
		free(enemies->airstrikeTable[i]);
	}
}

void BasicPreparationsAndSettings(SDL_Renderer* renderer, SDL_Window* window, SDL_Surface* charset)
{
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_SetWindowTitle(window, "Warblade Jan Krawczyk");
	SDL_ShowCursor(SDL_DISABLE); // hiding mouse cursor
	SDL_SetColorKey(charset, true, 0x000000);
}

void CountingTimeAndFps(double* worldTime, double* fps, int* timeAtStart, int* frames, double* fpsTimer, double* delta, int* menu)
{
	if (*menu == CLOSE_MENU)
	{
		int secondTime;
		secondTime = SDL_GetTicks();
		*delta = (secondTime - *timeAtStart) * 0.001;
		*timeAtStart = secondTime;
		*worldTime += *delta;

		*fpsTimer += *delta;
		if (*fpsTimer > 0.5)
		{
			*fps = *frames * 2;
			*frames = 0;
			*fpsTimer -= 0.5;
		};
	}
}

void PreparingPlayerAndEnemies(game_player* player, game_enemies* enemies)
{
	player->score = 0;
	player->level = 0;
	player->x = (LEFT + RIGHT) / 2;
	player->y = (TOP + BOTTOM) / 2;
	player->sprite = SDL_LoadBMP("./graphics/player_base.bmp");
	player->hitbox = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	player->hitbox->x = GAME_WIDTH / 2 - PLAYER_WIDTH / 2;
	player->hitbox->y = SCREEN_HEIGHT - (SCREEN_HEIGHT / 6) - PLAYER_HEIGHT / 2;
	player->hitbox->w = PLAYER_WIDTH;
	player->hitbox->h = PLAYER_HEIGHT;
	player->health = MAX_PLAYER_HEALTH;
	for (int i = 0; i < MAX_PLAYER_HEALTH; i++)
	{
		player->hearts[i] = SDL_LoadBMP("./graphics/heart.bmp");
	}
	for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
	{
		player->bulletsTable[i] = NULL;
	}
	for (int i = 0; i < MAX_HEARTS_ON_GROUND; i++)
	{
		player->heartsTable[i] = NULL;
	}

	enemies->amount = 0;
	for (int i = 0; i < MAX_ENEMIES_AMOUNT; i++)
	{
		enemies->enemiesTable[i] = NULL;
	}
	for (int i = 0; i < AIRSTRIKE_SIZE; i++)
	{
		enemies->airstrikeTable[i] = (game_airstrike*)malloc(sizeof(game_airstrike));
		enemies->airstrikeTable[i]->sprite = SDL_LoadBMP("./graphics/airstrike_mark.bmp");
		enemies->airstrikeTable[i]->hitbox = (SDL_Rect*)malloc(sizeof(SDL_Rect));
		enemies->airstrikeTable[i]->hitbox->w = AIRSTRIKE_WIDTH;
		enemies->airstrikeTable[i]->hitbox->h = AIRSTRIKE_HEIGHT;
		enemies->airstrikeTable[i]->x = 0;
		enemies->airstrikeTable[i]->y = 0;
		enemies->airstrikeTable[i]->hitbox->x = 0;
		enemies->airstrikeTable[i]->hitbox->y = 0;
	}
}

void PreventBullets(game_player* player, game_enemies* enemies)
{
	for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
	{
		if (player->bulletsTable[i] != NULL)
		{
			if (player->bulletsTable[i]->y < -DISTANCE_TILL_PLAYER_BULLET_DISAPPEAR)
			{
				SDL_FreeSurface(player->bulletsTable[i]->sprite);
				free(player->bulletsTable[i]);
				player->bulletsTable[i] = NULL;
			}
		}
	}
	for (int j = 0; j < MAX_ENEMIES_AMOUNT; j++)
	{
		if (enemies->enemiesTable[j] != NULL)
		{
			for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
			{
				if (enemies->enemiesTable[j]->bulletsTable[i] != NULL)
				{
					if (enemies->enemiesTable[j]->bulletsTable[i]->y > DISTANCE_TILL_ENEMY_BULLET_DISAPPEAR || enemies->enemiesTable[j]->bulletsTable[i]->y < -DISTANCE_TILL_ENEMY_BULLET_DISAPPEAR || enemies->enemiesTable[j]->bulletsTable[i]->x > DISTANCE_TILL_ENEMY_BULLET_DISAPPEAR + SCREEN_WIDTH || enemies->enemiesTable[j]->bulletsTable[i]->x < -DISTANCE_TILL_ENEMY_BULLET_DISAPPEAR)
					{
						SDL_FreeSurface(enemies->enemiesTable[j]->bulletsTable[i]->sprite);
						free(enemies->enemiesTable[j]->bulletsTable[i]);
						enemies->enemiesTable[j]->bulletsTable[i] = NULL;
					}
				}
			}
		}
	}
}

void CountingTimeForEvents(game_player* player, game_enemies* enemies,double* delta, int* enemyShoot, double* timeTillEnemyShoot, int* movingEnemyShoot, double* timeTillMovingEnemyShoot, int* invincible, double* timeTillInvinicbilityOff, int* bonusPoints, double* timeTillLossBonusPoints, int* airstrike, double* timeTillAirstrike, int* airstrikeBlow, double* timeTillAirstrikeBlow)
{
	if (*enemyShoot == NO_SHOOT)
	{
		*timeTillEnemyShoot += *delta;
		if (*timeTillEnemyShoot > SHOOT_SPEED)
		{
			*enemyShoot = SHOOT;
			*timeTillEnemyShoot = 0;
		}
	}
	if (*movingEnemyShoot == MOVING_NO_SHOOT)
	{
		*timeTillMovingEnemyShoot += *delta;
		if (*timeTillMovingEnemyShoot > MOVING_SHOOT_SPEED)
		{
			*movingEnemyShoot = MOVING_SHOOT;
			*timeTillMovingEnemyShoot = 0;
		}
	}
	if (*invincible == INVINCIBLE_ON)
	{
		*timeTillInvinicbilityOff += *delta;
		if (*timeTillInvinicbilityOff > INVINCIBILITY_TIME)
		{
			*invincible = INVINCIBLE_OFF;
			*timeTillInvinicbilityOff = 0;
			player->sprite = SDL_LoadBMP("./graphics/player_base.bmp");
		}
	}
	if (*bonusPoints > 1)
	{
		*timeTillLossBonusPoints += *delta;
		if (*timeTillLossBonusPoints > BONUS_TIME)
		{
			*bonusPoints = 1;
			*timeTillLossBonusPoints = 0;
		}
	}
	if (*airstrike == AIRSTRIKE_PREPARE_ON)
	{
		*timeTillAirstrike += *delta;
		if (*timeTillAirstrike > AIRSTRIKE_PREPARE_TIME)
		{
			*airstrikeBlow = AIRSTRIKE_FIRE_ON;
			for (int i = 0; i < AIRSTRIKE_SIZE; i++)
			{
				enemies->airstrikeTable[i]->sprite = SDL_LoadBMP("./graphics/airstrike_blow.bmp");
			}
			*airstrike = AIRSTRIKE_PREPARE_OFF;
			*timeTillAirstrike = 0;
		}
	}
	if (*airstrikeBlow == AIRSTRIKE_FIRE_ON)
	{
		*timeTillAirstrikeBlow += *delta;
		if (*timeTillAirstrikeBlow > AIRSTRIKE_FIRE_TIME)
		{
			for (int i = 0; i < AIRSTRIKE_SIZE; i++)
			{
				enemies->airstrikeTable[i]->sprite = SDL_LoadBMP("./graphics/airstrike_mark.bmp");
			}
			*airstrikeBlow = AIRSTRIKE_FIRE_OFF;
			*timeTillAirstrikeBlow = 0;
		}
	}
	if (*airstrike == AIRSTRIKE_PREPARE_OFF && *airstrikeBlow == AIRSTRIKE_FIRE_OFF)
	{
		for (int i = 0; i < AIRSTRIKE_SIZE; i++)
		{
			enemies->airstrikeTable[i]->sprite = SDL_LoadBMP("./graphics/airstrike_mark.bmp");
		}
	}
}



void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset) // drawing strings
{
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};

void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) //drawind sprites
{ 
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};

void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) 
{
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
};

void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) // rysowanie linii
{
	for(int i = 0; i < l; i++) 
	{
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};

void DrawRectangle(SDL_Surface *screen, int x, int y, int width, int height, Uint32 outlineColor, Uint32 fillColor) // rysowanie prostokata
{
	int i;
	DrawLine(screen, x, y, height, 0, 1, outlineColor);
	DrawLine(screen, x + width - 1, y, height, 0, 1, outlineColor);
	DrawLine(screen, x, y, width, 1, 0, outlineColor);
	DrawLine(screen, x, y + height - 1, width, 1, 0, outlineColor);
	for(i = y + 1; i < y + height - 1; i++)
		DrawLine(screen, x + 1, i, width - 2, 1, 0, fillColor);
};

void DrawPlayerGrade(SDL_Surface* screen, game_player* player)
{
	if (player->score > 80)
	{
		player->grade = SDL_LoadBMP("./graphics/grade_s.bmp");
	}
	else if (player->score > 50)
	{
		player->grade = SDL_LoadBMP("./graphics/grade_a.bmp");
	}
	else if (player->score > 30)
	{
		player->grade = SDL_LoadBMP("./graphics/grade_b.bmp");
	}
	else if (player->score > 10)
	{
		player->grade = SDL_LoadBMP("./graphics/grade_c.bmp");
	}
	else
	{
		player->grade = SDL_LoadBMP("./graphics/grade_d.bmp");
	}
	DrawSurface(screen, player->grade, GAME_WIDTH + SCOREBOARD_WIDTH / 2, SCREEN_HEIGHT - (SCREEN_HEIGHT / 4));
}

void DrawScoreBoard(SDL_Surface* screen, SDL_Surface* charset, double worldTime, double fps, game_player* player, int enemiesLeft)
{
	char text[128];
	int black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	DrawRectangle(screen, SCREEN_WIDTH - SCOREBOARD_WIDTH, 4, SCOREBOARD_WIDTH - 4, SCREEN_HEIGHT - 8, red, blue);
	DrawRectangle(screen, SCREEN_WIDTH - SCOREBOARD_WIDTH / 2 - GRADE_SQUARE_SIZE / 2, SCREEN_HEIGHT - (SCREEN_HEIGHT / 4) - (GRADE_SQUARE_SIZE / 2), GRADE_SQUARE_SIZE, GRADE_SQUARE_SIZE, red, black);
	DrawPlayerGrade(screen, player);
	sprintf(text, "WARBLADE");
	DrawString(screen, GAME_WIDTH + SCOREBOARD_WIDTH / 2 - strlen(text) * 8 / 2, 15, text, charset);
	sprintf(text, "BY JAN KRAWCZYK");
	DrawString(screen, GAME_WIDTH + SCOREBOARD_WIDTH / 2 - strlen(text) * 8 / 2, 31, text, charset);
	sprintf(text, "CURRENT SCORE: %d", player->score);
	DrawString(screen, GAME_WIDTH + SCOREBOARD_WIDTH / 2 - strlen(text) * 8 / 2, 79, text, charset);
	sprintf(text, "ENEMIES LEFT: %d", enemiesLeft);
	DrawString(screen, GAME_WIDTH + SCOREBOARD_WIDTH / 2 - strlen(text) * 8 / 2, 127, text, charset);
	sprintf(text, "CURRENT LEVEL: %d", player->level);
	DrawString(screen, GAME_WIDTH + SCOREBOARD_WIDTH / 2 - strlen(text) * 8 / 2, 175, text, charset);
	sprintf(text, "CURRENT GAME TIME: %.1lf s", worldTime);
	DrawString(screen, GAME_WIDTH + SCOREBOARD_WIDTH / 2 - strlen(text) * 8 / 2, 223, text, charset);
	sprintf(text, "CURRENT RANK:");
	DrawString(screen, GAME_WIDTH + SCOREBOARD_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT - (SCREEN_HEIGHT / 4) - (GRADE_SQUARE_SIZE / 2) - 16, text, charset);
	sprintf(text, "FRAMES PER SECOND: %.1f", fps);
	DrawString(screen, GAME_WIDTH + SCOREBOARD_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT - 20, text, charset);
}

void DrawEnemies(SDL_Surface* screen, game_enemies* enemies)
{
	for (int i = 0; i < MAX_ENEMIES_AMOUNT; i++)
	{
		if (enemies->enemiesTable[i] != NULL)
		{
			DrawSurface(screen, enemies->enemiesTable[i]->sprite, enemies->enemiesTable[i]->x, enemies->enemiesTable[i]->y);
		}
	}
}

void DrawPlayerHearts(SDL_Surface* screen, game_player* player)
{
	int index = 0;
	while (index < player->health)
	{
		DrawSurface(screen, player->hearts[index], GAME_WIDTH + SCOREBOARD_WIDTH / 2 - HEART_WIDTH + HEART_WIDTH * index, 271);
		index++;
	}
}

void DrawHeartsOnGround(SDL_Surface* screen, game_player* player)
{
	for (int i = 0; i < MAX_HEARTS_ON_GROUND; i++)
	{
		if (player->heartsTable[i] != NULL)
		{
			DrawSurface(screen, player->heartsTable[i]->sprite, player->heartsTable[i]->x, player->heartsTable[i]->y);
		}
	}
}

void DrawBullets(SDL_Surface* screen, game_player* player, game_enemies* enemies)
{
	for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
	{
		if (player->bulletsTable[i] != NULL)
		{
			DrawSurface(screen, player->bulletsTable[i]->sprite, player->bulletsTable[i]->x, player->bulletsTable[i]->y);
		}
	}
	for (int j = 0; j < MAX_ENEMIES_AMOUNT; j++)
	{
		if (enemies->enemiesTable[j] != NULL)
		{
			for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
			{
				if (enemies->enemiesTable[j]->bulletsTable[i] != NULL)
				{
					DrawSurface(screen, enemies->enemiesTable[j]->bulletsTable[i]->sprite, enemies->enemiesTable[j]->bulletsTable[i]->x, enemies->enemiesTable[j]->bulletsTable[i]->y);
				}
			}
		}
	}
}

void DrawAirstrike(SDL_Surface* screen, game_enemies* enemies, int* airstrike, int* airstrikeBlow)
{
	if (*airstrike == AIRSTRIKE_PREPARE_ON || *airstrikeBlow == AIRSTRIKE_FIRE_ON)
	{
		for (int i = 0; i < AIRSTRIKE_SIZE; i++)
		{
			DrawSurface(screen, enemies->airstrikeTable[i]->sprite, enemies->airstrikeTable[i]->x, enemies->airstrikeTable[i]->y);
		}
	}
}

void DrawGameOverMenu(SDL_Surface* screen, game_player* player, game_enemies* enemies, int* menu, int* levelMenu, int* gameOver, SDL_Renderer* renderer, SDL_Texture* scrtex, int* quit, SDL_Surface* charset, double* worldTime)
{
	SDL_Event event;
	char text[128];
	int red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	if (player->health == 0)
	{
		*gameOver = OPEN_GAME_OVER;
	}
	while (*gameOver == OPEN_GAME_OVER)
	{
		DrawRectangle(screen, (SCREEN_WIDTH - MENU_WIDTH) / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2, MENU_WIDTH, MENU_HEIGHT, red, blue);
		sprintf(text, "GAME OVER");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 20, text, charset);
		sprintf(text, "YOUR SCORE: %d", player->score);
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 46, text, charset);
		sprintf(text, "1 - GO BACK TO MENU");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 62, text, charset);
		sprintf(text, "ESC - EXIT THE GAME");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 78, text, charset);
		sprintf(text, "YOUR GRADE:");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 94, text, charset);
		DrawSurface(screen, player->grade, SCREEN_WIDTH / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 110 + GRADE_SQUARE_SIZE / 2);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					*quit = 1;
					*menu = CLOSE_MENU;
					*gameOver = CLOSE_GAME_OVER;
				}
				else if (event.key.keysym.sym == SDLK_1)
				{
					*menu = OPEN_MENU;
					*gameOver = CLOSE_GAME_OVER;
				}
				break;
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				*quit = 1;
				*menu = CLOSE_MENU;
				*gameOver = CLOSE_GAME_OVER;
				break;
			};
		};
	}
}

void DrawLevelSelector(SDL_Surface* screen, game_player* player, game_enemies* enemies, int* menu, int* levelMenu, SDL_Renderer* renderer, SDL_Texture* scrtex, int* quit, SDL_Surface* charset, double* worldTime)
{
	SDL_Event event;
	char text[128];
	int red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	while (*levelMenu == OPEN_LEVEL_MENU)
	{
		DrawRectangle(screen, (SCREEN_WIDTH - MENU_WIDTH) / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2, MENU_WIDTH, MENU_HEIGHT, red, blue);
		sprintf(text, "MENU");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 20, text, charset);
		sprintf(text, "1 - LEVEL 1");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 68, text, charset);
		sprintf(text, "2 - LEVEL 2");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 84, text, charset);
		sprintf(text, "3 - LEVEL 3");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 100, text, charset);
		sprintf(text, "4 - BACK TO MENU");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 116, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					*quit = 1;
					*menu = CLOSE_MENU;
					*levelMenu = CLOSE_LEVEL_MENU;
				}
				else if (event.key.keysym.sym == SDLK_1)
				{
					PreparingPlayerAndEnemies(player, enemies);
					*worldTime = 0;
					player->level = 0;
					*levelMenu = CLOSE_LEVEL_MENU;
				}
				else if (event.key.keysym.sym == SDLK_2)
				{
					*worldTime = 0;
					PreparingPlayerAndEnemies(player, enemies);
					player->level = 1;
					*levelMenu = CLOSE_LEVEL_MENU;
				}
				else if (event.key.keysym.sym == SDLK_3)
				{
					*worldTime = 0;
					PreparingPlayerAndEnemies(player, enemies);
					player->level = 2;
					*levelMenu = CLOSE_LEVEL_MENU;
				}
				else if (event.key.keysym.sym == SDLK_4)
				{
					*menu = OPEN_MENU;
					*levelMenu = CLOSE_LEVEL_MENU;
				}
				break;
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				*quit = 1;
				*menu = CLOSE_MENU;
				*levelMenu = CLOSE_LEVEL_MENU;
				break;
			};
		};
	}
}

void DrawMenuBase(SDL_Surface* screen, game_player* player, game_enemies* enemies, int* menu, int* levelMenu, SDL_Renderer* renderer, SDL_Texture* scrtex, int* quit, SDL_Surface* charset, double* worldTime)
{
	SDL_Event event;
	char text[128];
	int red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	while (*menu == OPEN_MENU)
	{
		DrawRectangle(screen, (SCREEN_WIDTH - MENU_WIDTH) / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2, MENU_WIDTH, MENU_HEIGHT, red, blue);
		sprintf(text, "MENU");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 20, text, charset);
		sprintf(text, "1 - NEW GAME");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 68, text, charset);
		sprintf(text, "2 - CONTINUE");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 84, text, charset);
		sprintf(text, "3 - LEVEL SELECTOR");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 100, text, charset);
		sprintf(text, "ESC - EXIT THE GAME");
		DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT - MENU_HEIGHT) / 2 + 116, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					*quit = 1;
					*menu = CLOSE_MENU;
				}
				else if (event.key.keysym.sym == SDLK_1)
				{
					PreparingPlayerAndEnemies(player, enemies);
					*menu = CLOSE_MENU;
				}
				else if (event.key.keysym.sym == SDLK_2)
				{
					*menu = CLOSE_MENU;
				}
				else if (event.key.keysym.sym == SDLK_3)
				{
					*levelMenu = OPEN_LEVEL_MENU;
					*menu = CLOSE_MENU;
				}
				break;
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				*quit = 1;
				*menu = CLOSE_MENU;
				break;
			};
		};
	}
}

void DrawAllMenu(SDL_Surface* screen, game_player* player, game_enemies* enemies, int* menu, int* levelMenu, int* gameOver, SDL_Renderer* renderer, SDL_Texture* scrtex, int* quit, SDL_Surface* charset, double* worldTime)
{
	DrawGameOverMenu(screen, player, enemies, menu, levelMenu, gameOver, renderer, scrtex, quit, charset, worldTime);
	DrawMenuBase(screen, player, enemies, menu, levelMenu, renderer, scrtex, quit, charset, worldTime);
	DrawLevelSelector(screen, player, enemies, menu, levelMenu, renderer, scrtex, quit, charset, worldTime);
}

void DrawGame(SDL_Surface* screen, SDL_Surface* charset, SDL_Surface* background, game_player* player, game_enemies* enemies, double worldTime, double fps, int* airstrike, int* airstrikeBlow)
{
	DrawSurface(screen, background, player->x, player->y); // drawing background
	DrawEnemies(screen, enemies); // drawing enemies
	DrawBullets(screen, player, enemies);
	DrawHeartsOnGround(screen, player);
	DrawAirstrike(screen, enemies, airstrike, airstrikeBlow);
	DrawSurface(screen, player->sprite, GAME_WIDTH / 2, SCREEN_HEIGHT - (SCREEN_HEIGHT / 6)); // drawing player on window
	DrawScoreBoard(screen, charset, worldTime, fps, player, enemies->amount); // drawing scoreboard
	DrawPlayerHearts(screen, player);
}


void MovingBulletsOnPlayerMove(int moveDirectionX, int moveDirectionY, game_player* player, game_enemies* enemies)
{
	for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
	{
		if (player->bulletsTable[i] != NULL)
		{
			player->bulletsTable[i]->x += moveDirectionX * PLAYER_SPEED;
			player->bulletsTable[i]->y += moveDirectionY * PLAYER_SPEED;
			player->bulletsTable[i]->hitbox->x += moveDirectionX * PLAYER_SPEED;
			player->bulletsTable[i]->hitbox->y += moveDirectionY * PLAYER_SPEED;
		}
	}
	for (int j = 0; j < MAX_ENEMIES_AMOUNT; j++)
	{
		if (enemies->enemiesTable[j] != NULL)
		{
			for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
			{
				if (enemies->enemiesTable[j]->bulletsTable[i] != NULL)
				{
					enemies->enemiesTable[j]->bulletsTable[i]->x += moveDirectionX * PLAYER_SPEED;
					enemies->enemiesTable[j]->bulletsTable[i]->y += moveDirectionY * PLAYER_SPEED;
					enemies->enemiesTable[j]->bulletsTable[i]->hitbox->x += moveDirectionX * PLAYER_SPEED;
					enemies->enemiesTable[j]->bulletsTable[i]->hitbox->y += moveDirectionY * PLAYER_SPEED;
				}
			}
		}
	}
}

void MovingHeartsOnPlayerMove(int moveDirectionX, int moveDirectionY, game_player* player)
{
	for (int i = 0; i < MAX_HEARTS_ON_GROUND; i++)
	{
		if (player->heartsTable[i] != NULL)
		{
			player->heartsTable[i]->x += moveDirectionX * PLAYER_SPEED;
			player->heartsTable[i]->y += moveDirectionY * PLAYER_SPEED;
			player->heartsTable[i]->hitbox->x += moveDirectionX * PLAYER_SPEED;
			player->heartsTable[i]->hitbox->y += moveDirectionY * PLAYER_SPEED;
		}
	}
}

void MovingEnemiesOnPlayerMove(int moveDirectionX, int moveDirectionY, game_player* player, game_enemies* enemies)
{
	for (int i = 0; i < MAX_ENEMIES_AMOUNT; i++)
	{
		if (enemies->enemiesTable[i] != NULL)
		{
			enemies->enemiesTable[i]->x += moveDirectionX * PLAYER_SPEED;
			enemies->enemiesTable[i]->y += moveDirectionY * PLAYER_SPEED;
			enemies->enemiesTable[i]->hitbox->x += moveDirectionX * PLAYER_SPEED;
			enemies->enemiesTable[i]->hitbox->y += moveDirectionY * PLAYER_SPEED;
		}
	}
}

void MovingAirstrikeOnPlayerMove(int moveDirectionX, int moveDirectionY, game_player* player, game_enemies* enemies)
{
	for (int i = 0; i < AIRSTRIKE_SIZE; i++)
	{
		enemies->airstrikeTable[i]->x += moveDirectionX * PLAYER_SPEED;
		enemies->airstrikeTable[i]->y += moveDirectionY * PLAYER_SPEED;
		enemies->airstrikeTable[i]->hitbox->x += moveDirectionX * PLAYER_SPEED;
		enemies->airstrikeTable[i]->hitbox->y += moveDirectionY * PLAYER_SPEED;
	}
}

void PlayerMove(int moveDirectionX, int moveDirectionY, int graphic, game_player* player, game_enemies* enemies, int invincible)
{
	player->x += moveDirectionX * PLAYER_SPEED;
	player->y += moveDirectionY * PLAYER_SPEED;
	if (graphic == NOT_BACKWARD)
	{
		if (invincible == INVINCIBLE_ON)
		{
			player->sprite = SDL_LoadBMP("./graphics/player_move_invincible.bmp");
		}
		else
		{
			player->sprite = SDL_LoadBMP("./graphics/player_move.bmp");
		}
	}
	else if (graphic == BACKWARD)
	{
		if (invincible == INVINCIBLE_ON)
		{
			player->sprite = SDL_LoadBMP("./graphics/player_move_backward_invincible.bmp");
		}
		else
		{
			player->sprite = SDL_LoadBMP("./graphics/player_move_backward.bmp");
		}
		
	}
	MovingHeartsOnPlayerMove(moveDirectionX, moveDirectionY, player);
	MovingBulletsOnPlayerMove(moveDirectionX, moveDirectionY, player, enemies);
	MovingEnemiesOnPlayerMove(moveDirectionX, moveDirectionY, player, enemies);
	MovingAirstrikeOnPlayerMove(moveDirectionX, moveDirectionY, player, enemies);
}

void PlayerStopMove(game_player* player, int invincible)
{
	if (invincible == INVINCIBLE_ON)
	{
		player->sprite = SDL_LoadBMP("./graphics/player_base_invincible.bmp");
	}
	else
	{
		player->sprite = SDL_LoadBMP("./graphics/player_base.bmp");
	}
}

void MoveMovingEnemies(game_enemies* enemies)
{
	for (int i = 0; i < MAX_ENEMIES_AMOUNT; i++)
	{
		if (enemies->enemiesTable[i] != NULL)
		{
			if (enemies->enemiesTable[i]->type == MOVING)
			{
				enemies->enemiesTable[i]->x += enemies->enemiesTable[i]->direction * MOVING_ENEMY_SPEED;
				enemies->enemiesTable[i]->hitbox->x += enemies->enemiesTable[i]->direction * MOVING_ENEMY_SPEED;
				enemies->enemiesTable[i]->distance += enemies->enemiesTable[i]->direction * MOVING_ENEMY_SPEED;
				if (enemies->enemiesTable[i]->distance > 200)
				{
					enemies->enemiesTable[i]->direction = LEFT_MOVE;
				}
				else if (enemies->enemiesTable[i]->distance < -200)
				{
					enemies->enemiesTable[i]->direction = RIGHT_MOVE;
				}
			}

		}
	}
}

void BulletFly(game_player* player, game_enemies* enemies)
{
	for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
	{
		if (player->bulletsTable[i] != NULL)
		{
			player->bulletsTable[i]->y += player->bulletsTable[i]->directionY * BULLET_SPEED;
			player->bulletsTable[i]->hitbox->y += player->bulletsTable[i]->directionY * BULLET_SPEED;
		}
	}
	for (int j = 0; j < MAX_ENEMIES_AMOUNT; j++)
	{
		if (enemies->enemiesTable[j] != NULL)
		{
			for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
			{
				if (enemies->enemiesTable[j]->bulletsTable[i] != NULL)
				{
					enemies->enemiesTable[j]->bulletsTable[i]->y += enemies->enemiesTable[j]->bulletsTable[i]->directionY * BULLET_SPEED;
					enemies->enemiesTable[j]->bulletsTable[i]->hitbox->y += enemies->enemiesTable[j]->bulletsTable[i]->directionY * BULLET_SPEED;
					enemies->enemiesTable[j]->bulletsTable[i]->x += enemies->enemiesTable[j]->bulletsTable[i]->directionX * BULLET_SPEED;
					enemies->enemiesTable[j]->bulletsTable[i]->hitbox->x += enemies->enemiesTable[j]->bulletsTable[i]->directionX * BULLET_SPEED;
				}
			}
		}
	}
}


void CreateEnemy(game_enemies* enemies, int enemyType)
{
	int index = 0;
	while (enemies->enemiesTable[index] != NULL)
	{
		index++;
	}
	enemies->enemiesTable[index] = (game_enemy*)malloc(sizeof(game_enemy));
	enemies->enemiesTable[index]->hitbox = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	enemies->enemiesTable[index]->x = RandomNumberInRange(LEFT + 200, RIGHT - 200);
	enemies->enemiesTable[index]->y = RandomNumberInRange(TOP + 200, 100);
	for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
	{
		enemies->enemiesTable[index]->bulletsTable[i] = NULL;
	}
	enemies->amount++;
	if (enemyType == COMMON)
	{
		enemies->enemiesTable[index]->type = COMMON;
		enemies->enemiesTable[index]->sprite = SDL_LoadBMP("./graphics/enemy_base.bmp");
		enemies->enemiesTable[index]->hitbox->x = enemies->enemiesTable[index]->x - BASIC_ENEMY_WIDTH / 2;
		enemies->enemiesTable[index]->hitbox->y = enemies->enemiesTable[index]->y - BASIC_ENEMY_HEIGHT / 2;
		enemies->enemiesTable[index]->hitbox->w = BASIC_ENEMY_WIDTH;
		enemies->enemiesTable[index]->hitbox->h = BASIC_ENEMY_HEIGHT;
	}
	else if (enemyType == MOVING)
	{
		enemies->enemiesTable[index]->type = MOVING;
		enemies->enemiesTable[index]->sprite = SDL_LoadBMP("./graphics/moving_enemy_base.bmp");
		enemies->enemiesTable[index]->hitbox->x = enemies->enemiesTable[index]->x - MOVING_ENEMY_WIDTH / 2;
		enemies->enemiesTable[index]->hitbox->y = enemies->enemiesTable[index]->y - MOVING_ENEMY_HEIGHT / 2;
		enemies->enemiesTable[index]->hitbox->w = MOVING_ENEMY_WIDTH;
		enemies->enemiesTable[index]->hitbox->h = MOVING_ENEMY_HEIGHT;
		enemies->enemiesTable[index]->direction = LEFT_MOVE;
		enemies->enemiesTable[index]->distance = 0;
	}
	else if (enemyType == ROUND)
	{
		enemies->enemiesTable[index]->type = ROUND;
		enemies->enemiesTable[index]->sprite = SDL_LoadBMP("./graphics/round_enemy_base.bmp");
		enemies->enemiesTable[index]->hitbox->x = enemies->enemiesTable[index]->x - ROUND_ENEMY_WIDTH / 2;
		enemies->enemiesTable[index]->hitbox->y = enemies->enemiesTable[index]->y - ROUND_ENEMY_HEIGHT / 2;
		enemies->enemiesTable[index]->hitbox->w = ROUND_ENEMY_WIDTH;
		enemies->enemiesTable[index]->hitbox->h = ROUND_ENEMY_HEIGHT;
	}
}

void CreateEnemies(game_enemies* enemies, int enemyNumber, int movingEnemyNumber, int roundEnemyNumber)
{
	for (int i = 0; i < enemyNumber; i++)
	{
		CreateEnemy(enemies, COMMON);
	}
	for (int i = 0; i < movingEnemyNumber; i++)
	{
		CreateEnemy(enemies, MOVING);
	}
	for (int i = 0; i < roundEnemyNumber; i++)
	{
		CreateEnemy(enemies, ROUND);
	}
}

void CreateBulletForPlayer(game_player* player)
{
	int index = 0;
	while (player->bulletsTable[index] != NULL)
	{
		index++;
	}
	player->bulletsTable[index] = (game_bullet*)malloc(sizeof(game_bullet));
	player->bulletsTable[index]->sprite = SDL_LoadBMP("./graphics/bullet.bmp");
	player->bulletsTable[index]->x = GAME_WIDTH/2;
	player->bulletsTable[index]->y = SCREEN_HEIGHT - SCREEN_HEIGHT/6;
	player->bulletsTable[index]->directionY = UP;
	player->bulletsTable[index]->hitbox = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	player->bulletsTable[index]->hitbox->x = player->bulletsTable[index]->x - BULLET_WIDHT / 2;
	player->bulletsTable[index]->hitbox->y = player->bulletsTable[index]->y - BULLET_HEIGHT / 2;
	player->bulletsTable[index]->hitbox->w = BULLET_WIDHT;
	player->bulletsTable[index]->hitbox->h = BULLET_HEIGHT;
}

void CreateBulletForEnemy(game_enemy* enemy)
{
	int index = 0;
	while (enemy->bulletsTable[index] != NULL)
	{
		index++;
	}
	enemy->bulletsTable[index] = (game_bullet*)malloc(sizeof(game_bullet));
	enemy->bulletsTable[index]->sprite = SDL_LoadBMP("./graphics/bullet.bmp");
	enemy->bulletsTable[index]->x = enemy->x;
	enemy->bulletsTable[index]->y = enemy->y;
	enemy->bulletsTable[index]->directionY = DOWN;
	enemy->bulletsTable[index]->directionX = 0;
	enemy->bulletsTable[index]->hitbox = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	enemy->bulletsTable[index]->hitbox->x = enemy->bulletsTable[index]->x - BULLET_WIDHT - 2;
	enemy->bulletsTable[index]->hitbox->y = enemy->bulletsTable[index]->y - BULLET_HEIGHT - 2;
	enemy->bulletsTable[index]->hitbox->w = BULLET_WIDHT;
	enemy->bulletsTable[index]->hitbox->h = BULLET_HEIGHT;
}

void CreateBulletForMovingEnemy(game_enemy* enemy)
{
	int index = 0;
 	while (enemy->bulletsTable[index] != NULL)
	{
		index++;
	}
	enemy->bulletsTable[index] = (game_bullet*)malloc(sizeof(game_bullet));
	enemy->bulletsTable[index]->sprite = SDL_LoadBMP("./graphics/moving_enemy_bullet.bmp");
	enemy->bulletsTable[index]->x = enemy->x;
	enemy->bulletsTable[index]->y = enemy->y;
	enemy->bulletsTable[index]->directionY = DOWN;
	enemy->bulletsTable[index]->directionX = 0;
	enemy->bulletsTable[index]->hitbox = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	enemy->bulletsTable[index]->hitbox->x = enemy->bulletsTable[index]->x - BIG_BULLET_WIDTH - 2;
	enemy->bulletsTable[index]->hitbox->y = enemy->bulletsTable[index]->y - BIG_BULLET_HEIGHT - 2;
	enemy->bulletsTable[index]->hitbox->w = BIG_BULLET_WIDTH;
	enemy->bulletsTable[index]->hitbox->h = BIG_BULLET_HEIGHT;
}

void CreateBulletForRoundEnemy(game_enemy* enemy, int directionX, int directionY)
{
	int index = 0;
	while (enemy->bulletsTable[index] != NULL)
	{
		index++;
	}
	enemy->bulletsTable[index] = (game_bullet*)malloc(sizeof(game_bullet));
	enemy->bulletsTable[index]->sprite = SDL_LoadBMP("./graphics/round_enemy_bullet.bmp");
	enemy->bulletsTable[index]->x = enemy->x;
	enemy->bulletsTable[index]->y = enemy->y;
	enemy->bulletsTable[index]->directionY = directionY;
	enemy->bulletsTable[index]->directionX = directionX;
	enemy->bulletsTable[index]->hitbox = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	enemy->bulletsTable[index]->hitbox->x = enemy->bulletsTable[index]->x - ROUND_BULLET_WIDTH - 2;
	enemy->bulletsTable[index]->hitbox->y = enemy->bulletsTable[index]->y - ROUND_BULLET_HEIGHT - 2;
	enemy->bulletsTable[index]->hitbox->w = ROUND_BULLET_WIDTH;
	enemy->bulletsTable[index]->hitbox->h = ROUND_BULLET_HEIGHT;
}

void CreateBulletsForRoundEnemy(game_enemy* enemy)
{
	CreateBulletForRoundEnemy(enemy, 2, 0);
	CreateBulletForRoundEnemy(enemy, -2, 0);
	CreateBulletForRoundEnemy(enemy, 1, 2);
	CreateBulletForRoundEnemy(enemy, -1, 2);
	CreateBulletForRoundEnemy(enemy, 1, -2);
	CreateBulletForRoundEnemy(enemy, -1, -2);
	CreateBulletForRoundEnemy(enemy, 2, 1);
	CreateBulletForRoundEnemy(enemy, -2, 1);
	CreateBulletForRoundEnemy(enemy, 2, -1);
	CreateBulletForRoundEnemy(enemy, -2, -1);
	CreateBulletForRoundEnemy(enemy, 0, 2);
	CreateBulletForRoundEnemy(enemy, 0, -2);
	CreateBulletForRoundEnemy(enemy, 2, 2);
	CreateBulletForRoundEnemy(enemy, -2, 2);
	CreateBulletForRoundEnemy(enemy, 2, -2);
	CreateBulletForRoundEnemy(enemy, -2, -2);
		
}

void EnemyFire(game_enemies* enemies, int* enemyShoot, int*movingEnemyShoot)
{
	if (*enemyShoot == SHOOT)
	{
		for (int i = 0; i < MAX_ENEMIES_AMOUNT; i++)
		{
			if (enemies->enemiesTable[i] != NULL)
			{
				if (enemies->enemiesTable[i]->type == COMMON)
				{
					CreateBulletForEnemy(enemies->enemiesTable[i]);
				}
			}
		}
		*enemyShoot = NO_SHOOT;
	}
	if (*movingEnemyShoot == MOVING_SHOOT)
	{
		for (int i = 0; i < MAX_ENEMIES_AMOUNT; i++)
		{
			if (enemies->enemiesTable[i] != NULL)
			{
				if (enemies->enemiesTable[i]->type == MOVING)
				{
					CreateBulletForMovingEnemy(enemies->enemiesTable[i]);
				}
				else if (enemies->enemiesTable[i]->type == ROUND)
				{
					CreateBulletsForRoundEnemy(enemies->enemiesTable[i]);
				}
			}
		}
		*movingEnemyShoot = MOVING_NO_SHOOT;
	}
}

void CreateHeart(game_player* player)
{
	int index = 0;
	while (player->heartsTable[index] != NULL)
	{
		index++;
	}
	player->heartsTable[index] = (game_heart*)malloc(sizeof(game_heart));
	player->heartsTable[index]->sprite = SDL_LoadBMP("./graphics/heart2.bmp");
	player->heartsTable[index]->x = RandomNumberInRange(LEFT + 100, RIGHT - 100);
	player->heartsTable[index]->y = RandomNumberInRange(0, 200);
	player->heartsTable[index]->hitbox = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	player->heartsTable[index]->hitbox->x = player->heartsTable[index]->x - HEART_WIDTH / 2;
	player->heartsTable[index]->hitbox->y = player->heartsTable[index]->y - HEART_HEIGHT / 2;
	player->heartsTable[index]->hitbox->w = HEART_WIDTH;
	player->heartsTable[index]->hitbox->h = HEART_HEIGHT;
}

void CreateAirstrike(game_enemies* enemies)
{
	for (int i = 0; i < AIRSTRIKE_SIZE; i++)
	{
		enemies->airstrikeTable[i]->x = RandomNumberInRange(-100, SCREEN_WIDTH + 100);
		enemies->airstrikeTable[i]->y = RandomNumberInRange(-100, SCREEN_HEIGHT + 100);
		enemies->airstrikeTable[i]->hitbox->x = enemies->airstrikeTable[i]->x;
		enemies->airstrikeTable[i]->hitbox->y = enemies->airstrikeTable[i]->y;
	}
}


void DetectHitOnEnemy(game_enemies* enemies, game_player* player, int* bonusPoints)
{
	for (int j = 0; j < MAX_ENEMIES_AMOUNT; j++)
	{
		if (enemies->enemiesTable[j] != NULL)
		{
			for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
			{
				if (player->bulletsTable[i] != NULL)
				{
					if (SDL_HasIntersection(enemies->enemiesTable[j]->hitbox, player->bulletsTable[i]->hitbox))
					{
						player->score += POINTS_GAIN_ON_HIT * *bonusPoints;
						*bonusPoints += 1;
						enemies->amount--;
						SDL_FreeSurface(enemies->enemiesTable[j]->sprite);
						free(enemies->enemiesTable[j]);
						enemies->enemiesTable[j] = NULL;
						SDL_FreeSurface(player->bulletsTable[i]->sprite);
						free(player->bulletsTable[i]);
						player->bulletsTable[i] = NULL;
						break;
					}
				}
			}
		}
	}
}

void DetectHitOnPlayer(game_enemies* enemies, game_player* player, int* invincible)
{
	if (*invincible == INVINCIBLE_OFF)
	{
		for (int j = 0; j < MAX_ENEMIES_AMOUNT; j++)
		{
			if (enemies->enemiesTable[j] != NULL)
			{
				for (int i = 0; i < MAX_BULLETS_PER_SHIP; i++)
				{
					if (enemies->enemiesTable[j]->bulletsTable[i] != NULL)
					{
						if (SDL_HasIntersection(enemies->enemiesTable[j]->bulletsTable[i]->hitbox, player->hitbox))
						{
							player->health--;
							player->score -= POINTS_LOST_ON_HIT;
							SDL_FreeSurface(enemies->enemiesTable[j]->bulletsTable[i]->sprite);
							free(enemies->enemiesTable[j]->bulletsTable[i]->hitbox);
							free(enemies->enemiesTable[j]->bulletsTable[i]);
							enemies->enemiesTable[j]->bulletsTable[i] = NULL;
							*invincible = INVINCIBLE_ON;
							break;
						}
					}
				}
			}
		}
	}
}

void DetectHitOnHeart(game_player* player, game_enemies* enemies, int* airstrike)
{
	if (player->health < MAX_PLAYER_HEALTH)
	{
		for (int i = 0; i < MAX_HEARTS_ON_GROUND; i++)
		{
			if (player->heartsTable[i] != NULL)
			{
				if (SDL_HasIntersection(player->heartsTable[i]->hitbox, player->hitbox))
				{
					player->health++;
					player->score += POINTS_GAIN_ON_COLLECTING_HEART;
					SDL_FreeSurface(player->heartsTable[i]->sprite);
					free(player->heartsTable[i]->hitbox);
					free(player->heartsTable[i]);
					player->heartsTable[i] = NULL;
					CreateAirstrike(enemies);
					*airstrike = AIRSTRIKE_PREPARE_ON;
					break;
				}
			}
		}
	}
}

void DetectHitOnAirstrike(game_player* player, game_enemies* enemies, int* airstrikeBlow, int* invincible)
{
	if (*airstrikeBlow == AIRSTRIKE_FIRE_ON)
	{
		for (int i = 0; i < AIRSTRIKE_SIZE; i++)
		{
			if (SDL_HasIntersection(player->hitbox, enemies->airstrikeTable[i]->hitbox))
			{
				player->score -= POINTS_LOST_ON_HIT;
				player->health -= 1;
				*invincible = INVINCIBLE_ON;
				*airstrikeBlow = AIRSTRIKE_FIRE_OFF;
				break;
			}
		}
	}
}

void DetectCollision(game_enemies* enemies, game_player* player, int* bonusPoints, int* invincible, int* airstrike, int* airstrikeBlow)
{
	DetectHitOnEnemy(enemies, player, bonusPoints);
	DetectHitOnPlayer(enemies, player, invincible);
	DetectHitOnHeart(player, enemies, airstrike);
	DetectHitOnAirstrike(player, enemies, airstrikeBlow, invincible);
}


void SpawningEnemies(game_player* player, game_enemies* enemies)
{
	if (enemies->amount == 0)
	{
		player->level++;
		player->x = (LEFT + RIGHT) / 2;
		player->y = (TOP + BOTTOM) / 2;
		if (player->level == 1)
		{
			CreateEnemies(enemies, 1, 0, 0);
		}
		else if (player->level == 2)
		{
			CreateEnemies(enemies, 0, 1, 0);
		}
		/*else if (player->level == 3)
		{
			CreateEnemies(enemies, 0, 0, 1);
		}*/
		else
		{
			CreateEnemies(enemies, 10 + player->level, 1, 2);
		}
		CreateHeart(player);
	}
}

void HandelingBullets(game_player* player, game_enemies* enemies, int* enemyShoot, int* movingEnemyShoot)
{
	BulletFly(player, enemies);
	EnemyFire(enemies, enemyShoot, movingEnemyShoot);
	PreventBullets(player, enemies);
	MoveMovingEnemies(enemies);
}



#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	// time and fps
	int timeAtStart = SDL_GetTicks(), quit = 0, frames = 0, frameTime2 = SDL_GetTicks(), frameTime = SDL_GetTicks(); 
	double worldTime = 0, fpsTimer = 0, fps = 0; 
	int enemyShoot = SHOOT, movingEnemyShoot = MOVING_SHOOT, invincible = INVINCIBLE_OFF, bonusPoints = 1, menu = OPEN_MENU, levelMenu = CLOSE_LEVEL_MENU, gameOver = CLOSE_GAME_OVER, airstrike = AIRSTRIKE_PREPARE_OFF, airstrikeBlow = AIRSTRIKE_FIRE_OFF;
	double timeTillEnemyShoot = 0, delta = 0, timeTillInvinicbilityOff = 0, timeTillLossBonusPoints = 0, timeTillMovingEnemyShoot = 0, timeTillairstrike = 0 , timeTillairstrikeBlow = 0;

	//SDL elements
	SDL_Event event;
	SDL_Surface* screen; // screen
	SDL_Surface* charset; //characters and letters
	SDL_Texture* scrtex; // a structure that contains an efficient, driver - specific representation of pixel data
	SDL_Window* window; // window
	SDL_Renderer* renderer; // a structure that contains a rendering state;
	SDL_Surface* background; // backgound texture
	
	//player and enemies
	game_player player;
	game_enemies enemies;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}
	if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer) != 0)
	{
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};
	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	//loading bitmaps
	charset = SDL_LoadBMP("./graphics/cs8x8.bmp");
	background = SDL_LoadBMP("./graphics/background.bmp");

	//creating level
	PreparingPlayerAndEnemies(&player, &enemies);
	BasicPreparationsAndSettings(renderer, window, charset);
	
	while(!quit) {
		//counting time, limitating and checking fps
		frameTime2 = SDL_GetTicks();
		CountingTimeAndFps(&worldTime, &fps, &timeAtStart, &frames, &fpsTimer, &delta, &menu);
		CountingTimeForEvents(&player, &enemies, &delta, &enemyShoot, &timeTillEnemyShoot, &movingEnemyShoot, &timeTillMovingEnemyShoot, &invincible, &timeTillInvinicbilityOff, &bonusPoints, &timeTillLossBonusPoints, &airstrike, &timeTillairstrikeBlow, &airstrikeBlow, &timeTillairstrikeBlow);

		//drawing all elements
		DrawGame(screen, charset, background, &player, &enemies, worldTime, fps, &airstrike, &airstrikeBlow);
		DrawAllMenu(screen, &player, &enemies, &menu, &levelMenu, &gameOver, renderer, scrtex, &quit, charset, &worldTime);
		SpawningEnemies(&player, &enemies);

		//updating textures and renderer
		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		HandelingBullets(&player, &enemies, &enemyShoot, &movingEnemyShoot);
		DetectCollision(&enemies, &player, &bonusPoints, &invincible, &airstrike, &airstrikeBlow);

		// handling of events
		while(SDL_PollEvent(&event)) 
		{
			switch(event.type) 
			{
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = 1;
					}
					else if (event.key.keysym.sym == SDLK_UP && -TOP - PLAYER_SPEED + SCREEN_HEIGHT/3 > player.y) // moving up
					{
						PlayerMove(0, 1, NOT_BACKWARD, &player, &enemies, invincible);
					}
					else if (event.key.keysym.sym == SDLK_DOWN && -BOTTOM + PLAYER_SPEED + SCREEN_HEIGHT/3< player.y) // moving down
					{
						PlayerMove(0, -1, BACKWARD, &player, &enemies, invincible);
					}
					else if (event.key.keysym.sym == SDLK_RIGHT && -RIGHT + PLAYER_SPEED + SCREEN_WIDTH/2 + SCREEN_WIDTH/4< player.x) // moving right
					{
						PlayerMove(-1, 0, NOT_BACKWARD, &player, &enemies, invincible);
					}
					else if (event.key.keysym.sym == SDLK_LEFT && -LEFT - PLAYER_SPEED + SCREEN_WIDTH/2 + SCREEN_WIDTH/4> player.x) // moving left
					{
						PlayerMove(1, 0, NOT_BACKWARD, &player, &enemies, invincible);
					}
					else if (event.key.keysym.sym == SDLK_SPACE) // shooting
					{
						CreateBulletForPlayer(&player);
					}
					else if (event.key.keysym.sym == SDLK_n) // starting new game
					{
						PreparingPlayerAndEnemies(&player, &enemies);
						worldTime = 0;
					}
					else if (event.key.keysym.sym == SDLK_m) // open menu
					{
						menu = OPEN_MENU;
					}
					break;
				case SDL_KEYUP:
					PlayerStopMove(&player, invincible);
					break;
				case SDL_QUIT:
					quit = 1;
					break;
			};
		};
		//keeping track and limiting frames
		frames++;
		frameTime = SDL_GetTicks() - frameTime2;
		if (FRAME_DELAY > frameTime) {
			SDL_Delay(FRAME_DELAY - frameTime);
		}
	};
	//ending game
	FreeAllSurfaces(charset, screen, scrtex, renderer, window, &player, &enemies);
	SDL_Quit();
	return 0;
	};
