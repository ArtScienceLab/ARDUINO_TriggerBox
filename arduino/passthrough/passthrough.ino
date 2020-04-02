//On an ARDUINO uno recieves input on pin 2 and 3 and passes it throug
// pin 2 is mapped to digital pin 8 and 9
// pin 3 is mapped to digital pin 10 and 11

//See here: https://www.instructables.com/id/Fast-digitalRead-digitalWrite-for-Arduino/
//Since it is time critical the registers are used:
// B0 = digital pin 8
// B1 = digital pin 9
// B2 = digital pin 10
// B3 = digital pin 11
// B5 = LED on digital pin 13
//D2 = digital pin 2
//D3 = digital pin 3

//print debug information
boolean debug = false;

volatile boolean pin2Changed = false;
volatile boolean pin3Changed = false;

void passPin2() {
  pin2Changed = true;
}

void passPin3() {
  pin3Changed = true;
}


void setup() {
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), passPin2, CHANGE);

  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), passPin3, CHANGE); 

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);

  if(debug)
    Serial.begin(115200);
}

void loop() {
  if(pin2Changed){
    //D2 = digital pin 2
    int on = PIND & B00000100;
    if(on == 0){
      //since port 2 is off, disabe port B0 and B1
      //leave the rest as is (but blink the led on B5)
      PORTB = PORTB & B11011100;
    }else{
      //since port 2 is on, enable port B0 and B1
      //leave the rest as is (but blink the led on B5)
      PORTB = PORTB | B00100011;
    }
    pin2Changed = false;

    if(debug){
      Serial.print("Pin 2 changed ");
      Serial.print(digitalRead(2));
      Serial.print(" ");
      Serial.print(on);
      Serial.print(" pin 8 ");
      Serial.print(digitalRead(8));
      Serial.print(" pin 9 ");
      Serial.print(digitalRead(9));
      Serial.println(" ");
    }
  }

  if(pin3Changed){
    //D3 = digital pin 3
    int on = PIND & B00001000;
    if(on > 0){
      //since port 3 is on, enable port B2 and B3
      //leave the rest as is
      PORTB = PORTB | B00101100;
    }else{
      //since port 3 is off, disabe port B2 and B3
      //leave the rest as is
      PORTB = PORTB & B11010011;
    }
     pin3Changed = false;

    if(debug){
      Serial.print("Pin 3 changed ");
      Serial.print(digitalRead(3));
      Serial.print(" ");
      Serial.print(on);
      Serial.print(" pin 10 ");
      Serial.print(digitalRead(10));
      Serial.print(" pin 11 ");
      Serial.print(digitalRead(11));
      Serial.println(" ");
    }
  }
}
