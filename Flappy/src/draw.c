#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "FreeRTOS.h"
#include "task.h"
#include "TUM_Draw.h"
#include "TUM_Font.h"
#include "TUM_Event.h"
#include "TUM_Ball.h"
#include "TUM_Utils.h"
#include "TUM_Print.h"
#include "demo_tasks.h"
#include "defines.h"
#include "buttons.h"
#include "TUM_Sound.h"
#include "stdbool.h"

#define FPS_AVERAGE_COUNT 50
#define LOGO_FILENAME "freertos.jpg"
#define LOGO_FILENAME1 "background.png"
#define LOGO_FILENAME2 "ground.png"
#define LOGO_FILENAME3 "play(1).png"
#define LOGO_FILENAME4 "bird2.png"
#define LOGO_FILENAME5 "FlappyBirdLogo(2).png"
#define LOGO_FILENAME6 "birdflying.png"
#define LOGO_FILENAME7 "pipe_bottom.png"
#define LOGO_FILENAME8 "pipe_top (1).png"
#define LOGO_FILENAME9 "Game Over(1).png"
#define LOGO_FILENAME10 "Pause.(1).png"
#define LOGO_FILENAME11 "menu(3).png"
#define LOGO_FILENAME12 "score(1).png"
#define LOGO_FILENAME13 "pas de medaille.png"
#define LOGO_FILENAME14 "medaille d'or.png"
#define LOGO_FILENAME15 "medaille bronze.png"
#define LOGO_FILENAME16 "medaille d'argent.png"
#define LOGO_FILENAME17 "zero.png"
#define LOGO_FILENAME18 "one.png"
#define LOGO_FILENAME19 "three.png"
#define LOGO_FILENAME20 "two.png"
#define LOGO_FILENAME21 "RESTART(1).png"
#define LOGO_FILENAME22 "MENU(1).png"
#define LOGO_FILENAME23 "TICK.png"

#define FLOOR_SPEED 0.5
#define FLOOR_WIDTH 50
#define PIPE_HEIGHT 486
#define PIPE_WIDTH 60
#define BIRD_WIDTH 30
#define BIRD_HEIGHT 20
#define FLOOR_POSITION 400


#define CAVE_SIZE_X SCREEN_WIDTH / 2
#define CAVE_SIZE_Y SCREEN_HEIGHT / 2
#define CAVE_X CAVE_SIZE_X / 2
#define CAVE_Y CAVE_SIZE_Y / 2
#define CAVE_THICKNESS 25

image_handle_t Background_FlappyBird = NULL;
image_handle_t Ground_FB = NULL;
image_handle_t Play_B = NULL;
image_handle_t Bird1 = NULL;
image_handle_t Bird2 = NULL;
image_handle_t Bird3 = NULL;
image_handle_t FlappyBird = NULL;
image_handle_t pipebottom = NULL;
image_handle_t pipetop = NULL;
image_handle_t Game_Over = NULL;
image_handle_t MenuScreen = NULL;
image_handle_t pausebtn = NULL;
image_handle_t scoreScreen = NULL;
image_handle_t bronze = NULL;
image_handle_t gold = NULL;
image_handle_t silver = NULL;
image_handle_t platinium = NULL;
image_handle_t one = NULL;
image_handle_t two = NULL;
image_handle_t three = NULL;
image_handle_t zero = NULL;
image_handle_t MENU = NULL;
image_handle_t RESTART = NULL;
image_handle_t tick = NULL;

typedef struct {
	int posx;
	int posy;
} Bird;
Bird bird1;

typedef struct {
	int X;
	int Y;
	int GAP;
	
} Pipe;

typedef struct {
	int BEST;
	int score2;
	int cheatscore; 
	int score ;
}SetScore ;
SetScore SCORE ={.BEST=0,.score=0,.score2=0,.cheatscore=0}; 
 

typedef struct {
	int floorx; 
}Floor;
Floor Ground;  


void checkDraw(unsigned char status, const char *msg)
{
	if (status) {
		if (msg)
			fprints(stderr, "[ERROR] %s, %s\n", msg,
				tumGetErrorMessage());
		else {
			fprints(stderr, "[ERROR] %s\n", tumGetErrorMessage());
		}
	}
}

void vDrawClearScreen(void)
{
	checkDraw(tumDrawClear(White), __FUNCTION__);
}

void vDrawCaveBoundingBox(void)
{
	checkDraw(tumDrawFilledBox(CAVE_X - CAVE_THICKNESS,
				   CAVE_Y - CAVE_THICKNESS,
				   CAVE_SIZE_X + CAVE_THICKNESS * 2,
				   CAVE_SIZE_Y + CAVE_THICKNESS * 2, TUMBlue),
		  __FUNCTION__);

	checkDraw(tumDrawFilledBox(CAVE_X, CAVE_Y, CAVE_SIZE_X, CAVE_SIZE_Y,
				   Aqua),
		  __FUNCTION__);
}
void vDrawFPS(void)
{
	static unsigned int periods[FPS_AVERAGE_COUNT] = { 0 };
	static unsigned int periods_total = 0;
	static unsigned int index = 0;
	static unsigned int average_count = 0;
	static TickType_t xLastWakeTime = 0, prevWakeTime = 0;
	static char str[10] = { 0 };
	static int text_width;
	int fps = 0;
	font_handle_t cur_font = tumFontGetCurFontHandle();

	if (average_count < FPS_AVERAGE_COUNT) {
		average_count++;
	} else {
		periods_total -= periods[index];
	}

	xLastWakeTime = xTaskGetTickCount();

	if (prevWakeTime != xLastWakeTime) {
		periods[index] =
			configTICK_RATE_HZ / (xLastWakeTime - prevWakeTime);
		prevWakeTime = xLastWakeTime;
	} else {
		periods[index] = 0;
	}

	periods_total += periods[index];

	if (index == (FPS_AVERAGE_COUNT - 1)) {
		index = 0;
	} else {
		index++;
	}

	fps = periods_total / average_count;

	tumFontSelectFontFromName(FPS_FONT);

	sprintf(str, "FPS: %2d", fps);

	if (!tumGetTextSize((char *)str, &text_width, NULL))
		checkDraw(tumDrawText(str, SCREEN_WIDTH - text_width - 10,
				      SCREEN_HEIGHT - DEFAULT_FONT_SIZE * 1.5,
				      Skyblue),__FUNCTION__);

	tumFontSelectFontFromHandle(cur_font);
	tumFontPutFontHandle(cur_font);
}

void vBackground(void) // draw Flappy Background. 
{
	static int image_height;

	if (Background_FlappyBird == NULL) {
		Background_FlappyBird = tumDrawLoadImage(LOGO_FILENAME1);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(
		     Background_FlappyBird)) != -1) {
		checkDraw(tumDrawLoadedImage(Background_FlappyBird, 0,
					     SCREEN_HEIGHT + 50 - image_height),
			  __FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME1);
	}
}
void validationtick(int x, int y) // draw tick in menu. 
{
	static int image_height;

	if (tick == NULL) {
		tick = tumDrawLoadImage(LOGO_FILENAME23);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(
		     tick)) != -1) {
		checkDraw(tumDrawLoadedImage(tick, x,y),
			  __FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME23);
	}
}

void vStaticGround(void) // print not moving ground for start screen.
{
	static int image_height;

	if (Ground_FB == NULL) {
		Ground_FB = tumDrawLoadImage(LOGO_FILENAME2);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(Ground_FB)) != -1) {
		checkDraw(
			tumDrawLoadedImage(Ground_FB, 0,SCREEN_HEIGHT + 100 - image_height),__FUNCTION__);
	} 
	else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME2);
	}
}

void vMenu(void) // draw Menubutton. 
{
	static int image_height;

	if (MenuScreen == NULL) {
		MenuScreen = tumDrawLoadImage(LOGO_FILENAME11);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(MenuScreen)) != -1) {
		checkDraw(tumDrawLoadedImage(MenuScreen, 300,SCREEN_HEIGHT - 190),
			  			__FUNCTION__);
	} else {
		fprints(stderr,
			"Failed to get size of image '%s', does it exist?\n",
			LOGO_FILENAME2);
	}
}
void vpause(void) //draw Pausebutton. 
{
	static int image_height;

	if (pausebtn == NULL) {
		pausebtn = tumDrawLoadImage(LOGO_FILENAME10);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(pausebtn)) != -1) {
		checkDraw(tumDrawLoadedImage(pausebtn, 520, 10), __FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME10);
	}
}
void vzero(void) // draw Zero while waiting for the game to start. 
{
	static int image_height;

	if (zero == NULL) {
		zero = tumDrawLoadImage(LOGO_FILENAME17);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(zero)) != -1) {
		checkDraw(tumDrawLoadedImage(zero, 250, 30), __FUNCTION__);
	} else {fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME17);
	}
}
void vone(void) // draw One while waiting for the game to start.
{
	static int image_height;

	if (one == NULL) {
		one = tumDrawLoadImage(LOGO_FILENAME18);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(one)) != -1) {
		checkDraw(tumDrawLoadedImage(one, 250, 30), __FUNCTION__);
	} else {
		fprints(stderr,
			"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME18);
	}
}
void vtwo(void) // draw Two while waiting for the game to start.
{
	static int image_height;

	if (two == NULL) {
		two = tumDrawLoadImage(LOGO_FILENAME20);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(two)) != -1) {
		checkDraw(tumDrawLoadedImage(two, 250, 30), __FUNCTION__);
	} else {
		fprints(stderr,
			"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME20);
	}
}
void vthree(void) // draw Three while waiting for the game to start.
{
	static int image_height;

	if (three == NULL) {
		three = tumDrawLoadImage(LOGO_FILENAME19);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(three)) != -1) {
		checkDraw(tumDrawLoadedImage(three, 250, 30), __FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME19);
	}
}

void vGameOver(void) // draw GameOver logo.
{
	static int image_height;

	if (Game_Over == NULL) {
		Game_Over = tumDrawLoadImage(LOGO_FILENAME9);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(Game_Over)) != -1) {
		checkDraw(tumDrawLoadedImage(Game_Over, 90, 10), __FUNCTION__);
	} else {
		fprints(stderr,
			"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME9);
	}
}

void vplatinum(void)  // draw picture for platinum 
{
	static int image_height;

	if (platinium == NULL) {
		platinium = tumDrawLoadImage(LOGO_FILENAME13);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(platinium)) != -1) {
		checkDraw(tumDrawLoadedImage(platinium, 150, 238), __FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME13);
	}
}
void vbronze(void) //draw bronze medal 
{
	static int image_height;

	if (bronze == NULL) {
		bronze = tumDrawLoadImage(LOGO_FILENAME15);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(bronze)) != -1) {
		checkDraw(tumDrawLoadedImage(bronze, 150, 230), __FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME15);
	}
}
void vgold(void) // draw Gold medal 
{
	static int image_height;

	if (gold == NULL) {
		gold = tumDrawLoadImage(LOGO_FILENAME14);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(gold)) != -1) {
		checkDraw(tumDrawLoadedImage(gold, 150, 238), __FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME14);
	}
}

void vsilver(void) // draw silver medal 
{
	static int image_height;

	if (silver == NULL) {
		silver = tumDrawLoadImage(LOGO_FILENAME16);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(silver)) != -1) {
		checkDraw(tumDrawLoadedImage(silver, 148, 238), __FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME16);
	}
}

void vScore(void) // draw gameover screen 
{
	static int image_height;

	if (scoreScreen == NULL) {
		scoreScreen = tumDrawLoadImage(LOGO_FILENAME12);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(scoreScreen)) != -1) {
		checkDraw(tumDrawLoadedImage(scoreScreen, 100, 150),
			  __FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME12);
	}
}

void vPlayButton(int x, int y) // vdraw play button
{
	static int image_height;

	if (Play_B == NULL) {
		Play_B = tumDrawLoadImage(LOGO_FILENAME3);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(Play_B)) != -1) {
		checkDraw(tumDrawLoadedImage(Play_B, x, y), __FUNCTION__);
	} else {
		fprints(stderr,
			"Failed to get size of image '%s', does it exist?\n",
			LOGO_FILENAME3);
	}
}
void vRestartButton(int x, int y) // draw restart button
{
	static int image_height;

	if (RESTART == NULL) {
		RESTART = tumDrawLoadImage(LOGO_FILENAME21);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(RESTART)) != -1) {
		checkDraw(tumDrawLoadedImage(RESTART, x, y), __FUNCTION__);
	} else {
	fprints(stderr,
			"Failed to get size of image '%s', does it exist?\n",
			LOGO_FILENAME21);
	}
}
void vMENU(int x, int y) // draw MenuScreen
{
	static int image_height;

	if (MENU == NULL) {
		MENU = tumDrawLoadImage(LOGO_FILENAME22);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(MENU)) != -1) {
		checkDraw(tumDrawLoadedImage(MENU, x, y), __FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME22);
	}
}


void vBirdFalling(int x1, int y1) // draw Bird in Falling mode
{
	static int image_height1; 
	static int y2;
	y2 = SCREEN_HEIGHT - 260 - image_height1 + y1; // y is initialised 
	if (Bird1 == NULL) {
		Bird1 = tumDrawLoadImage(LOGO_FILENAME4);
	}
	image_height1 = tumDrawGetLoadedImageHeight(Bird1);
	if ((image_height1 = tumDrawGetLoadedImageHeight(Bird1)) != -1) {
		checkDraw(tumDrawLoadedImage(Bird1, x1, y2), __FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME4);
	}
}

void vBirdFlying(int x2, int y2)//draw bird in flying mode 
{	static int image_height2;
	static int y1;

	y1 = SCREEN_HEIGHT - 260 - image_height2 + y2; // y1 is inisialised. 
	if (Bird2 == NULL) {
		Bird2 = tumDrawLoadImage(LOGO_FILENAME6);
	}
	image_height2 = tumDrawGetLoadedImageHeight(Bird2);
	if ((image_height2 = tumDrawGetLoadedImageHeight(Bird2)) != -1) {
		checkDraw(tumDrawLoadedImage(Bird2, x2, y1), __FUNCTION__);

	} else {
		fprints(stderr,
			"Failed to get size of image '%s', does it exist?\n",
			LOGO_FILENAME6);

	}
}

void vBirdPlaying1(void) 
{
	static int x, y = 0, i = 0, gravity;
	x = 30 + i;
	gravity = 20;
	i = i + 1;
	if (xSemaphoreTake(buttons.lock, 0) == pdTRUE) {
		if (buttons.buttons[KEYCODE(SPACE)] == 0) {
			gravity = 20;
			y += gravity;
			if (y + SCREEN_HEIGHT - 260-BIRD_HEIGHT <= 0) {
				y = -SCREEN_HEIGHT + 260+20;
			}
			if (y + SCREEN_HEIGHT - 260 >= 400) {
				y = -SCREEN_HEIGHT + 260 + 400;
			}
			if (bird1.posx+BIRD_WIDTH>=SCREEN_WIDTH)
			{
				x=30; 
			}

			vBirdFalling(x, y);

			bird1.posx = x;
			bird1.posy = y + SCREEN_HEIGHT - 260 - BIRD_HEIGHT;

		} else {
			gravity = -20;
			y += gravity;
			if (y + SCREEN_HEIGHT - 260 - BIRD_HEIGHT <= 0) {
				y = -SCREEN_HEIGHT + 260 + BIRD_HEIGHT;
			}

			if (y + SCREEN_HEIGHT - 260 >= 400) {
				y = 400 - SCREEN_HEIGHT + 260 + 480;
			}
			if (bird1.posx+BIRD_WIDTH>=SCREEN_WIDTH)
			{
				x=30; 
			}
			vBirdFlying(x, y);
			tumSoundPlaySample(g4);

			bird1.posx = x;
			bird1.posy = y + SCREEN_HEIGHT - 260 - 20;
		}
	}
	xSemaphoreGive(buttons.lock);
}
void ScoreText(int x, int y, char *str, int variable) // PRINT SCORE FOR EACH PLAY 
{
	ssize_t prev_font_size = tumFontGetCurFontSize();

	tumFontSetSize((ssize_t)30); // GET TEXT SIZE

	sprintf(str, "SCORE : %5d", variable);
	checkDraw(tumDrawText(str, x, y, White), __FUNCTION__);
	tumFontSetSize(prev_font_size);
}

void vBirdPlaying(Pipe pipe1, Pipe pipe2, Pipe pipe3) // MAIN PLAYING FUNCTION
{	static int x, y,i,gravity;
	static char str1[100] = { 0 };
	ScoreText(250, 20, str1, SCORE.score);
	x = 30 + i;
	gravity = 17; //SET GRAVITY THAT ATTRACTS THE BIRD
	i = i + 0.1;
	if (xSemaphoreTake(buttons.lock, 0) == pdTRUE) {
		if (buttons.buttons[KEYCODE(SPACE)] == 0) { // FALLING MODE 
			gravity = 20;
			y+= gravity;
			//MAKE SURE THAT THE BIRD DOESN'T SURPASS THE SCREEN 
			if (y+SCREEN_HEIGHT - 260 -BIRD_HEIGHT <= 0) {
				y=-SCREEN_HEIGHT + 260 +BIRD_HEIGHT; 
			}
			vBirdFalling(x,y);// DRAW BIRD
			bird1.posx = x; // BIRD  XPOSIION 
			bird1.posy = y + SCREEN_HEIGHT -260-BIRD_HEIGHT; //BIRD Y POSITION
			// GAMEoVER WHEN THE BIRD COLLIDES WITH FLOOR 
			if (bird1.posy >= FLOOR_POSITION) { 
				tumSoundPlaySample(g5);//PLAY SOUND 
				vTaskResume(gameover); 
				y = 0;
				x = 30;
			}
			//GAMEOVER WHEN BIRD COLLIDES WITH FIRST PIPE
			if ((bird1.posx >= pipe1.X - BIRD_HEIGHT &&
			     bird1.posx + BIRD_WIDTH <=
				     pipe1.X - BIRD_HEIGHT + PIPE_WIDTH)) {
				if (bird1.posy + BIRD_HEIGHT >=
					    pipe1.Y - PIPE_HEIGHT ||
				    bird1.posy <=
					    pipe1.Y - pipe1.GAP - PIPE_HEIGHT) {
					tumSoundPlaySample(g5);
					vTaskResume(gameover);
					y = 0; // INISIALIZE PIRD POITIONS FOR NEXT GAME 
					x = 30;
				}
					 }
				// IF ALWAYS ALIVE ADD SCORE 
				if (bird1.posx>pipe1.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.score+= 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str1,SCORE.score);
				}
			
			//GAMEOVER WHEN BIRD COLLIDES WITH SECOND PIPE
			if ((bird1.posx >= pipe2.X - BIRD_HEIGHT &&
			     bird1.posx + BIRD_WIDTH <=
				     pipe2.X - BIRD_HEIGHT + PIPE_WIDTH)) {
				if (bird1.posy + BIRD_HEIGHT >=
					    pipe2.Y - PIPE_HEIGHT ||
				    bird1.posy <=pipe2.Y - pipe2.GAP - PIPE_HEIGHT) {
					tumSoundPlaySample(g5);
					vTaskResume(gameover);
					y = 0; // INISIALIZE PIRD POITIONS FOR NEXT GAME 
					x = 30;
				}
					 }
				// IF ALWAYS ALIVE ADD SCORE 
				if (bird1.posx>pipe2.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.score += 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str1,SCORE.score);
				}
			
			//GAMEOVER WHEN BIRD COLLIDES WITH THIRD PIPE
			if ((bird1.posx >= pipe3.X - BIRD_HEIGHT &&
			     bird1.posx + BIRD_WIDTH <=
				     pipe3.X - BIRD_HEIGHT + PIPE_WIDTH)) {
				if (bird1.posy + BIRD_HEIGHT >=
					    pipe3.Y - PIPE_HEIGHT ||
				    bird1.posy <=
					    pipe3.Y - pipe3.GAP - PIPE_HEIGHT) {
					tumSoundPlaySample(g5);

					vTaskResume(gameover);
					y = 0;
					x = 30;
				}
					 }
				// IF ALWAYS ALIVE ADD SCORE 
				if (bird1.posx>pipe3.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.score += 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str1,SCORE.score);
				}
			

			ScoreText(250, 20, str1, SCORE.score);
		} else { // FLYING MODE
			gravity = -10;
			y+= gravity; //MAKE THE BIRD FLY
			// MAKE SURE THE BIRD DOESN'T SUPASS THE SCREEN 
			if (y+ SCREEN_HEIGHT - 260 -BIRD_HEIGHT <= 0) {
				y=-SCREEN_HEIGHT + 260 +BIRD_WIDTH;
			}
			vBirdFlying(x,y);
			tumSoundPlaySample(g4); //PLAY SOUND FOR FLYING 
			bird1.posx = x; // GET BIRD POSITION 
			bird1.posy = y + SCREEN_HEIGHT - 260 - BIRD_HEIGHT;
			if (bird1.posy >= FLOOR_POSITION) {
				vTaskResume(gameover);
				tumSoundPlaySample(g5);
				y = 0; // INITIALIZE BIRD POSITION FOR NEXT GAME 
				x = 30;
			}
			//GAMEOVER WHEN BIRD COLLIDES WITH FIRST PIPE
			if ((bird1.posx >= pipe1.X - BIRD_HEIGHT &&
			     bird1.posx + BIRD_WIDTH <=
				     pipe1.X - BIRD_HEIGHT + PIPE_WIDTH)) {
				if (bird1.posy + BIRD_HEIGHT >=
					    pipe1.Y - PIPE_HEIGHT ||
				    bird1.posy <=
					    pipe1.Y - pipe1.GAP - PIPE_HEIGHT) {
					vTaskResume(gameover);
					tumSoundPlaySample(g5);
					y = 0; // GET BIRD POSITION 
					x = 30;
				}
					 }
				// IF ALWAYS ALIVE ADD SCORE 
				if (bird1.posx>pipe1.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.score += 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str1,SCORE.score);
				}
			
			//GAMEOVER WHEN BIRD COLLIDES WITH SECOND PIPE
			if ((bird1.posx >= pipe2.X - BIRD_HEIGHT &&
			     bird1.posx + BIRD_WIDTH <=
				     pipe2.X - BIRD_HEIGHT + PIPE_WIDTH)) {
				if (bird1.posy + BIRD_HEIGHT >=
					    pipe2.Y - PIPE_HEIGHT ||
				    bird1.posy <=
					    pipe2.Y - pipe2.GAP - PIPE_HEIGHT) {
					vTaskResume(gameover);
					tumSoundPlaySample(g5);
					y = 0; // GET BIRD POSITION 
					x = 30;
				}
					 }
				// IF ALWAYS ALIVE ADD SCORE 
				if (bird1.posx>pipe2.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.score += 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str1, SCORE.score);
				}
			
			//GAMEOVER WHEN BIRD COLLIDES WITH THIRD PIPE
			if ((bird1.posx >= pipe3.X - BIRD_HEIGHT &&
			     bird1.posx + BIRD_WIDTH <=
				     pipe3.X - BIRD_HEIGHT + PIPE_WIDTH)) {
				if (bird1.posy + BIRD_HEIGHT >=
					    pipe3.Y - PIPE_HEIGHT ||
				    bird1.posy <=
					    pipe3.Y - pipe3.GAP - PIPE_HEIGHT) {
					vTaskResume(gameover);
					tumSoundPlaySample(g5);
					y = 0; // GET BIRD POSITION 
					x = 30;
				}
					 }
				// IF ALWAYS ALIVE ADD SCORE 
				if (bird1.posx>pipe3.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.score += 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str1, SCORE.score);
				}
			ScoreText(250, 20, str1,SCORE.score);
		} 
		// NEEDED FOR BESTSCORE AND MEDALS 	
		SCORE.score2=SCORE.score;
	}

	xSemaphoreGive(buttons.lock);
}

void playcheatmode(Pipe pipe1, Pipe pipe2, Pipe pipe3) // PLAY IN CHEAT MODE 
{	static int x, y, i, gravity;
	static char str2[100] = { 0 };
	ScoreText(250, 20, str2,SCORE.cheatscore);
	x = 30 + i;
	gravity = 17;
	i = i + 0.1;
	if (xSemaphoreTake(buttons.lock, 0) == pdTRUE) {
		if (buttons.buttons[KEYCODE(SPACE)] == 0) { // FALLING MODE 

			gravity = 20;
			y+= gravity;
			// BIRD DOENS'T SURPASS THE SCREEN 
			if (y + SCREEN_HEIGHT - 260 -BIRD_HEIGHT <= 0) {
				y =-SCREEN_HEIGHT + 260 +BIRD_HEIGHT;
			}
			// BIRD KEEPS WALKING WHEN REACHING FLOOR 
			if (y + SCREEN_HEIGHT - 260 >=FLOOR_POSITION) {
				y =-SCREEN_HEIGHT + 260 + FLOOR_POSITION;
			}
			vBirdFalling(x, y);
			bird1.posx = x; // GET BIRD POSITIONS 
			bird1.posy = y + SCREEN_HEIGHT - 260 -BIRD_HEIGHT;
			// ADD SCORE FIRST PIPE 
			if (bird1.posx>pipe1.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.cheatscore += 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str2,SCORE.cheatscore);
				}
			// ADD SCORE FOR SECOND PIPE 
			if (bird1.posx>pipe2.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.cheatscore+= 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str2, SCORE.cheatscore);
				}
			// ADD SCORE FOR THIRD PIPE 
			if (bird1.posx>pipe3.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.cheatscore += 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str2, SCORE.cheatscore);
				}


			ScoreText(250, 20, str2, SCORE.cheatscore);
		} else { // FALLING MODE 
			gravity = -10;
			y += gravity;
			if (y+ SCREEN_HEIGHT - 260 -BIRD_HEIGHT<= 0) {
				y=-SCREEN_HEIGHT + 260 +BIRD_WIDTH;
			}
			if (y + SCREEN_HEIGHT - 260 >= 400) {
				y =-SCREEN_HEIGHT + 260 + 400;
			}
			vBirdFlying(x, y);
			tumSoundPlaySample(g4);
			bird1.posx = x;
			bird1.posy = y + SCREEN_HEIGHT - 260 -BIRD_HEIGHT;
			if (bird1.posx>pipe1.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.cheatscore += 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str2, SCORE.cheatscore);
				}
			if (bird1.posx>pipe2.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.cheatscore += 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str2, SCORE.cheatscore);
				}
			if (bird1.posx>pipe3.X-BIRD_HEIGHT+PIPE_WIDTH){
					SCORE.cheatscore += 1;
					tumSoundPlaySample(e5);
					ScoreText(250, 20, str2, SCORE.cheatscore);
				}
			ScoreText(250, 20, str2, SCORE.cheatscore);
		}
	}
	xSemaphoreGive(buttons.lock);
}

void PrintScore(void) // PRINT BESTSCORE + MEDALS 
{	static char str2[100] = { 0 };
	static char str1[100] = { 0 };
	ssize_t prev_font_size = tumFontGetCurFontSize();
	tumFontSetSize((ssize_t)30);
	sprintf(str1, "%5d", SCORE.score2);
	checkDraw(tumDrawText(str1, 445, 220, Black), __FUNCTION__);
	tumFontSetSize(prev_font_size);

	if (SCORE.score2 > 10 && SCORE.score2 <20)
		vbronze();
	if (20 <= SCORE.score2 && SCORE.score2<30)
		vsilver();
	if (30 <= SCORE.score2 && SCORE.score2<40)
		vgold();
	if (40 <= SCORE.score2)
		vplatinum();
		SCORE.score=0; 

	if (SCORE.score2 >= SCORE.BEST) {
		SCORE.BEST =SCORE.score2;
		sprintf(str2, "%5d",SCORE.BEST);
		tumFontSetSize((ssize_t)30);
		checkDraw(tumDrawText(str2, 445, 300, Black), __FUNCTION__);
		tumFontSetSize(prev_font_size);
	} else {
		tumFontSetSize((ssize_t)30);
		sprintf(str2, "%5d", SCORE.BEST);
		checkDraw(tumDrawText(str2, 445, 300, Black), __FUNCTION__);
		tumFontSetSize(prev_font_size);
	}
}

void vMovingGround(void) // DRAW MOVING GROUND 
{	
	Ground.floorx -= 5;
	if (Ground.floorx < -FLOOR_WIDTH) {
		Ground.floorx = 0;
	}
	static int image_height;
	if (Ground_FB == NULL) {
		Ground_FB = tumDrawLoadImage(LOGO_FILENAME2);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(Ground_FB)) != -1) {
		checkDraw(tumDrawLoadedImage(Ground_FB, Ground.floorx,FLOOR_POSITION),__FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME2);
	}

}

void vFlappyLogo(void) // DRAW FLAPPYLOGO FOR START SCREEN 
{
	static int image_height;

	if (FlappyBird == NULL) {
		FlappyBird = tumDrawLoadImage(LOGO_FILENAME5);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(FlappyBird)) != -1) {
		checkDraw(
			tumDrawLoadedImage(FlappyBird, 150,
					   SCREEN_HEIGHT - 360 - image_height),__FUNCTION__);
	} else {
		fprints(stderr,"Failed to get size of image '%s', does it exist?\n",LOGO_FILENAME5);
	}
}

void vgenerate_pipes_bottom(Pipe pipe1, Pipe pipe2, Pipe pipe3) // GENERATE ALL PIPES 
{
	static int image_height;
	const TickType_t xDelay = 100 / portTICK_PERIOD_MS;

	if (pipebottom == NULL) {
		pipebottom = tumDrawLoadImage(LOGO_FILENAME7);
	}
	if (pipetop == NULL) {
		pipetop = tumDrawLoadImage(LOGO_FILENAME8);
	}
	if ((image_height = tumDrawGetLoadedImageHeight(pipebottom)) != -1) {
		checkDraw(tumDrawLoadedImage(pipebottom, pipe1.X,
					     pipe1.Y - PIPE_HEIGHT),
			  __FUNCTION__);

		checkDraw(tumDrawLoadedImage(pipetop, pipe1.X,
					     pipe1.Y - pipe1.GAP - 2 *PIPE_HEIGHT),
			  __FUNCTION__);
		checkDraw(tumDrawLoadedImage(pipebottom, pipe2.X,
					     pipe2.Y - PIPE_HEIGHT),
			  __FUNCTION__);

		checkDraw(tumDrawLoadedImage(pipetop, pipe2.X,
					     pipe2.Y - pipe2.GAP - 2 * PIPE_HEIGHT),
			  __FUNCTION__);

		checkDraw(tumDrawLoadedImage(pipebottom, pipe3.X,
					     pipe3.Y - PIPE_HEIGHT),
			  __FUNCTION__);

		checkDraw(tumDrawLoadedImage(pipetop, pipe3.X,
					     pipe3.Y - pipe3.GAP - 2 * PIPE_HEIGHT),
			  __FUNCTION__);

		vTaskDelay(xDelay);
	}

	else {
		fprints(stderr,
			"Failed to get size of image '%s', does it exist?\n",
			LOGO_FILENAME7);
	}
}

void vStartScreen(void) // ELEMENTS FOR STARTSCREEN 
{
	vBackground();
	vStaticGround();
	vPlayButton(160, SCREEN_HEIGHT -125 -72);
	vBirdFalling(270, 0);
	vFlappyLogo();
	vMenu();
}
void vPlayScreen(void) // ELEMENTS OF PLAY SCREEN 
{
	vBackground();
	vMovingGround();
	vpause();
}

