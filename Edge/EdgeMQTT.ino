#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "KADHIR";
const char* password = "hello@123";

const char* mqtt_server = "192.168.1.51";
const char *mqtt_username = "baskaran";
const char *mqtt_password = "tsip123";
const int mqtt_port = 1883;


const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0*60*60;  //UTC
//const long  gmtOffset_sec = 5.5*60*60;  //India Standard Time
const int   daylightOffset_sec = 0; //Day Light

const char* thingID = "com.toshiba.dt:gsme_001_001";

long lastMsg = 0;
WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  client.setServer(mqtt_server, mqtt_port);

while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("Public EMQX MQTT broker connected");
    } else {
        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(2000);
    }
}


}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void loop() {
  // put your main code here, to run repeatedly:
  client.loop();

  long nowt = millis();
  if (nowt - lastMsg > 5000) {
    struct tm timeinfo;
    time_t now;

    if(!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time");
    }
    else{

    }

    //    29-03-2024T00:00:00.123Z
    char dateTime[50];
    strcpy(dateTime,String(timeinfo.tm_year).c_str());
    strcat(dateTime,"-");
    strcat(dateTime,String(timeinfo.tm_mon).c_str());
    strcat(dateTime,"-");
    strcat(dateTime,String(timeinfo.tm_mday).c_str());
    strcat(dateTime,"T");
    strcat(dateTime,String(timeinfo.tm_hour).c_str());
    strcat(dateTime,":");
    strcat(dateTime,String(timeinfo.tm_min).c_str());
    strcat(dateTime,":");
    strcat(dateTime,String(timeinfo.tm_sec).c_str());
    strcat(dateTime,"Z");

    time(&now);

    lastMsg = nowt;
    StaticJsonDocument<200> doc;
    doc["src"] = "TEUR COMM HUB System Model";
    doc["dataprov"] = "GSME";
    doc["date"] = dateTime;
    doc["summ"] = random(200);
    doc["consump"] = random(10);
    doc["cost"] = random(150);
    doc["timeStamp"] = now;
    doc["thingId"] = thingID;

    char topic[50];
    strcpy(topic,"devices/");
    strcat(topic,thingID);
    // Convert the value to a char array
    char tempString[200];
    serializeJson(doc, tempString);
    Serial.print("Topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    Serial.println(tempString);
    client.publish(topic, tempString);
  }
}
