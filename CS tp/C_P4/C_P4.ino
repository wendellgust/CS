#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <MultiFunctionShield.h>

void TaskLed (void *pvParameters);
void TaskButton (void *pvParameters);
void TaskDisplay (void *pvParameters);
void TaskBuzzer (void *pvParameters);

int counter=0;
MultiFunctionShield MFS;

SemaphoreHandle_t Sem;

void setup() {
  MFS.begin();

  pinMode (LED_1_PIN, OUTPUT);
  digitalWrite(LED_1_PIN,HIGH);

  Serial.begin (9600);

  xTaskCreate (TaskLed, "Led", 128, NULL, 1, NULL);
  xTaskCreate (TaskButton, "Button", 128, NULL, 1, NULL);
  xTaskCreate (TaskDisplay, "Display", 128, NULL, 1, NULL);
  xTaskCreate (TaskBuzzer, "Display", 128, NULL, 2, NULL);

  Sem = xSemaphoreCreateBinary();
  xSemaphoreGive(Sem);

  vTaskStartScheduler();
}

void TaskLed (void *pvParameters) {
  while(1)
  {
    digitalWrite(LED_1_PIN,LOW);
    vTaskDelay (500/ portTICK_PERIOD_MS);
    digitalWrite(LED_1_PIN,HIGH);
    vTaskDelay (500/ portTICK_PERIOD_MS);
  }
}

void TaskButton (void *pvParameters)
{
  while(1)
  {
    while (digitalRead(BUTTON_1_PIN));
    counter++;
    xSemaphoreGive(Sem);
    vTaskDelay (250/ portTICK_PERIOD_MS);
  }
}

void TaskDisplay (void *pvParameters)
{
  while(1)
  {
    MFS.Display(counter);
    vTaskDelay (250/ portTICK_PERIOD_MS);
  }
}
void TaskBuzzer (void *pvParameters)
{
  while(1)
  {
    if (xSemaphoreTake(Sem, portMAX_DELAY) == pdTRUE){
      if ((counter > 0) && (counter % 5 == 0))
      {
        digitalWrite(BUZZER_PIN, LOW);
        vTaskDelay (100/portTICK_PERIOD_MS);
        digitalWrite(BUZZER_PIN, HIGH);
      }
    }
    vTaskDelay (250/ portTICK_PERIOD_MS);
  }
}
void loop(){}