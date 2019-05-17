#define REVERS  SEND_CHAR(0x00)       /**< \brief Send first custom created character - card revers*/
#define CUR     SEND_CHAR(0x01)       /**< \brief Send second custom created character - cursor*/
#define AV1     SEND_CHAR(0x02)       /**< \brief Send 3rd custom created character - card icon*/
#define AV2     SEND_CHAR(0x03)       /**< \brief Send 4th custom created character - card icon*/
#define AV3     SEND_CHAR(0x04)       /**< \brief Send 5th custom created character - card icon*/
#define AV4     SEND_CHAR(0x05)       /**< \brief Send 6th custom created character - card icon*/
#define AV5     SEND_CHAR(0x06)       /**< \brief Send 7th custom created character - card icon*/
#define AV6     SEND_CHAR(0x07)       /**< \brief Send 8th custom created character - card icon*/

#define LEFT    (P4IN&BIT4)==0        /**< \brief Declares boolean instruction which checks if the first button was pressed */
#define B1      (P4IN&BIT4)==0        /**< \brief Declares boolean instruction which checks if the first button was pressed */
#define RIGHT   (P4IN&BIT5)==0        /**< \brief Declares boolean instruction which checks if the second button was pressed */
#define B2      (P4IN&BIT5)==0        /**< \brief Declares boolean instruction which checks if the second button was pressed */
#define UP      (P4IN&BIT6)==0        /**< \brief Declares boolean instruction which checks if the third button was pressed */
#define B3      (P4IN&BIT6)==0        /**< \brief Declares boolean instruction which checks if the third button was pressed */
#define ENTER   (P4IN&BIT7)==0        /**< \brief Declares boolean instruction which checks if the fourth button was pressed */
#define B4      (P4IN&BIT7)==0        /**< \brief Declares boolean instruction which checks if the fourth button was pressed */

#define LIMIT   10                    /**< \brief Constant number defining the size of the score array. */

void print(char * str);
void initChars(void);
void initLogic(int sCLS);
void sort(void);
unsigned int random(int range);
void delayS(unsigned int duration);
void delay100ms(unsigned int duration);
void tableShuffle(void);
void printMenu(void);
void printDifficult(void);
void printTable(void);
void printWin(void);
void printRules(void);
void printScores(void);
int check(void);
void runGame(void);
void startGame(int mode);
void menu(void);
