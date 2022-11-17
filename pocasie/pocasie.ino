
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


const char* ssid     = "Your Network Name";
const char* password = "Your Network Password";
String APIKEY = "Your API key";
String CityID = "3058780";          //ID Mesta
bool id = false;
WiFiClient client;
char servername[] = "api.openweathermap.org";            // pripojenie na API
String result;


void setup() {


   Serial.begin(115200);
    Wire.begin(5, 4);

  WiFi.mode(WIFI_STA);   //Pripojenie na wifi
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Addresa 0x3D pre 128x64
  delay(200);
  display.clearDisplay();
  display.setTextSize(1);             // Normálna 1:1 škála veľkosti pixelov
  display.setTextColor(SSD1306_WHITE);        // Biely text
  display.setCursor(0, 0);            // Začne vľavo hore
  display.print("Pripaja sa k Wi-Fi.");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }

  Serial.println("");
  Serial.println("IP addresa: ");
  Serial.println(WiFi.localIP());
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Pripojene ");
  display.println("IP Addresa: ");
  display.println(WiFi.localIP());
  display.display();
  delay(1000);
  display.clearDisplay();

}

void loop()                    
{
  if (client.connect(servername, 80))
  { //Pripojenie na API
    client.println("GET /data/2.5/weather?id=" + CityID + "&units=metric&APPID=" + APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Pripojenie ukoncene");
    client.println();
  }
  else {
    Serial.println("nepodarilo sa pripojit");        //chybná správa, keď sa nepripojí
    Serial.println();
  }

  while (client.connected() && !client.available())
    delay(1);                                          //čakanie na dáta
  while (client.connected() || client.available())
  {
    char c = client.read();
    result = result + c;
  }

  client.stop();                                      //stop
  result.replace('[', ' ');
  result.replace(']', ' ');
  //Serial.println(result);
  char jsonArray [result.length() + 1];
  result.toCharArray(jsonArray, sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  StaticJsonDocument<1024> doc;
  DeserializationError  error = deserializeJson(doc, jsonArray);


  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }

  String location = doc["name"];
  String country = doc["sys"]["country"];
  int temperature = doc["main"]["temp"];
  int humidity = doc["main"]["humidity"];
  float pressure = doc["main"]["pressure"];
  int id = doc["id"];
  float Speed = doc["wind"]["speed"];

  display.clearDisplay();
  display.setCursor(0, 0);            //Začne vypisovať vľavo hore
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.print(" Poloha: ");
  display.print(country);
  display.print(" ");
  display.println(location);
  
  display.println();
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.print("Teplota: ");
  display.print(temperature);
  display.print((char)247);          //stupen celzia znak
  display.print("C");
  display.println();
  display.print("Vlhkost: ");
  display.print(humidity);
  display.println("%");
  display.print("Tlak: ");
  display.print(pressure);
  display.println(" hpa");
  display.print("Vietor: ");
  display.print(Speed);
  display.print(" m/s");
  display.println();
  display.println();
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.print("     Marek Guran     ");
  



  display.display();
  
  delay(600000);     //Obnovenie dát každých 10 minút

}
