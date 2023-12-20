/*
  A simple program designed to setup and demonstrate the Keyes Vibration Sensor Module - BDAA100015 
  The program monitors connected analog and digital pin and outputs the value to the
  serial monitor.
  
  modified 26th September 2019
  by Sebastian Karam - Flux Workshop
*/
//For DK
#define HEADER_SIZE 5
#define MAGIC_1 2
#define MAGIC_2 4
#define OPCODE_UART_DATA 1

#define SLAVE_ADDRESS 0x36
int SendData, DATA_SIZE, error;
int ranging_mm = 0;
int tof_1 = 0;
int tof_2 = 0;

int area = 0; //填入area代號 0:A, 1:B, 2:C, 3:D

int analogpin = 1; // define analog OUT signal pin
int analog; // define variable to store value read from pin
int digitalpin = 3; // define digital OUT signal pin
int digital; // define variable to store value read from pin

unsigned int i = 0;
int signal_list[500] = {};
int threshold = 700;
int count = 0;
int threshold_cut = 70;
int cut = 0;
  
void setup() {
  pinMode(analogpin, INPUT); // set the OUT signal pin as an input
  pinMode(digitalpin, OUTPUT); // set the OUT signal pin as an input
  // Start the hardware serial port for the serial monitor.
//  Serial.begin(115200, SERIAL_8E1);
  //Serial.begin(9600); // launch the serial monitor
  Serial.begin(9600, SERIAL_8E1);
//  Serial.println("Flux Workshop Example");
}

void loop() { 
  analog = analogRead(analogpin);  // read the voltage level on the A0
  digital = digitalRead(digitalpin);  // read the voltage level on the D2
  
  char readBuf[128];
  int len = 128;
  if (Serial.available()) {
    int size = Serial.readBytes(readBuf, 128);
    if (size >= HEADER_SIZE && readBuf[0] == MAGIC_1 && readBuf[1] == MAGIC_2 && readBuf[2] == OPCODE_UART_DATA) {
      int dataSize = readBuf[3] * 256 + readBuf[4];
      if (size - HEADER_SIZE >= dataSize) {
        Serial.print("got data: ");
        for (int i = HEADER_SIZE; i < size; i++) {
          Serial.print(readBuf[i]);
          Serial.print(" ");
        }
        Serial.println();
      }
    }
  }


//  Serial.println((String)"Light level: Analog " + analog + " Digital " + digital ); // send the result to the serial monitor
//  Serial.println(analog); // send the result to the serial monitor
  delay(5); // pause for a moment before repeating
  i += 1;
////  Serial.println((String)i);
  if (i>500){
    i=0; //reset to beginning of array, so you don't try to save readings outside of the bounds of the array
  }
  signal_list[i-1] = analog;
//  
  if (i==500) {
    for (int j=0;j<i;j+=1) {
      if (signal_list[j] > threshold) {
        count += 1;
//        Serial.println((String)count);
      }
    }
     // cutting tree
    if (count > threshold_cut) {
      cut = 1;
      area = random(1,5);
      //Serial.println((String)cut);
      
      for (int j=0; j<8; j+=1){  //發送訊號持續5s
        char writeData[2] = {cut,area};
        int writeSize = sizeof(writeData);
        digitalWrite(digitalpin,HIGH);
        Serial.write(MAGIC_1);
        Serial.write(MAGIC_2);
        Serial.write(OPCODE_UART_DATA);
        Serial.write(writeSize / 256);
        Serial.write(writeSize % 256);
        Serial.write(writeData, 2);
        Serial.println(writeData);
        delay(1000);
      }
      area = 0;
      digitalWrite(digitalpin,LOW);
    }
    else {
      cut = 0;
      //Serial.println((String)cut);

      //每2.5發送一次無砍樹的訊號，以確保連線正常
      if (i==500){ 
        char writeData[2] = {cut, area};
        int writeSize = sizeof(writeData);
        Serial.write(MAGIC_1);
        Serial.write(MAGIC_2);
        Serial.write(OPCODE_UART_DATA);
        Serial.write(writeSize / 256);
        Serial.write(writeSize % 256);
        Serial.write(writeData, 2);
        Serial.println("writeData");
      }
    }
  }
  count = 0;
}
