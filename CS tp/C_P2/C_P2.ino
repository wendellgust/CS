#include <Arduino_FreeRTOS.h>
#include <MultiFunctionShield.h>
#include <semphr.h>

int var_global = 0;
SemaphoreHandle_t mutex; // Declare a mutex

MultiFunctionShield MFS;

void TaskCounter (void *pvParameters);
void taskButton1 (void *pvParameters);

void setup() {

  MFS.begin();
  
  MFS.Display(var_global);

  Serial.begin(9600);
  // Create mutex
  mutex = xSemaphoreCreateMutex();

  xTaskCreate(TaskCounter, "Task1", 128, NULL, 1, NULL);
  xTaskCreate(TaskCounter, "Task2", 128, NULL, 1, NULL);
  xTaskCreate (taskButton1, "Input_1", 128, NULL, 1, NULL);

  vTaskStartScheduler();
}

void TaskCounter (void *pvParameters){
  char *taskName = pcTaskGetName(NULL);

  while(1)
  {
    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
      int var_local = var_global;
      var_local++;
      var_global = var_local;
      MFS.Display(var_global);
      Serial.println(taskName + String(": ") + var_local);
      // Release the mutex after accessing var_global
      xSemaphoreGive(mutex);
    }

    vTaskDelay(random(50,500) / portTICK_PERIOD_MS);
  }
}

void taskButton1(void *pvParameters){
  while(1) {
    while (digitalRead(BUTTON_1_PIN));
    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
      var_global = var_global + 100;
      xSemaphoreGive(mutex);  
    }
    vTaskDelay(250/portTICK_PERIOD_MS); 
  }
}

void loop() {
}