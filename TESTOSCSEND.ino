#include <OSCMessage.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    
#include <Keypad.h>

/*
    Make an Keypad Lock and send OSC message over UDP with sucessful code entry
    
 */

// ==========================================================================================
//   KEYPAD CREATE
// ==========================================================================================



const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns

// Define the Keymap
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};

// ==========================================================================================
//   PIN CONFIG
// ==========================================================================================

// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 9, 8, 7, 6 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 12, 11, 10 }; 

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );



int redled =  3;
int greedled = 4;

// ==========================================================================================
//   KEYPAD CODES
// ==========================================================================================

char keyHistory[4] {'z','z','z','z'} ; //allows 10 key presses to be stored

char codearray[4] {'x','x','x','x'} ;

int indexKeypress; // 



char code1[4] = {'1','2','3','4'};
char code2[4] = {'2','3','4','5'};
char code3[4] = {'3','4','5','6'};



EthernetUDP Udp;

//the Arduino's IP
IPAddress ip(128, 32, 122, 252);
//destination IP
IPAddress outIp(128, 32, 122, 125);
const unsigned int outPort = 9999;

 byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // you can find this written on the board of some Arduino Ethernets or shields




void setup() {

   int i, t;
    
    Ethernet.begin(mac,ip);
    Udp.begin(8888);

  for(i=6; i<13; i++)
    pinMode(i, INPUT); //set keypad pins to input
  
  for(t=3; t<5; t++)
    pinMode(t, OUTPUT); //set led pins to output

}


void loop(){

   while (true)

  codemaker();
  
}


// ==========================================================================================
//   OSC MESSAGES
// ==========================================================================================

void toto1 ()

{
  OSCMessage msg("/toto/1"); //the message wants an OSC address as first argument
  Udp.beginPacket(outIp, outPort);
    msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message

  delay(20);
}

void toto2 ()

{
  OSCMessage msg("/toto/2");
  Udp.beginPacket(outIp, outPort);
    msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message

  delay(20);
}

void toto3 ()

{
  OSCMessage msg("/toto/3");
  Udp.beginPacket(outIp, outPort);
    msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message

  delay(20);
}

// ==========================================================================================
//   RIGHT ANSWER
// ==========================================================================================

void winner ()

{
  digitalWrite(greedled, HIGH);
  delay (100);
  digitalWrite(greedled, LOW);
  delay (100);
  digitalWrite(greedled, HIGH);
  delay (100);
  digitalWrite(greedled, LOW);
}


// ==========================================================================================
//   WRONG ANSWER
// ==========================================================================================

void failcode ()

{
  digitalWrite (redled, HIGH);
  delay (50);
  digitalWrite (redled, LOW);
  delay (50);
  digitalWrite (redled, HIGH);
  delay (50);
  digitalWrite (redled, LOW);
  delay (50);
  digitalWrite (redled, HIGH);
  delay (50);
  digitalWrite (redled, LOW);
  delay (50);
}

// ==========================================================================================
//   CODE BAIT & SWITCH
// ==========================================================================================




void codematcher () {

  strncpy(codearray, keyHistory, 4);

  if (codearray == code1)
    {
      toto1();
      winner();
    }

  else
  
  if (codearray == code2)

    {
      toto2();
      winner();
    }

  else
  
  if (codearray == code3)

    {
      toto3();
      winner();
    }

  else

    {
      failcode();
    }


  
}

// ==========================================================================================
//   INPUT CODE
// ==========================================================================================

void codemaker ()

{
  char key = kpd.getKey();
  if(key)  // Check for a valid key.
  {
    switch (key)
    {
      case '*':
        break;
      case '#':
        break;
      default:
        indexKeypress=0; //index to first
      while (indexKeypress <4) 
      {
      keyHistory[indexKeypress]=kpd.getKey();
      if (indexKeypress == 3)
      codematcher ();
      }
    }
  }
}





