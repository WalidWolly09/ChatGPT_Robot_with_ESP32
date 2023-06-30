//Code for ChatGPT robot using ESP-32 project 
//made by Walid M. 
//please give credits if you want to use or modify the code for projects that will be shared online.


#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ChatGPT.hpp>
#include <BluetoothSerial.h>

static const char *ssid = "Your WI-FI's SSID";
static const char *password = "Your Wi-Fi Password";
WiFiClientSecure client;
ChatGPT<WiFiClientSecure> chat_gpt(&client, "v1", "Your API Key for ChatGPT ");
String result;
boolean newData = false;

BluetoothSerial SerialBT;

const byte numChars = 32;
char receivedChars[numChars];          //first type of data, which will be a text
char receivedCharForMoving;  //second type of data which Can be : F, B, R, L, S

const int MR1 = 12; //ESP32 pins (MR=Right Motor) (ML=Left Motor) (1=Forward) (2=Backward)
const int MR2 = 14; 
const int ML1 = 27;
const int ML2 = 26;

void setup() {
  //______________________________WiFi______________________________________________
  delay(1000);
  Serial.begin(115200);
  delay(1000);
  Serial.print("Connecting to WiFi network: ");
  Serial.print(ssid);
  Serial.println("'...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting...");
    delay(500);
  }
  Serial.println("Connected to Wifi");
  // Ignore SSL certificate validation
  client.setInsecure();
  //
   SerialBT.begin("ESP32 Walid");

  
  //______________________________Motors______________________________________________
  pinMode(MR1, OUTPUT); 
  pinMode(MR2, OUTPUT);
  pinMode(ML1, OUTPUT);
  pinMode(ML2, OUTPUT);
}


void Forward(){
        //RIGHT MOTOR
      digitalWrite(MR1,HIGH);//MOVE FRONT
      digitalWrite(MR2,LOW); //MOVE BACK
      //LEFT MOTOR
      digitalWrite(ML1,LOW);//MOVE BACK
      digitalWrite(ML2,HIGH);//MOVE FRONT
}
void Backward(){
      digitalWrite(MR1,LOW);
      digitalWrite(MR2,HIGH);
      digitalWrite(ML1,HIGH);
      digitalWrite(ML2,LOW);
}
void Left(){
      digitalWrite(MR1,HIGH);
      digitalWrite(MR2,LOW);
      digitalWrite(ML1,HIGH);
      digitalWrite(ML2,LOW);
}
void Right(){
      digitalWrite(MR1,LOW);
      digitalWrite(MR2,HIGH);
      digitalWrite(ML1,LOW);
      digitalWrite(ML2,HIGH);
}
void Stop(){
      digitalWrite(MR1,LOW); 
      digitalWrite(MR2,LOW);
      digitalWrite(ML1,LOW); 
      digitalWrite(ML2,LOW); 
}

void loop() {
   //______________________________Moving Commands_____________________________________
   if (SerialBT.available()) {
   receivedCharForMoving =(char)SerialBT.peek();
    Serial.println(receivedCharForMoving);
    if(receivedCharForMoving == 'F')
    {   Serial.println("F");
      Forward();
       
    }
    if(receivedCharForMoving == 'B')
    {    Serial.println("B");
 
      Backward(); 
    }         
     if(receivedCharForMoving == 'L')
    {   Serial.println("L");

      Left();
    }        
    if(receivedCharForMoving == 'R')
    {   Serial.println("R");

      Right(); 
    }
    if(receivedCharForMoving == 'S')
    {  Serial.println("S");
      Stop();
    }
  }

    recvWithStartEndMarkers();
    showNewData();
}

//______________________________Get received question from BT device______________________
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (SerialBT.available() > 0 && newData == false) {
        rc = SerialBT.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}
//____________________________Send RESULT to the BT device___________________________
void showNewData() {
    if (newData == true) {
    Serial.println("[ChatGPT]");
    if (chat_gpt.simple_message("gpt-3.5-turbo-0301", "user", receivedChars, result)) {
        SerialBT.print(result);
        Serial.println(result);
        
    } else {
      Serial.println("===ERROR===");
      Serial.println(result); 
    }

        newData = false;
    }
}