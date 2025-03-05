#include <MultiFunctionShield.h>
#include <Arduino_FreeRTOS.h>

void taskBlink1 (void *pvParameters);
void taskButton1 (void *pvParameters);

void taskBlink2 (void *pvParameters);
void taskButton2 (void *pvParameters);

long interval_1 = 500;
long interval_2 = 500;

MultiFunctionShield MFS;

void setup() {
    MFS.begin();

    pinMode (LED_1_PIN, OUTPUT);
    pinMode (LED_2_PIN, OUTPUT);

    MFS.Display(interval_1);
    Serial.begin(9600);

    xTaskCreate (taskBlink1, "LED_1", 128, NULL, 1, NULL);
    xTaskCreate (taskButton1, "Input_1", 128, NULL, 1, NULL);
    xTaskCreate (taskBlink2, "LED_2", 128, NULL, 1, NULL);
    xTaskCreate (taskButton2, "Input_2", 128, NULL, 1, NULL);


    vTaskStartScheduler();
}

void taskBlink1 (void *pvParameters) {
  while(1)
  {
    digitalWrite(LED_1_PIN, HIGH);
    vTaskDelay (interval_1 / portTICK_PERIOD_MS);
    digitalWrite(LED_1_PIN, LOW);
    vTaskDelay (interval_1 / portTICK_PERIOD_MS);
  }
}

void taskButton1(void *pvParameters) {
  while(1) {
    while (digitalRead(BUTTON_1_PIN));
    interval_1 = interval_1 + 100;
    vTaskDelay (250 / portTICK_PERIOD_MS);
    MFS.Display(interval_1);
  }
}

void taskBlink2(void *pvParameters) {
  while(1)
  {
    digitalWrite(LED_2_PIN, HIGH);
    vTaskDelay (interval_2 / portTICK_PERIOD_MS);
    digitalWrite(LED_2_PIN, LOW);
    vTaskDelay (interval_2 / portTICK_PERIOD_MS);
  }
}

void taskButton2(void *pvParameters) {
  while(1) {
    while (digitalRead(BUTTON_2_PIN));

    interval_2 = interval_2 + 100;
    vTaskDelay (250 / portTICK_PERIOD_MS);
    //MFS.Display(interval_2);
  }
}


void loop(){
}
