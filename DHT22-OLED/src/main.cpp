#include <Arduino.h>
//library DHT22
#include <DHT22.h>
//library freeRTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//library OLED
#include <Wire.h>             
#include "SSD1306Wire.h"       
SSD1306Wire display(0x3c, 5, 4);  
SemaphoreHandle_t s1;

//define pin data for temp&hum sensor
#define pinDATA 32 

DHT22 dht22(pinDATA); 
 

void OLED(void *pvParameters)
{
  for(;;){
    //xSemaphoreTake(s1, portMAX_DELAY);
    Serial.println("\n task 1");
    display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "wa 7assan");
  display.display();
  vTaskDelay(1000/portTICK_PERIOD_MS);
  //xSemaphoreGive(s1);
  }
  
  

}
void temperature_hum(void *pvParameters){
  for(;;){
    //xSemaphoreTake(s1, portMAX_DELAY);
    Serial.println("\n task 2");
    float t = dht22.getTemperature();
  float h = dht22.getHumidity();

  if (dht22.getLastError() != dht22.OK) {
    Serial.print("last error :");
    Serial.println(dht22.getLastError());
  }

  Serial.print("h=");Serial.print(h,1);Serial.print("\t");
  Serial.print("t=");Serial.println(t,1);
    vTaskDelay(1000/portTICK_PERIOD_MS);
  //xSemaphoreGive(s1);
  }
  
  

}
void setup() {
  //initialisation OLED
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "SETUP...");
  display.display();
  s1 = xSemaphoreCreateBinary();
  xSemaphoreGive(s1);
  xTaskCreate(temperature_hum,"temperature_hum",10000,NULL,1,NULL);
  xTaskCreate(OLED,"OLED",10000,NULL,1,NULL);
  //xTaskCreatePinnedToCore();
  //xTaskCreatePinnedToCore(OLED, "OLED", 10000, NULL, 1, NULL,1);
  Serial.begin(115200);
  Serial.println("\n setup completed");
}

void loop() {


}
