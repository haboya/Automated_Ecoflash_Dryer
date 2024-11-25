
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
// #include <WiFiClient.h>

#define BELL_SERVER_NAME    "ecoflash_dryer"
#define USING_NODEMCU
ESP8266WebServer server(80);


bool motor_state;
float temperature;
float feeder;
uint8_t moisture;
String duration = "00:00:00";

bool appendFile(String dir, String text)
{
    File file = LittleFS.open(dir, "a");

    if (!file)
    {
        return false;
    }

    text += "\n";
    file.print(text);
    file.close();
    return true;
}

bool writeFile(String dir, String text)
{
    File file = LittleFS.open(dir, "w");

    if (!file)
    {
        return false;
    }

    file.print(text);
    delay(1);
    file.close();
    return true;
}

String getContentType(String filename)
{
    if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".gif"))
        return "image/gif";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".jpeg"))
        return "image/jpeg";
    return "text/plain";
}

bool handleFileRequests( String path )
{
    if (path.endsWith("/")){ path += "index.html"; }

    String contentType = getContentType(path);

    if (LittleFS.exists(path))
    {
        File file = LittleFS.open(path, "r");
        server.streamFile(file, contentType);
        file.close();
        return true;
    }

    return false;
}

void serveUpdates(void)
{
    String res = "{";
    res += "\"running\":" + String(motor_state) + ",";
    res += "\"temperature\":" + String(temperature) + ",";
    res += "\"moisture\":" + String(moisture) + ",";
    res += "\"feeding\":" + String(feeder) + ",";
    res += "\"uptime\":\"" + String(duration) + "\"}";

    #ifdef USING_NODEMCU
        Serial.println(res);
    #endif

    server.send(200, "text/plain", res);
}

bool bell_serverStop( void )
{
    server.stop();
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);

    return true;
}

bool bell_serverStart( void )
{
    WiFi.forceSleepWake();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(BELL_SERVER_NAME, ""); // Start the access point with no password
    MDNS.begin("ecoflash");     // Start the mDNS responder for Ahome.local
    
    server.on("/getstatus", HTTP_GET, serveUpdates);

    server.onNotFound([]() {                              // If the client requests any URI
        if (!handleFileRequests(server.uri()))                  // send it if it exists
        server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });

    server.begin();

    IPAddress ip = WiFi.softAPIP();
    char ipaddr[50] = {0};
    sprintf(ipaddr, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    #ifdef USING_NODEMCU
        Serial.println(ipaddr);
    #endif

    return true;
}

void setup()
{
    Serial.begin(9600);
    // LittleFS.format();
    LittleFS.begin();
    #ifdef USING_NODEMCU
        Serial.println("server init");
    #endif
    bell_serverStart();
}

bool prev_state = false;
unsigned long run_counts = 0;
unsigned long run_millis = 0;
void loop()
{
    if(prev_state != motor_state)
    {
        prev_state = motor_state;
        run_counts = 0;
    }
    server.handleClient();

    if((millis() - run_millis > 910) && (motor_state == true))
    {
        run_counts++;
        int hrs = run_counts/3600;
        int mns = (run_counts%3600)/60;
        int scs = (run_counts%3600)%60;
        duration = String(hrs) + ":" + String(mns) + ":" + String(scs);
        run_millis = millis();
    }
}

/// @brief read incoming bytes from atmega328p
void serialEvent()
{
  if(Serial.available())
  {
    String msg = "";
    char c = Serial.read();
    if( c == '*'){
      msg = Serial.readStringUntil('#');
      msg.trim();
      Serial.println(msg);

      String value = "";
      int count = 0;
      value = msg.substring(count, msg.indexOf(','));
      int new_val = value.toInt();
      // Serial.print("v1:");Serial.println(new_val);
      motor_state = (bool)new_val;

      count = msg.indexOf(',');
      // count = msg.indexOf(',', count+1);
      value = msg.substring(count+1);
      temperature = value.toFloat();

      count = msg.indexOf(',', count+1);
      value = msg.substring(count+1);
      moisture = value.toInt();
    }
    while(Serial.available()) c = Serial.read();
  } 
}
/* ----------------------------------- EOF ---------------------------------- */
