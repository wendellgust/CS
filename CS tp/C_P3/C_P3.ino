#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <MultiFunctionShield.h>

void TaskProd (void *pvParameters);
void TaskCons (void *pvParameters);

int sharedBuffer = 0;
SemaphoreHandle_t binSem; //Binary semaphore

MultiFunctionShield MFS;

void setup() {
  MFS.begin();

  pinMode (LED_1_PIN, OUTPUT);
  pinMode (LED_2_PIN, OUTPUT);

  digitalWrite(LED_2_PIN,HIGH);
  digitalWrite(LED_2_PIN,HIGH);

  Serial.begin (9600);

  xTaskCreate (TaskProd, "Prod", 128, NULL, 1, NULL);
  xTaskCreate (TaskCons, "Cons", 128, NULL, 1, NULL);
  
  binSem = xSemaphoreCreateBinary(); //Create a binary semaphore

  vTaskStartScheduler();
}
//Procucer task
void TaskProd (void *pvParameters) {
  while(1){
  if (sharedBuffer == 0){
    sharedBuffer++;
    Serial.print(pcTaskGetName(xTaskGetCurrentTaskHandle()));
    Serial.print(": Buffer is ");
    Serial.println(sharedBuffer);
  }
  else{
    Serial.print(pcTaskGetName(xTaskGetCurrentTaskHandle()));
    Serial.println(": Nothing to produce.");
  }
  digitalWrite(LED_1_PIN,LOW);
  digitalWrite(LED_2_PIN,HIGH);
  xSemaphoreGive(binSem); //Release the semaphore
  vTaskDelay (random(50,500)/ portTICK_PERIOD_MS);
  }
}

//Consumer task
void TaskCons (void *pvParameters){
  while(1){
    if (xSemaphoreTake(binSem, portMAX_DELAY) == pdTRUE){
      if(sharedBuffer == 1){
        sharedBuffer = 0;
        Serial.print(pcTaskGetName(xTaskGetCurrentTaskHandle()));
        Serial.print(": NOthing to consume.");
        Serial.println(sharedBuffer);
      }
      else{
        Serial.print(pcTaskGetName(xTaskGetCurrentTaskHandle()));
        Serial.println(": Nothing to cunsume.");
      }
      digitalWrite(LED_1_PIN, HIGH);
      digitalWrite(LED_2_PIN, LOW);
      vTaskDelay(random(50, 500)/portTICK_PERIOD_MS);
    }
    vTaskDelay (250/ portTICK_PERIOD_MS);
  }
}

void loop(){
}
