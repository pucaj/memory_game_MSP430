#include "lib.h"
#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include <time.h>

unsigned char logicTable[32];         /**< \brief Logic table containing all the cards at their positions*/
unsigned char revTable[32];           /**< \brief Reveal table that indicates which cards are chosen to be revealed and which have already been guessed correctly*/
unsigned char revealed      = 0;      /**< \brief Counter of revealed cards */
unsigned char count         = 0;      /**< \brief Timer A interrupt counter to measure time independly*/
unsigned int i              = 0;      /**< \brief Global iteration variable */
unsigned int score          = 0;      /**< \brief Variable that contains current score */
unsigned int scores[LIMIT];           /**< \brief Array of the highest scores */
unsigned char cursor        = 0;      /**< \brief Current cursor position*/
unsigned char size          = 0;      /**< \brief Board width */
unsigned int gameTime       = 0;      /**< \brief Game time in seconds */
char win                    = 0;      /**< \brief Number of currectly guessed cards */
signed char first           = -1;     /**< \brief Position of the first revealed card */

#pragma vector=TIMERA0_VECTOR
/** \brief Function handling timer A interruptions
 *
 * \param void
 * \return void
 *
 */
__interrupt void Timer_A (void){
    ++count;
    _BIC_SR_IRQ(LPM3_bits);
}

/** \brief Function that prints out given string onto the LCD display
 *
 * \param str Pointer to char
 * \return void
 *
 */
void print(char * str){
  int i = 0;
  while(*(str+i) != '\0'){
    SEND_CHAR(*(str+i++));
  }
}

/** \brief Function that defines custom created characters and loads them to the CG_RAM memory
 * \return void
 *
 */
void initChars(){
  char k;
  SEND_CMD(CG_RAM_ADDR);

  char a[8] = {0x1F,0x15,0x11,0x15,0x15,0x11,0x15,0x1F};
  for(k = 0; k < 8; k++)
    SEND_CHAR(a[k]);

  char b[8] = {0x00,0x00,0x00,0x0E,0x0E,0x00,0x00,0x00};
  for(k = 0; k < 8; k++)
    SEND_CHAR(b[k]);

  char c[8] = {0x00,0x0A,0x15,0x11,0x0A,0x04,0x00,0x00};
  for(k = 0; k < 8; k++)
    SEND_CHAR(c[k]);

  char d[8] = {0x00,0x1B,0x1F,0x15,0x0E,0x04,0x00,0x00};
  for(k = 0; k < 8; k++)
    SEND_CHAR(d[k]);

  char e[8] = {0x04,0x14,0x15,0x15,0x0D,0x06,0x04,0x04};
  for(k = 0; k < 8; k++)
    SEND_CHAR(e[k]);

  char f[8] = {0x00,0x11,0x0A,0x1F,0x11,0x11,0x1F,0x00};
  for(k = 0; k < 8; k++)
    SEND_CHAR(f[k]);

  char g[8] = {0x0E,0x0E,0x05,0x1F,0x14,0x04,0x17,0x1D};
  for(k = 0; k < 8; k++)
    SEND_CHAR(g[k]);

  char h[8] = {0x00,0x04,0x0E,0x1F,0x0E,0x0A,0x0E,0x00};
  for(k = 0; k < 8; k++)
    SEND_CHAR(h[k]);

  SEND_CMD(DD_RAM_ADDR);
}

/** \brief Function that initializes game logic
 *
 * \param sCLS Clear scores. If positive clear the score table.
 * \return void
 * Resets variables to their default values
 */
void initLogic(int sCLS){
  for(i = 0; i < size*2; i++) {
    logicTable[i] = 0; revTable[i] = 0;
    if(i < LIMIT && sCLS > 0) {scores[i] = 0;}
  }
  win = 0; revealed = 0; first = -1; cursor = 0; count = 0;
}

/** \brief Function that sorts the score array in ascending order - insertion sort.
 * \return void
 *
 *
 */
void sort(){
  int temp, j, k;

  for (k = 1; k < LIMIT; k++){
    temp = scores[k];
    for (j = k; (j > 0) && (scores[j-1] > temp); j--)
      scores[j] = scores[j-1];
    scores[j] = temp;
  }
}

/** \brief Function generating pseudorandom value from the specified range.
 *
 * \param range Specifies max value.
 * \return Returns random number from the range: <0, range).
 *
 */
unsigned int random(int range){
  return clock()%range;
}

/** \brief Generates delay.
 *
 * \param duration Delay duration in seconds.
 * \return void
 *
 */
void delayS(unsigned int duration){
  unsigned int j = 0;
  duration *= 4;
  while(duration--){
    for(j = 0;j < 0xFFFF; j++);
    for(j = 0;j < 0xFFFF; j++);
  }
}

/** \brief Generates delay.
 *
 * \param duration Delay duration in about 100ms multiples
 * \return void
 *
 */
void delay100ms(unsigned int duration){
  unsigned int j = 0;
  while(duration--){
    for(j = 0;j < 0xFFFF; j++);
  }
}

/** \brief Function that shuffles the board.
 * \return void
 *
 * Assigning card numbers to random places on the board.
 */
void tableShuffle(){
  int rng; int j;
  for(i = 1; i < size + 1; i++)
    for(j = 2; j > 0; j--){
      rng = random(2 * size);
      while(logicTable[rng] > 0)
        rng = random(2 * size);
      if(logicTable[rng] == 0)
        logicTable[rng] = i%6 + 1;
    }
}

/** \brief Prints out the game menu onto the LCD display
 * \return void
 *
 */
void printMenu(){
  SEND_CMD(DD_RAM_ADDR);
  print("1.Start  2.Rules");
  SEND_CMD(DD_RAM_ADDR2);
  print("3.Scores 4.About");
}

/** \brief Prints out available levels of the game difficulty.
 * \return void
 *
 */
void printDifficult(){
  SEND_CMD(DD_RAM_ADDR);
  print("1.Easy 2.Medium");
  SEND_CMD(DD_RAM_ADDR2);
  print("     3.Hard");
}

/** \brief Prints out the entire game board with revealed cards(if any) and current cursor position
 * \return void
 *
 */
void printTable(){
  unsigned int j;
  SEND_CMD(DD_RAM_ADDR);
  for(j = 0; j < 2; j++){
    if(j == 1) SEND_CMD(DD_RAM_ADDR2);
    for(i = 0; i < size; i++){
      if(i + j*size == cursor && revealed != 2) {CUR; continue;}

      if(revTable[i + j*size] == 0){
        REVERS; continue;
      } else
        if(revTable[i + j*size] == 1){
          switch(logicTable[i + j*size]){
            case 1: AV1; break;
            case 2: AV2; break;
            case 3: AV3; break;
            case 4: AV4; break;
            case 5: AV5; break;
            case 6: AV6; break;
          }
          continue;
      }
      else if(revTable[i + j*size] == 2) SEND_CHAR(' ');
    }
  }
}

/** \brief Prints out the message about the victory and total score.
 * \return void
 *
 */
void printWin(){
  clearDisplay();
  SEND_CMD(DD_RAM_ADDR);
  print("    You win!    ");
  SEND_CMD(DD_RAM_ADDR2);
  print("Your score: ");
  printDecimal(score);

  if(scores[0] < score){
    scores[0] = score;
    sort();
  }
  score = 0; win = 0;

  while(!B1 && !B2 && !B3 && !B4);
  delayS(4);
}

/** \brief Prints out the game instructions
 * \return void
 *
 */
void printRules(){
  clearDisplay();
  SEND_CMD(DD_RAM_ADDR);
  print("Controls:");
  SEND_CMD(DD_RAM_ADDR2);
  print("1. Left 2. Right");
  delayS(2);
  for(;;) if(B1 || B2 || B3 || B4) break;

  clearDisplay();
  SEND_CMD(DD_RAM_ADDR);
  print("3. Up / Down");
  SEND_CMD(DD_RAM_ADDR2);
  print("4. Confirm   ...");
  delayS(2);
  for(;;) if(B1 || B2 || B3 || B4) break;
}

/** \brief Displays the highest scores
 * \return void
 *
 */
void printScores(){
  int p = 0;

  for(p = LIMIT - 1; p >= 0; p--){
    if(scores[LIMIT - 1] == 0) {
      print("Scores are empty"); delay100ms(10);
      while(!B1 && !B2 && !B3 && !B4);
      break;
    }
    if(scores[p] == 0) {
      break;
    }

    clearDisplay();
    SEND_CMD(DD_RAM_ADDR);
    print("Scores:");
    SEND_CMD(DD_RAM_ADDR2);
    SEND_CHAR('#');
    printDecimal(LIMIT - p);
    print(": ");
    printDecimal(scores[p]);

    for(;;){
      if(B1 || B2 || B3 || B4) { delay100ms(10); break; }
    }
  }


  delayS(1);
  clearDisplay();
  printMenu();
}

/** \brief Checks if the two chosen elements are equal
 * \return 0 if elements aren't equal
 * \return 1 if it's a pair but the elements were not the last two on the board
 * \return 2 if it's a pair and the elements were the last two on the board
 */
int check(){
  if(logicTable[first] == logicTable[cursor]) {
    revTable[first]  = 2;
    revTable[cursor] = 2;
    score += 10;
    win += 2;
    if(win == size*2) return 2;
    else              return 1;
  } else {
    revTable[first]  = 0;
    revTable[cursor] = 0;
    first = -1;
    return 0;
  }
}

/** \brief Function that contains the main game loop
 * \return void
 *
 */

void runGame(){
  for(;;){
    _BIS_SR(LPM3_bits);
    if(count % 100 == 0){ count = 0; gameTime -= 1; }
    if(gameTime == 0){
      clearDisplay();
      SEND_CMD(DD_RAM_ADDR);
      print("  Time's over!");
      SEND_CMD(DD_RAM_ADDR2);
      print("Your score: ");
      printDecimal(score);

      if(scores[0] < score){
       scores[0] = score;
       sort();
      }
      score = 0; win = 0;

      while(!B1 && !B2 && !B3 && !B4);

      delayS(4);
      break;
    }

    if(LEFT){
      delay100ms(5);
      if(cursor == size)    cursor = size * 2 - 1;
      else if(cursor == 0)  cursor = size - 1;
      else                  cursor -= 1;
      printTable();
    }

    if(RIGHT){
      delay100ms(5);
      if(cursor == size - 1)            cursor = 0;
      else if(cursor == size * 2 - 1)   cursor = size;
      else                              cursor += 1;
      printTable();
    }

    if(UP){
      delay100ms(5);
      if(cursor < size)     cursor += size;
      else                  cursor = cursor % size;
      printTable();
    }

    if(ENTER){
      delayS(1);
      if(revTable[cursor] < 1){
        revTable[cursor] = 1;
        if(revealed == 1){
          revealed += 1;
          printTable();
          revealed = 0;
          if(check() == 2) { score += gameTime; printWin(); break; }
          delay100ms(4);
        } else if(revealed == 0){ revealed += 1; first = cursor; }
      }
    }

    if(LEFT && RIGHT && UP && ENTER) {delayS(2); break;}

  }
}

/** \brief Function that starts the game.
 *
 * \param mode Integer that specifies game mode {easy, medium, hard}.
 * \return void
 *
 */
void startGame(int mode){
  clearDisplay();
  switch(mode){
    case 0: size =  6;  gameTime = 60;  break;
    case 1: size = 10;  gameTime = 100; break;
    case 2: size = 16;  gameTime = 360; break;
  }

  SEND_CMD(DD_RAM_ADDR);
  print("---*LOADING*---");
  SEND_CMD(DD_RAM_ADDR2);
  print("--***-----***--");
  tableShuffle();
  clearDisplay();

  SEND_CMD(DD_RAM_ADDR);
  print("You have ");
  printDecimal(gameTime);
  print(" secs");
  SEND_CMD(DD_RAM_ADDR2);
  print("Good luck!");
  delayS(3);

  clearDisplay();
  printTable();

  count = 0;
  runGame();
  initLogic(-1);
}

/** \brief Menu functionality.
 *
 *  Function with a build-in endless loop that listens to the incoming signals from the buttons to choose the correct element from a menu list.
 *
 *  \return void
 */
void menu(){
  if(B1){
  clearDisplay(); printDifficult(); delayS(1);

    for(;;){
      if(B1){
        startGame(0);
        clearDisplay();
        delay100ms(5);
        printScores();
        delay100ms(5);
        break;
      }

      if(B2){
        startGame(1);
        clearDisplay();
        delay100ms(5);
        printScores();
        delay100ms(5);
        break;
      }

      if(B3){
        startGame(2);
        clearDisplay();
        delay100ms(5);
        printScores();
        delay100ms(5);
        break;
      }

      if(B4) { delayS(2);  printMenu();  break; }
    }
  }

  if(B2){
    delayS(1);
    clearDisplay();
    printRules();
    printMenu();
    delayS(2);
  }

  if(B3){
    clearDisplay();
    delay100ms(5);
    printScores();
  }

  if(B4){
    clearDisplay();
    SEND_CMD(DD_RAM_ADDR);
    print("     Daniel");
    SEND_CMD(DD_RAM_ADDR2);
    print("   Skorczynski");

    delayS(6);
    clearDisplay();
    printMenu();
  }

}
