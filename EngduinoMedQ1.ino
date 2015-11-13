#include <EngduinoLEDs.h>
#include <EngduinoAccelerometer.h>
#include <Wire.h>


/* Engduino Medium Q1 - Left/Right Game
 * by Bagus Maulana
 * 
 * This program presents a simple memory game where players have to memorize a left/right sequence and repeat it themselves by tilting the Engduino.
 * If the player gets the sequence correctly, the sequence becomes progressively longer and longer. Otherwise, the sequence stays at the same length.
 * The player is given three "lives" at start. Whenever the player makes a wrong answer, he/she loses one life.
 * Once the player's lives reach zero, the game is over. When this happens, the game is reset.
 * The point of the game is to reach as long a sequence as possible.
 * 
 * HOW TO PLAY
 * At first, you will be given a sequence (starting at length 1) shown on the LEDs. You will need to memorise this sequence (e.g. left-right-left)
 * When the blue light appears, tilt the Engduino on the direction shown on the LEDs during the sequence. You will need to answer one-by-one.
 * Once your answer has been taken, white light will be shown to indicate your answer.
 * This will repeat until the sequence is over.
 * If you got the sequence correctly, green light will be shown from all LEDs.
 * If you fail, red light will be shown from all LEDs.
 * Once your lives are over, a second display of red light will appear, and the game will be reset.
 */


void setup() {
  EngduinoLEDs.begin();
  EngduinoAccelerometer.begin();
}


void loop() {
  
  float tilt[3]; //array to read tilt from accelerometer
  int level = 1; //level = length of sequence. Initializes at level 1, which increases for every successful question.
  int lives = 3; //player's lives. For every wrong answer, the player's lives is reduced by 1. If lives=0, game over, and the game is restarted.
  
  while(1) {
    //main loop. Variables (level, lives) declared outside of main loop so that they do not get reset in every start of cycle.
    
    //each time a new question is started, two arrays of length level is initialized.
    //The first array is used to store a randomly generated sequence, and the second array is used to store user input (answer).
    int question[level];
    int answer[level];
    
    //display current lives and level on the LEDs
    displayLives(lives);
    displayLevel(level);
    
    
    int i;
    for(i=0;i<level;i++) {
      //generate random sequence consisting of zeroes and ones
      //let 0 = "left" side (side adjacent to usb port and on/off switch, LEDs 0 and 13-15) and 1 = "right" side (side opposite, LEDs 6-9).
      question[i]=random(2);
    }
    
    displaySequence(question,level); //display the sequence on question[] on the LEDs
    
    
    for(i=0;i<level;i++) {
      
      timeToAnswer(8,100); //display blue light of increasing brightness on all LEDs to indicate time to answer.
      
      //read user input from accelerometer, based on whether the Engduino is tilted to the left or to the right by the time the blue light ends.
      EngduinoAccelerometer.xyz(tilt);
      if(tilt[1] < 0) { //tilted left: negative y readings from accelerometer (and vice versa)
        answer[i] = 0;
        answerIsLeftMessage();
      } else {
        answer[i] = 1;
        answerIsRightMessage();
      }

    }
    
    
    boolean check = arrayCompare(question,answer,level); //checks if answer is correct (question[] == answer[])
    
    if(check==true) {
      //if answer is correct, increase level by 1
      winMessage();
      level++;
    
    } else {
      //if answer is wrong, player loses 1 life
      loseMessage();
      lives--;
      
      if(lives==0) {
        //game over, resets game
        gameOverMessage();
        level = 1;
        lives = 3;
      }
    
    }
  
  } //ends while(1)
} //ends loop()


//functions

void setLEDs(int a, int b, int red, int green, int blue) {
  //set multiple LEDs between the range a and b
  int i;
  for(i=a;i<=b;i++) {
    EngduinoLEDs.setLED(i,red,green,blue);
  }
}


void setLeftLEDs(int red, int green, int blue) {
  //set LEDs on the "left" side (0,13,14,15)
  EngduinoLEDs.setLED(0,red,green,blue);
  setLEDs(13,15,red,green,blue);
}


void setRightLEDs(int red, int green, int blue) {
  //set LEDs on the "right" side (6,7,8,9)
  setLEDs(6,9,red,green,blue);
}


void displayLives(int lives) {
  //display current amount of lives
  delay(500);
  setLEDs(0,lives-1,0,8,0);
  delay(500);
  EngduinoLEDs.setAll(OFF);
  delay(500);
}


void displayLevel(int level) {
  //display current level, up to level 88. white = tens, blue = ones.
  int lv = min(level,88);
  int ones = lv%10;
  int tens = lv/10;

  setLEDs(0,tens-1,8,8,8);
  setLEDs(tens,tens+ones-1,0,0,8);

  delay(500);
  EngduinoLEDs.setAll(OFF);
  delay(500);
}


void displaySequence(int question[], int length) {
  //display the sequence generated on question[] using the LEDs
  int i;
  for(i=0;i<length;i++) {
    if(question[i]==0) { //0 = "left"
      setLeftLEDs(0,8,0);
    } else if(question[i]==1) { //1 = "right"
      setRightLEDs(0,8,0);
    }

    delay(500);
    EngduinoLEDs.setAll(OFF);
    delay(300);
  }
}


void timeToAnswer(int maxBrightness, int interval) {
  //display blue light of increasing brightness on all LEDs
  int i;
  for(i=0;i<=8;i++) {
    EngduinoLEDs.setAll(BLUE,i);
    delay(interval);
  }

  EngduinoLEDs.setAll(OFF);
}


void answerIsLeftMessage() {
  //message to show that answer has been recorded as 'left' (0)
  setLeftLEDs(8,8,8);
  delay(200);
}


void answerIsRightMessage() {
  //message to show that answer has been recorded as 'right' (1)
  setRightLEDs(8,8,8);
  delay(200);
}


boolean arrayCompare(int array1[], int array2[], int length) {
  //compare two arrays. return true if both arrays are equal, false otherwise.
  int i;
  for(i=0;i<length;i++) {
    if(array1[i]!=array2[i]){
      return false;
    }
  }

  return true;
}


void winMessage() {
  //"correct answer" message
  EngduinoLEDs.setAll(OFF);
  delay(500);
  EngduinoLEDs.setAll(GREEN,8);
  delay(500);
  EngduinoLEDs.setAll(OFF);
}


void loseMessage() {
  //"wrong answer" message
  EngduinoLEDs.setAll(OFF);
  delay(500);
  EngduinoLEDs.setAll(RED,8);
  delay(500);
  EngduinoLEDs.setAll(OFF);
}


void gameOverMessage() {
  //"game over" message (displayed after loseMessage)
  delay(500);
  EngduinoLEDs.setAll(RED);
  delay(1000);
  EngduinoLEDs.setAll(OFF);
  delay(500);
}