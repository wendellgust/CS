
#include <MultiFunctionShield.h>
#include <Arduino_FreeRTOS.h>

void taskBlink1 (void *pvParameters);
void taskPrint (void *pvParameters);

MultiFunctionShield MFS;

void setup() {
  MFS.begin();
  pinMode (LED_1_PIN, OUTPUT);

  Serial.begin(9600);

  xTaskCreate (taskBlink1, //Function
  "Led1", //Description
  128, //Stack "size"
  NULL, //Parameters for the function
  1, //Priority
  NULL); //Task handler

  xTaskCreate (taskPrint, "Print", 128, NULL, 1, NULL);
  
  vTaskStartScheduler();
}

void taskBlink1 (void *pvParameters) {
  while(1) {
    digitalWrite(LED_1_PIN, HIGH);
    vTaskDelay (500 / portTICK_PERIOD_MS);
    digitalWrite(LED_1_PIN, LOW);
    vTaskDelay (500 / portTICK_PERIOD_MS);
  }
}

void taskPrint (void *pvParameters) {
  int countTime = 0;
  while(1) {
    countTime++;
    Serial.println(countTime);
    vTaskDelay (1000 / portTICK_PERIOD_MS);
  }
}

void loop() {

}