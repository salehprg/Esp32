#include <Arduino.h>

const int led2 = 2; // Pin of the LED
const int led17 = 17; // Pin of the LED

void toggleLED(void * parameter){
  while(true){ // infinite loop

    Serial.print("Task is running on: ");
    Serial.println(xPortGetCoreID());
    // Turn the LED on
    digitalWrite(led2, HIGH);

    // Pause the task for 500ms
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // Turn the LED off
    digitalWrite(led2, LOW);

    // Pause the task again for 500ms
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {

  Serial.begin(115200);
  Serial.print("Startup is running on: ");
  Serial.println(xPortGetCoreID());

  pinMode(led2, OUTPUT);
  pinMode(led17, OUTPUT);

  xTaskCreatePinnedToCore(
    toggleLED,    // Function that should be called
    "Toggle LED",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL ,        // Task handle
    0           
  );

}

void loop() {
  digitalWrite(led17, LOW);
  Serial.println("Message From Loop");

  delay(150);
  digitalWrite(led17, HIGH);
  delay(150);

}