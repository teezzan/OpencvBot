#include <WiFi.h>

const char* ssid     = "------";
const char* password = "-------";
#define CHANL0     0
#define CHANL1     1
// use 13 bit precission for LEDC timer
#define LEDBIT  13

// use 5000 Hz as a LEDC base frequency
#define LEDFREQ     5000

// set pins for the l298n
const int en1=25;
const int in1=26;
const int in2=27;
int spd1 = 125;
int spd2 = 125;
int s=125;
const int en2=18;
const int in4=17;
const int in3=16;

WiFiServer server(80);

// method takes pin instead of channel
void analogWr(uint8_t pin, uint32_t value, uint32_t valueMax = 255) {
  // calculate duty, 8191 from 2 ^ 13 - 1
  int tee=0;
  if(value>valueMax){tee=valueMax;}
  else{tee=value;}
  uint32_t duty = (8191 / valueMax) * tee;

  if(pin==en1){
    // write duty to pin
    ledcWrite(CHANL0, duty);}
  else if(pin==en2){
    // write duty to pin
    ledcWrite(CHANL1, duty);}
  
 
}



void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(en1, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(en2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Setup timer and attach timer to a led pin
  ledcSetup(CHANL0, LEDFREQ, LEDBIT);
  ledcAttachPin(en1, CHANL0);
  ledcSetup(CHANL1, LEDFREQ, LEDBIT);
  ledcAttachPin(en2, CHANL1);


  
delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
}




// move any motor forward
void forward(int en, int in1, int in2, int so){
  int spde=so;
  
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWr(en, spde);
  }
  
// move any motor backward
void backward(int en, int in1, int in2, int so){
  int spde=so;
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWr(en, spde);
  }
  
// stop any motion
void brake(int en, int in1, int in2){
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWr(en, 255);
  }

  //set speed (pmw)
void setspd1(int sp){
  spd1=sp;
  }

void setspd2(int sp){
  spd2=sp;
  }
void setspd(int sp){
  spd2=sp;
  spd1=sp;
  }

//specialized functions for each motor
//void forward1(){
//  forward(en1,in1,in2);
//  }
//void forward2(){
//  forward(en2,in3,in4);
//  }
void genfwd(int spd){
  setspd(spd);
  forward(en1,in1,in2,spd);
  forward(en2,in3,in4,spd);
  }
//void backward1(){
//  backward(en1,in1,in2);
//  }
//void backward2(){
//  backward(en2,in3,in4);
//  }
void genbwd(int spd){
  setspd(spd);
  backward(en1,in1,in2,spd);
  backward(en2,in3,in4,spd);
  }
  
void bankleft(){
  forward(en1,in1,in2,200);
  forward(en2,in3,in4,130);}
  
void bankright(){
  forward(en1,in1,in2,130);
  forward(en2,in3,in4,200);}
  
 
void turn(){
  setspd(150);
  forward(en2,in3,in4,125);
  backward(en1,in1,in2,125);
  }


void loop() {
  // put your main code here, to run repeatedly:
WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/forward\">here</a> to move forward.<br>");
            client.print("Click <a href=\"/backward\">here</a> to move backwards.<br>");
            client.print("Click <a href=\"/left\">here</a> to turn left.<br>");
            client.print("Click <a href=\"/right\">here</a> to turn right.<br>");
            client.print("Click <a href=\"/stop\">here</a> to stop.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /forward")) {
          genfwd(200);
          Serial.println("4wd");
          delay(2000);
          brake(en1,in1,in2);               
          brake(en2,in3,in4);
        }
        if (currentLine.endsWith("GET /backward")) {
          genbwd(125);
          Serial.println("bwd");
          delay(2000);
          brake(en1,in1,in2);               
          brake(en2,in3,in4);
          
        }
        if (currentLine.endsWith("GET /left")) {
         bankleft();
         Serial.println("leftwd");
         delay(2000);
         brake(en1,in1,in2);               
         brake(en2,in3,in4);
        }
        if (currentLine.endsWith("GET /right")) {
         bankright();             
         Serial.println("rwd");
         delay(2000);
         brake(en1,in1,in2);               
         brake(en2,in3,in4);
        }
        if (currentLine.endsWith("GET /stop")) {
          brake(en1,in1,in2);               
          brake(en2,in3,in4);
          Serial.println("stp");
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
