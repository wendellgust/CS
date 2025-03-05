#include <MultiFunctionShield.h>
#include <Arduino_FreeRTOS.h>

void taskBlink1 (void *pvParameters);
void taskButton1 (void *pvParameters);

void taskBlink2 (void *pvParameters);
void taskButton2 (void *pvParameters);

//void taskBlink3 (void *pvParameters);
void taskButton3 (void *pvParameters);

long interval_1 = 800;
long interval_2 = 1000;

int stateBot3 = HIGH;
int multplier = 1;

MultiFunctionShield MFS;

void setup(){
    MFS.begin();

    pinMode (LED_1_PIN, OUTPUT);
    pinMode (LED_2_PIN, OUTPUT);
    pinMode (LED_3_PIN, OUTPUT);

    //LED_3_PIN == LOW;

    MFS.Display(interval_1);
    Serial.begin(9600);

    xTaskCreate (taskBlink1, "LED_1", 128, NULL, 1, NULL);
    xTaskCreate (taskButton1, "Input_1", 128, NULL, 1, NULL);
    xTaskCreate (taskBlink2, "LED_2", 128, NULL, 1, NULL);
    xTaskCreate (taskButton2, "Input_2", 128, NULL, 1, NULL);
    //xTaskCreate (taskBlink3, "LED_3", 128, NULL, 1, NULL);
    xTaskCreate (taskButton3, "Input_3", 128, NULL, 1, NULL);


    vTaskStartScheduler();
}

void taskBlink1 (void *pvParameters){
  while(1){
    digitalWrite(LED_1_PIN, HIGH);
    vTaskDelay (interval_1 / portTICK_PERIOD_MS);
    digitalWrite(LED_1_PIN, LOW);
    vTaskDelay (interval_1 / portTICK_PERIOD_MS);
  }
}

void taskButton1(void *pvParameters){
  while(1) {
    if(multplier == -1){
      while (digitalRead(BUTTON_1_PIN));
      interval_1 = interval_1 - 100;
      vTaskDelay (250 / portTICK_PERIOD_MS);
      MFS.Display(interval_1);
    }
    if(multplier == 1){
      while (digitalRead(BUTTON_1_PIN));
      interval_1 = interval_1 + 100;
      vTaskDelay (250 / portTICK_PERIOD_MS);
      MFS.Display(interval_1);
    }
  }
}

void taskBlink2(void *pvParameters){
  while(1){
    digitalWrite(LED_2_PIN, LOW);
    vTaskDelay (interval_2 / portTICK_PERIOD_MS);
    digitalWrite(LED_2_PIN, HIGH);
    vTaskDelay (interval_2 / portTICK_PERIOD_MS);
  }
}

void taskButton2(void *pvParameters){
  while(1){
    if(multplier == -1){
      while (digitalRead(BUTTON_2_PIN));
      interval_2 = interval_2 - 100;
      vTaskDelay (250 / portTICK_PERIOD_MS);
      //MFS.Display(interval_2);
    }
    if(multplier == 1){
      while (digitalRead(BUTTON_2_PIN));
      interval_2 = interval_2 + 100;
      vTaskDelay (250 / portTICK_PERIOD_MS);
      //MFS.Display(interval_2);
    }
  }
}

/*void taskBlink3 (void *pvParameters){
  while(1){
    if(stateBot3 == LOW) digitalWrite(LED_3_PIN, HIGH);
    if(stateBot3 == HIGH) digitalWrite(LED_3_PIN, LOW);
    vTaskDelay (250 / portTICK_PERIOD_MS); 
  }
}*/

void taskButton3(void *pvParameters){
  while(1){
    int value = multplier ? HIGH : LOW;
    digitalWrite(LED_3_PIN, value);
    while(digitalRead(BUTTON_3_PIN));
    multplier *= -1;
    vTaskDelay (250 / portTICK_PERIOD_MS);
  }
}

/*void taskButton3(void *pvParameters){
  while(1){
    if(digitalRead(BUTTON_3_PIN) == HIGH && stateBot3 == LOW){
      digitalWrite(LED_3_PIN, LOW);
    }
    if(digitalRead(BUTTON_3_PIN) == LOW && stateBot3 == HIGH){
      digitalWrite(LED_3_PIN, HIGH); 
      stateBot3 = !stateBot3;  
    }
    vTaskDelay (250 / portTICK_PERIOD_MS);
  }
}*/

void loop(){
}
