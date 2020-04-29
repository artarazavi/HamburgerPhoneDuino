//wifi
#include <SPI.h>
#include <WiFiNINA.h>
#include <Firebase_Arduino_WiFiNINA_HTTPCLient.h>
#include <Firebase_Arduino_WiFiNINA.h>
#include "arduino_secrets.h"

//keypad
#include <Keypad.h>


//Firebase settings
char firehost[] = FIREBASE_HOST; 
char fireauth[] = FIREBASE_AUTH;

//Wi-Fi settings
char ssid[] = SECRET_SSID; 
char pass[] = SECRET_PASS; 
int status = WL_IDLE_STATUS;

FirebaseData firebaseData;

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','D'},
  {'4','5','6','B'},
  {'7','8','9','A'},
  {'*','0','#','C'}
};

// connect the pins from right to left to pin 2, 3, 4, 5,6,7,8,9
byte rowPins[ROWS] = {5,4,3,2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9,8,7,6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
bool calling = false;
char inData[10];
int index;

void setup()
{
  Firebase.begin(firehost, fireauth, ssid, pass);
  Firebase.reconnectWiFi(true);

  Serial.begin(9600);

  sendKeyToFB("", "connection initiated");
}

void loop()
{ 
    char key = keypad.getKey();
    // just print the pressed key
   if (key){
    Serial.print("Key ");
    Serial.print(key);
    Serial.println(" is pressed");
    
    if(key == 'B'){
        Serial.println("calling begin");
        calling = true;
        index = 0;
        inData[index] = '\0';
        sendKeyToFB("", "dialing");
    }
    else if(key == 'C'){
        Serial.println("call initiated");
        calling = false;
        sendKeyToFB(inData, "in call");
        index = 0;
        inData[index] = '\0';  
    }
    else if(calling == true){
        inData[index] = key;
        index++;
        inData[index] = '\0';  
    }    
  } 
}

void sendKeyToFB(char key[], char eventStatus[])
{
  sendKeyToFBHelper(key, "/test/button_press");
  sendKeyToFBHelper(eventStatus, "/test/event_status");
}

void sendKeyToFBHelper(char key[], char path[])
{
  Serial.println("connecting to firebase");
  if (Firebase.setString(firebaseData, path, key)){
      
      //Success, then read the payload value return from server
      //This confirmed that your data was set to database as float number
    
      if (firebaseData.dataType() == "string")
        Serial.println(firebaseData.stringData());
    
    } else {
      //Failed, then print out the error detail
        Serial.println(firebaseData.errorReason());
    }
}
