#include <TimerThree.h>

#include <Servo.h>
// use this code to test game
//First column servo command pos = 19
//Seventh column servo command pos = 157

int colToken[7] = {5,5,5,5,5,5,5};
int gameboard[6][7];
int colToken_cpy[7] = {5,5,5,5,5,5,5};
int gameboard_cpy[6][7];
int turn = 1;
int level = 4;
int M = 6;
int N = 7;
volatile int count  = 0;

int verSum[21];
int horSum[24];
int posDiagSum[12];
int negDiagSum[12];
int verSum_cpy[21];
int horSum_cpy[24];
int posDiagSum_cpy[12];
int negDiagSum_cpy[12];
bool winState = false;
byte state = 0;
byte next_state = 0;
bool gameOver = false;
bool PlayerWin_flag = false;
bool CPUwin_flag = false;
bool Draw_flag = false;
byte Move = 7;
bool players_turn = true;
int turnOrder[13] = {7,7,7,7,0,1,2,3,4,5,6,7,0};
int turnOrdercount = 0;


volatile bool blinkLED_flag = false;
volatile bool blinkLevel_flag = false;
volatile bool blinkPlayer_flag = false;
volatile bool blinkWinner_flag = false;
volatile bool blinkState = true;
volatile bool blinkState_pin0 = true;
volatile bool blinkState_pin1 = true;
volatile bool blinkState_pin2 = true;
volatile bool blinkState_pin3 = true;
volatile bool blinkState_pin4 = true;
volatile bool blinkState_pin5 = true;
volatile bool blinkState_pin6 = true;


int interrupt_signal_pin = 18;
int enable_input_ic_pin = 12;
int A0_pin = 22;
int A1_pin = 24;
int A2_pin = 26;
int LED_0_pin = 53;
int LED_1_pin = 51;
int LED_2_pin = 49;
int LED_3_pin = 47;
int LED_4_pin = 45;
int LED_5_pin = 43;
int LED_6_pin = 41;
int LED_win_pin = 8;
int LED_draw_pin = 9;
int LED_lose_pin = 10;
int LED_choose_cpu_pin = 23;
int LED_choose_move_pin = 25;
int LED_wait_pin = 27;
int RESET_pin = 18;
int servo_chip_dispenser_pin = 11; //green 
int servo_chute_position_pin = 12; //white
int servo_chip_release_pin = 13; //blue

volatile byte state1 = HIGH;
volatile byte state2 = HIGH;
volatile byte state3 = HIGH;
volatile bool sample_flag = false;
volatile byte sum = 7;
volatile bool isValidmove = false; 
volatile bool isValid = false;
int dummyPin = 37;
int lastsum = 0;

int posupdateD = 90;
int posupdateC = 87;
int posupdateG = 90;

int posCCenter = 87;
int pos6 = 155;    // variable to store the servo position
int pos5 = 132;    // variable to store the servo position
int pos4 = 110;    // variable to store the servo position
int pos3 = 87;    // variable to store the servo position
int pos2 = 65;    // variable to store the servo position
int pos1 = 43;    // variable to store the servo position
int pos0 = 20;

int posHuman = 180;    // variable to store the servo position
int posDCenter = 100 ;    // variable to store the servo position
int posAI = 0;    // variable to store the servo position

int posL = 50;    // variable to store the servo position
int posGCenter = 90 ;    // variable to store the servo position
int posR = 130;    // variable to store the servo position

int Speed = 12;

Servo chip_dispenser;
Servo chute;
Servo chip_release;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  chip_dispenser.attach(servo_chip_dispenser_pin);
  chute.attach(servo_chute_position_pin);
  chip_release.attach(servo_chip_release_pin);
  pinMode(interrupt_signal_pin, INPUT);
  pinMode(enable_input_ic_pin, OUTPUT);
  pinMode(A0_pin,INPUT);
  pinMode(A1_pin,INPUT);
  pinMode(A2_pin,INPUT);
  pinMode(dummyPin,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interrupt_signal_pin),collect,RISING);
  pinMode(LED_0_pin,OUTPUT);
  pinMode(LED_1_pin,OUTPUT);
  pinMode(LED_2_pin,OUTPUT);
  pinMode(LED_3_pin,OUTPUT);
  pinMode(LED_4_pin,OUTPUT);
  pinMode(LED_5_pin,OUTPUT);
  pinMode(LED_6_pin,OUTPUT);


    Timer3.initialize(250000);
    Timer3.attachInterrupt(blinkLED); // blinkLED to run every 0.5 seconds
  
}

void loop() {
  // put your main code here, to run repeatedly:
  state = next_state;
  Serial.print("We are in state: ");
  Serial.println(state,DEC);
  switch (state){
    case 0: //set up game state
      createBoard();
      createScoreVectors();
      evalBoard();
      printBoard();


      blinkLED_flag = true;   
      next_state = 1;
      break;
    case 1: // pick cpu level
        blinkLevel_flag = true;
//      while(isValid == false){
//        
//        state1 = digitalRead(A0_pin);
//        state2 = digitalRead(A1_pin);
//        state3 = digitalRead(A2_pin);
//        sum = 4*state3 + 2 *state2 + state1;
//        level = sum;
//        if(isCPUlevelValid(level) == true){
//          isValid = true;
//          Serial.println("we set flag to true");
//        }
//        else{
//          Serial.println("flag is false");
//        }
//      }
        Serial.println("pick your level");
        if(sample_flag == true){
          sample_flag = false;
          noInterrupts();
          sum = 4*state3 + 2 *state2 + state1;
          
          level = sum;
          interrupts();
        }


        if(isCPUlevelValid(level) == true){
                 next_state = 2;
                 blinkLevel_flag = false;
                 sum = 10;
               }
               else{
                  next_state = 1;
                }
              

      break;
    case 2: // player pick move using controller
      players_turn = false;
      blinkPlayer_flag = true;
      blinkLED_flag = true;
      digitalWrite(LED_choose_move_pin,HIGH);

        if(sample_flag == true){
          sample_flag = false;
          noInterrupts();
          sum = 4*state3 + 2 *state2 + state1;
          
          Move = sum;
          interrupts();
        }
        
        
        if(isMoveValid(Move) == true){
          Serial.println(Move,DEC);
          next_state = 3;
          digitalWrite(interrupt_signal_pin,HIGH);
          blinkPlayer_flag = false;
          blinkLED_flag = false;
          digitalWrite(LED_choose_move_pin,LOW);
        }else{
          next_state = 2;
        }
      

      break;
    case 3: // play moves using servo comands
      digitalWrite(LED_wait_pin,HIGH);
      playMove(Move);
      printBoard();
      delay(2000);
      servo_commands(Move);
      Move = 10;
      Serial.print("The token at column ");
      Serial.print(Move,DEC);
      Serial.print(" is: ");
      Serial.print(colToken[Move]);
      Serial.println();
      next_state = 4;
      break;
    case 4: // check the win state
      evalBoard();
      winState = checkWinState();
                if (winState == true) {
                    if (turn % 2 == 0) {
                        //Serial.print("Player one won!!!\n");
                        noInterrupts();
                        PlayerWin_flag = true;
                        interrupts();
                        digitalWrite(LED_wait_pin,LOW);
                    }
                    else{
                      //Serial.print("The CPU won!!!\n");
                      noInterrupts();
                      CPUwin_flag = true;
                      interrupts();
                    }
                    next_state = 6;
                    digitalWrite(LED_wait_pin,LOW);
                    break;
                }
            if (turn == 43) {
                //Serial.print("The Game is a draw.");
                noInterrupts();
                Draw_flag = true;
                interrupts();
                next_state = 6;
                digitalWrite(LED_wait_pin,LOW);
                break;
            }
            if (players_turn == true) {
                next_state = 2;
                delay(5000);
                digitalWrite(LED_wait_pin,LOW);
                Serial.println("Pick move");
            }else {
            next_state = 5;
            delay(5000);
            digitalWrite(LED_wait_pin,LOW);
            } 
      break;
    case 5: // AI picks its move
      digitalWrite(LED_choose_cpu_pin,HIGH);
      Move = AIpickMove();
      delay(3000);
      //Serial.print("the CPU move is: ");
      //Serial.print(Move,DEC);
      //Serial.println();
      players_turn = true;
      next_state = 3;
      digitalWrite(LED_choose_cpu_pin,LOW);
      break;
    case 6: // goes to this state when the game is over
    //Serial.print("The game is over\n");
      gameOver = true;
      noInterrupts();
      blinkWinner_flag = true;
      blinkLED_flag = true;
      interrupts();
      next_state = 6;
      break;
    default: // if error in the program go to state 6
      //Serial.print("We are in the default state.\n");
      next_state = 6;
      break;
  }
}

void collect(void){
  Serial.println("enter isr");
    sample_flag = true;
    state1 = digitalRead(A0_pin);
    state2 = digitalRead(A1_pin);
    state3 = digitalRead(A2_pin);
    //sum = 4*state3 + 2 *state2 + state1;
   
   }

int AIpickMove(void){
    int col =0;
    switch (level){
      case 0:
        col = AIrandMove();
        break;
      case 1:
        col = PickBestMove();
        break;
      case 2:
        col = PickBestMove();
        break;
      default:
        col = AIrandMove();
      break;
    }
    
    return col;
}

int PickBestMove(void){
    int best_col = 10;
    int best_score = -1000;
    int score = 0;
    copyBoard();
    
    for (int col =0; col<7; col++) {
        score = 0;

        if ((colToken_cpy[col] >= 0) && (colToken_cpy[col] <= 5) ) {
            
        
        gameboard_cpy[colToken_cpy[col]][col] = -1;
        evalBoardCopy();
        gameboard_cpy[colToken_cpy[col]][col] = 0;
        

        if (col == 3) {
            score = 6;
        }
        
        
        for (int x =0; x<24; x++) {
            if (horSum_cpy[x]==-4) {
                score+=500; // it was 100000
            }
            if (horSum_cpy[x]==-3) {
                score+=3;
            }
            if (horSum_cpy[x]==-2) {
                score+=2;
            }
            if (horSum_cpy[x]==2) {
                score-=40; //it was 40
            }
            if (horSum_cpy[x]==3) {
                score-=100; // it was 100
            }
            
        }
        
        for (int x =0; x<21; x++) {
            if (verSum_cpy[x]==-4) {
                score+=500;
            }
            if (verSum_cpy[x]==-3) {
                score+=2;
            }
            if (verSum_cpy[x]==-2) {
                score+=1;
            }
            if (verSum_cpy[x]==2) {
                score-=40;
            }
            if (verSum_cpy[x]==3) {
                score-=100;
            }
        }
        for (int x =0; x<12; x++) {
            if (posDiagSum_cpy[x]==-4) {
                score+=500;
            }
            if (posDiagSum_cpy[x]==-3) {
                score+=2;
            }
            if (posDiagSum_cpy[x]==-2) {
                score+=1;
            }
            if (posDiagSum_cpy[x]==2) {
                score-=40;
            }

            if (posDiagSum_cpy[x]==3) {
                score-=100;
            }
        }
        for (int x =0; x<12; x++) {
            if (negDiagSum_cpy[x]==-4) {
                score+=500;
            }
            if (negDiagSum_cpy[x]==-3) {
                score+=2;
            }
            if (negDiagSum_cpy[x]==-2) {
                score+=1;
            }
            if (negDiagSum_cpy[x]==2) {
                score-=40;
            }
            if (negDiagSum_cpy[x]==3) {
                score-=100;
            }
            
        }
        if (score >= best_score) {
            best_score = score;
            best_col = col;
            
        }
        //Serial.print("score for col ");
        //Serial.print(col, DEC);
        //Serial.print(" is :");
        //Serial.println(score,DEC);
        }
        else{
            //Serial.print("for col : ");
            //Serial.print(col,DEC);
            //Serial.println(" we did not compute");
        }
    }
    if (best_col == 10) {
        //Serial.print("Error in the fuction\n");
        
    }
    return best_col;
} 

int AIrandMove(void){
    //int col = rand() % 7;
    int col = random(0,7);
    bool validmove = false;
    while (validmove == false) {
        col = random(0,6);
        //col = rand()%7;
        validmove = isMoveValid(col);
    }
    return col;
}

bool isMoveValid(int Move){
    bool Check;
    if(Move <0 || Move > 6){
     //Serial.println("invalid input");
    }
    else{
      
    
    
    int col = colToken[Move];
    if (col <  0  || col > 5) {
        Check = false;
        
    }
    else{
        Check = true;
    }
    }
    return Check;
}

void playMove(int Move){
    if (turn % 2 == 0) {
        gameboard[colToken[Move]][Move] = -1;
    }
    else{
        gameboard[colToken[Move]][Move] = 1;
    }
    
    colToken[Move]--;
    turn++;
}

void printBoard(void){
    for (int m=0; m<M; m++) {
        for (int n=0; n<N; n++) {
          if(gameboard[m][n] == 1 || gameboard[m][n] == 0){
            Serial.print(" ");
            Serial.print(gameboard[m][n], DEC);
            
        }else{
          Serial.print(gameboard[m][n], DEC);
        }
        }
        Serial.println();
        
    }
    Serial.println("------------");
}
void copyBoard(void){
    for (int a=0; a<M; a++) {
        for (int b=0; b<N; b++) {
            gameboard_cpy[a][b] = gameboard[a][b];
        }
    }
    for (int i = 0; i<7; i++) {
        colToken_cpy[i] = colToken[i];
    }
    
    
    
    
}

void evalBoardCopy(void){
    // vertical check function
    int count = 0;
    for (int x = 0; x < 7; x++) {
        for (int y =0; y<3; y++) {
            verSum_cpy[count] = gameboard_cpy[y][x]+gameboard_cpy[y+1][x]+gameboard_cpy[y+2][x]+gameboard_cpy[y+3][x];
            count++;
        }
    }
    count = 0;
    // horizontal check function
    for (int x = 0; x < 6; x++) {
        for (int y =0; y<4; y++) {
            horSum_cpy[count] = gameboard_cpy[x][y]+gameboard_cpy[x][y+1]+gameboard_cpy[x][y+2]+gameboard_cpy[x][y+3];
            count++;
        }
    }
    count = 0;
    // Positive Edge Diagnol check function
    for(int r =0; r < 3; r++){
        for (int c =0; c <4; c++) {
            posDiagSum_cpy[count] = gameboard_cpy[r][c] + gameboard_cpy[r+1][c+1] + gameboard_cpy[r+2][c+2] + gameboard_cpy[r+3][c+3];
            count++;
        }
    }
    count = 0;
    // Negative Edge Diagnol check function
    for(int r =0; r < 3; r++){
        for (int c = 6; c > 2; c--) {
            negDiagSum_cpy[count] = gameboard_cpy[r][c] + gameboard_cpy[r+1][c-1] + gameboard_cpy[r+2][c-2] + gameboard_cpy[r+3][c-3];
            count++;
        }
    }
}

void createBoard(void){
    for (int a=0; a<M; a++) {
        for (int b=0; b<N; b++) {
            gameboard[a][b] = 0;
        }
    }
}

void evalBoard(void){
    int count = 0;
    
    // vertical check function
    for (int x = 0; x < 7; x++) {
        for (int y =0; y<3; y++) {
            verSum[count] = gameboard[y][x]+gameboard[y+1][x]+gameboard[y+2][x]+gameboard[y+3][x];
            count++;
        }
    }
    count = 0;
    // horizontal check function
    for (int x = 0; x < 6; x++) {
        for (int y =0; y<4; y++) {
            horSum[count] = gameboard[x][y]+gameboard[x][y+1]+gameboard[x][y+2]+gameboard[x][y+3];
            count++;
        }
    }
    count = 0;
    // Positive Edge Diagnol check function
    for(int r =0; r < 3; r++){
        for (int c =0; c <4; c++) {
            posDiagSum[count] = gameboard[r][c] + gameboard[r+1][c+1] + gameboard[r+2][c+2] + gameboard[r+3][c+3];
            count++;
        }
    }
    count = 0;
    // Negative Edge Diagnol check function
    for(int r =0; r < 3; r++){
        for (int c = 6; c > 2; c--) {
            negDiagSum[count] = gameboard[r][c] + gameboard[r+1][c-1] + gameboard[r+2][c-2] + gameboard[r+3][c-3];
            count++;
        }
    }
}

void createScoreVectors(void){
    for (int j = 0; j<24; j++) {
        if (j<21) {
            verSum[j] =0;
        }
        if (j<12){
            posDiagSum[j] = 0;
            negDiagSum[j] = 0;
        }
        horSum[j]=0;
    }
}




bool checkWinState(void){
    bool win = false;
    for (int now = 0; now <21; now++) {
        if (verSum[now] == 4 || verSum[now] == -4){
            win = true;
            break;
        }
    }
    
    for (int now = 0; now <24; now++) {
        if(horSum[now] == 4 || horSum[now] == -4){
            win = true;
            break; 
        }
    }

    
    for (int now = 0; now <12; now++) {
        if(posDiagSum[now] == 4 || posDiagSum[now] == -4){
            win = true;
            break;
        }
    }

    
    for (int now = 0; now <12; now++) {
        if(negDiagSum[now] == 4 || negDiagSum[now] == -4){
            win = true;
            break;
        }
    }

    return win;
}

int servo_commands(int col){
    GateMoveTo(90, Speed); // turn 90 into closed_gate;
    ChuteMoveTo(pos3,Speed);
    
    switch(col){
      case 0:
        if(players_turn == false){
          DispenserMoveTo(posCCenter,  10);// turn 4 into dispense_speed
        delay(1000);
        DispenserMoveTo(180, 10);
        delay(1000);
        }else{
          Serial.println("dispense flag false");
        DispenserMoveTo(posCCenter, 10);
        delay(1000);  
        DispenserMoveTo(0, 10);  
        delay(1000);      
        }
        
        ChuteMoveTo(pos0, Speed);
        delay(3000);
        GateMoveTo(50, Speed); // turn 50 to gate_open
        delay(3000);
        break;
      case 1:
      if(players_turn == false){
      DispenserMoveTo(posCCenter,  10);// turn 4 into dispense_speed
        delay(1000);
        DispenserMoveTo(180, 10);
        delay(1000);
        }else{
          Serial.println("dispense flag false");
        DispenserMoveTo(posCCenter, 10);
        delay(1000);  
        DispenserMoveTo(0, 10);  
        delay(1000);      
        }
        
        ChuteMoveTo(pos1, Speed);
        delay(3000);
        GateMoveTo(50, Speed); // turn 50 to gate_open
        delay(3000);
        break;
      case 2:
      if(players_turn == false){
        DispenserMoveTo(posCCenter,  10);// turn 4 into dispense_speed
        delay(1000);
        DispenserMoveTo(180, 10);
        delay(1000);
        }else{
          Serial.println("dispense flag false");
        DispenserMoveTo(posCCenter, 10);
        delay(1000);  
        DispenserMoveTo(0, 10);  
        delay(1000);      
        }
        
        ChuteMoveTo(pos2, Speed);
        delay(3000);
        GateMoveTo(50, Speed); // turn 50 to gate_open
        delay(3000);
      
        break;
      case 3:
      if(players_turn == false){
        DispenserMoveTo(posCCenter,  10);// turn 4 into dispense_speed
        delay(1000);
        DispenserMoveTo(180, 10);
        delay(1000);
        }else{
          Serial.println("dispense flag false");
        DispenserMoveTo(posCCenter, 10);
        delay(1000);  
        DispenserMoveTo(0, 10);  
        delay(1000);      
        }
        
        ChuteMoveTo(pos3, Speed);
        delay(3000);
        GateMoveTo(50, Speed); // turn 50 to gate_open
        delay(3000);
      
        break;
      case 4:
      if(players_turn == false){
        DispenserMoveTo(posCCenter,  10);// turn 4 into dispense_speed
        delay(1000);
        DispenserMoveTo(180, 10);
        delay(1000);
        }else{
          Serial.println("dispense flag false");
        DispenserMoveTo(posCCenter, 10);
        delay(1000);  
        DispenserMoveTo(0, 10);  
        delay(1000);      
        }
        
        ChuteMoveTo(pos4, Speed);
        delay(3000);
        GateMoveTo(50, Speed); // turn 50 to gate_open
        delay(3000);
      
        break;
      case 5:
      if(players_turn == false){
        DispenserMoveTo(posCCenter,  10);// turn 4 into dispense_speed
        delay(1000);
        DispenserMoveTo(180, 10);
        delay(1000);
        }else{
          Serial.println("dispense flag false");
        DispenserMoveTo(posCCenter, 10);
        delay(1000);  
        DispenserMoveTo(0, 10);  
        delay(1000);      
        }
        
        ChuteMoveTo(pos5, Speed);
        delay(3000);
        GateMoveTo(130, Speed); // turn 50 to gate_open
        delay(3000);
      
        break;
      case 6:
      if(players_turn == false){
        DispenserMoveTo(posCCenter,  10);// turn 4 into dispense_speed
        delay(1000);
        DispenserMoveTo(180, 10);
        delay(1000);
        }else{
          Serial.println("dispense flag false");
        DispenserMoveTo(posCCenter, 10);
        delay(1000);  
        DispenserMoveTo(0, 10);  
        delay(1000);      
        }
        
        ChuteMoveTo(pos6, Speed);
        delay(3000);
        GateMoveTo(130, Speed); // turn 50 to gate_open
        delay(3000);
      
        break;
      default:
        break;
}
}


   
bool isCPUlevelValid(int level){
  bool check = false;
   if(level == 0 || level == 1 || level == 2){
    check = true;
   }
  return check;
}

void GateMoveTo(int position, int speed){
  int mapSpeed = map(speed,0,30,30,0);
  if(position > posGCenter){
    for(posGCenter = posupdateG; posGCenter <= position ;posGCenter++){
      chip_release.write(posGCenter);
      posupdateG = posGCenter;
      delay(mapSpeed);
    }
  }else{
    for(posGCenter = posupdateG; posGCenter >= position ;posGCenter--){
      chip_release.write(posGCenter);
      posupdateG = posGCenter;
      delay(mapSpeed);
    }
  }
}

void ChuteMoveTo(int position, int speed){
  int mapSpeed = map(speed,0,30,30,0);
  if(position > posGCenter){
    for(posGCenter = posupdateG; posGCenter <= position ;posGCenter++){
      chute.write(posGCenter);
      posupdateG = posGCenter;
      delay(mapSpeed);
    }
  }else{
    for(posGCenter = posupdateG; posGCenter >= position ;posGCenter--){
      chute.write(posGCenter);
      posupdateG = posGCenter;
      delay(mapSpeed);
    }
  }
}

void DispenserMoveTo(int position, int speed){
  int mapSpeed = map(speed,0,30,30,0);
  if(position > posDCenter){
    for(posDCenter = posupdateD; posDCenter <= position ;posDCenter++){
      chip_dispenser.write(posDCenter);
      posupdateD = posDCenter;
      delay(mapSpeed);
    }
  }else{
    for(posDCenter = posupdateD; posDCenter >= position ;posDCenter--){
      chip_dispenser.write(posDCenter);
      posupdateD = posDCenter;
      delay(mapSpeed);
    }
  }
}

void blinkLED(void)
{
  if(blinkLED_flag == true){
    if(blinkLevel_flag == true){
      LevelBlink();
    }

    if(blinkPlayer_flag == true){
      PlayerBlink();
    }

    if(blinkWinner_flag == true){
      WinnerBlink();
    }
    
    
  }else{
    digitalWrite(LED_0_pin, LOW);
    digitalWrite(LED_1_pin, LOW);
    digitalWrite(LED_2_pin, LOW);
    digitalWrite(LED_3_pin, LOW);
    digitalWrite(LED_4_pin, LOW);
    digitalWrite(LED_5_pin, LOW);
    digitalWrite(LED_6_pin, LOW);
  }

}

void PlayerBlink(void){
  if(colToken[0] > -1){
    if(blinkState_pin0 == true){
     digitalWrite(LED_0_pin, HIGH); 
     blinkState_pin0 = false;
    }else{
      digitalWrite(LED_0_pin, LOW);
     blinkState_pin0 = true;
    }
    
  }else{
    digitalWrite(LED_0_pin, LOW);
  }

  if(colToken[1] > -1){
    if(blinkState_pin1 == true){
     digitalWrite(LED_1_pin, HIGH); 
     blinkState_pin1 = false;
    }else{
      digitalWrite(LED_1_pin, LOW);
      blinkState_pin1 = true;
    }
    
  }else{
    digitalWrite(LED_1_pin, LOW);
  }

  if(colToken[2] > -1){
    if(blinkState_pin2 == true){
     digitalWrite(LED_2_pin, HIGH); 
     blinkState_pin2 = false;
    }else{
      digitalWrite(LED_2_pin, LOW);
      blinkState_pin2 = true;
    }
    
  }else{
    digitalWrite(LED_2_pin, LOW);
  }

  if(colToken[3] > -1){
    if(blinkState_pin3 == true){
     digitalWrite(LED_3_pin, HIGH); 
     blinkState_pin3 = false;
    }else{
      digitalWrite(LED_3_pin, LOW);
      blinkState_pin3 = true;
    }
    
  }else{
    digitalWrite(LED_3_pin, LOW);
  }

  if(colToken[4] > -1){
    if(blinkState_pin4 == true){
     digitalWrite(LED_4_pin, HIGH); 
     blinkState_pin4 = false;
    }else{
      digitalWrite(LED_4_pin, LOW);
      blinkState_pin4 = true;
    }
    
  }else{
    digitalWrite(LED_4_pin, LOW);
    
  }

  if(colToken[5] > -1){
    if(blinkState_pin5 == true){
     digitalWrite(LED_5_pin, HIGH); 
     blinkState_pin5 = false;
    }else{
      digitalWrite(LED_5_pin, LOW);
      blinkState_pin5 = true;
    }
    
  }else{
    digitalWrite(LED_5_pin, LOW);
  }

  if(colToken[6] > -1){
    if(blinkState_pin6 == true){
     digitalWrite(LED_6_pin, HIGH); 
     blinkState_pin6 = false;
    }else{
      digitalWrite(LED_6_pin, LOW);
      blinkState_pin6 = true;
    }
    
  }else{
    digitalWrite(LED_6_pin, LOW);
  }

}

void LevelBlink(void){

    if(blinkState == true){
     digitalWrite(LED_0_pin, HIGH);
     digitalWrite(LED_1_pin, HIGH);
     digitalWrite(LED_2_pin, HIGH); 
     blinkState = false;
    }else{
      digitalWrite(LED_0_pin, LOW);
      digitalWrite(LED_1_pin, LOW);
      digitalWrite(LED_2_pin, LOW);
      blinkState = true;
    }

      digitalWrite(LED_3_pin, LOW);
      digitalWrite(LED_4_pin, LOW);
      digitalWrite(LED_5_pin, LOW);
      digitalWrite(LED_6_pin, LOW);
    

}
void WinnerBlink(void){
  if(gameOver == true){ 
    if(PlayerWin_flag == true){
      if(blinkState == true){
        digitalWrite(LED_win_pin, HIGH);
        blinkState = false;
      }else{
        digitalWrite(LED_win_pin, LOW);
        blinkState = true;
      }
       

    }
    if(CPUwin_flag == true){
       if(blinkState == true){
        digitalWrite(LED_lose_pin, HIGH);
        blinkState = false;
      }else{
        digitalWrite(LED_lose_pin, LOW);
        blinkState = true;
      }
    }
    if(Draw_flag == true){
       if(blinkState == true){
        digitalWrite(LED_draw_pin, HIGH);
        blinkState = false;
      }else{
        digitalWrite(LED_draw_pin, LOW);
        blinkState = true;
      }
    }
  }
}
