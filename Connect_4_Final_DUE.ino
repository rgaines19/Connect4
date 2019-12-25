#include <Servo.h>

int state = 0;
int next_state = 0;
const int ROW = 6;
const int COL = 7;
const int VERTICAL_WIN_STATES = 21;
const int HORIZONTAL_WIN_STATES = 24;
const int DIAGANOL_WIN_STATES = 12;
const int PLAYER_1_WIN = 4;
const int PLAYER_2_WIN = -4;
const int PLAYER_1_PIECE = 1;
const int PLAYER_2_PIECE = -1;
const int ALPHA_INIT = -2147483648; // smallest 32 bit number ie 2^31
const int BETA_INIT = 2147483647;   // largest  32 bit number 2^31 -1
int level = 1;
int turn = 1;

struct game{
  int gameboard[ROW][COL];
  int colToken[COL];
};

struct minmax_vars{
    int column;
    int value;
};

struct score_vectors{
    int verSum[VERTICAL_WIN_STATES];
    int horSum[HORIZONTAL_WIN_STATES];
    int posDiagSum[DIAGANOL_WIN_STATES];
    int negDiagSum[DIAGANOL_WIN_STATES];
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  state = next_state;
  switch (state){
    case 0: //set up game state



      next_state = 1;
      break;
    case 1: // pick cpu level


      next_state = 2;
      break;
    case 2: // player pick move



      next_state = 3;
      break;
    case 3: // play moves using servo comands
      break;
    case 4: // check the win state 
      break;
    case 5: // AI picks its move
      break;
    case 6: // goes to this state when the game is over
      next_state = 6;
      break;
    default: // if error in the program go to state 6
      next_state = 6;
      break;
  }
}

struct game createGame(void){
  struct game g;
  for(int a =0; a < ROW; a++){
    for(int b = 0; b < COL; b++){
      g.gameboard[a][b] = 0;
    }
  }

  for(int i = 0; i<COL; i++){
    g.colToken[i] = 5;
  }
  return g;
}

struct game copygame(struct game g){
  struct game g1;
  for(int a =0; a < ROW; a++){
    for(int b = 0; b < COL; b++){
      g1.gameboard[a][b] = g.gameboard[a][b];
    }
  }

  for(int i = 0; i<COL; i++){
    g1.colToken[i] = g.colToken[i];
  }
  return g;
}

bool checkWinState(struct game g, struct score_vectors s, int playerPiece){
    bool win = false;
    int check_value;
    if(playerPiece == PLAYER_2_PIECE){
        check_value = PLAYER_2_WIN;
    }
    else{
        check_value = PLAYER_1_WIN;
    }
    for (int now = 0; now <VERTICAL_WIN_STATES; now++) {
        if (s.verSum[now] == check_value){
            win = true;
            break;
        }
    }
    
    for (int now = 0; now <HORIZONTAL_WIN_STATES; now++) {
        if(s.horSum[now] == check_value){
            win = true;
            break;
        }
    }

    
    for (int now = 0; now <DIAGANOL_WIN_STATES; now++) {
        if(s.posDiagSum[now] == check_value){
            win = true;
            break;
        }
    }

    
    for (int now = 0; now <DIAGANOL_WIN_STATES; now++) {
        if(s.negDiagSum[now] == check_value){
            win = true;
            break;
        }
    }

    return win;
}
struct game playMove(struct game g,int col, int playerPiece){
    g.gameboard[g.colToken[col]][col] = playerPiece;
    g.colToken[col]--;
    return g;
}

struct score_vectors evalBoard(struct game g, struct score_vectors s){
    int count = 0;
    
    // vertical check function
    for (int x = 0; x < 7; x++) {
        for (int y =0; y<3; y++) {
            s.verSum[count] = g.gameboard[y][x]+g.gameboard[y+1][x]+g.gameboard[y+2][x]+g.gameboard[y+3][x];
            count++;
        }
    }
    count = 0;
    // horizontal check function
    for (int x = 0; x < 6; x++) {
        for (int y =0; y<4; y++) {
            s.horSum[count] = g.gameboard[x][y]+g.gameboard[x][y+1]+g.gameboard[x][y+2]+g.gameboard[x][y+3];
            count++;
        }
    }
    count = 0;
    // Positive Edge Diagnol check function
    for(int r =0; r < 3; r++){
        for (int c =0; c <4; c++) {
            s.posDiagSum[count] = g.gameboard[r][c] + g.gameboard[r+1][c+1] + g.gameboard[r+2][c+2] + g.gameboard[r+3][c+3];
            count++;
        }
    }
    count = 0;
    // Negative Edge Diagnol check function
    for(int r =0; r < 3; r++){
        for (int c = 6; c > 2; c--) {
            s.negDiagSum[count] = g.gameboard[r][c] + g.gameboard[r+1][c-1] + g.gameboard[r+2][c-2] + g.gameboard[r+3][c-3];
            count++;
        }
    }
    return s;
}

void printBoard(struct game g){
    for (int m=0; m<ROW; m++) {
        for (int n=0; n<COL; n++) {
            printf("%4d ", g.gameboard[m][n]);
        }
        printf("\n");
    }
}


int AIpickMove(struct game g, struct score_vectors s){
    int col =0;
    struct minmax_vars m;
    m.column =0;
    m.value =0;
    switch (level){
      case 1:
        col = AIrandMove(g);
        break;
      case 2:
        m = minimax(g,1,true,m,s);
        col = m.column;
        break;
      case 3:
        m = minimax(g,4,true,m,s);
        col = m.column;
        break;
      default:
      break;
    }
    
    return col;
}

int scoreBoard(struct game g,struct score_vectors s,int playerPiece){
    int score = 0;
    int center_column_count = 0;
    int player = 1;
    if (playerPiece == PLAYER_2_PIECE) {
        player = -1;
    }
 
    for (int i = 0; i<ROW; i++) {
        if (g.gameboard[i][3]== 1*player) {
            center_column_count++;
        }
    }
    score += center_column_count*1000;
        
        for (int x =0; x<24; x++) {
            if (s.horSum[x]==4*player) {
                score+=100000;
            }
            if (s.horSum[x]==3*player) {
                score+=0;
            }
            if (s.horSum[x]==2*player) {
                score+=0;
            }
            
            if (s.horSum[x]==(-2)*player) {
                score-=500;
            }
             
            
            if (s.horSum[x]==(-3)*player) {
                score-=10000;
            }
            
            if (s.horSum[x]==(-4)*player) {
                score-=1000000000;
            }
            
        }
        
        for (int x =0; x<21; x++) {
            if (s.verSum[x]==4*player) {
                score+=100000;
            }
            if (s.verSum[x]==3*player) {
                score+=0;
            }
            if (s.verSum[x]==2*player) {
                score+=0;
            }
            
            if (s.verSum[x]==(-2)*player) {
                score-=1000;
            }
            
            if (s.verSum[x]==(-3)*player) {
                score-=10000;
            }
            if (s.verSum[x]==(-4)*player) {
                score-=1000000000;
            }
        }
        for (int x =0; x<12; x++) {
            if (s.posDiagSum[x]==4*player) {
                score+=100000;
            }
            if (s.posDiagSum[x]==3*player) {
                score+=0;
            }
            if (s.posDiagSum[x]==2*player) {
                score+=0;
            }
            
            if (s.posDiagSum[x]==-2*player) {
                score-=1000;
            }
            
            if (s.posDiagSum[x]==(-3)*player) {
                score-=10000;
            }
            if (s.posDiagSum[x]==(-4)*player) {
                score-=1000000000;
            }
        }
        for (int x =0; x<12; x++) {
            if (s.negDiagSum[x]==4*player) {
                score+=100000;
            }
            if (s.negDiagSum[x]==3*player) {
                score+=0;
            }
            if (s.negDiagSum[x]==2*player) {
                score+=0;
            }
            
            if (s.negDiagSum[x]==(-2)*player) {
                score-=1000;
            }
             
            if (s.negDiagSum[x]==(-3)*player) {
                score-=10000;
            }
            if (s.negDiagSum[x]==(-4)*player) {
                score-=1000000000;
            }
            
        }
        
    
    return score;
    
}

bool valid_moves_left(struct game g){
    bool valid = false;
    int count = 0;
    for (int i =0; i < COL; i++) {
        if (g.colToken[i] == -1) {
            count++;
        }
        
    }
    if (count == 7) {
        valid = true;
    }
    return valid;
}

bool is_terminal_node(struct game g, struct score_vectors s){
    bool winstate = false;
    bool cek1 = checkWinState(g, s, PLAYER_1_PIECE);
    bool cek2 = checkWinState(g, s, PLAYER_2_PIECE);
    bool cek3 = valid_moves_left(g);
    if (cek1 || cek2 ||cek3 ) {
        winstate = true;
    }
            return winstate;
}

        
    struct score_vectors createScoreVectors(struct score_vectors s){
        for (int j = 0; j<24; j++) {
            if (j<21) {
                s.verSum[j] =0;
            }
            if (j<12){
                s.posDiagSum[j] = 0;
                s.negDiagSum[j] = 0;
            }
            s.horSum[j]=0;
        }
        return s;
    }
struct minmax_vars minimax(struct game g,int depth, bool maximizingPlayer, struct minmax_vars m,struct score_vectors s){

bool is_terminal = is_terminal_node(g,s);
if ((depth <= 0) ||  (is_terminal == true)) {
    if (is_terminal == true) {
        if (checkWinState(g, s, PLAYER_1_PIECE) == true) {
            m.value = -100000000;
            //printf("The score is: %d\n", m.value);
            return m;
        }
    
        else if((checkWinState(g, s, PLAYER_2_PIECE) == true)){
            m.value = 100000000;
            //printf("The score is: %d\n", m.value);
            return m;
        }
        else {
            m.value = 0;
            return m;
        }
    }else{
        s = evalBoard(g, s);
        m.value = scoreBoard(g,s,PLAYER_2_PIECE);
        //printf("The score is: %d\n",m.value);
        return m;
    }
}
    if (maximizingPlayer == true) {
        int value = ALPHA_INIT;
        for (int col = 0; col < COL; col++) {
            if (g.colToken[col] > -1 && g.colToken[col] < 6) {
                struct game board_copy;
                board_copy = copygame(g);
                board_copy = playMove(board_copy, col, PLAYER_2_PIECE);
                s = evalBoard(board_copy, s);
                //printf("\n");
                //printBoard(board_copy);
                m = minimax(board_copy, depth-1, false, m,s);
                if (m.value > value) {
                    value = m.value;
                    m.column = col;
                }
               
            }
            
        }
        return m;
    }
    else
    {
        int value = BETA_INIT;
        for (int col = 0; col < COL; col++) {
            if (g.colToken[col] > -1 && g.colToken[col] < 6) {
                struct game board_copy;
                board_copy = copygame(g);
                board_copy = playMove(board_copy, col, PLAYER_1_PIECE);
                s = evalBoard(board_copy, s);
                //printf("\n");
                //printBoard(board_copy);
                m = minimax(board_copy, depth-1,true, m,s);
                if (m.value < value) {
                    value = m.value;
                    m.column = col;
                }
                
            }
            else{
                //printf("column full\n");
            }
            
        }
        return m;
    }
}
int AIrandMove(struct game g){
    //int col = rand() % 7;
    int col = random(0,7);
    bool validmove = false;
    while (validmove == false) {
        col = random(0,6);
        //col = rand()%7;
        validmove = isMoveValid(col, g);
    }
    return col;
}

bool isMoveValid(int move, struct game g){
    bool Check;
    int col = g.colToken[move];
    if (col <  0  || col > 5) {
        Check = false;
        
    }
    else{
        Check = true;
    }
    return Check;
}
