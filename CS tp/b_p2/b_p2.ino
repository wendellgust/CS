#include <MultiFunctionShield.h>
#include <Arduino_FreeRTOS.h>

void taskBlink1 (void *pvParameters);
void taskInput (void *pvParameters);
void taskDisplay (void *pvParameters);

long interval = 500;
int counter = 0;
int LED_state = LOW;
MultiFunctionShield MFS;

void setup() {
  MFS.begin();
  pinMode (LED_1_PIN, OUTPUT);
  Serial.begin(9600);

  xTaskCreate (taskBlink1, "LED", 128, NULL, 1, NULL);
  xTaskCreate (taskInput, "Input", 128, NULL, 1, NULL);
  xtaskCreate (taskDisplay, "Display", 128, NULL, 1, NULL);

  vTaskStartScheduler();
}

void taskBlink1 (void *pvParameters) {
  while(1) {
    digitalWrite(LED_1_PIN, HIGH);
    vTaskDelay (interval / portTICK_PERIOD_MS);
    digitalWrite(LED_1_PIN, LOW);
    vTaskDelay (interval / portTICK_PERIOD_MS);
  }
}

void taksDiplay(void *pvParameters) {
  while(1) {
    // capture rising edge (RE) of led
    if(digitalRead(LED_1_PIN) == HIGH && LED_state == LOW){
      counter++;
      LED_state = HIGH;
    }
    // capture falling edge (FE) of led
    if(digitalRead(LED_1_PIN) == LOW && LED_state == HIGH){
      counter++;
      LED_state = LOW;
    }
    MFS.Display(counter);

    /* Outro metodo que funciona, so nao é a melhore solução
    vTaskDelay(interval/ portTICK_PERIOD_MS);
    vTaskDelay(interval/ portTICK_PERIOD_MS);
    */

  }
}

void taskInput(void *pvParameters) {
  while(1) {
    if (Serial.available() > 0) {
      int temp = Serial.parseInt();
      if (temp > 0)
        interval = temp;
    } // close if
  } //  close while
} // close taskinput

void loop() {
//No code
}