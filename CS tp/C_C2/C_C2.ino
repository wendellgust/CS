#include <Arduino_FreeRTOS.h>
#include <MultiFunctionShield.h>
void task1(void *pvParameters);
void task2(void *pvParameters);
MultiFunctionShield MFS;

void taskButton1 (void *pvParameters);
void taskButton2 (void *pvParameters);

void setup() {
  MFS.begin();

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);

  Serial.begin(9600);

  xTaskCreate(task1, "Task1", 128, NULL, 1, NULL);
  xTaskCreate(task2, "Task2", 128, NULL, 1, NULL);
  xTaskCreate(taskButton1, "Button1", 128, NULL, 1, NULL);
  xTaskCreate(taskButton2, "Button2", 128, NULL, 1, NULL);

  vTaskStartScheduler();
}

void task1(void *pvParameters){
  int stateLED = HIGH;
  TickType_t lastWakeTime = xTaskGetTickCount();
  
  while (1) {
    long timeInitial = micros();
    digitalWrite(LED_1_PIN, stateLED);
    stateLED = !stateLED;
    //unexpected processing load
    vTaskDelay(random(50,500)/portTICK_PERIOD_MS);
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000));
    Serial.print ("Task 1: ");
    Serial.println (micros() - timeInitial);
  }
}

void task2(void *pvParameters){
  int stateLED = HIGH;
  TickType_t lastWakeTime = xTaskGetTickCount();
    while (1) {
    long timeInitial = micros();
    digitalWrite(LED_2_PIN, stateLED);
    stateLED = !stateLED;
    //unexpected processing load
    vTaskDelay(random(50,500)/portTICK_PERIOD_MS);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    Serial.print ("Task 2: ");
    Serial.println (micros() - timeInitial);
  }
}

void loop() {
}