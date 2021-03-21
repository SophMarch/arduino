/*
  Setting up IFTTT with MKR1000

  Describe what it does in layman's terms.  Refer to the components
  attached to the various pins.

  The circuit:
  * list the components attached to each input
  * list the components attached to each output

  Created day month year
  By author's name
  Modified day month year
  By author's name

  http://url/of/online/tutorial.cc

*/

#include "arduino_secrets.h"
#include <WiFi101.h>

//For Internet connection
const char* MY_SSID = SECRET_SSID;
const char* MY_PASS =  SECRET_PASS;
int status = WL_IDLE_STATUS;

//For IFTTT
WiFiClient IftttClient;
const char* MY_HOST = "maker.ifttt.com";
int  MY_PORT = 80;
const char* MY_EVENT = SECRET_EVENT;
const char* MY_API_KEY = SECRET_API_KEY;

//Sensors inputs management
int lightPin = A0;
int temperaturePin = A1;
float temperatureValueC;
int lightValue;
int frenquencyUpdateMs;

void setup() 
{
  //Initiate serial and wait for port to open:
  Serial.begin(9600);
  //while(!Serial);
  delay(2000);

  //Connect to Wifi
  Serial.print("Connecting Wifi... ");
  while(WiFi.begin(MY_SSID, MY_PASS) != WL_CONNECTED){
    Serial.print(". ");
    delay(500);  
  }
  Serial.print(WiFi.SSID());
  Serial.println(" connected!");
  Serial.println("");
}


void loop()
{
  temperatureValueC = get_temperatureC();
  lightValue = get_light();
  frenquencyUpdateMs = 1 * 60 * 1000; //update every x min
  send_values_to_itttf(temperatureValueC, lightValue, frenquencyUpdateMs);
}

/****************Helper functions*****************/

float get_temperatureC()
{
  int readPin = analogRead(temperaturePin);
  float voltage = readPin * 3.3 / 1024.0; //convert ADC reading to voltage - Vcc 3.3 for MKR1000 change if other board used
  float temperatureC = (voltage - 0.5) * 100; //remove offset and convert to degree Celsius 

  return temperatureC; 
}

int get_light()
{
  int light = analogRead(lightPin);

  return light;
}

void send_values_to_itttf(float temperatureC, int light, int frequencyMs)
{
  String _temperature = String(temperatureC);
  String _light = String(light);
 
  if (IftttClient.connect(MY_HOST, MY_PORT)) // Connect to server and send message
  {
    Serial.print("Connected to IFTTT, sending... ");
    IftttClient.println(String("GET ") + "/trigger/" + String(MY_EVENT) + "/with/key/" + String(MY_API_KEY) + "?value1=" + _temperature + "&value2=" + _light + " HTTP/1.1");
    IftttClient.println("Host: "+ String(MY_HOST));
    IftttClient.println("Connection: close");
    IftttClient.println();
    
    Serial.print("temperature in C: ");
    Serial.print(_temperature);
    Serial.print(" and light: ");
    Serial.println(_light);
    
    delay(frequencyMs);
    IftttClient.stop();  // Disconnect from the server
  }
  else 
  {
    Serial.println("Failed to connect to client");
  }
}
