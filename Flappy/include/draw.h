#ifndef __DRAW_H__
#define __DRAW_H__

extern image_handle_t logo_image;
extern image_handle_t Background_FlappyBird;
extern image_handle_t Ground_FB;
extern image_handle_t Play_B;
extern image_handle_t Bird1;
extern image_handle_t FlappyBird;
extern image_handle_t pipebottom;
extern image_handle_t Game_Over;
extern image_handle_t MenuScreen;
extern image_handle_t pausebtn;
extern image_handle_t scoreScreen;
extern image_handle_t bronze;
extern image_handle_t gold;
extern image_handle_t silver;
extern image_handle_t platinium;
extern image_handle_t zero;
extern image_handle_t one;
extern image_handle_t two;
extern image_handle_t three;
extern image_handle_t RESTART;
extern image_handle_t MENU;
extern image_handle_t tick ;

typedef struct {
	int posx;
	int posy;
} Bird;
extern Bird bird1;

typedef struct {
	int X;
	int Y;
	int GAP;
	int floorx; 
} Pipe;
extern Pipe pipe1;
extern Pipe pipe2;


typedef struct {
	int condition;
} Cheat; 

typedef struct {
	int BEST;
	int score2;
	int cheatscore;
	int score;  
}SCORE; 

typedef struct {
	int floorx; 
}Floor;
extern Floor Ground; 



void vDrawClearScreen(void);
void vDrawFPS(void);
void vBackground(void);
void vStaticGround(void);
void vPlayButton(int x, int y);
void vplatinum(void);
void vbronze(void);
void vgold(void);
void vsilver(void);
void vzero(void);
void vone(void);
void vtwo(void);
void vthree(void);
void vRestartButton(int x, int y);
void vMENU(int x, int y);
void vBirdFalling(int x, int y);
void vMovingGround();
void vBirdFlying(int x, int y);
void vFlappyLogo(void);
void vStartScreen(void);
void vPlayScreen(void);
void vpause(void);
void PrintScore(void);
void vGameOver(void);
void vScore(void);
void validationtick(int x, int y ); 
void vDrawButtonText(void);
void vDrawSpriteAnnimations(TickType_t xLastFrameTime);
void vDrawInitAnnimations(void);
void vgenerate_pipes_bottom(Pipe pipe1, Pipe pipe2, Pipe pipe3);
void vBirdPlaying(Pipe pipe1, Pipe pipe2, Pipe pipe3);
void playcheatmode(Pipe pipe1, Pipe pipe2, Pipe pipe3);
void vBirdPlaying1(void);

#endif //__DRAW_H__