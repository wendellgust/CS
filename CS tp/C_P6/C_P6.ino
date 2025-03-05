#include <Arduino_FreeRTOS.h>
#include <MultiFunctionShield.h>
#include <queue.h>

void task1(void *pvParameters);
void task2(void *pvParameters);

MultiFunctionShield MFS;
QueueHandle_t queueNumber;

void setup() {
  MFS.begin();
  Serial.begin(9600);

  xTaskCreate(task1, "Task1", 128, NULL, 1, NULL);
  xTaskCreate(task2, "Task2", 128, NULL, 1, NULL);

  queueNumber = xQueueCreate(1, sizeof(int)); //create the queue

  vTaskStartScheduler();
}

void task1(void *pvParameters){
  while (1) {
    int numberSend = random (1,10);
    //Send data from the queue
    xQueueOverwrite(queueNumber, (void *)&numberSend);
    vTaskDelay(random(50,1000)/portTICK_PERIOD_MS);
  }
}

void task2(void *pvParameters)
{
int value=0;
  while (1) {
    int numberReceive = 0;
    //Receive data from the queue

    xQueueReceive(queueNumber, (void *)&numberReceive, portMAX_DELAY);
    value -= numberReceive;

    MFS.Display(value);
    vTaskDelay(50/portTICK_PERIOD_MS);
  }
}
void loop() {
}