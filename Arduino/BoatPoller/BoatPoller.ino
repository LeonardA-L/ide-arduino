/*
  Web client

 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen

 */

#include <SPI.h>
#include <Ethernet.h>

// LED Related Stuff
int redPin = 5;
int greenPin = 6;
int bluePin = 3;

int frame = 100;
int maxbrightness = 30;

// Tweet Related Stuff
String lastTweetId = "NO TWEET YET";
String tweetBuffer = "";

// Ethernet related stuff
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "92.222.89.148";    // IP of the server  
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
EthernetClient client;


// Setup phase
void setup() {
  // Initialize GPIOs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Getting DHCP...");

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // Start fetching last tweet
  getTweetId();
  // Reset GPIOs
  resetLeds();
}

// This function launches a HTTP request to the server to fetch the ID of the last interesting tweet
// Picking interesting tweets is all done on the server side
void getTweetId() {
  //Serial.println("Polling");
  tweetBuffer = "";
  if (client.connect(server, 3030)) {
    //Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /api HTTP/1.1");
    client.println("Host: 92.222.89.148");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

// Reset GPIOs
void resetLeds() {
  analogWrite(redPin, 0.0f);
  analogWrite(greenPin, 0.0f);
  analogWrite(bluePin, 0.0f);
}

// This function makes two leds blink alternatively
void blinkLeds(int c1, int c2) {
  resetLeds();
  analogWrite(c1, maxbrightness);
  delay(frame);
  resetLeds();
  analogWrite(c2, maxbrightness);
  delay(frame);
  resetLeds();
}

// This is the GPIO sequence to play when a tweet arrives.
// So far it's a blue blink, then blue-red blink, then red-green blink
void ledSequence() {
  for(int i=0; i<15; i++) {
    blinkLeds(bluePin, 1000); // #nopin
  }
  for(int i=0; i<10; i++) {
    blinkLeds(bluePin, redPin);
  }
  for(int i=0; i<10; i++) {
    blinkLeds(greenPin, redPin);
  }
}

// The main loop
void loop() {
  // TODO remove this
  while(true){
  ledSequence();
  delay(1000);
  }
  
  // if there are incoming bytes available
  // from the server, read them and store the tweet ID
  if (client.available()) {
    char c = client.read();
    //Serial.print(c);
    tweetBuffer += c;
    if(c == '\n') {
      tweetBuffer = "";
    }
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    //Serial.println();
    //Serial.println("disconnecting.");
    client.stop();

    //Serial.println("New tweet id " + tweetBuffer);
    //Serial.println("Last tweet id " + lastTweetId);
    // Check if the newest tweet ID is different than the last tweet ID
    if(tweetBuffer != lastTweetId && tweetBuffer.length() > 5) {
      Serial.println("\\o/ " + tweetBuffer + " \\o/");
      lastTweetId = tweetBuffer;
      // If yes, launch the led sequence
      ledSequence();
    }

    // Wait a while and then restart
    delay(2000);
    getTweetId();
  }
}

