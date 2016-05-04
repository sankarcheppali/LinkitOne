// LinkIt One sketch for MQTT Demo
#define __LINKIT_ONE_DEBUG__

#include <LGPRS.h>
#include <LGPRSClient.h>
#include <stdlib.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define chPrefix "/nodemcu/linkit50/v01"
#define chToDevice "/tod"
#define chAnalog "/analog"
#define chFromDevice "/fromd"
#define mqttuser "mqtt user name" //replace with your mqtt user name
#define mqttpwd "mqtt user password" //replace with your mqtt user password


/*
	Modify to your MQTT broker - Select only one
*/
char mqttBroker[] = "m10.cloudmqtt.com";
LGPRSClient c;
PubSubClient client( c );
unsigned long lastSend;
void InitLGPRS()
{
  // Keep retrying until connected to GPRS
  Serial.println("Connecting to GPRS");
  while (0 ==  LGPRS.attachGPRS()) {
    delay(1000);
  }
  Serial.println("Connected to GPRS");
  delay(10000);
  //wait for some time here
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    // Attempt to connect
    if ( client.connect("linkitone1",mqttuser,mqttpwd) ) {	// Better use some random name
      Serial.println( "[DONE]" );
      // Subscribe to topic "inTopic"
       char inTopics[100];
       (String(chPrefix)+chToDevice).toCharArray( inTopics, sizeof(inTopics)) ;
       Serial.print("Subscribed to ->");
       Serial.println(inTopics);
      client.subscribe(inTopics );
    }
    else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 10 seconds]" );
      // Wait 5 seconds before retrying
      delay( 10000 );
    }
  }
}


void setup()
{
   delay( 10000 );
   Serial.begin( 115200 );
   InitLGPRS();
   client.setServer( mqttBroker, 16019 );
   client.setCallback( callback );
   lastSend = 0;
}

void loop()
{
  if( !client.connected() ) { //check if we are connected or not,if not connect again
    reconnect();
  }
  if( millis()-lastSend > 50000 ) {	// Send an update only after 5 seconds
    sendAnalogData();
    lastSend = millis();
  }
  client.loop();
}

void callback( char* topic, byte* payload, unsigned int length ) {
  Serial.print( "Recived message on Topic:" );
  Serial.print( topic );
  Serial.print( "    Message:");
char payloadC[length];
  for (int i=0;i<length;i++) {
    payloadC[i]=(char)payload[i];
    Serial.print( (char)payload[i] );
  }
    Serial.println();
}

void sendAnalogData() {
  // Read data to send
  int data_A0 = analogRead( A0 );
  int data_A1 = analogRead( A1 );
  int data_A2 = analogRead( A2 );

 //Prepare JSON String
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["A0"]=int(data_A0);
    root["A1"]=int(data_A1);
  // Send payload
  char buffer1[200];
  root.printTo(buffer1, sizeof(buffer1));
  //build channel name
  char topic[100];
  (String(chPrefix)+chFromDevice+chAnalog).toCharArray( topic, sizeof(topic));
  client.publish(topic, buffer1 );
  Serial.println("Date Sent:");
  root.printTo(Serial);

}
