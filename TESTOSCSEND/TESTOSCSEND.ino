// Yes, this is actually -*-c++-*-
/*
    Make an Keypad Lock and send OSC message over UDP with sucessful code entry
 */

#include <OSCMessage.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include "./Keypad.h"
#include "./Timer.h"

// Codes
char codes[3][4] = {
  {'1', '2', '3', '4'},
  {'2', '3', '4', '5'},
  {'3', '4', '5', '6'}
};

// Timeout for OSC messages
#define TIMEOUT 4000

//the Arduino's IP
IPAddress ip(192, 168, 52, 3);
//destination IP
IPAddress outIp(192, 168, 52, 222);
const unsigned int outPort = 8000;


// ==========================================================================================
//   KEYPAD CREATE
// ==========================================================================================

const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns

// Define the Keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'#', '0', '*'}
};

// ==========================================================================================
//   PIN CONFIG
// ==========================================================================================

// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 9, 8, 7, 6 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { A2, A1, A0 };

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

Timer timer;
int8_t timers[3] = {-1,-1,-1};

int redled =  3;
int greedled = 4;

// ==========================================================================================
//   KEYPAD CODES
// ==========================================================================================

const int historySize = 10;
char keyHistory[historySize]; //allows 10 key presses to be stored

char codearray[4] {'x', 'x', 'x', 'x'} ;

int indexKeypress = 0;





EthernetUDP Udp;


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
}; // you can find this written on the board of some Arduino Ethernets or shields




void setup() {

  for (int i = 0; i < historySize; i++) keyHistory[i] = 0;

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  int i, t;

  Ethernet.begin(mac, ip);
  Udp.begin(9999);
  for (t = 3; t < 5; t++) pinMode(t, OUTPUT); //set led pins to output
}


void loop()
{
  timer.update();
  codemaker();
}


// ==========================================================================================
//   OSC MESSAGES
// ==========================================================================================
OSCMessage msg1("/toto/1");
OSCMessage msg2("/toto/2");
OSCMessage msg3("/toto/3");
OSCMessage *msgs[3] = {&msg1, &msg2, &msg3};

void toto(int id) {
  if (id >=0 && id < 3) {
        msgs[id]->add(1);
        Udp.beginPacket(outIp, outPort);
        msgs[id]->send(Udp);
        Udp.endPacket();
        msgs[id]->empty(); // free space occupied by message
        timers[id] = timer.after(TIMEOUT, timeout, (void *)id);
  }
}

void timeout(void *context) {
  int id = (int)context;
  timer.stop(timers[id]);
  msgs[id]->add(0);
  Udp.beginPacket(outIp, outPort);
  msgs[id]->send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msgs[id]->empty(); // free space occupied by message
}
// ==========================================================================================
//   RIGHT ANSWER
// ==========================================================================================

void winner ()

{
  for (int i = 0; i < 5; i++) {
    digitalWrite(greedled, HIGH);
    delay (100);
    digitalWrite(greedled, LOW);
    delay (100);
  }
}


// ==========================================================================================
//   WRONG ANSWER
// ==========================================================================================

void failcode ()

{
  for (int i = 0; i < 10; i++) {
    digitalWrite (redled, HIGH);
    delay (50);
    digitalWrite (redled, LOW);
    delay (50);
  }
}

// ==========================================================================================
//   CODE BAIT & SWITCH
// ==========================================================================================

void codematcher () {
  for (int i = 0; i < 3; i++) { // # valid codes
    int j;
    for (j = 0; j < 4; j++) { // code digits
      if (codes[i][j] != keyHistory[(indexKeypress + historySize - 4 + j)%historySize]) break;
    }
    if (j == 4) {
      Serial.println("Winner");
      toto(i);
      winner();
      return;
    }
  }
  Serial.println("Fail");
  failcode();
}

// ==========================================================================================
//   INPUT CODE
// ==========================================================================================

void acceptInput(int key) {
  keyHistory[indexKeypress] = key;
  indexKeypress = (indexKeypress + 1) % historySize;
  digitalWrite(greedled, HIGH);
  digitalWrite(redled, HIGH);
  delay(50);
  digitalWrite(greedled, LOW);
  digitalWrite(redled, LOW);
}

void codemaker ()

{
  char key = kpd.getKey();
  if (key) {  // Check for a valid key.
    Serial.print("key pressed: ");
    Serial.println(key);

    switch (key)
    {
      case '*':
      case '#':
        codematcher ();
        break;
      default:
        acceptInput(key);
    }

  }
}





