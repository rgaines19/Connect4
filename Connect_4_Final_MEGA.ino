#include <Servo.h>
//First column servo command pos = 19
//Seventh column servo command pos = 157

int colToken[7] = {5,5,5,5,5,5,5};
int gameboard[6][7];
int colToken_cpy[7] = {5,5,5,5,5,5,5};
int gameboard_cpy[6][7];
int turn = 1;
int level = 2;
int M = 6;
int N = 7;
int count  = 0;

int verSum[21];
int horSum[24];
int posDiagSum[12];
int negDiagSum[12];
int verSum_cpy[21];
int horSum_cpy[24];
int posDiagSum_cpy[12];
int negDiagSum_cpy[12];
bool winState = false;

int state = 0;
int next_state = 0;
bool gameOver = false;
int Move;
bool players_turn = true;
int turnOrder[13] = {7,7,7,7,0,1,2,3,4,5,6,7,0};
int turnOrdercount = 0;


volatile byte A0_state = LOW;
volatile byte A1_state = LOW;
volatile byte A2_state = LOW;

int interrupt_signal_pin = 2;
int enable_input_ic_pin = 12;
int A0_pin = 52;
int A1_pin = 50;
int A2_pin = 48;
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
int servo_chip_dispenser_pin = 4;
int servo_chute_position_pin = 5;
int servo_chip_release_pin = 6;

volatile byte state1 = LOW;
volatile byte state2 = LOW;
volatile byte state3 = LOW;
int sum = 10;
bool isValidmove = false; 
bool isValid = false;

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
  pinMode(A0_pin,INPUT);
  pinMode(A0_pin,INPUT);
  attachInterrupt(digitalPinToInterrupt(interrupt_signal_pin),collect,RISING);
  
  
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


      next_state = 1;
      break;
    case 1: // pick cpu level
      while(isValid == false){
        level = sum;
        delay(100);
        if(isCPUlevelValid(level) == true){
          isValid = true;
        }
      }

      next_state = 2;
      isValid = false;
      break;
    case 2: // player pick move using controller
//      Move = turnOrder[turnOrdercount];
//      turnOrdercount++;
      players_turn = false;
      sum = 10;
      Move = sum;
      Serial.print("Sum is: ");
      Serial.println(Move,DEC);
      interrupts();
      while(isValidmove == false){
        Move = sum;
        delay(100);
        if(isMoveValid(Move) == true){
          isValidmove = true;
        }
        delay(100);
        Serial.println(isValidmove,DEC);
      }
      
      if(isMoveValid(Move) == true){
      next_state = 3;
      isValidmove = false;
      digitalWrite(interrupt_signal_pin,HIGH);
      Serial.print("Sum is: ");
      Serial.println(Move,DEC);
      noInterrupts();
      }
      else{
        next_state = 2;
      }
      break;
    case 3: // play moves using servo comands
      playMove(Move);
      printBoard();
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
                        Serial.print("Player one won!!!\n");
                        digitalWrite(LED_win_pin,HIGH);
                    }
                    else{
                      Serial.print("The CPU won!!!\n");
                      digitalWrite(LED_lose_pin,HIGH);
                    }
                    next_state = 6;
                    break;
                }
            if (turn == 43) {
                Serial.print("The Game is a draw.");
                digitalWrite(LED_draw_pin,HIGH);
                next_state = 6;
                break;
            }
            if (players_turn == true) {
                next_state = 2;
            }else {
            next_state = 5;
            } 
      break;
    case 5: // AI picks its move
      Move = AIpickMove();
      Serial.print("the CPU move is: ");
      Serial.print(Move,DEC);
      Serial.println();
      players_turn = true;
      next_state = 3;
      break;
    case 6: // goes to this state when the game is over
    Serial.print("The game is over\n");
      gameOver = true;
      next_state = 6;
      break;
    default: // if error in the program go to state 6
      Serial.print("We are in the default state.\n");
      next_state = 6;
      break;
  }
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
                score+=50; // it was 100000
            }
            if (horSum_cpy[x]==-3) {
                score+=3;
            }
            if (horSum_cpy[x]==-2) {
                score+=2;
            }
            if (horSum_cpy[x]==2) {
                score-=5; //it was 40
            }
            if (horSum_cpy[x]==3) {
                score-=10; // it was 100
            }
            
        }
        
        for (int x =0; x<21; x++) {
            if (verSum_cpy[x]==-4) {
                score+=50;
            }
            if (verSum_cpy[x]==-3) {
                score+=2;
            }
            if (verSum_cpy[x]==-2) {
                score+=1;
            }
            if (verSum_cpy[x]==2) {
                score-=5;
            }
            if (verSum_cpy[x]==3) {
                score-=10;
            }
        }
        for (int x =0; x<12; x++) {
            if (posDiagSum_cpy[x]==-4) {
                score+=50;
            }
            if (posDiagSum_cpy[x]==-3) {
                score+=2;
            }
            if (posDiagSum_cpy[x]==-2) {
                score+=1;
            }
            if (posDiagSum_cpy[x]==2) {
                score-=5;
            }

            if (posDiagSum_cpy[x]==3) {
                score-=10;
            }
        }
        for (int x =0; x<12; x++) {
            if (negDiagSum_cpy[x]==-4) {
                score+=50;
            }
            if (negDiagSum_cpy[x]==-3) {
                score+=2;
            }
            if (negDiagSum_cpy[x]==-2) {
                score+=1;
            }
            if (negDiagSum_cpy[x]==2) {
                score-=5;
            }
            if (negDiagSum_cpy[x]==3) {
                score-=10;
            }
            
        }
        if (score >= best_score) {
            best_score = score;
            best_col = col;
            
        }
        Serial.print("score for col ");
        Serial.print(col, DEC);
        Serial.print(" is :");
        Serial.println(score,DEC);
        }
        else{
            Serial.print("for col : ");
            Serial.print(col,DEC);
            Serial.println(" we did not compute");
        }
    }
    if (best_col == 10) {
        Serial.print("Error in the fuction\n");
        
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
    int col = colToken[Move];
    if (col <  0  || col > 5) {
        Check = false;
        
    }
    else{
        Check = true;
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
    switch(col){
      case 0:
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
      case 1:
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
      case 2:
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
      case 3:
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
      case 4:
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
      case 5:
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
      case 6:
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
        chip_dispenser.write(5);
        delay(50);
        chute.write(5);
        delay(50);
        chip_release.write(5);
        delay(50);
}
}

void collect(void){
    count++;
    state1 = digitalRead(A0_pin);
    state2 = digitalRead(A1_pin);
    state3 = digitalRead(A2_pin);
 

    digitalWrite(enable_input_ic_pin, HIGH);
    Serial.print("You have entered the interrupt : ");
    Serial.println(count,DEC);

    sum = 4*state3 + 2 *state2 + state1 - 1;
    
    Serial.print("Sum is: ");
    Serial.println(sum,DEC);
        Serial.print("A0 is: ");
    Serial.println(state1,DEC);
        Serial.print("A1 is: ");
    Serial.println(state2,DEC);
        Serial.print("A2 is: ");
    Serial.println(state3,DEC);
   
    

    
    
}
bool isCPUlevelValid(int level){
  bool check = false;
   if(level == 0 || level == 1 || level == 2){
    check = true;
   }
  return check;
}
