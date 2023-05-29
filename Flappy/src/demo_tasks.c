#include "TUM_Draw.h"
#include "TUM_Event.h"
#include "TUM_Ball.h"
#include "TUM_Sound.h"
#include "TUM_Utils.h"
#include "TUM_Print.h"

#include "defines.h"
#include "main.h"
#include "demo_tasks.h"
#include "draw.h"
#include "async_message_queues.h"
#include "async_sockets.h"
#include "buttons.h"
#include "state_machine.h"
#include "timers.h"
#include "time.h"

TaskHandle_t DemoTask1 = NULL;
TaskHandle_t DemoTask2 = NULL;
TaskHandle_t DemoSendTask = NULL;
TaskHandle_t PauseScreen = NULL;
TaskHandle_t Menu = NULL;
TaskHandle_t gameover = NULL;

#define PIPE_GAP 50
#define RESTART_WIDTH 67
#define RESTART_HEIGHT 72
#define FLOOR_SPEED 0.5
#define FLOOR_WIDTH 50
#define PIPE_HEIGHT 486
#define PIPE_WIDTH 60
#define BIRD_WIDTH 30
#define BIRD_HEIGHT 20

TickType_t now = 0;
TickType_t reboot = 0;

Pipe pipe1;
Pipe pipe2;
Pipe pipe3;
Cheat cheatmode;  

void vStart(void *pvParameters) // task for startscreen 
{	cheatmode.condition=0;
	vTaskSuspend(Menu);
	while (1) {
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
			    pdTRUE) {
				vDrawClearScreen();
				vStartScreen();
				// Press Menubutton to check Menu options 
				if (300 <= tumEventGetMouseX() && tumEventGetMouseX() <= 467 &&
				    SCREEN_HEIGHT - 190 <= tumEventGetMouseY() &&
				    tumEventGetMouseY() <=SCREEN_HEIGHT - 130) {
					if (tumEventGetMouseLeft() == 1) {
						vTaskResume(Menu);
					}
				}
				reboot = now;
				// Get input and check for state change
				vCheckStateInput1();
			}
	}
}


void vGameOverScreen(void *pvParameters) // Task for gameover 
{
	while (1) {
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
			    pdTRUE) {
				vTaskSuspend(DemoTask2);
				vGameOver(); // draw logo for gameover 
				vScore(); // print score 
				vPlayButton(520, SCREEN_HEIGHT - 100);
				vRestartButton(30, SCREEN_HEIGHT - 100);
				PrintScore(); // print best score 
				// INITIALIZE PIPES POSITION FOR NEXT GAME 
				pipe1.X = SCREEN_WIDTH; 
				pipe2.X = SCREEN_WIDTH + SCREEN_WIDTH / 3;
				pipe3.X = SCREEN_WIDTH + SCREEN_WIDTH / 3 * 2+5;
				// PRESS HOME BUTTON TO RETURN TO HOMESCREEN 
				if (50 <= tumEventGetMouseX() && tumEventGetMouseX() <= 50 + RESTART_WIDTH &&
				    SCREEN_HEIGHT - 100 <=tumEventGetMouseY() &&
				    tumEventGetMouseY() <=SCREEN_HEIGHT - 100 +RESTART_HEIGHT) {
					if (tumEventGetMouseLeft() == 1) {
						vTaskResume(DemoTask1);
						vTaskSuspend(gameover);
					}
				}
				vCheckStateInput2();
				reboot = now; // REINITIALIZE TIME 
				// RESET CHEATMODE 
				cheatmode.condition= 0;
			}
	}
}

void vPauseScreen(void *pvParameters) // PAUSE SCREEN 
{
	while (1) {
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
			    pdTRUE) {
				vBackground();
				vFlappyLogo();
				vPlayButton(520, 300);
				vMovingGround();
				// PRESS PAUSE 
				if (520 <= tumEventGetMouseX() &&tumEventGetMouseX() <= 520 + RESTART_WIDTH &&
				    300 <= tumEventGetMouseY() &&tumEventGetMouseY() <= 300 + RESTART_HEIGHT) {
					if (tumEventGetMouseLeft() == 1) {
						vTaskResume(DemoTask2);
						vTaskSuspend(PauseScreen);
					}
				}
				reboot = now;
			}
	}
}

void vMenuS(void *pvParameters) // MENU SCREEN 
{
	cheatmode.condition= 0;
	while (1) {
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
			    pdTRUE) {
				vMENU(50, 110);
				vPlayButton(520, SCREEN_HEIGHT - 100);
				// CHEATMODE SELECTION 
				if (110 <= tumEventGetMouseX() && tumEventGetMouseX() <= 239 &&
				    254 <= tumEventGetMouseY() && tumEventGetMouseY() <= 408) {
					if (tumEventGetMouseLeft() == 1) {
						cheatmode.condition = 1;
						validationtick(249,245); 
					}
				}
				// GO BACK TO STARTSCREEN 
				if (520 <= tumEventGetMouseX() && tumEventGetMouseX() <= 520 +RESTART_WIDTH &&
				    SCREEN_HEIGHT - 100 <= tumEventGetMouseY() &&
				    tumEventGetMouseY() <=SCREEN_HEIGHT - 100 + RESTART_HEIGHT) {
					if (tumEventGetMouseLeft() == 1) {
						vTaskResume(DemoTask1);
						vTaskSuspend(Menu);
					}
				}
				//RESET TIME 
				reboot = now;
			}
	}
}

void myTimerCallback(TimerHandle_t xTimer) // TIMER FUNCTION 
{
	printf("TIMER STARTED");
}

TimerHandle_t myTimer = NULL;

void vPlay(void *pvParameters) // play screen 

{	time_t t; 
	float timer = 0;
	vTaskSuspend(PauseScreen);
	vTaskSuspend(gameover);
	srand(time (&t));
	pipe1.Y = (rand() % (650 - 550)) + 650;
	srand(time (&t));
	pipe2.Y = pipe1.Y + rand() % 20 + 70;
	srand(time (&t));
	pipe3.Y = pipe1.Y + rand() % 20 + 90;
	srand(time (&t));
	pipe1.GAP = rand() % 30 + 100;
	srand(time (&t));
	pipe2.GAP = rand() % 30 + 100;
	srand(time (&t));
	pipe3.GAP = rand() % 50 + 100;
	pipe1.X = SCREEN_WIDTH;
	pipe2.X = SCREEN_WIDTH + SCREEN_WIDTH / 3;
	pipe3.X = SCREEN_WIDTH + SCREEN_WIDTH / 3 * 2+5;
	prints("Task 1 init'd\n");
	myTimer = xTimerCreate("My Timer", pdMS_TO_TICKS(5000), pdTRUE,
			       (void *)0, myTimerCallback);
	if (xTimerStart(myTimer, 0) == pdFAIL) {
		printf("TIMER START failed\n");
	}
	now = 0;
	reboot = 0;
	reboot = now;
	while (1) {
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==pdTRUE) {
				srand(time(&t));
				xGetButtonInput(); 
				vBackground();
				vMovingGround();
				now = xTaskGetTickCount();
				// timing to start PLAY
				timer = (now - reboot) / 1000;
				if (timer <= 5) {
					vBirdFalling(30, 0);
					if (timer == 2)
						vthree();
					if (timer == 3)
						vtwo();
					if (timer == 4)
						vone();
					if (timer == 5)
						vzero();
				// CHEAT MODE PLAY
				} else if (cheatmode.condition == 0) {
					vpause();
					if (520 <= tumEventGetMouseX() &&
					    tumEventGetMouseX() <= 520 + 67 &&
					    10 <= tumEventGetMouseY() &&
					    tumEventGetMouseY() <= 10 + 72) {
						if (tumEventGetMouseLeft() ==1) {
							vTaskResume(PauseScreen);
							vTaskSuspend(DemoTask2);
						}
					}
					vBirdPlaying(pipe1, pipe2, pipe3);
					pipe1.X -= 20;
					pipe2.X -= 20;
					pipe3.X -= 20;
					vgenerate_pipes_bottom(pipe1, pipe2,pipe3);
					vPlayScreen();
					//REINITIALIZE PIPES 
					if (pipe1.X <= BIRD_HEIGHT-PIPE_WIDTH) {
						pipe1.X = SCREEN_WIDTH;
						srand(timer);
						pipe1.Y =(rand() % (650 - 550)) +650;
						srand(timer);
						pipe1.GAP = rand() % 30 + 100;
					}
					if (pipe2.X <= BIRD_HEIGHT-PIPE_WIDTH) {
						pipe2.X = SCREEN_WIDTH;
						srand(timer);
						pipe2.Y = pipe1.Y +rand() % 20 + 70;
						srand(timer);
						pipe2.GAP = rand() % 30 + 100;
					}

					if (pipe3.X <= BIRD_HEIGHT-PIPE_WIDTH) {
						pipe3.X = SCREEN_WIDTH;
						pipe3.Y = pipe1.Y +rand() % 30 + 90;
						pipe3.GAP = rand() % 50 + 100;
					}

				// NORMAL MODE 
				} else {
					vpause();
					if (520 <= tumEventGetMouseX() &&
					    tumEventGetMouseX() <= 520 + RESTART_WIDTH &&
					    10 <= tumEventGetMouseY() &&
					    tumEventGetMouseY() <= 10 + RESTART_HEIGHT) {
						if (tumEventGetMouseLeft() ==1) {
							vTaskResume(
							PauseScreen);
							vTaskSuspend(DemoTask2);
						}
					}
					playcheatmode(pipe1, pipe2, pipe3);
					pipe1.X -= 20;
					pipe2.X -= 20;
					pipe3.X -= 20;
					vgenerate_pipes_bottom(pipe1, pipe2,pipe3);
					vPlayScreen();
					//REINITIALIZE PIPES 
					if (pipe1.X <= BIRD_HEIGHT-PIPE_WIDTH) {
						pipe1.X = SCREEN_WIDTH;
						pipe1.Y =(rand() % (650 - 550)) +650;
						pipe1.GAP = rand() % 30 + 100;
					}
					if (pipe2.X <= BIRD_HEIGHT-PIPE_WIDTH) {
						pipe2.X = SCREEN_WIDTH;
						pipe2.Y = pipe1.Y +rand() % 20 + 70;
						pipe2.GAP = rand() % 30 + 100;
					}

					if (pipe3.X <=BIRD_HEIGHT-PIPE_WIDTH) {
						pipe3.X = SCREEN_WIDTH;
						pipe3.Y = pipe1.Y +rand() % 30 + 90;
						pipe3.GAP = rand() % 50 + 100;
					}
				}
			}
	}
	// RESET CHEATMODE
	cheatmode.condition = 0;
}
void vDemoSendTask(void *pvParameters)
{
	static char udp_1_buffer[10] = { 0 };
	static char udp_2_buffer[10] = { 0 };
	static char tcp_buffer[10] = { 0 };
	while (1) {
		prints("*****TICK******\n");
		if (mq_one) {
			aIOMessageQueuePut(mq_one_name, "Hello MQ one");
		}
		if (mq_two) {
			aIOMessageQueuePut(mq_two_name, "Hello MQ two");
		}
		//sprintf(str, "x: %d",bird1.posx);
		// posy
		udp_1_buffer[0] = (char)(bird1.posy & 0xff);
		udp_1_buffer[1] = (char)(bird1.posy >> 8);
		udp_2_buffer[0] = (char)(bird1.posy & 0xff);
		udp_2_buffer[1] = (char)(bird1.posy >> 8);
		// score
		udp_1_buffer[2] = (char)(0);
		udp_1_buffer[3] = (char)(0);

		if (udp_soc_one)
			aIOSocketPut(UDP, NULL, UDP_TEST_PORT_1, udp_1_buffer,
				     strlen(udp_1_buffer));
		if (udp_soc_two)
			aIOSocketPut(UDP, "192.168.2.126", UDP_TEST_PORT_2,
				     udp_2_buffer, strlen(udp_2_buffer));
		if (tcp_soc)
			aIOSocketPut(TCP, NULL, TCP_TEST_PORT, tcp_buffer,
				     strlen(tcp_buffer));

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

int createDemoTasks(void)
{
	if (xTaskCreate(vStart, "DemoTask1", mainGENERIC_STACK_SIZE * 2,
			NULL, mainGENERIC_PRIORITY + 1, &DemoTask1) != pdPASS) {
		PRINT_TASK_ERROR("DemoTask1");
		goto err_task1;
	}
	if (xTaskCreate(vPlay, "DemoTask2", mainGENERIC_STACK_SIZE * 2,
			NULL, mainGENERIC_PRIORITY + 1, &DemoTask2) != pdPASS) {
		PRINT_TASK_ERROR("DemoTask2");
		goto err_task2;
	}
	if (xTaskCreate(vDemoSendTask, "DemoSendTask",
			mainGENERIC_STACK_SIZE * 2, NULL,
			configMAX_PRIORITIES - 1, &DemoSendTask) != pdPASS) {
		PRINT_TASK_ERROR("DemoSendTask");
		goto err_send_task;
	}
	if (xTaskCreate(vMenuS, "MenuScreen", mainGENERIC_STACK_SIZE * 2, NULL,
			configMAX_PRIORITIES - 1, &Menu) != pdPASS) {
		PRINT_TASK_ERROR("MenuError");
		goto err_Menu;
	}
	if (xTaskCreate(vGameOverScreen, "GameOver", mainGENERIC_STACK_SIZE * 2,
			NULL, configMAX_PRIORITIES - 1, &gameover) != pdPASS) {
		PRINT_TASK_ERROR("GameOver");
		goto err_gameover;
	}
	if (xTaskCreate(vPauseScreen, "Pause", mainGENERIC_STACK_SIZE * 2, NULL,
			configMAX_PRIORITIES - 1, &PauseScreen) != pdPASS) {
		PRINT_TASK_ERROR("Pause");
		goto err_pause;
	}
	

	vTaskSuspend(DemoTask1);
	vTaskSuspend(DemoTask2);
	vTaskSuspend(gameover);
	vTaskSuspend(PauseScreen);
	vTaskSuspend(Menu);

	return 0;

err_send_task:
	vTaskDelete(DemoTask2);
err_task2:
	vTaskDelete(gameover);
err_gameover:
	vTaskDelete(Menu);
err_pause:
	vTaskDelete(DemoTask1);
err_Menu:
	vTaskDelete(PauseScreen);

err_task1:
	return -1;
}

void deleteDemoTasks(void)
{
	if (DemoTask1) {
		vTaskDelete(DemoTask1);
	}
	if (DemoTask2) {
		vTaskDelete(DemoTask2);
	}
	if (DemoSendTask) {
		vTaskDelete(DemoSendTask);
	}
	if (gameover) {
		vTaskDelete(gameover);
	}
	if (PauseScreen) {
		vTaskDelete(PauseScreen);
	}
	if (Menu) {
		vTaskDelete(Menu);
	}
}