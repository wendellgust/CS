#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <MultiFunctionShield.h>

void TaskProd (void *pvParameters);
void TaskCons (void *pvParameters);

int sharedBuffer = 0;
SemaphoreHandle_t Full; //Binary semaphore
SemaphoreHandle_t Empty; //Binary semaphore

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
  
  Full = xSemaphoreCreateBinary(); //Create a binary semaphore
  Empty = xSemaphoreCreateBinary(); //Create a secound binary semaphore
  xSemaphoreGive(Empty);

  vTaskStartScheduler();
}
//Procucer task
void TaskProd (void *pvParameters) {
  while(1){
  if (xSemaphoreTake(Empty, portMAX_DELAY) == pdTRUE){
    sharedBuffer++;
    Serial.print(pcTaskGetName(xTaskGetCurrentTaskHandle()));
    Serial.print(": Buffer is ");
    Serial.println(sharedBuffer);
  }
  digitalWrite(LED_1_PIN,LOW);
  digitalWrite(LED_2_PIN,HIGH);
  xSemaphoreGive(Full);
  
  vTaskDelay (random(50,500)/ portTICK_PERIOD_MS);
  }
}

//Consumer task
void TaskCons (void *pvParameters){
  while(1){
    if (xSemaphoreTake(Full, portMAX_DELAY) == pdTRUE){
      sharedBuffer = 0;
      Serial.print(pcTaskGetName(xTaskGetCurrentTaskHandle()));
      Serial.print(": Nothing to consume.");
      Serial.println(sharedBuffer);
    }
    digitalWrite(LED_1_PIN, HIGH);
    digitalWrite(LED_2_PIN, LOW);
    xSemaphoreGive(Empty);
    vTaskDelay(random(50, 500)/portTICK_PERIOD_MS);;
  }
}

void loop(){
}
