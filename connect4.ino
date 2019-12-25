//
//  main.c
//  connect 4 made in c
//
//  Created by Ron Gaines on 9/4/19.
//  Copyright © 2019 Ron Gaines. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int colToken[7] = {5,5,5,5,5,5,5};
int gameboard[6][7];
int M = 6;
int N = 7;
int turn = 1;
int pushButton = 2;
int pushButton1 = 7;
int pushButton2 = 8;
int pushButton3 = 9;
int pushButton4 = 10;
int pushButton5 = 11;
int pushButton6 = 12;
int pushButton7 = 13;
volatile byte stateVector[7] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};

//int StateVector[7] = {state0, state1, state2,state3,state4,state5,state6};

int verSum[21];
int horSum[24];
int posDiagSum[12];
int negDiagSum[12];
bool winState = false;

bool isMoveValid(int move);
bool checkWinState(void);

int pickMove(void);
void playMove(int move);
void printBoard(void);
void createBoard(void);
void evalBoard(void);
void printScoreVectors(void);
void createScoreVectors(void);
int rand(void);
int AIpickMove(void);

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  pinMode(pushButton, INPUT);
  pinMode(pushButton1, INPUT);
  pinMode(pushButton2, INPUT);
  attachInterrupt(digitalPinToInterrupt(pushButton), collect, FALLING);
}

void loop(void) {
    createBoard();
    createScoreVectors();
    evalBoard();
    printBoard();
    
    int size = 0;
    size = sizeof(gameboard);
    //Serial.print("The size of this board in memory is %d bytes", size);
    //Serial.print("\n");

    while (turn < 45) {
        // Game becomes a draw when turn counter hits
        if (turn == 45) {
            Serial.print("The Game is a draw.");
            break;
        }
    
    int move;
            if (turn % 2 == 0) {
            move = AIpickMove();
            playMove(move);
            Serial.print("the CPU move is: ");
            Serial.print(move, DEC);
            Serial.println();
            
        }
        else{
            bool PlayerMove = false;
            while(PlayerMove == false){
              move = pickMove();
              PlayerMove = isMoveValid(move);
              delay(100);
            }
            
            Serial.print("your move is: ");
            Serial.print(move, DEC);
            Serial.println();
            playMove(move);
        }
    
    printBoard();   
    
    Serial.print("The token at column ");
    Serial.print(move, DEC);
    Serial.print("is: ");
    Serial.print(colToken[move],DEC);
    Serial.println();
    evalBoard();
    //printScoreVectors();
    winState = checkWinState();
        if (winState == true) {
            if (turn % 2 == 0) {
                Serial.print("Player one won!!!\n");
            }
            else{
              Serial.print("The CPU won!!!\n");
            }
            
            break;
        }
    
    }
    
    return 0;
}

void collect(void){
    stateVector[0] = digitalRead(pushButton1);
    stateVector[1] = digitalRead(pushButton2);
    stateVector[2] = HIGH;
    stateVector[3] = HIGH;
    stateVector[4] = HIGH;
    stateVector[5] = HIGH;
    stateVector[6] = HIGH;
}

bool isMoveValid(int move){
    bool Check;
    int col = colToken[move];
    if (col <  0  || col > 5) {
        Check = false;
        
    }
    else{
        Check = true;
    }
    return Check;
}

int pickMove(void){
    int col = -1;
    //bool Check;
    
    //printf("Enter a column: ");
    //scanf("%d", &col);
    for(int m = 0; m < 7; m++){
      if(stateVector[m] == 0){
        col = m;
      }
    }
    //Check = isMoveValid(col);
    /*
    while (Check == false) {
        printf("Your pick is not valid.\n Enter a column: ");
        scanf("%d", &col);
        Check = isMoveValid(col);
    
    while (col < 0 || col > 6) {
        printf("Your pick is not valid.\n Enter a column: ");
        scanf("%d", &col);
        Check = isMoveValid(col);
        
    }
    
    }
   */ 
    return col;
}

void playMove(int move){
    if (turn % 2 == 0) {
        gameboard[colToken[move]][move] = -1;
    }
    else{
        gameboard[colToken[move]][move] = 1;
        resetStateVector(move);
    }
    
    colToken[move]--;
    turn++;
}

void printBoard(void){
    for (int m=0; m<M; m++) {
        for (int n=0; n<N; n++) {
            
            if(gameboard[m][n] == 0 || gameboard[m][n] == 1){
             Serial.print(" "); 
            }
            Serial.print(gameboard[m][n], DEC);
        }
        Serial.println();
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

void printScoreVectors(void){
    for (int now = 0; now <21
         ; now++) {
        printf("%d ",verSum[now]);
    }
    printf("\n");
    for (int now = 0; now <24; now++) {
        printf("%d ",horSum[now]);
    }
    printf("\n");
    
    for (int now = 0; now <12; now++) {
        printf("%d ",posDiagSum[now]);
    }
    printf("\n");
    
    for (int now = 0; now <12; now++) {
        printf("%d ",negDiagSum[now]);
    }
    printf("\n");

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

int AIpickMove(void){
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
void resetStateVector(int move){
  stateVector[move] = HIGH;
}
