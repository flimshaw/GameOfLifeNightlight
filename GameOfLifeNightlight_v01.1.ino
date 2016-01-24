/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED Matrix backpacks 
  ----> http://www.adafruit.com/products/872
  ----> http://www.adafruit.com/products/871
  ----> http://www.adafruit.com/products/870

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
#include <TinyWireM.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

#define SET_BIT(p,n) ((p) |= (1 << (n)))
#define CLR_BIT(p,n) ((p) &= (~(1) << (n)))
#define MAX_GENERATIONS 125

// this function sets up the screen and sets up a
// timer that’ll restart the processor if it crashes
// for some reason
void setup() {
  WDTCR = WDTCR | (1<<WDE) | (1<<WDP3);
  SET_BIT( DDRB, 1 );
  CLR_BIT( PINB, 1 );
  matrix.begin(0x70);  // pass in the address
  matrix.setBrightness(15) ;
}

// here’s how big our screen is
#define NUMROWS 8
#define NUMCOLS 8

// this is the starting pattern we will show,
// the 1’s are lights that are on, and the first
// pattern is a glider
boolean gameBoard[NUMROWS][NUMCOLS] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 1, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};


// (continued)
// this is basically a scratch pad we use to
// find out what the next iteration of the game
// board will be

boolean newGameBoard[NUMROWS][NUMCOLS] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
};

// here’s a true/false flag to see if all the cells
// have died, a count of the currently alive cells,
// and how many generations of life we’ve run through

bool ALIVE = true;
int lifeCount = 0;
int genCount = 0;

// all these are vars used for making random numbers

byte m_w = 24;
unsigned long m_z = 163;
unsigned long counter = 1;
unsigned long tmp;

// super small random number generating function
// I wrote, but I kind of forget how it works...

byte getRandom()
{
  m_z += 82;
  if(m_z >= 255) m_z -= 255; 
  return byte(m_z);
}


// this function fills the game board with a “random”
// pattern of on/off lights to start with, then we
// apply the three rules to it over and over

void randomizeGameBoard() {
  for( byte y = 0; y < NUMROWS; y++ ) {  
    for( byte x = 0; x < NUMCOLS; x++ ) {
      if( getRandom() % m_w < m_w >> 2 ) { 
        gameBoard[y][x] = 1;
      } else {
        gameBoard[y][x] = 0;
      }
    }
  }
}

// Counts the number of active aroun this cell.
// Cells outside the board are considered “off”
// Returns a number in the range of 0 <= n < 9

byte countNeighbors(byte row, byte col) {
  byte count = 0;
  for (char rowDelta=-1; rowDelta<=1; rowDelta++) {
    for (char colDelta=-1; colDelta<=1; colDelta++) {
      // skip the center cell
      if (!(colDelta == 0 && rowDelta == 0)) {
        if (isCellAlive(rowDelta+row, colDelta+col)) {
          count++;
        }
      }
    }
  }
  return count;
}

// This gets the status of a given cell, wrapping
// around for cells that are off the game board

boolean isCellAlive(char row, char col) {

  if( row < 0 ) row = NUMROWS-1;
  if( col < 0 ) col = NUMCOLS-1;

  if( row == NUMROWS ) row = 0;
  if( col == NUMCOLS ) col = 0;

  return (gameBoard[row][col] == 1);
}

// Here’s the meaty bit: this is where we loop over
// each cell in the board, count its neighbors, and
// find out if it will live or die in the next round

void calculateNewGameBoard() {

  ALIVE = false;
  
  for (byte row=0; row<NUMROWS; row++) {
    for (byte col=0; col<NUMCOLS; col++) {
      byte numNeighbors = countNeighbors(row, col);
  bool g = gameBoard[row][col];
      
  // Any live cell with fewer than two live
  // neighbours dies, as if caused by 
  // under-population.
      if (g && numNeighbors < 2) {
        newGameBoard[row][col] = false;

  // Any live cell with two or three live 
  // neighbours lives on to the next generation.
      } else if (g && (numNeighbors == 2 || numNeighbors == 3)) {
        newGameBoard[row][col] = true;

      // Any live cell with more than three live
  //neighbours dies, as if by overcrowding.
      } else if (g && numNeighbors > 3) {
        newGameBoard[row][col] = false;

      // Any dead cell with exactly three live 
  // neighbours becomes a live cell, as if by
  // reproduction.
      } else if (!g && numNeighbors == 3) {
        newGameBoard[row][col] = true;

      // All other cells will remain off
      } else {
        newGameBoard[row][col] = false;
      }

      
  // if there is an alive cell, mark ALIVE to
  // true, and tell the processor we’re still
  // alive.
  
  if( g == true && ALIVE == false ) {
        ALIVE = true;
        asm("wdr");
      }
    }
  }
}

// This copies the new game board to the one we use
// to drive the display.

void swapGameBoards() {
  for (byte row=0; row<NUMROWS; row++) {
    for (byte col=0; col<NUMCOLS; col++) {
      gameBoard[row][col] = newGameBoard[row][col];
    }
  }
}

// this function loops forever, calling the functions
// above to keep the simulation running

void loop() {
  
  // Increase the number of generations we have
  // calculated. If we’ve done more than 125, or
  // no cells are alive, randomize the board.  
  lifeCount++;
  if( lifeCount >= MAX_GENERATIONS || ALIVE == false ) {
    randomizeGameBoard();
    lifeCount = 0;
  }
  
  // Apply one iteration of the Game of Life rules
  // do the game board
  calculateNewGameBoard();

  // clear the LED display
  matrix.clear();
  
  // for each pixel in the display, set it to on if
  // its corresponding cell is alive in the new board

  for( int y = 0; y < 8; y++ ) {
    for( int x = 0; x < 8; x++ ) {
      if( newGameBoard[y][x] ) matrix.drawPixel(x, y, LED_ON);
    }    
  }

  // copy the new game board to the old one, so we
  // have it to start with in the next round
  swapGameBoards();

  // transmit the new pixels to the display
  matrix.writeDisplay();

  // wait for 50 milliseconds, doing nothing at all
  delay(50);

}
