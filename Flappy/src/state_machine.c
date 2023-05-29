
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#include "buttons.h"
#include "main.h"
#include "demo_tasks.h"
#include "state_machine.h"

#include "TUM_Draw.h"   
#include "TUM_Event.h"
//#include "SDL2/SDL_mouse.h"

QueueHandle_t StateQueue = NULL;

int vCheckStateInput(void)
{
    if (xSemaphoreTake(buttons.lock, 0) == pdTRUE) {
        if (buttons.buttons[KEYCODE(RETURN)]) {
            buttons.buttons[KEYCODE(RETURN)] = 0;
            if (StateQueue) {
                xSemaphoreGive(buttons.lock);
                xQueueSend(StateQueue, &next_state_signal, 0);
                return 0;
            }
            return -1;
        }
        xSemaphoreGive(buttons.lock);
    }

    return 0;
}

int vCheckStateInput1(void)
{
   if  (160<=tumEventGetMouseX() && tumEventGetMouseX()<=160+67 &&
   SCREEN_HEIGHT-125-72<=tumEventGetMouseY() && tumEventGetMouseY()<=SCREEN_HEIGHT-125) {
        if (tumEventGetMouseLeft()==1){
            if (StateQueue) {
                xQueueSend(StateQueue, &next_state_signal, 0);
                return 0;
            }
            return -1;
        } 
    }
    return 0;
}

int vCheckStateInput2(void)
{
   if  (520<=tumEventGetMouseX() && tumEventGetMouseX()<=520+67 &&  SCREEN_HEIGHT-100<=tumEventGetMouseY() && tumEventGetMouseY()<=SCREEN_HEIGHT-100+72) {
        if (tumEventGetMouseLeft()==1){
           
            if (StateQueue) {
                
                xQueueSend(StateQueue, &next_state_signal, 0);
                return 0;
            }
            return -1;
        }
        
    }

    return 0;
}
/*
 * Example basic state machine with sequential states
 */
void basicSequentialStateMachine(void *pvParameters)
{
    unsigned char current_state = STARTING_STATE; // Default state
    unsigned char state_changed =
        1; // Only re-evaluate state if it has changed
    unsigned char input = 0;

    const int state_change_period = STATE_DEBOUNCE_DELAY;

    TickType_t last_change = xTaskGetTickCount();

    while (1) {
        if (state_changed) {
            goto initial_state;
        }

        // Handle state machine input
        if (StateQueue)
            if (xQueueReceive(StateQueue, &input, portMAX_DELAY) ==
                pdTRUE)
                if (xTaskGetTickCount() - last_change >
                    state_change_period) {
                    changeState(&current_state, input);
                    state_changed = 1;
                    last_change = xTaskGetTickCount();
                }

initial_state:
        // Handle current state
        if (state_changed) {
            switch (current_state) {
                case STATE_ONE:
                    if (DemoTask2) {
                        vTaskSuspend(DemoTask2);
                    }
                    if (DemoTask1) {
                        vTaskResume(DemoTask1);
                    }
                    if(gameover){
                        vTaskSuspend(gameover); 
                    }
                    if(Menu){
                        vTaskSuspend(Menu); 
                    }
                    break;
                case STATE_TWO:
                    if (DemoTask1) {
                        vTaskSuspend(DemoTask1);
                    }
                    if (DemoTask2) {
                        vTaskResume(DemoTask2);
                    }
                    if(gameover){
                        vTaskSuspend(gameover); 
                    }
                    if(PauseScreen){
                        vTaskSuspend(PauseScreen); 
                    }
                    break;
                    case STATE_THREE:
                    if (DemoTask2) {
                        vTaskResume(DemoTask2);
                    }
                    if (DemoTask1) {
                        vTaskSuspend(DemoTask1);
                    }
                    if(gameover){
                        vTaskSuspend(gameover); 
                    }
                    break;
                    case STATE_FOUR:
                    if (DemoTask2) {
                        vTaskResume(DemoTask2);
                    }
                    if(gameover){
                        vTaskSuspend(gameover); 
                    }
                     if (DemoTask1) {
                        vTaskSuspend(DemoTask1);
                    }
                    break;
                    case STATE_FIVE:
                    if (PauseScreen) {
                        vTaskSuspend(PauseScreen);
                    }
                    if (DemoTask2) {
                        vTaskSuspend(DemoTask2);
                    }
                    if(gameover){
                        vTaskSuspend(gameover); 
                    }
                     if (DemoTask1) {
                        vTaskSuspend(DemoTask1);
                    }
                    break;

                default:
                    break;
            }
            state_changed = 0;
        }
    }
}

/*
 * Changes the state, either forwards of backwards
 */
void changeState(volatile unsigned char *state, unsigned char forwards)
{
    switch (forwards) {
        case NEXT_TASK:
            if (*state == STATE_COUNT - 1) {
                *state = 0;
            }
            else {
                (*state)++;
            }
            break;
        case PREV_TASK:
            if (*state == 0) {
                *state = STATE_COUNT - 1;
            }
            else {
                (*state)--;
            }
            break;
        default:
            break;
    }
}
