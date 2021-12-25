
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <ESPmDNS.h>
#include <Update.h>


const char* ssid = "MyESP";
const char* password = "12345678";

const char* index_path = "/index.html";

const char* update_path = "/update.html";

const char* upload_firmware_path = "/uploadFW";
const char* upload_spiffs_path = "/uploadUI";

const char* current_path = "/CalCurrent.html";
const char* current_calib_path = "/calibratecurrent";

const char* voltage_path = "/CalVoltage.html";
const char* voltage_calib_path = "/calibratevoltage";

const char* Row_Template_path = "/RowTemplate.html";

AsyncWebServer server(80);

String CurrentContent()
{
  String page = SPIFFS.open(current_path).readString();
  String data = SPIFFS.open(Row_Template_path).readString();

  String result = "";

  for(int i = 0; i < 5; i++)
  {
    String temp = data;
    temp.replace("{ID}" , String(i));
    temp.replace("{VALUE}" , "10");

    result += temp;
    result += "\n";
  }
  
  page.replace("{DATA}" , result);
  return page;
}

String VoltageContent()
{
  String page = SPIFFS.open(voltage_path).readString();
  String data = SPIFFS.open(Row_Template_path).readString();

  String result = "";

  for(int i = 0; i < 1; i++)
  {
    String temp = data;
    temp.replace("{ID}" , String(i));
    temp.replace("{VALUE}" , "10");

    result += temp;
    result += "\n";
  }
  
  page.replace("{DATA}" , result);
  return page;
}

void serveResource()
{
  // Route to load style.css file
  server.on("/css/bootstarp.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/bootstarp.min.css", "text/css");
  });
  server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/style.css", "text/css");
  });
  server.on("/js/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/bootstrap.bundle.min.js", String());
  });
  server.on("/js/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/jquery.min.js", String());
  });
  server.on("/3.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/3.jpg", "image");
  });
}

void handleUploadFirmware(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  //Upload Start
  if(!index)
  {
    if (!Update.begin(UPDATE_SIZE_UNKNOWN , U_FLASH))
    { //start with max available size
      Update.printError(Serial);
    }
  }
  /* flashing firmware to ESP*/
  if (Update.write(data , len) != len)
  {
    Update.printError(Serial);
  }

  if(final)
  {
    if (Update.end(true))
    { //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", len);
    }
    else
    {
      Update.printError(Serial);
    }
  }
}

void handleUploadSPIFFS(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  //Upload Start
  if(!index)
  {
    if (!Update.begin(UPDATE_SIZE_UNKNOWN , U_SPIFFS))
    { //start with max available size
      Update.printError(Serial);
    }
  }
  /* flashing firmware to ESP*/
  if (Update.write(data , len) != len)
  {
    Update.printError(Serial);
  }

  if(final)
  {
    if (Update.end(true))
    { //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", len);
    }
    else
    {
      Update.printError(Serial);
    }
  }
}

void makeRoute()
{
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, index_path, String(), false, nullptr);
  });

  // Route for CurrentCalibration web page
  server.on(current_path, HTTP_GET, [](AsyncWebServerRequest *request)
  {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", CurrentContent());
    request->send(response);
  });

  // Route for VoltageCalibration web page
  server.on(voltage_path, HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", VoltageContent());
    request->send(response);
  });

  // Route for Update web page
  server.on(update_path, HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, update_path, String(), false, nullptr);
  });

  server.on(current_calib_path, HTTP_POST, [](AsyncWebServerRequest *request){

    if(request->args() <= 0)
      request->send(400 , "text/plain" , "Zero arguman");
    else
    {
      int j = 0;
      String strdata = request->arg(j);
      
      int id = 0;
      int value = 0;

      char char_array[strdata.length() + 1];
      strcpy(char_array, strdata.c_str());

      char delim[] = "\n";
      char *ptr = strtok(char_array, delim);
      int i = 0;

      while(ptr != NULL)
      {
        if(i == 0)
          id = atoi(ptr);
        else
          value = atoi(ptr);

        ptr = strtok(NULL, delim);
        i++;
      }

      if(value < 0 || id < 0)
        request->send(400 , "text/plain" , "Invalid input");

      request->send(200 , "text/plain" , "Succed");
    }
  });

  server.on(voltage_calib_path, HTTP_POST, [](AsyncWebServerRequest *request){

    if(request->args() <= 0)
      request->send(400 , "text/plain" , "Zero arguman");
    else
    {
      int j = 0;
      String strdata = request->arg(j);
      
      int id = 0;
      int value = 0;

      char char_array[strdata.length() + 1];
      strcpy(char_array, strdata.c_str());

      char delim[] = "\n";
      char *ptr = strtok(char_array, delim);
      int i = 0;

      while(ptr != NULL)
      {
        if(i == 0)
          id = atoi(ptr);
        else
          value = atoi(ptr);

        ptr = strtok(NULL, delim);
        i++;
      }

      if(value < 0 || id < 0)
        request->send(400 , "text/plain" , "Invalid input");

      request->send(200 , "text/plain" , "Succed");
    }
  });

  server.on(upload_firmware_path, HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain" , (Update.hasError() ? "FAIL" : "OK"));

    ESP.restart();
  } , handleUploadFirmware);

  server.on(upload_spiffs_path, HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain" , (Update.hasError() ? "FAIL" : "OK"));

    ESP.restart();
  } , handleUploadSPIFFS);

}

void startWebServer()
{
  makeRoute();
  
  serveResource();

  server.begin();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("url: http://");
  Serial.println(IP);

  if (!MDNS.begin(ssid))
  { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("ESP32 Ready ! v1.0");

  startWebServer();
}
 
void loop(){
  
}
