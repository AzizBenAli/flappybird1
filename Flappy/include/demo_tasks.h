#ifndef __DEMO_TASKS_H__
#define __DEMO_TASKS_H__

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t DemoTask1;
extern TaskHandle_t DemoTask2;
extern TaskHandle_t DemoSendTask;
extern TaskHandle_t gameover;
extern TaskHandle_t PauseScreen;
extern TaskHandle_t Menu; 

int createDemoTasks(void);
void deleteDemoTasks(void);
int vCheckStateInput2(void); 
int vCheckStateInput1(void);

  
  

#endif // __DEMO_TASKS_H__