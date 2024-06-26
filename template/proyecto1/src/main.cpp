/*######################################################################
  //#  G0B1T: uC EXAMPLES. 2024.
  //######################################################################
  //# Copyright (C) 2024. F.E.Segura-Quijano (FES) fsegura@uniandes.edu.co
  //#
  //# This program is free software: you can redistribute it and/or modify
  //# it under the terms of the GNU General Public License as published by
  //# the Free Software Foundation, version 3 of the License.
  //#
  //# This program is distributed in the hope that it will be useful,
  //# but WITHOUT ANY WARRANTY; without even the implied warranty of
  //# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  //# GNU General Public License for more details.
  //#
  //# You should have received a copy of the GNU General Public License
  //# along with this program.  If not, see <http://www.gnu.org/licenses/>
  //####################################################################*/

//=======================================================
//  LIBRARY Definition
//=======================================================
#include <Arduino.h>

#define REALMATRIX // Variable to use real matrix. Comment to not use it.
#define START_PIN 1
// #define LEFT_PIN 19
// #define RIGHT_PIN 21
#define RIGHT_PIN 21
#define LEFT_PIN 19
#ifdef REALMATRIX
#include "LedControl.h"
/* Pin definition for MAX72XX.
 ARDUINO pin 12 is connected to the DataIn  - In ESP32 pin 23
 ARDUINO pin 11 is connected to the CLK     - In ESP32 pin 18
 ARDUINO pin 10 is connected to LOAD        - In ES32 pin 5
 We have only a single MAX72XX.
 */
//#define RESET_PIN 2


LedControl lc=LedControl(23,18,5,1);  
#endif

//=======================================================
//  IF PROBLEMS
//=======================================================
// In LedControl.h  change the following line:
// #include <avr/pgmspace.h>
// by 
/* #if (defined(__AVR__))
 #include <avr\pgmspace.h>
 #else
 #include <pgmspace.h>
 #endif
*/

//=======================================================
//  REGISTERS (VARIABLES) to control the game
//=======================================================
/* Registers to background cars.*/
byte RegBACKGTYPE_dataRANDOM;
byte RegBACKGTYPE_dataZEROS = B00000000;

/* Time delays. */
unsigned long delaytime = 2000;

/* Global Variables */
int i = 0;
/* Agrega una variable para contar las filas de carros pasadas */
uint8_t carsPassed = 0;
uint8_t currentLevel = 1;
const uint8_t rowsPerLevel[] = {10, 15, 20}; // Cantidad de filas por nivel
uint8_t playerMoveCounter = 0;
/* States ans signals to change state*/
enum State_enum {STATERESET, STATESTART, STATECLEAR, STATECHECK, STATELEFT, STATERIGTH, STATELOST, STATENUNO,STATEDOS, STATETRES, STATEWON};
uint8_t state = STATESTART;

enum Keys_enum {RESET_KEY, START_KEY, LEFT_KEY, RIGHT_KEY, NO_KEY};
uint8_t keys = RESET_KEY;

enum Status_enum {LOST, CONTINUE, NIVELDOS, NIVELTRES, WIN};
uint8_t Status = CONTINUE;




/* Key to control game by serial input. */
  int incomingByte;

/* Pointer and Matrix to Control Driver. */
  byte RegMatrix[8];
  byte *pointerRegMatrix;

/* Pointer and Register to control bottom car. */
  byte RegCar[1];
  byte *pointerRegCar;

/* Pointer and Register (Variable) to move bottom car */
  byte ShiftDir[1];
  byte *pointerShiftDir;

//=======================================================
//  SETUP Arduino function
//=======================================================
/* Setup function initialization */
void setup()
{

  //pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(START_PIN, INPUT_PULLUP);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
#ifdef REALMATRIX
  /* The MAX72XX is in power-saving mode on startup, we have to do a wakeup call. */
  lc.shutdown(0, false);
  /* Set the brightness to a medium values. */
  lc.setIntensity(0, 8);
  /* Clear the display. */
  lc.clearDisplay(0);
#endif
  /* Serial port initialization. */
  Serial.begin(9600);

  /* Pointer to use Matrix between functions. */
  pointerRegMatrix = &RegMatrix[0];

  /* Pointer to use VectorCar between functions. */
  pointerRegCar = &RegCar[0];

  /* Pointer to use shift dir between functions */
  pointerShiftDir = &ShiftDir[0];
}

//=======================================================
//  FUNCTION: writeResetMatrix
//=======================================================
/* Data matrix when reset*/
void writeResetMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */

  /* Here is the data to reset matrix */
  pointerRegMatrix[7] = B11111111;
  pointerRegMatrix[6] = B11111111;
  pointerRegMatrix[5] = B11111111;
  pointerRegMatrix[4] = B11111111;
  pointerRegMatrix[3] = B11111111;
  pointerRegMatrix[2] = B11111111;
  pointerRegMatrix[1] = B11111111;
  pointerRegMatrix[0] = B11111111;
  /* Here is the data to reset bottomCar */
  pointerRegCar[0] = B00000000;
}
//=======================================================
//  FUNCTION: writeStartMatrix
//=======================================================
void writeStartMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */

  /* Here is the data to start matrix */
  pointerRegMatrix[7] = B01111110;
  pointerRegMatrix[6] = B10000001;
  pointerRegMatrix[5] = B10011001;
  pointerRegMatrix[4] = B10000001;
  pointerRegMatrix[3] = B10100101;
  pointerRegMatrix[2] = B10011001;
  pointerRegMatrix[1] = B10000001;
  pointerRegMatrix[0] = B01111110;
/* Here is the data to start bottomCar */
  pointerRegCar[0] = B00000000;
}
//=======================================================
//  FUNCTION: writeClearMatrix
//=======================================================
void writeClearMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */

  /* Here is the data to clear matrix */
  pointerRegMatrix[7] = B00000000;
  pointerRegMatrix[6] = B00000000;
  pointerRegMatrix[5] = B00000000;
  pointerRegMatrix[4] = B00000000;
  pointerRegMatrix[3] = B00000000;
  pointerRegMatrix[2] = B00000000;
  pointerRegMatrix[1] = B00000000;
  pointerRegMatrix[0] = B00000000;
  /* Here is the data to clear bottomCar */
  pointerRegCar[0] = B00010000;
}
//=======================================================
//  FUNCTION: writeLostMatrix
//=======================================================
void writeLostMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */

  /* Here is the data to lost matrix */
  pointerRegMatrix[7] = B01111110;
  pointerRegMatrix[6] = B10000001;
  pointerRegMatrix[5] = B10011001;
  pointerRegMatrix[4] = B10000001;
  pointerRegMatrix[3] = B10011001;
  pointerRegMatrix[2] = B10100101;
  pointerRegMatrix[1] = B10000001;
  pointerRegMatrix[0] = B01111110;
  /* Here is the data to lost matrix */
  pointerRegCar[0] = B00000000;
}
//=======================================================
//  FUNCTION: writeGoCarsMatrix
//=======================================================
void writeGoCarsMatrix(byte *pointerRegMatrix)
{
  /* Global variables. */
  int m;

  i = i + 1;
  /* Here is the data to start matrix */
  RegBACKGTYPE_dataRANDOM = random(1, 255);

  for (m = 0; m < 7; m++)
  {
    pointerRegMatrix[m] = pointerRegMatrix[m + 1];
  }
  /* Verifica si la última fila ahora está vacía */
  if (pointerRegMatrix[7] == 0) {
    /* Incrementa el contador de filas pasadas */
    carsPassed++;
  }
  Serial.print("car pass: ");
  Serial.print(carsPassed);

  if (currentLevel==2)
    Status=NIVELDOS;
  else if (currentLevel==3)
    Status= NIVELTRES;
  

  if (i % 2 == 0)
    pointerRegMatrix[7] = RegBACKGTYPE_dataRANDOM;
  else
    pointerRegMatrix[7] = RegBACKGTYPE_dataZEROS;
}
//=======================================================
//  FUNCTION: writeCarBase
//=======================================================
void writeCarBase(byte *pointerRegCar, byte *pointerShiftDir)
{
  /* Global variables. */
  int m;
  
  /* Here is the data to start matrix */
  if (pointerShiftDir[0] == B00000001)
  {
    if (pointerRegCar[0] == B00000001)
      pointerRegCar[0] = pointerRegCar[0];
    else
      pointerRegCar[0] = pointerRegCar[0] >> 1;
  }
  else if (pointerShiftDir[0] == B00000010)
  {
    if (pointerRegCar[0] == B10000000)
      pointerRegCar[0] = pointerRegCar[0];
    else
      pointerRegCar[0] = pointerRegCar[0] << 1;
  }
  else
    pointerRegCar[0] = pointerRegCar[0];
}
//=======================================================
//  FUNCTION: checkLostMatrix (leds and console)
//=======================================================
void checkLostMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */
  byte check1, check2;

  check1 = pointerRegCar[0] ^ pointerRegMatrix[0];
  check2 = pointerRegCar[0] | pointerRegMatrix[0];

  if (pointerRegCar[0] == pointerRegMatrix[0])
    Status = LOST;
  else if (check1 != check2)
    Status = LOST;
  else
    Status = CONTINUE;

   /* Reinicia el contador de filas pasadas si el jugador pierde */
  if (Status == LOST) {
    carsPassed = 0;
  }
}
//=======================================================
//  FUNCTION: printBits (by console all bits)
//=======================================================
void printBits(byte myByte)
{
  for (byte mask = 0x80; mask; mask >>= 1) {
    if (mask  & myByte)
      Serial.print('1');
    else
      Serial.print('0');
  }
}
//=======================================================
//  FUNCTION: PrintMatrix (Console)
//=======================================================
void PrintMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */
  int m;

  for (m = 7; m >= 1; m--)
  {
    printBits(pointerRegMatrix[m]);
    Serial.println();
  }
  printBits(pointerRegMatrix[0] | pointerRegCar[0]);
  Serial.println();
}
//=======================================================
//  FUNCTION: PrintALLMatrix (leds and console)
//=======================================================
void PrintALLMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */
  int m;

#ifdef REALMATRIX
  /* Display data one by one in matrix. */
  for (m = 7; m >= 1; m--)
  {
    lc.setRow(0, m, pointerRegMatrix[m]);
  }
  lc.setRow(0, m, (pointerRegMatrix[0] | pointerRegCar[0]));
#endif
  /* Display data one by one in console. */
  Serial.println("########");
  Serial.println("########");
  Serial.println("########");
  Serial.println("########");
  PrintMatrix(pointerRegMatrix, pointerRegCar);
  Serial.println("########");
}


//=======================================================
//  FUNCTION: read_KEY
//=======================================================
byte read_KEY(void)
{
  int buttonStateStart = digitalRead(START_PIN);
  //int buttonStateReset = digitalRead(RESET_PIN);
  int buttonStateLeft = digitalRead(LEFT_PIN);
  int buttonStateRight = digitalRead(RIGHT_PIN);
  if (Serial.available() > 0)
  {
    incomingByte = Serial.read();
    delay(10);
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  }


  if (buttonStateStart != HIGH)
  {
    /* code */
    keys = START_KEY;
    incomingByte= 'S';
    //return keys;
  }
  // // if (buttonStateReset != HIGH)
  // // {
  // //   /* code */
  // //   keys = RESET_KEY;
  // //   incomingByte= 'R';
  // //   return keys;
  // // }
  if (buttonStateLeft != HIGH)
  {
    /* code */
    keys = LEFT_KEY;
    incomingByte= 'A';
    //return keys;
  }
  if (buttonStateRight != HIGH)
  {
    /* code */
    keys = RIGHT_KEY;
    incomingByte= 'D';
    //return keys;
  }
  else 
  {
    /* code */
    keys = NO_KEY;
    //return keys;
  }
  switch (incomingByte)
  {
    case 'R':
      keys = RESET_KEY;
      break;
    case 'S':
      keys = START_KEY;
      break;
    case 'A':
      keys = LEFT_KEY;
      break;
    case 'D':
      keys = RIGHT_KEY;
      break;

    default:
      keys = NO_KEY;
      break;
  }
return keys;
}
//=======================================================
//  FUNCTION: state_machine_run
//=======================================================
void state_machine_run(byte *pointerRegMatrix, byte *pointerRegCar, byte *pointerShiftDir)
{
  /* Global variables. */

  PrintALLMatrix(pointerRegMatrix, pointerRegCar);

  switch (state)
  {
    case STATERESET:
      currentLevel = 1; // Reiniciar el nivel
      carsPassed = 0; // Reiniciar el contador de coches pasados
      delaytime = 2000;
      writeResetMatrix(pointerRegMatrix,pointerRegCar);
      delay(delaytime);
      if (keys == RESET_KEY)
        state = STATERESET;
      else if (keys == START_KEY)
        state = STATESTART;
      break;

    case STATESTART:
      writeStartMatrix(pointerRegMatrix,pointerRegCar);
      delay(delaytime);
      state = STATENUNO;
      break;

    case STATENUNO:
        /* Here is the data to start matrix */
        pointerRegMatrix[7] = B00111100;
        pointerRegMatrix[6] = B00111110;
        pointerRegMatrix[5] = B00111111;
        pointerRegMatrix[4] = B00111100;
        pointerRegMatrix[3] = B00111100;
        pointerRegMatrix[2] = B00111100;
        pointerRegMatrix[1] = B11111111;
        pointerRegMatrix[0] = B11111111;
        delay (delaytime);
        state=STATECLEAR;
        break;


    case STATECLEAR:
      writeClearMatrix(pointerRegMatrix,pointerRegCar);
      delay(delaytime);
      state = STATECHECK;
      break;

    case STATECHECK:
      // pointerShiftDir[0] = B00000000;
      // writeCarBase(pointerRegCar, pointerShiftDir);
      // writeGoCarsMatrix(pointerRegMatrix);
      // delay(delaytime);
      // checkLostMatrix(pointerRegMatrix, pointerRegCar);
      
      pointerShiftDir[0] = B00000000;
      // Movimiento del carro del jugador
      if (keys == LEFT_KEY && playerMoveCounter >=1) {
        pointerShiftDir[0] = B00000001;
        playerMoveCounter = 0; // Reiniciamos el contador después de mover el carro del jugador
      } else if (keys == RIGHT_KEY && playerMoveCounter >= 1) {
        pointerShiftDir[0] = B00000010;
        playerMoveCounter = 0; // Reiniciamos el contador después de mover el carro del jugador
      }
      // Movimiento de los carros que bajan
      if (i % 5 == 0) { // Solo se mueven los carros cada 5 iteraciones para que sean más lentos
        writeCarBase(pointerRegCar, pointerShiftDir);
      }
      // Incrementamos el contador para el movimiento del carro del jugador
      if (keys == LEFT_KEY || keys == RIGHT_KEY) {
        playerMoveCounter++;
      }
      writeGoCarsMatrix(pointerRegMatrix);
      delay(delaytime);
      checkLostMatrix(pointerRegMatrix, pointerRegCar);
       Serial.print("Current Level: ");
        Serial.println(currentLevel);
        Serial.print("Cars Passed: ");
        Serial.println(carsPassed);
        Serial.print("Rows per Level ");
        Serial.print(currentLevel);
        Serial.print(": ");
        Serial.println(rowsPerLevel[currentLevel - 1]);

        if (carsPassed == rowsPerLevel[currentLevel - 1]) {
          if (currentLevel == 1) {
            currentLevel = 2;
            carsPassed = 0;
            state = STATEDOS;
            break;
          } else if (currentLevel == 2) {
            currentLevel = 3;
            carsPassed = 0;
            state = STATETRES;
            break;
          } else if (currentLevel == 3) {
            state = STATEWON;
            break;
          }
        }

      if (Status == LOST)
        state = STATELOST;
      else if (Status == NIVELDOS)
        state = STATEDOS;
      else if (Status == NIVELTRES)
        state = STATETRES;
      else if ( Status == WIN)
        state = STATEWON;
      else if (keys == RESET_KEY)
        state = STATERESET;
      else if (keys == LEFT_KEY)
        state = STATELEFT;
      else if (keys == RIGHT_KEY)
        state = STATERIGTH;
      else if (keys == NO_KEY)
        state = STATECHECK;
      else
        state = STATECHECK;
      break;
      

    case STATEDOS:
         /* Here is the data to start matrix */
        pointerRegMatrix[7] = B11111110;
        pointerRegMatrix[6] = B11000011;
        pointerRegMatrix[5] = B11000001;
        pointerRegMatrix[4] = B01100000;
        pointerRegMatrix[3] = B00110000;
        pointerRegMatrix[2] = B00011000;
        pointerRegMatrix[1] = B11111111;
        pointerRegMatrix[0] = B11111111;
        delay (delaytime);
        delaytime = delaytime/3;
        state=STATECLEAR;
        break;

    case STATETRES:
         /* Here is the data to start matrix */
        pointerRegMatrix[7] = B11111111;
        pointerRegMatrix[6] = B11000000;
        pointerRegMatrix[5] = B11110000;
        pointerRegMatrix[4] = B11000000;
        pointerRegMatrix[3] = B01100000;
        pointerRegMatrix[2] = B00110000;
        pointerRegMatrix[1] = B11111111;
        pointerRegMatrix[0] = B11111111;
        delaytime = 2000;
        delay (delaytime);
        delaytime = delaytime/3;
        delaytime = delaytime/3;
        state=STATECLEAR;
        break;
      
    case STATEWON:
      pointerRegMatrix[0] = B00011000;
      pointerRegMatrix[1] = B00111100;
      pointerRegMatrix[2] = B01100110;
      pointerRegMatrix[3] = B11000011;
      pointerRegMatrix[4] = B11000011;
      pointerRegMatrix[5] = B01100110;
      pointerRegMatrix[6] = B00111100;
      pointerRegMatrix[7] = B00011000;

      //   PrintALLMatrix(pointerRegMatrix, pointerRegCar);

      if (keys == START_KEY) {
          state = STATERESET; // Volver al estado inicial
          currentLevel = 1; // Reiniciar el nivel
          carsPassed = 0; // Reiniciar el contador de coches pasados
          delaytime = 2000; // Reiniciar la velocidad del juego (ajustar según sea necesario)
          break;
      }
      break;

    case STATELEFT:
      pointerShiftDir[0] = B00000001;
      writeCarBase(pointerRegCar, pointerShiftDir);
      state = STATECHECK;
      break;

    case STATERIGTH:
      pointerShiftDir[0] = B00000010;
      writeCarBase(pointerRegCar, pointerShiftDir);
      state = STATECHECK;
      break;

    case STATELOST:
      writeLostMatrix(pointerRegMatrix,pointerRegCar);
      delaytime = 2000;
      delay(delaytime);
      if (keys == START_KEY)
        state = STATESTART;
      else
        state = STATERESET;
      break;

    default:
      state = STATERESET;
      break;
  }
}
//=======================================================
//  FUNCTION: Arduino loop
//=======================================================
void loop()
{
  read_KEY();
  state_machine_run(pointerRegMatrix,pointerRegCar,pointerShiftDir);
  delay(1);
}
