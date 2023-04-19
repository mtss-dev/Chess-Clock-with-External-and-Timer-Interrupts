//Author: Matheus Silva dos Santos

//Pins used for the buttons and LEDs
  #define blackButtonPin 2
  #define whiteButtonPin 3
  #define whiteLedPin 10
  #define blackLedPin 8

 //Global variables
  int count = 0;                             //timer overflow counter
  int firstPrintWhite = 0;             //Flag to print the initial time of the white player, in our case "10:00", on the Serial Monitor
  int firstPrintBlack = 0;              //Flag to print the initial time of the black player, in our case "10:00", on the Serial Monitor
  int whiteTimer = 600;            //Timer for the white player (10 min = 600s)
  int blackTimer = 600;             //Timer for the black player (10 min = 600s)
  volatile boolean whiteLedOn = false;    //Flag to enable or disable the LED of the white player
  volatile boolean blackLedOn = false;     //Flag to enable or disable the LED of the black player
  volatile boolean printFlag = false;       //Flag to enable or disable the LED of the black player
  volatile boolean whiteTurn = false;  //Flag to print that it is the turn of the white player on the Serial Monitor
  volatile boolean blackTurn = false;   //Flag to print that it is the turn of the black player on the Serial Monitor
  
  void setup() {
    Serial.begin(9600);
    cli();   //disables global interrupt
    
    pinMode(whiteLedPin,OUTPUT);
    pinMode(blackLedPin,OUTPUT);
    pinMode(whiteButtonPin,INPUT_PULLUP);
    pinMode(blackButtonPin,INPUT_PULLUP);
    digitalWrite(whiteLedPin, LOW);
    digitalWrite(blackLedPin, LOW);  
  
    attachInterrupt(digitalPinToInterrupt(whiteButtonPin),whiteButton, RISING);    //External interrupt for the white player button
    attachInterrupt(digitalPinToInterrupt(blackButtonPin),blackButton, RISING);    //External interrupt for the black player button
   
    TCCR2A = 0;                                   //zero TCCR2A register
    TCNT2 = 0x06;                                 //Start Timer2 at 6
    TCCR2B = ((TCCR2B & B11111000) | B00000110);  // prescaler 256
    TIMSK2 |= 0x01;                               //Set the least significant bit
    sei();                                        //enable global interrupt
  }
  
  void loop() {
    if(whiteLedOn){                    //If the white player's LED flag is "true", turn on the white player's LED and print their timer
      digitalWrite(whiteLedPin,HIGH);
      if(whiteTurn){                //Prints on the Serial Monitor that it is the white player's turn
        Serial.println("White Player's Turn");
        whiteTurn = !whiteTurn;
      }
      if(firstPrintWhite == 0){     //Print the initial time of the white player
        printTime(whiteTimer);
        firstPrintWhite = 1;
      }
      if(printFlag){                     //print the white player's timer on the Serial Monitor                 
        printTime(whiteTimer);
        printFlag = !printFlag;
      }
    }
    
    if(!whiteLedOn) digitalWrite(whiteLedPin,LOW); //If the white player's LED flag is "false", turn off the white player's LED
    
    if(blackLedOn){                    //If the flag for the Black player's LED is set to "true", turn on the Black player's LED and print its timer.
      digitalWrite(blackLedPin,HIGH);
      if(blackTurn){                //Print on the Serial Monitor that it will be Black player's turn.
        Serial.println("Black player's turn");
        blackTurn = !blackTurn;
      }
      if(firstPrintBlack == 0){     //Print the initial time of the Black player
        printTime(blackTimer);
        firstPrintBlack = 1;
      }
      if(printFlag){                    //Print the timer of the Black player on the Serial Monitor.
        printTime(blackTimer);
        printFlag = !printFlag; 
      }
    }
    
    if(!blackLedOn) digitalWrite(blackLedPin,LOW); //If the flag of the Black player's LED is "false", turn off the Black player's LED.

    //If any timer runs out, the times of each player will be printed, and the game will be finished!
    if((whiteTimer == 0) or (blackTimer == 0)){
      Serial.println("The timer of one of the players has run out!");
      Serial.println("---------------------------------------------");
      Serial.print("Timer for White player: ");
      printTime(whiteTimer);
      Serial.print("Timer for Black player: ");
      printTime(blackTimer);
      Serial.println("---------------------------------------------");
      Serial.println("GAME OVER!");
      while(1);
    }
  }
  
  void whiteButton()
  {
    if(digitalRead(blackLedPin) != 1){ //Set the LED flag of the Black player to "true" and the LED flag of the White player to "false".
                                       //If the pin of the black player's LED is not in HIGH
      blackLedOn = !blackLedOn;
      whiteLedOn = !blackLedOn;
      blackTurn = !blackTurn;
    }
  }
  
  void blackButton()
  {
    if(digitalRead(whiteLedPin) != 1){ //Changes the Flag of the LED of the White player to "true" and the Flag of the LED of the Black player to "false".
                                       //If the LED pin of the White player is not set to HIGH, set the LED Flag of the White player to "true".
      whiteLedOn = !whiteLedOn;
      blackLedOn = !whiteLedOn;
      whiteTurn = !whiteTurn;
    }
  }

  //Timer Interrupt.
  //I'm using "Timer 2" for both the timer of the White player and the Black player
  ISR(TIMER2_OVF_vect) {
    if(digitalRead(whiteLedPin) == 1)  //Check if the Led pin of the White player is in HIGH
      count++;                             //increment the timer overflow counter
      TCNT2 = 0x06;                       //Starts Timer2 at 6.
      if (count == 250) {                  //verifies if the timer overflow counter has reached 250.
        whiteTimer --;           //Decrement the timer of the White player.
        count = 0;                         //Reset the counter.
        printFlag = !printFlag;           //Change the Flag to "true" to print the timer of the White player on the Serial Monitor.
      }
  
      if(digitalRead(blackLedPin) == 1) //Checks if the pin of the Black player's LED is in HIGH.
      count++;                             //increment the timer overflow counter
      TCNT2 = 0x06;                       //Starts Timer2 at 6.
      if (count == 250) {                  //verifies if the timer overflow counter has reached 250.
        blackTimer--;             //Decrement the timer of the Black player.
        count = 0;                         //Reset the counter.
        printFlag = !printFlag;           //Change the Flag to "true" to print the timer of the Black player on the Serial Monitor.
      }
  }

  //Function to print the players' timers.
  //With it, it avoids the loop() part of the program from being filled with "Serial.prints"
  void printTime(int temp){
    if((temp / 60) < 10)Serial.print("0");
    Serial.print(temp / 60);
    Serial.print(":");
    if((temp % 60) < 10)Serial.print("0"); 
    Serial.println(temp % 60);
  }
