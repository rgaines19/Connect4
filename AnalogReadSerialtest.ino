/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/

int pushButton = 2;
int pushButton1 = 8;
int pushButton2 = 9;
int pushButton3 = 10;
volatile byte state1 = LOW;
volatile byte state2 = LOW;
volatile byte state3 = LOW;
volatile byte last_state1 = LOW;
volatile byte last_state2 = LOW;
volatile byte last_state3 = LOW;
int led = 0;
int LEDoutput = 6;
int count = 0;
 int sum = 0;


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(pushButton, INPUT);
  pinMode(pushButton1, INPUT);
  pinMode(pushButton2, INPUT);
  pinMode(pushButton3, INPUT);
  pinMode(LEDoutput, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pushButton), collect, RISING);
  digitalWrite(LEDoutput, LOW);
  

}

// the loop routine runs over and over again forever:
void loop() {
    
    if(count == 0){
      printStates();
      count++;
    }
    
 

  

}

 
void printStates(void){
  Serial.print("A0 is: ");
  delay(5);
  Serial.println(state3, DEC);
  delay(5);
  Serial.print("A1 is: ");
  delay(5);
  Serial.println(state2, DEC);
  delay(5);
  Serial.print("A2 is: ");
  delay(5);
  Serial.println(state1, DEC);
  delay(5);
}



void updateStates(void){
  last_state1 = state1;
  last_state2 = state2;
  last_state3 = state3;
  //delay(150);
}

void collect(void){
    state1 = digitalRead(pushButton1);
    state2 = digitalRead(pushButton2);
    state3 = digitalRead(pushButton3);
    /*
      int x = 0;
    for(int i = 0; i <= 2000; i++){
      x++;
    }
    */
    sum = 4*state1 + 2 *state2 + state3;
  
    Serial.print("You have entered the interrupt : ");
    Serial.println(count,DEC);
    Serial.println(sum,DEC);
    
    printStates();
    count++;
    
}
