
/*******************************************
 * projet 12 : knock lock
 * auteur: Florent poiron
 * utilisation: a la premiere utilisation de la serrure, une code secret de 3 coup sera enregistré
 *              et pour les utilisation suivante seule l'utilisation de se code deverouillera la serrure.
 *              Ce code est stocké en eeprom, donc le redemarrage de l'arduino ne remet pas a zero le code.
 */

#include <Servo.h>
#include <EEPROM.h>

Servo myServo;

const int piezo = A0;     
const int switchPin = 2;   
const int yellowLed = 3;    
const int greenLed = 4;    
const int redLed = 5;   


int knockVal;

int switchVal;


const int quietKnock = 40;
const int loudKnock = 100;

boolean locked = false;

int numberOfKnocks = 0;
boolean checkForKnock(int value);

boolean checkInterval(unsigned long code[], unsigned long interval[], int nbInterval);
void EEPROMWritelong(int address, long value);

long EEPROMReadlong(long address);

unsigned long intervalCode[2];
unsigned long intervalKnock[2];
int i =0;
unsigned long firstKnock;

boolean firstTime = true;
void setup() {

  myServo.attach(9);

  pinMode(yellowLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  pinMode(switchPin, INPUT);


  Serial.begin(9600);

  digitalWrite(greenLed, HIGH);

  myServo.write(0);

  Serial.println("the box is unlocked!");

  if(EEPROM.read(420)==1){
    firstTime=false;
    intervalCode[0] = EEPROMReadlong(400);
    intervalCode[1] = EEPROMReadlong(410);
    Serial.println("code existant");
    Serial.println(intervalCode[0]);
    Serial.println(intervalCode[1]);
  }

  
}

void loop() {

  if (locked == false) {


    switchVal = digitalRead(switchPin);


    if (switchVal == HIGH) {

      locked = true;


      digitalWrite(greenLed, LOW);
      digitalWrite(redLed, HIGH);


      myServo.write(90);

      Serial.println("the box is locked!");

      delay(1000);
    }
  }

  if (locked == true) {

    knockVal = analogRead(piezo);




    if(numberOfKnocks == 0 && knockVal > 0){
       if (checkForKnock(knockVal) == true) {
         firstKnock = millis();
        Serial.println(firstKnock);
        numberOfKnocks++;
      }
    }else{


    if (numberOfKnocks > 0 && numberOfKnocks < 3 && knockVal > 0) {


      if (checkForKnock(knockVal) == true) {
        unsigned long currentTime = millis();
          intervalKnock[i] = currentTime-firstKnock;
            
          Serial.print("nouveau interval : ");
          Serial.println(intervalKnock[i]);
          Serial.print("current time :");
          Serial.println(currentTime);
          i++;
          numberOfKnocks++;
        }

       // Serial.print(3 - numberOfKnocks);
        //Serial.println(" more knocks to go");
      }
    }

    if (numberOfKnocks >= 3) {

    
    if(checkInterval(intervalKnock,intervalCode,i)){
      locked = false;
  

      myServo.write(0);

      delay(20);


      digitalWrite(greenLed, HIGH);
      digitalWrite(redLed, LOW);
     // Serial.println("the box is unlocked!");
    }
    if(firstTime == true){
      intervalCode[0]=intervalKnock[0];
      intervalCode[1]=intervalKnock[1];

      locked = false;

      myServo.write(0);

      delay(20);


      digitalWrite(greenLed, HIGH);
      digitalWrite(redLed, LOW);
     // Serial.println("the box is unlocked!");
      firstTime=false;

      EEPROMWritelong(400,intervalCode[0]);
      EEPROMWritelong(410,intervalCode[1]);
      EEPROM.write(420,1);
      Serial.println("premiere fois");
      
    }
      i = 0;
      numberOfKnocks = 0;
 
    }
  }
}


boolean checkForKnock(int value) {

  if (value > quietKnock && value < loudKnock) {

    digitalWrite(yellowLed, HIGH);
    delay(50);
    digitalWrite(yellowLed, LOW);

    //Serial.print("Valid knock of value ");
    //Serial.println(value);

    return true;
  }

  else {

    //Serial.print("Bad knock value ");
    //Serial.println(value);

    return false;
  }
}

boolean checkInterval(unsigned long code[], unsigned long interval[], int nbInterval){

    boolean codeOk = false;
    
    for(int i = 0 ; i < nbInterval; i++){
      if(code[i]> (interval[i] - 100) && code [i]< (interval[i] + 100))
        codeOk=true;
      else
        codeOk = false;
    }
    Serial.println(codeOk);
    return codeOk;
}

//This function will write a 4 byte (32bit) long to the eeprom at
//the specified address to address + 3.
void EEPROMWritelong(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }

long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }

