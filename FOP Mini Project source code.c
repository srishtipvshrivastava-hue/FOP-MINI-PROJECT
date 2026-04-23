#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

char playerName[50];
time_t startTime;
int lives=3;
char input;



#include <limits.h>
#define ROW 17
#define COL 13
#define MAX_PATH 300

struct Node {
    int x, y;
    int g, h, f;
    int parentX, parentY;
};

//min heap
struct MinHeap {
    struct Node arr[100];
    int size;
};

struct Node path[MAX_PATH];
int pathLength = 0;

//useful functions
int isValid(int x, int y) {
    return (x >= 0 && x < ROW && y >= 0 && y < COL);
}
int isBlocked(int x, int y, int grid[17][13]){
    return grid[x][y] == 2;
}
int heuristic(int x, int y, int gx, int gy){
    return abs(x - gx) + abs(y - gy);
}

//heap function
void swap( struct Node *a, struct Node *b){
    struct Node temp = *a;
    *a = *b;
    *b = temp;
}
void heapifyUP(struct MinHeap *heap, int i){
    while (i > 0){
        int parent = (i-1)/2;
        if ( (*heap).arr[parent].f > (*heap).arr[i].f){
            swap(&(*heap).arr[parent],&(*heap).arr[i]);
            i = parent;
        } else break;

    }
}
void heapifyDown(struct MinHeap *heap, int i){
    int smallest = i;
    int left = 2*i+1;
    int right = 2*i+2;
    if (left < (*heap).size && (*heap).arr[left].f < (*heap).arr[smallest].f)
        smallest = left;
    if (right < (*heap).size && (*heap).arr[right].f < (*heap).arr[smallest].f)
        smallest = right;
    if (smallest != i){
        swap(&(*heap).arr[i],&(*heap).arr[smallest]);
        heapifyDown(heap,smallest); 
    }
}
void insertHeap(struct MinHeap *heap, struct Node node){
    (*heap).arr[(*heap).size] = node;
    heapifyUP(heap, (*heap).size);
    (*heap).size++;
}
struct Node extractMin(struct MinHeap *heap){
    struct Node root = (*heap).arr[0];
    (*heap).arr[0] = (*heap).arr[--(*heap).size];
    heapifyDown(heap,0);
    return root;
}

//---------A* algorithm-------------

void ReconstructPath(struct Node GoalNode){
    int x = GoalNode.x;
    int y =  GoalNode.y;
    pathLength = 0;
    while (x != -1 && y != -1 && pathLength < MAX_PATH){
        path[pathLength].x = x;
        path[pathLength].y = y;
        pathLength++;

        int px = GoalNode.parentX;
        int py = GoalNode.parentY;
        x = px;
        y = py;

        GoalNode.x = x;
        GoalNode.y = y;
        GoalNode.parentX = px;
        GoalNode.parentY = py;
    }

    for (int i = 0; i < pathLength/2; i++) {
        struct Node temp = path[i];
        path[i] = path[pathLength - i - 1];
        path[pathLength - i - 1] = temp;
    }
    
}

void aStar(int sx, int sy, int gx, int gy, int grid[ROW][COL]){
    struct MinHeap open;
    open.size = 0;
    int closed[ROW][COL] = {0};
    
    struct Node start = {sx, sy, 0, 0, 0, -1, -1};
    start.h = heuristic(sx, sy, gx, gy);
    start.f = start.h;
    insertHeap(&open, start);

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (open.size > 0){
        struct Node current = extractMin(&open);
        
        if (current.x == gx && current.y == gy){
            ReconstructPath(current);
            return;
        }

        closed[current.x][current.y] = 1;
        for(int i = 0 ; i < 4; i++){
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (!isValid(nx, ny) || isBlocked(nx, ny, grid) || closed[nx][ny])
                continue;

                struct Node neighbour;
                neighbour.x = nx;
                neighbour.y = ny;
                neighbour.g = current.g+1;
                neighbour.h = heuristic(nx, ny, gx, gy);
                neighbour.f = neighbour.g + neighbour.h;
                neighbour.parentX = current.x;
                neighbour.parentY = current.y;

                insertHeap(&open, neighbour);
        }
    }
}


/* ── Arcade cabinet frame ─────────────────────────────────
   grid_cols = 11 (easy) or 13 (hard)
   each cell prints 2 chars, so grid visual width = cols*2
   inner panel width = grid_cols*2 + 4 (2 pad each side)
   ────────────────────────────────────────────────────── */
void cabinet_top(int score, int lives_left, int power_up, int grid_cols) {
    int iw = grid_cols * 2 + 4;   /* inner width */
    int bw = iw + 6;              /* border width (3 each side) */

    /* red top border */
    printf("\033[31m");
    for (int i = 0; i < bw; i++) printf("█");
    printf("\033[0m\n\r");

    /* marquee: yellow text on white bg */
    printf("\033[31m███\033[0m");
    printf("\033[1;33;107m");
    int pad = (iw - 7) / 2;
    for (int i = 0; i < pad; i++) printf(" ");
    printf("PAC-MAN");
    for (int i = 0; i < iw - 7 - pad; i++) printf(" ");
    printf("\033[0m\033[31m███\033[0m\n\r");

    /* red divider */
    printf("\033[31m");
    for (int i = 0; i < bw; i++) printf("█");
    printf("\033[0m\n\r");

    /* panel top yellow rule */
    printf("\033[31m███\033[40m\033[33m");
    for (int i = 0; i < iw; i++) printf("─");
    printf("\033[0m\033[31m███\033[0m\n\r");

    /* score/lives bar */
    char bar1[128];
    char bar2[132];
    if(input=='s'){
      snprintf(bar1, sizeof bar1, "\033[93mSCORE:%-4d\033[0m\033[40m\033[92mLIVES:%d\033[0m\033[40m \033[96m PWR:%-3s\033[0m\033[40m",
             score, lives_left, power_up ? "ON" : "OFF");
      printf("\033[31m███\033[40m%s", bar1);
      }
     else
      {snprintf(bar2, sizeof bar2, "\033[93mSCORE:%-4d\033[0m\033[40m\033[92mLIVES:%d\033[0m\033[40m \033[96m PWR:%-3s\033[0m\033[40m",
               score, lives_left, power_up ? "ON" : "OFF");
      printf("\033[31m███\033[40m%s", bar2);
      }
    /* pad remainder */
    //int vis = 1 + 10 + 2 + 8 + 5 + 7;
    //for (int i = vis; i < iw; i++) printf(" ");
    //printf("\033[0m\033[31m███\033[0m\n\r");
    /*if(input=='w'){
       int vis_easy = 1 + 10 + 2 + 8 + 5 + 7;
       for (int i = vis_easy; i < iw; i++) printf(" ");
    printf("\033[0m\033[31m███\033[0m\n\r");
    }
    if(input=='s'){
      int vis_hard = 1 + 10 + 2 + 8 + 5 + 7;
      for (int i = vis_hard; i < iw; i++) printf(" ");
    printf("\033[0m\033[31m███\033[0m\n\r");
    }
    */
    int vis;

     if (grid_cols == 11){
       vis = strlen(bar1);
       }// easy
     else{
       vis = strlen(bar2);
       }// hard (same width)

    for (int i = vis; i < iw; i++)
    printf(" ");
    printf("\033[0m\033[31m███\033[0m\n\r");

    /* empty spacer */
    printf("\033[31m███\033[40m");
    for (int i = 0; i < iw; i++) printf(" ");
    printf("\033[0m\033[31m███\033[0m\n\r");
}

void cabinet_row_open(void) {
    printf("\033[31m███\033[40m  \033[0m");
}

void cabinet_row_close(void) {
    printf("\033[40m  \033[0m\033[31m███\033[0m\n\r");
}

void cabinet_bottom(int grid_cols) {
    int iw = grid_cols * 2 + 4;
    int bw = iw + 6;

    /* empty spacer */
    printf("\033[31m███\033[40m");
    for (int i = 0; i < iw; i++) printf(" ");
    printf("\033[0m\033[31m███\033[0m\n\r");

    /* panel bottom yellow rule */
    printf("\033[31m███\033[40m\033[33m");
    for (int i = 0; i < iw; i++) printf("─");
    printf("\033[0m\033[31m███\033[0m\n\r");

    /* joystick shaft row */
    printf("\033[31m███\033[40m ");
    printf(" \033[37m│\033[0m\033[40m");
    int mid = iw - 9;
    for (int i = 0; i < mid/2; i++) printf(" ");
    printf("\033[97m  \033[0m\033[40m");
    for (int i = 0; i < mid - mid/2; i++) printf(" ");
    printf("\033[37m│\033[0m\033[40m  ");
    printf(" \033[0m\033[31m███\033[0m\n\r");

    /* joystick ball row */
    printf("\033[31m███\033[40m ");
    printf("\033[91m(●)\033[0m\033[40m");
    int gap = iw - 10;
    for (int i = 0; i < gap/2; i++) printf(" ");
    printf("\033[97m  \033[0m\033[40m");
    for (int i = 0; i < gap - gap/2; i++) printf(" ");
    printf("\033[94m(●)\033[0m\033[40m");
    printf(" \033[0m\033[31m███\033[0m\n\r");

    /* joystick base row */
    printf("\033[31m███\033[40m ");
    printf("\033[90m▄▄▄▄▄\033[0m\033[40m");
    for (int i = 0; i < iw - 12; i++) printf(" ");
    printf("\033[90m▄▄▄▄▄\033[0m\033[40m");
    printf(" \033[0m\033[31m███\033[0m\n\r");

    /* red bottom border */
    printf("\033[31m");
    for (int i = 0; i < bw; i++) printf("█");
    printf("\033[0m\n\r");
}


void saveScore(char playerName[], int duration, int score) {
    FILE *fp;
    int fileExists = 0;

    fp = fopen("scores.csv", "r");
    if (fp != NULL) {
        fileExists = 1;
        fclose(fp);
    }

    fp = fopen("scores.csv", "a");
    if (fp == NULL) {
        printf("Error opening file\n");
        return;
    }

    if (!fileExists) {
        fprintf(fp, "PlayerName,DateTime,DurationSeconds,Score\n");
    }

    time_t now = time(NULL);
    char dateTime[30];
    strcpy(dateTime, ctime(&now));
    dateTime[strlen(dateTime) - 1] = '\0';

    fprintf(fp, "%s,%s,%d,%d\n", playerName, dateTime, duration, score);
    fclose(fp);
}

void showLeaderboard(){ FILE *fp = fopen("scores.csv","r"); if (!fp){ printf("No leaderboard yet!\n"); return; }

char line[200]; while (fgets(line, sizeof(line), fp)) { printf("%s",line); } fclose(fp); }

int end_S(int game_sit, int score, int duration); //global declaration

char getInput(char last_dir)
{
    fd_set inputdeviceset;
    struct timeval wait_t;

    FD_ZERO(&inputdeviceset);
    FD_SET(STDIN_FILENO, &inputdeviceset);

    wait_t.tv_sec = 0;
    wait_t.tv_usec = 0;   

    int ret_val = select(STDIN_FILENO + 1, &inputdeviceset, NULL, NULL, &wait_t);

    if (ret_val > 0) {
        char input;
        read(STDIN_FILENO, &input, 1);
        input = tolower(input);

        if (input == 'w' || input == 'a' || input == 's' || input == 'd')
            return input;
        
    }

    return last_dir;  
}

void live_check(int lives){
if (lives == 3){
printf(" ☺ ☺ ☺\n\r");
}
else if (lives == 2){
printf(" ☺ ☺\n\r");
}
else if (lives == 1){
printf(" ☺ \n\r");
}
}

int check_direction(int *pacman, char direction)
{
    switch(direction)
    {
        case 'a':  
        return *(pacman - 1);   // left
        case 'd':  
        return *(pacman + 1);   // right
        case 'w':  
        return *(pacman - 13);  // up
        case 's':  
        return *(pacman + 13);  // down
        default: 
        return 0;
    }
}

int* movement(int *pacman, int grid[17][13], char direction)
{
    int *move = pacman;

    // movement
    if(direction == 'a')        
    move = pacman - 1;   // left
    else if(direction == 'd')   
    move = pacman + 1;   // right
    else if(direction == 'w')  
    move = pacman - 13;  // up
    else if(direction == 's')   
    move = pacman + 13;  // down;

    // wall check
    if (*move == 2)
        return pacman;  // hit wall, stay

    // teleportation
    if (move == &grid[0][6])
       move = &grid[15][6];
    else if(move == &grid[16][6])
       move = &grid[1][6];
    
    if (move == &grid[8][0])
       move = &grid[8][11];
    else if(move == &grid[8][12])
       move = &grid[8][1];

    return move;
}

int death_check(int *pacman, int *ghost1, int *ghost2, int *ghost3){
    if (ghost1 == pacman || ghost2 == pacman ||ghost3 == pacman){
        return 1;
    }
    return 0;
}

int pac_update(int *pacman, int *power_up, int *power_timer)
{
    int score=0;
    
    if(*pacman==1)  //checking and eating the pellets
    {
        *pacman=0;
         score++;
    }
    else if(*pacman==3) //power up
        {*pacman=0;
        *power_up=1;
        *power_timer=30; 
        }
    
    return score;
}

char ghost_decide_dirn(int *ghost, char dirn, int *randchance){
    
    if (check_direction(ghost,dirn) == 2){
        switch(dirn)
        {
         case 'w':
            dirn = 's';
            break;
         case 'a':
            dirn = 'd';
            break;
         case 's':
            dirn = 'w';
            break;
         case 'd':
            dirn = 'a';
            break;
        }
    }
    
    if ((dirn != 'd') && (dirn != 'a')  && check_direction(ghost,'a') != 2 && rand()%20 < (*randchance)++){
        dirn = 'a';
        *randchance = 10;
    }
    else if ((dirn != 's') && (dirn != 'w') && check_direction(ghost,'w')!= 2 && rand()%20 < *randchance){
        dirn = 'w';
        *randchance = 10;
    }
    else if ((dirn != 'w') && (dirn != 's') && check_direction(ghost,'s') != 2 && rand()%20 < *randchance){
        dirn = 's';
        *randchance = 10;
    }
    else if ((dirn != 'a') && (dirn != 'd') && check_direction(ghost,'d') != 2 && rand()%20 < *randchance){
        dirn = 'd';
        *randchance = 10;
    }
    return dirn;
}

void clear_screen() {
    printf("\033[2J\033[H");
}

void game_delay() {
    usleep(250000); 
}

void initialize_positions(int **pacman, char *pacmandirn, int **ghost1, char *ghost1dirn, int **ghost2, char *ghost2dirn, int **ghost3, char *ghost3dirn, int grid[17][13]){
    *pacman = &grid[12][6];   //char initialization
    *ghost1 = &grid[6][5];
    *ghost1dirn = 'a';
    *ghost2 = &grid[6][6];
    *ghost2dirn = 'd';
    *ghost3 = &grid[6][7];
    *ghost3dirn = 'd';
}
    

char pacman_dir = 'w';  // default direction

//Terminal settings reconfiguration

struct termios og_settings;

void disable_raw_mode(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_settings);
}

void enable_raw_mode(){
    tcgetattr(STDIN_FILENO,&og_settings);
    atexit(disable_raw_mode);
    struct termios raw;
    raw.c_lflag &= ~(ICANON|ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int* movement2(int *pacman, int grid2[11][11], char direction)
{
    int *move = pacman;

    // movement
    if(direction == 'a')        
    move = pacman - 1;   // left
    else if(direction == 'd')   
    move = pacman + 1;   // right
    else if(direction == 'w')  
    move = pacman - 11;  // up
    else if(direction == 's')   
    move = pacman + 11;  // down;

    // wall check
    if (*move == 2)
        return pacman;  // hit wall, stay

    //teleportation
    if (move == &grid2[5][10])
       move = &grid2[5][0];
    else if(move == &grid2[5][0])
       move = &grid2[5][10];
       
    return move;
}


void initialize_positions2(int **pacman, char *pacmandirn,
                           int **ghost1, char *ghost1dirn,
                           int **ghost2, char *ghost2dirn,
                           int **ghost3, char *ghost3dirn,
                           int grid2[11][11])
{
    *pacman = &grid2[8][5];
    *ghost1 = &grid2[5][4];
    *ghost1dirn = 'a';
    *ghost2 = &grid2[5][6];
    *ghost2dirn = 'd';
    *ghost3 = &grid2[2][5];
    *ghost3dirn = 'd';
}


void easyrunGame() //easymode
{    
     startTime = time(NULL);
     int score =0;   //variable declaration
     int lives =3;
     int power_up = 0;
     int power_timer = 0;
     int ghost_timer = 0;
     char move;

    int grid2[11][11] =
    {
      {2,2,2,2,2,2,2,2,2,2,2},
      {2,1,1,1,1,2,1,1,1,3,2},
      {2,1,2,2,1,0,1,2,2,1,2},
      {2,1,2,1,1,2,1,1,2,1,2},
      {2,1,1,1,2,2,2,1,1,1,2},
      {0,1,2,1,0,1,0,1,2,1,0},
      {2,1,1,1,2,2,2,1,1,1,2},
      {2,1,2,1,1,2,1,1,2,1,2},
      {2,1,2,2,1,0,1,2,2,1,2},
      {2,3,1,1,1,2,1,1,1,1,2},
      {2,2,2,2,2,2,2,2,2,2,2}
    };   
    
        
    int *pacman;        //char initialization
    int *ghost1, *ghost2, *ghost3;
    int randomchance1 = 10, randomchance2 = 10, randomchance3 = 10;;
    char ghost1dirn, ghost2dirn, ghost3dirn;
    int colors[5] = {91,92,93,94,95};
    int counter = 0;

    initialize_positions2(&pacman, &move,&ghost1, &ghost1dirn,&ghost2, &ghost2dirn,&ghost3, &ghost3dirn,grid2);

    while (1)
    {
        clear_screen();
        printf("Score = %d\tLives = %d\tPower=%s\n\n",score, lives, power_up ? "ON" : "OFF");
        live_check(lives);
        
#define BLUE    "\033[34m"
#define BRIGHT_BLUE    "\033[94m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define ORANGE  "\033[93m"
#define BRIGHT_PINK    "\033[95m"
#define RESET   "\033[0m"

counter++;
int color = colors[counter%5];

        cabinet_top(score, lives, power_up, 11);

        /* ---- PRINT GRID (NEW FRAME) ---- */
        for (int i = 0; i < 11; i++)
        {
            cabinet_row_open();
            for (int j = 0; j < 11; j++)
            {
                if (&grid2[i][j] == pacman)
                    printf((power_up)? "\033[%dm" "☺ " RESET:ORANGE"☺ " RESET,color);
                else if (&grid2[i][j] == ghost1)
                    printf((!power_up)?RED"ᗣ "RESET :BRIGHT_BLUE "ᗣ " RESET);
                else if (&grid2[i][j] == ghost2)
                    printf((!power_up)?YELLOW"ᗣ " RESET:BRIGHT_BLUE "ᗣ " RESET);
                else if (&grid2[i][j] == ghost3)
                    printf((!power_up)?BRIGHT_PINK"ᗣ " RESET:BRIGHT_BLUE "ᗣ " RESET);
                else if (grid2[i][j] == 3)
                    printf("\033[%dm" "🍒" RESET,color);
                else if (grid2[i][j] == 2)
                    printf(BRIGHT_BLUE "[]" RESET);
                else if (grid2[i][j] == 1)
                    printf("· ");
                else
                    printf("  ");

            }
            cabinet_row_close();
        }

        cabinet_bottom(11);
        game_delay();

       
       pacman_dir = getInput(pacman_dir);
       move = pacman_dir;   //get direction

        //movement function calling
        pacman = movement2(pacman,grid2 , move);
        score+=pac_update(pacman, &power_up, &power_timer);
        
        ghost_timer++;
        
        if (power_up==0 && death_check(pacman,ghost1,ghost2,ghost3)){
            lives--;
            initialize_positions2(&pacman, &move, &ghost1, &ghost1dirn, &ghost2, &ghost2dirn, &ghost3, &ghost3dirn, grid2);
        }


       if(power_up==1){
            if (ghost1 == pacman)
            {ghost1 = &grid2[5][4];
            }

            if (ghost2 == pacman)
            {ghost2 = &grid2[5][6];
            }
    
            if (ghost3 == pacman)
            {ghost3 = &grid2[2][5];
            }
        } 
        
        //ghost movement calling & death check conditions
        
        if (ghost_timer >= 3) 
        {

        ghost1dirn = ghost_decide_dirn(ghost1, ghost1dirn, &randomchance1);
        ghost2dirn = ghost_decide_dirn(ghost2, ghost2dirn, &randomchance2);
        ghost3dirn = ghost_decide_dirn(ghost3, ghost3dirn, &randomchance3);

        ghost1 = movement2(ghost1, grid2, ghost1dirn);
        ghost2 = movement2(ghost2, grid2, ghost2dirn);
        ghost3 = movement2(ghost3, grid2, ghost3dirn);


        ghost_timer = 0; //timer reset
        
        if (power_up==0 && death_check(pacman,ghost1,ghost2,ghost3)){
            lives--;
            initialize_positions2(&pacman, &move, &ghost1, &ghost1dirn, &ghost2, &ghost2dirn, &ghost3, &ghost3dirn, grid2);
        }
      }

       if(power_up==1){
            if (power_timer > 0)
            power_timer--;
            else
            power_up = 0;
                
            if (ghost1 == pacman)
            {ghost1 = &grid2[5][4];
            }

            if (ghost2 == pacman)
            {ghost2 = &grid2[5][6];
            }
    
            if (ghost3 == pacman)
            {ghost3 = &grid2[2][5];
            }
        } 
        //check win and lose
        
        if (score > 50){
            time_t endTime = time(NULL);
            int duration = (int)difftime(endTime, startTime);
            saveScore(playerName, duration, score);
            int choice = end_S(1, score, duration);
            if (choice == 0){
               exit(0);
            }
            else if (choice == 1){
               return;
            }
            else if (choice == 2){
               return;
            }
     }
        if (lives == 0){
           time_t endTime = time(NULL);
           int duration = (int)difftime(endTime, startTime);
           saveScore(playerName, duration, score);
   int choice = end_S(0, score, duration);
            if (choice == 0){
               exit(0);
            }
            else if (choice == 1){
               return;
            }
            else if (choice == 2){
               return;
            }
}
}
}

void runGame() //hard level
{
    startTime = time(NULL);
    int ghost_timer = 0;
    int *pacman;  
    int *ghost1,*ghost2,*ghost3;
    int randomchance1 = 10, randomchance2 = 10, randomchance3 = 10;;
    char move, ghost1dirn,ghost2dirn, ghost3dirn ;
    char pacman_dir = 'w'; 
    int score = 0;
    int lives =3;
    int power_up = 0;
    int power_timer = 0;

    int pacmanX, pacmanY = -1;
    int ghost1X, ghost1Y = -1;


    int grid[17][13] =
    {
      {2,2,2,2,2,2,0,2,2,2,2,2,2},
      {2,2,1,1,1,2,1,2,1,1,1,2,2},
      {2,1,3,2,1,1,1,1,1,2,1,1,2},
      {2,1,2,2,1,2,2,2,1,2,2,1,2},
      {2,1,1,1,1,1,1,1,1,1,1,1,2},
      {2,1,2,1,2,2,0,2,2,1,2,1,2},
      {2,1,1,1,2,0,0,0,2,1,1,1,2},
      {2,1,2,1,2,2,2,2,2,1,2,1,2},
      {0,1,2,1,1,1,1,1,1,1,2,1,0},
      {2,1,1,1,2,1,2,1,2,1,1,1,2},
      {2,1,2,1,1,1,2,1,1,1,2,1,2},
      {2,1,2,2,2,1,2,1,2,2,2,1,2},
      {2,1,1,1,1,1,0,1,1,1,1,1,2},
      {2,1,2,2,1,2,2,2,1,2,2,1,2},
      {2,1,1,2,1,1,1,1,1,2,3,1,2},
      {2,2,1,1,1,2,1,2,1,1,1,2,2},
      {2,2,2,2,2,2,0,2,2,2,2,2,2}
    };
    
    
    initialize_positions(&pacman, &move, &ghost1, &ghost1dirn, &ghost2, &ghost2dirn, &ghost3, &ghost3dirn, grid);
    int colors[5] = {91,92,93,94,95};
    int counter = 0;

    while (1)
    {
        clear_screen();
        printf("Score = %d\tLives = %d\tPower=%s\n\n\r",score, lives, power_up ? "ON" : "OFF");
        live_check(lives);

#define BLUE    "\033[34m"
#define BRIGHT_BLUE    "\033[94m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define ORANGE  "\033[93m"
#define BRIGHT_PINK    "\033[95m"
#define RESET   "\033[0m"

counter++;
int color = colors[counter%5];

        cabinet_top(score, lives, power_up, 13);

        for (int i = 0; i < 17; i++)
        {
            cabinet_row_open();
            for (int j = 0; j < 13; j++)
            {
               if (&grid[i][j] == pacman){
		    pacmanX = i;
                    pacmanY = j;
                    printf((power_up)? "\033[%dm" "☺ " RESET:ORANGE"☺ " RESET,color);
		}
                else if (&grid[i][j] == ghost1){
			ghost1X = i;
			ghost1Y = j;
                    printf((!power_up)?RED"ᗣ "RESET :BRIGHT_BLUE "ᗣ " RESET);
		}
                else if (&grid[i][j] == ghost2)
                    printf((!power_up)?YELLOW"ᗣ " RESET:BRIGHT_BLUE "ᗣ " RESET);
                else if (&grid[i][j] == ghost3)
                    printf((!power_up)?BRIGHT_PINK"ᗣ " RESET:BRIGHT_BLUE "ᗣ " RESET);
                else if (grid[i][j] == 3)
                    printf("\033[%dm" "🍒" RESET,color);
                else if (grid[i][j] == 2)
                    printf(BRIGHT_BLUE "[]" RESET);
                else if (grid[i][j] == 1)
                    printf("· ");
                else
                    printf("  ");

            }
            cabinet_row_close();
        }

        cabinet_bottom(13);
        game_delay();

       
       pacman_dir = getInput(pacman_dir);
       move = pacman_dir;

        //movement function calling
        pacman = movement(pacman, grid, move);
        score+=pac_update(pacman, &power_up, &power_timer);
        
        ghost_timer++;
        
        if (power_up==0 && death_check(pacman,ghost1,ghost2,ghost3))
        {
            lives--;
            initialize_positions(&pacman, &move, &ghost1, &ghost1dirn, &ghost2, &ghost2dirn, &ghost3, &ghost3dirn, grid);
        }


       if(power_up==1){
            if (ghost1 == pacman)
            {ghost1 = &grid[6][5];
	     ghost1X = 6;
	     ghost1Y = 5;
            }

            if (ghost2 == pacman)
            {ghost2 = &grid[6][6];
	    }
    
            if (ghost3 == pacman)
            {ghost3 = &grid[6][7];
            }
	}
        
        //ghost movement calling
        if (ghost_timer >= 3) {

        //ghost1dirn = ghost_decide_dirn(ghost1, ghost1dirn, &randomchance1);
        ghost2dirn = ghost_decide_dirn(ghost2, ghost2dirn, &randomchance2);
        ghost3dirn = ghost_decide_dirn(ghost3, ghost3dirn, &randomchance3);

        //ghost1 = movement(ghost1, grid, ghost1dirn);
        ghost2 = movement(ghost2, grid, ghost2dirn);
        ghost3 = movement(ghost3, grid, ghost3dirn);

        aStar(pacmanX, pacmanY, ghost1X, ghost1Y, grid);
        if (pathLength > 1) {
        ghost1X = path[1].x;
        ghost1Y = path[1].y;
        ghost1 = &grid[ghost1X][ghost1Y];
        }



        ghost_timer = 0;
        
        if (power_up==0 && death_check(pacman,ghost1,ghost2,ghost3)){
            lives--;
            initialize_positions(&pacman, &move, &ghost1, &ghost1dirn, &ghost2, &ghost2dirn, &ghost3, &ghost3dirn, grid);
        }
      }

       if(power_up==1){
            if (power_timer > 0)
            power_timer--;
            else
            power_up = 0;
                
            if (ghost1 == pacman)
            {ghost1 = &grid[6][5];
            }

            if (ghost2 == pacman)
            {ghost2 = &grid[6][6];
	    }
    
            if (ghost3 == pacman)
            {ghost3 = &grid[6][7];
            }
            
        } 
        //check win and lose
        
        if (score > 101){
            time_t endTime = time(NULL);
            int duration = (int)difftime(endTime, startTime);
            saveScore(playerName, duration, score);
            int choice = end_S(1, score, duration);
            if (choice == 0){
               exit(0);
            }
            else if (choice == 1){
               return;
            }
            else if (choice == 2){
               return;
            }
     }
        if (lives == 0){
           time_t endTime = time(NULL);
           int duration = (int)difftime(endTime, startTime);
           saveScore(playerName, duration, score);
   int choice = end_S(0, score, duration);
            if (choice == 0){
               exit(0);
            }
            else if (choice == 1){
               return;
            }
            else if (choice == 2){
               return;
            }
     }
     //frame++;
   ghost_timer++;
   
   }
}

int start_S(){
	
	printf(
"\033[96m████████████████████████████████████████████████████████████████\n\r"
"███\033[96m                                                          \033[96m███\n\r"

"███  \033[93m ██████╗  █████╗  ██████╗███╗   ███╗ █████╗ ███╗   ██╗ \033[96m ███\n\r"
"███  \033[93m ██╔══██╗██╔══██╗██╔════╝████╗ ████║██╔══██╗████╗  ██║ \033[96m ███\n\r"
"███  \033[93m ██████╔╝███████║██║     ██╔████╔██║███████║██╔██╗ ██║ \033[96m ███\n\r"
"███  \033[93m ██╔═══╝ ██╔══██║██║     ██║╚██╔╝██║██╔══██║██║╚██╗██║ \033[96m ███\n\r"
"███  \033[93m ██║     ██║  ██║╚██████╗██║ ╚═╝ ██║██║  ██║██║ ╚████║ \033[96m ███\n\r"
"███  \033[93m ╚═╝     ╚═╝  ╚═╝ ╚═════╝╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝ \033[96m ███\n\r"

"███\033[96m                                                          \033[96m███\n\r"
"███\033[96m                   ___________________                  \033[96m  ███\n\r"
"███\033[96m                   ||    Controls   ||                  \033[96m  ███\n\r"
"███\033[96m                   ||   W = up  ↑   ||                  \033[96m  ███\n\r"
"███\033[96m                   ||   A = left ←  ||                  \033[96m  ███\n\r"
"███\033[96m                   ||   S = down →  ||                  \033[96m  ███\n\r"
"███\033[96m                   ||   D = right ↓ ||                   \033[96m ███\n\r"
"███\033[96m                   ||_______________||                  \033[96m  ███\n\r"
"███\033[96m                                                          \033[96m███\n\r"
"███\033[96m       Press: W = Easy Mode   S = Hard Mode               \033[96m███\n\r"
"███\033[96m                L = Leaderboard  Q = Quit                 \033[96m███\n\r"
"███\033[96m                                                          \033[96m███\n\r"
"███\033[96m           Eat pellets · avoid ghosts · collect 🍒        \033[96m███\n\r"
"███\033[96m         Power-up: eat ghosts before timer runs out       \033[96m███\n\r"
"███\033[96m                                                          \033[96m███\n\r"

"\033[96m████████████████████████████████████████████████████████████████\033[0m\n\r"
);
     char input;
   int val = 0; 
   while (val == 0){
       scanf(" %c",&input);
       input = tolower(input);
       if (input == 's'){
           val = 1;
printf("\n\n\rEnter Player Name: ");

   	    scanf("%49s", playerName);
           return 1;  
       }
       if (input == 'w'){
           val = 1;
printf("\n\n\rEnter Player Name: ");

   	    scanf("%49s", playerName);
           return 2;  
       }
       else if (input == 'q'){
           val = 1;
           return 0;
       }
else if (input == 'l'){ showLeaderboard(); }

           else{
           printf("Wrong input Try again\n\r");
       }
   }
   
}

int end_S(int game_sit,int score,int duration){

           disable_raw_mode();
           clear_screen();
int minutes = duration/60; int seconds = duration%60;

           if (game_sit == 1){
        	printf(
    "\033[96m" // Cyan Walls%02d:%02d 
    "█████████████████████████████████████████████████████████████████████████████████████████\n\r"
    "███                                                                                   ███\n\r"
    "███              \033[32m██╗   ██╗ ██████╗ ██╗   ██╗    ██╗    ██╗██╗███╗   ██╗\033[96m               ███\n\r"
    "███              \033[93m╚██╗ ██╔╝██╔═══██╗██║   ██║    ██║    ██║██║████╗  ██║\033[96m               ███\n\r"
    "███               \033[32m╚████╔╝ ██║   ██║██║   ██║    ██║ █╗ ██║██║██╔██╗ ██║\033[96m               ███\n\r"
    "███                \033[93m╚██╔╝  ██║   ██║██║   ██║    ██║███╗██║██║██║╚██╗██║\033[96m               ███\n\r"
    "███                 \033[32m██║   ╚██████╔╝╚██████╔╝    ╚███╔███╔╝██║██║ ╚████║\033[96m               ███\n\r"
    "███                                                                                   ███\n\r"
    
     "███                                       \033[93m██████████\033[96m                                  ███\n\r"
     "███                                     \033[93m██████████████\033[96m                                ███\n\r"
     "███                                   \033[93m████████████\033[96m                                    ███\n\r"
     "███                                   \033[93m██████████     ● ● ●\033[96m                            ███\n\r"
     "███                                   \033[93m████████████\033[96m                                    ███\n\r"
     "███                                     \033[93m██████████████\033[96m                                ███\n\r"
     "███                                        \033[93m██████████\033[96m                                 ███\n\r"
     // BIG SCORE SECTION
    "███                       \033[33mFINAL SCORE\033[96m                          \033[33mTIME\033[96m                   ███\n\r"
        "███                 \033[1;33m┏━━━━━━━━━━━━━━━━━━┓              ┏━━━━━━━━━━━━━━━━━━┓\033[0;96m            ███\n\r"
        "███                 \033[1;33m┃\033[1;97m      %06d      \033[1;33m┃              ┃\033[1;97m      %02d:%02d      \033[1;33m ┃\033[0;96m            ███\n\r"
        "███                 \033[1;33m┗━━━━━━━━━━━━━━━━━━┛              ┗━━━━━━━━━━━━━━━━━━┛\033[0;96m            ███\n\r"
        "███                                                                                   ███\n\r"
        "███                                                                                   ███\n\r"
    // GHOST SCENE
    "███                                                                                   ███\n\r"
    "███                     \033[93m[S]\033[96m PLAY AGAIN                \033[91m[Q]\033[96m QUIT                        ███\n\r"
    "███                                                                                   ███\n\r"
    "█████████████████████████████████████████████████████████████████████████████████████████\n\r"
    "\033[0m", score, minutes, seconds);       
	}
           else{
           	printf(
    "\033[96m"  // Cyan walls start
    "████████████████████████████████████████████████████████████████████████████████████████\n\r"
    "███                                                                                  ███\n\r"

    // GAME OVER in RED
    "███      \033[35m██████╗  █████╗ ███╗   ███╗███████╗    ██████╗ ██╗   ██╗███████╗██████╗\033[96m     ███\n\r"
    "███     \033[31m██╔════╝ ██╔══██╗████╗ ████║██╔════╝   ██╔═══██╗██║   ██║██╔════╝██╔══██╗\033[96m    ███\n\r"
    "███     \033[35m██║  ███╗███████║██╔████╔██║█████╗     ██║   ██║██║   ██║█████╗  ██████╔╝\033[96m    ███\n\r"
    "███     \033[31m██║   ██║██╔══██║██║╚██╔╝██║██╔══╝     ██║   ██║╚██╗ ██╔╝██╔══╝  ██╔══██╗\033[96m    ███\n\r"
    "███     \033[35m╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗   ╚██████╔╝ ╚████╔╝ ███████╗██║  ██║\033[96m    ███\n\r"
    "███      \033[31m╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝    ╚═════╝   ╚═══╝  ╚══════╝╚═╝  ╚═╝\033[96m    ███\n\r"

    "███                                                                                  ███\n\r"

    // RED GHOST (Centered)
    "███                                    \033[31m██████████\033[96m                                    ███\n\r"
    "███                                  \033[31m██████████████\033[96m                                  ███\n\r"
    "███                                 \033[31m███  ██████  ███\033[96m                                 ███\n\r"
    "███                                 \033[31m███  ██████  ███\033[96m                                 ███\n\r"
    "███                                 \033[31m████████████████\033[96m                                 ███\n\r"
    "███                                 \033[31m████████████████\033[96m                                 ███\n\r"
    "███                                  \033[31m██  ██  ██  ██\033[96m                                  ███\n\r"
    "███                                                                                  ███\n\r"
      "███                     \033[33mFINAL SCORE\033[96m                          \033[33mTIME\033[96m                    ███\n\r"
        "███                 \033[1;33m┏━━━━━━━━━━━━━━━━━━┓              ┏━━━━━━━━━━━━━━━━━━┓\033[0;96m           ███\n\r"
        "███                 \033[1;33m┃\033[1;97m      %06d      \033[1;33m┃              ┃\033[1;97m      %02d:%02d      \033[1;33m ┃\033[0;96m           ███\n\r"
        "███                 \033[1;33m┗━━━━━━━━━━━━━━━━━━┛              ┗━━━━━━━━━━━━━━━━━━┛\033[0;96m           ███\n\r"
        "███                                                                                  ███\n\r"
        "███                                                                                  ███\n\r"
    "███                                                                                  ███\n\r"
    "███                     \033[93m[S]\033[96m PLAY AGAIN                \033[91m[Q]\033[96m QUIT                       ███\n\r"
    "███                                                                                  ███\n\r"

    "████████████████████████████████████████████████████████████████████████████████████████\n\r"

    "\033[0m",   // Reset everything
    score, minutes, seconds);    
	   }
           char input;
           int val = 0; 
           while(val == 0){
               scanf(" %c",&input);
               input = tolower(input);
               if (input == 's'){
                   val = 1;
                   return 1;
               }
               if (input == 'w'){
                   val = 1;
                   return 2;
               }
               else if (input == 'q'){
                   val = 1;
                   return 0; 
               }
               else {
                   printf("Wrong input Try again\n\r");
               }
           }
       }
       
int main() {

   while(1){
       int choice = start_S();
       if (choice == 1){
           enable_raw_mode();
           runGame();
       }
       else if (choice == 2){
           enable_raw_mode();
           easyrunGame();
       }
       else{
           break;
       }
   }
}