#include <Arduino.h>
#include <analogWrite.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <SPIFFSIO.cpp>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>


//define GPIO DC Motors
#define  pin_in1 14
#define  pin_in2 12
#define  pin_enm 13

#define  pin_in3 27
#define  pin_in4 26


int toPWM(int v){
   
		return map(v, 0,150,0,255);

}

// back motor cw retation with pwm 
void smovecw(int speed){
  
        analogWrite(pin_enm,toPWM(speed),255);
        digitalWrite(pin_in1, LOW);
        digitalWrite(pin_in2, HIGH);
        delay(10);
  }    

//back motor ccw rotation with pwm
  void smoveccw( int speed){
        analogWrite(pin_enm,toPWM(speed),255);
        digitalWrite(pin_in1, HIGH);
        digitalWrite(pin_in2, LOW);
       delay(10);
        
  }

// front motor cw rotation
  void dmovecw(){

        digitalWrite(pin_in3, LOW);
        digitalWrite(pin_in4, HIGH);
        delay(10);
  }    

// front motor ccw rotation
  void dmoveccw(){

        digitalWrite(pin_in3, HIGH);
        digitalWrite(pin_in4, LOW);
        delay(10);
  }
  // motors off
  void stop() {
                  
                  analogWrite(pin_enm,0);
                  digitalWrite(pin_in1, LOW);
                  digitalWrite(pin_in2, LOW);     
                  digitalWrite(pin_in3, LOW);
                  digitalWrite(pin_in4, LOW);
                  delay(10);
    }  


// initialize output pins
void initmotors(){

  pinMode(pin_enm, OUTPUT);
  pinMode(pin_in1, OUTPUT);
  pinMode(pin_in2, OUTPUT);
  pinMode(pin_in3, OUTPUT);
  pinMode(pin_in4, OUTPUT);

  analogWriteResolution(pin_enm, 10);
}

//define variable parse json
uint32_t _distance ;
const char * _angle;
const char * _type;
int _degre;

//rotation state
enum State {
        up,
        down,
        right,
        left,
        nothing
};

// convert char string to enum 

 State stringtoenum(const char *str){

  if (!strcmp(str,"up")) return up;
  if (!strcmp(str,"down")) return down;
  if (!strcmp(str,"right")) return right;
  if (!strcmp(str,"left")) return left;
  return nothing;
 }

 inline const char *stateToString(State val)
{
  switch (val)
  {
  case up:
    return "up";
  case down:
    return "down";
  case right:
    return "right";
  case left:
    return "left";
  default:
    return "nothing";
  }
}


// ACCESS POINT 
#define AP_NAME "RC_CAR"
#define AP_PASSWORD ""

//access point state
bool accessPointMode = true;
bool useAP = false;

// station mode variable
const char *ssid;
const char *password;


// AsyncWebServer object on port 80
AsyncWebServer server(80);







/** Scan for wifi access points and return how many found */
int wifiScan()
{
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0)
    Serial.println("No networks found");
  return n;
}

/** Read saved wifi settings
 * 'ssid' and 'password' files can be created manually to data folder for connecting to predefined wifi
 * */
void getSavedWifi()
{
  ssid = strdup(readString("/ssid").c_str());
  password = strdup(readString("/password").c_str());
  if (strcmp(ssid, "-1") != 0 && strcmp(password, "-1") != 0)
    accessPointMode = false;
}

/** Try to connect to wifi or create an access point */
bool setupWifi()
{

  if (accessPointMode)
  {
    Serial.print("Creating access point: ");
    Serial.print(AP_NAME);
    Serial.print(" - ");
    Serial.println(AP_PASSWORD);
    WiFi.softAP(AP_NAME, AP_PASSWORD);
  }
  else
  {
    // Try to connect
    WiFi.begin(ssid, password);
    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries <= 8)
    {
      Serial.println("Connecting to WiFi..");
      delay(1000);
      tries++;
    }

    // Failed, setup ap for configuring
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.print("WiFi connection failed");
      accessPointMode = true;
      WiFi.disconnect();
      setupWifi();
      return false;
    }
    else // successfully connected
    {
      accessPointMode = false;
      Serial.print("Current IP: ");
      Serial.print(accessPointMode ? WiFi.softAPIP() : WiFi.localIP());
    }
  }

  Serial.print("Current IP: ");
  Serial.print(accessPointMode ? WiFi.softAPIP() : WiFi.localIP());
  if (accessPointMode)
    delay(3500);
  return true;
}


bool setupDNS(){

  if(!MDNS.begin("ESP32CAR")){
  Serial.println("Error DNS");
  return true ;
}

}



void rotat() {



const State _state = stringtoenum(_angle);


if (strcmp(_type,"end") == 0)  {
  Serial.println("Stop");
     stop();

     } else 
     {
     switch (_state) {

        case up :
          if(_distance >=0 && _distance <=150 ){
                 
                smovecw(_distance);
                // write duty to LEDC
             
                } 
          break;

        case down :
        if(_distance >=0 && _distance <=150 ){
                
                smoveccw(_distance);
                // write duty to LEDC
                } 
        break;
        case right:

         if(_distance >=0 && _distance <=50 ){
           
         dmovecw();

       } else  if (_distance >=50 && _distance <=150 ){
               

              if(_degre >=0 && _degre <=45 ){
                

                dmovecw();
                smovecw(_distance);

              } else if (_degre >=315 && _degre <=359 ){
                
                dmovecw();
                smoveccw(_distance);
                }   
        }
        break;

        case left :

        if(_distance >=0 && _distance <=50 ){

        
         dmoveccw();

       } else  if (_distance >=50 && _distance <=150 ){
          
              if(_degre >=135 && _degre <=180 ){
                
                dmoveccw();
                smovecw(_distance);

              } else if (_degre >=180 && _degre <=225 ){
                

                dmoveccw();
                smoveccw(_distance);
                }   
        }

        break ;
    case nothing :
      stop();
      break;
     }
    } 
   } 
// TODO: Validate data received and add permission management / restriction for certain features
/** Setup URL routers and handlers for server */
void setupServer()
{
  // Pages
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (accessPointMode && !useAP)
     // request->send(SPIFFS, "/wifisetup.html", "text/html");
   // else
      request->send(SPIFFS, "/index.html", "text/html");
      else
      request->send(SPIFFS, "/index.html", "text/html");
  });
  // Pages
 

  
  // Cache static resources
  server.serveStatic("/css/materialize.min.css", SPIFFS, "/css/materialize.min.css", "max-age=3600");
  server.serveStatic("/css/style.css", SPIFFS, "/css/style.css", "max-age=3600");
  server.serveStatic("/js/materialize.min.js", SPIFFS, "/js/materialize.min.js", "max-age=3600");

  server.serveStatic("/js/jquery-3.5.0.min.js", SPIFFS, "/js/jquery-3.5.0.min.js", "max-age=3600");
  server.serveStatic("/js/setupwifi.js", SPIFFS, "/js/setupwifi.js", "max-age=3600");
  server.serveStatic("/js/nipplejs.min.js", SPIFFS, "/js/nipplejs.min.js", "max-age=3600");
  server.serveStatic("/js/raphael-2.1.4.min.js", SPIFFS, "/js/raphael-2.1.4.min.js", "max-age=3600");
  server.serveStatic("/js/justgage.js", SPIFFS, "/js/justgage.js", "max-age=3600");

  server.serveStatic("/js/speedbat.js", SPIFFS, "/js/speedbat.js", "max-age=3600");

  server.serveStatic("/js/init.js", SPIFFS, "/js/init.js", "max-age=3600");

  // Get current state of the machine as a json object, ie. all variables that can be changed using the json API
  server.on("/api/get-state", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &jsonObj = jsonBuffer.createObject();

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    jsonObj.printTo(*response);
    request->send(response);
  });

  // Get a single variable value from the machine
  AsyncCallbackJsonWebHandler *getValueHandler = new AsyncCallbackJsonWebHandler("/api/get-value", [](AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject &jsonObj = json.as<JsonObject>();


    /*
    if (getVariable(jsonObj["id"]) != NULL)
      jsonObj["v"] = *getVariable(jsonObj["id"]);
    else if (getPin(jsonObj["id"]) != -1)
      jsonObj["v"] = getPinValue(getPin(jsonObj["id"]));
    */
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    jsonObj.printTo(*response);
    request->send(response);
  });

  // Set a variable value of the machine
  AsyncCallbackJsonWebHandler *setValueHandler = new AsyncCallbackJsonWebHandler("/api/set-value", [&](AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject &jsonObj = json.as<JsonObject>();

    /*
    setValue(jsonObj["id"], jsonObj["v"]);
    if (getVariable(jsonObj["id"]) != NULL)
      jsonObj["v"] = *getVariable(jsonObj["id"]);
     */ 
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    jsonObj.printTo(*response);
    request->send(response);
  });

  // Start scanning access points
  server.on("/api/access-points", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    JsonArray &jsonAPs = json.createNestedArray("accessPoints");

    // Scan wifis and add to json
    int n = wifiScan();
    for (int i = 0; i < n; ++i)
    {
      JsonObject &accessPoint = jsonBuffer.createObject();
      accessPoint["SSID"] = WiFi.SSID(i);
      accessPoint["encryption"] = WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? false : true;
      jsonAPs.add(accessPoint);
    }
    json.printTo(*response);
    request->send(response);
  });

  // Set wifi parameters and try to to connect with them
  AsyncCallbackJsonWebHandler *setWifiHandler = new AsyncCallbackJsonWebHandler("/api/set-wifi", [&](AsyncWebServerRequest *request, JsonVariant &json) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");

    ssid = json["SSID"];
    password = json["password"];

    DynamicJsonBuffer jsonBuffer;
    JsonObject &returnJson = jsonBuffer.createObject();

    accessPointMode = false;
    if (setupWifi())
    {
      writeFile("/ssid", ssid);
      writeFile("/password", password);
      returnJson["info"] = "Connected to " + String(ssid) + " with IP: " + WiFi.localIP().toString();
    }
    else
    {
      returnJson["info"] = "Couldn't connect to " + String(ssid);
    }

    returnJson.printTo(*response);
    request->send(response);
  });

 //json from joydata joystick movements
  AsyncCallbackJsonWebHandler *controlHandler = new AsyncCallbackJsonWebHandler("/api/joydata", [&](AsyncWebServerRequest *request, JsonVariant &json) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");

    _distance = json["distance"];
   // Serial.println(_distance);
     _angle = json["direction"]["angle"];
   //Serial.println(_angle);
     _type = json["type"];

   // Serial.println(_type);
    _degre = json["angle"]["degree"];
    // Serial.println(_degre);


   //Driving
     rotat();

    DynamicJsonBuffer jsonBuffer;
    JsonObject &returnJson = jsonBuffer.createObject();

    if (strcmp(_type, "end") == 0) {
     returnJson["info"] = "Stop";
     returnJson["speed"] = 0 ;
     } else
     {
    returnJson["info"] = "Driving";
    returnJson["speed"] = _distance ;
     }

    returnJson.printTo(*response);
    request->send(response);
  });

  //server.addHandler(getValueHandler);
  //server.addHandler(setValueHandler);
  server.addHandler(setWifiHandler);
  server.addHandler(controlHandler);
}


void setup() {

  Serial.begin(115200);
  initmotors();
  initializeSPIFFS();
  getSavedWifi();
  setupWifi();
  setupDNS();
  setupServer();
  server.begin();
}

void loop() {
  
}