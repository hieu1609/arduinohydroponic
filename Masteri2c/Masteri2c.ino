#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<string.h>

void recdata(char * tp, byte * conte, unsigned int length)
{
  String topic = String(tp);
  //byte -> char* -> String
  //remove ID
  int flag;
  for (int i = 0; i < topic.length(); i++) {
    if(topic.charAt(i) == '=') {
      flag = i;
    }
  }
  String newTopic = topic;
  newTopic.remove(0, flag + 1);
  String content = String((char*)conte);
  content.remove(length);
  //format message to send for UNO
  String data = newTopic + "," + content;
  Serial.println(data);
  char mess[20];
  data.toCharArray(mess,20);
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write(mess);
  Wire.endTransmission();  
}

WiFiClient client;
//server, port, [callback], client
PubSubClient MQTT("m24.cloudmqtt.com", 15217, recdata, client);

void setup() 
{
  Serial.begin(9600); /* begin serial for debug */
  //connect wifi
  WiFi.begin("MHHH","giahieu99");
  //check wifi
  while(1)
  {
    delay(100);
    if(WiFi.status()==WL_CONNECTED)
      break;
  }
  Serial.println("successfully connected wifi");
  
  //check connect server
  while(1)
  {
    delay(500);
    //connect with server
    if(MQTT.connect("ESP8266", "tmlgemnz", "7fub13-eRIeR"))
      break;
  }
  Serial.println("successfully connected server MQTT");
  //control relay module follow topic
  MQTT.subscribe("6=pump");
  MQTT.subscribe("6=ppm");
  MQTT.subscribe("6=waterIn");
  MQTT.subscribe("6=waterOut");
  MQTT.subscribe("6=mix");
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
}

void loop() 
{
  String s = "";
  MQTT.loop();
  //MQTT.publish("al13","1");
  Wire.beginTransmission(8); /* begin with device address 8 */


  Wire.requestFrom(8, 40); /* request & read data of size 13 from slave */
  while(Wire.available())
  {
    char c = Wire.read(); 
    if(c=='0'||c=='1'||c=='2'||c=='3'||c=='4'||c=='5'||c=='6'||c=='7'||c=='8'||c=='9'||c=='.'||c=='=')
      s+=c;
  }

  Serial.print(s);
  Serial.println();
  char buffer[40];
  s.toCharArray(buffer, 40);
  
  MQTT.publish("update",buffer);
  Wire.endTransmission();    /* stop transmitting */
  delay(5000);
} 
