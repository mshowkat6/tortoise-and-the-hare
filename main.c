//======================================================================
//	Looping sound begins once you run the game. Press start to move from the start screen to the instructions. Move using up, down, left and right arrow keys.
//	Dig by holding down the B button and a fruit/veg should appear after a couple of frames. Tilemap modification also occurs after digging.
//  The player should see a flower tile near the player for the tilemap modification. Collide with the fruit/veg to collect it. Palette modification occurs
//  when the player collides with the fruit/veg. The flower that spawned during tilemap modification should turn a different color. 
//	When you collide with fruit/veg it increases the score. Also the non-looping sound plays. Once you collect 5 fruit/veg, you win the game. 
//  However, a turtle aka the enemy should randomly spawn when you dig. Press A to throw spinning carrots at the turtle. 
//  Keep throwing carrots at the turtle to kill it. If you collide with the turtle you lose.
//======================================================================

#include "gba.h"
#include "mode0.h"
#include "sprites.h"
#include "print.h"
#include "startBG.h"
#include "gardenBG.h"
#include "pausedBG.h"
#include "winBG.h"
#include "loseBG.h"
#include "finalSpriteSheet.h"
#include "game.h"
#include "digitalSound.h"
#include "backgroundMusic.h"
#include "finalInstruction.h"
#include "moving.h"

#define INSTRUCTIONHEIGHT 70

void initialize();
void goToStart();
void start();
void goToGame();
void game();
void goToInstructions();
void instructions();
void goToPause();
void pause();
void goToWin();
void win();
void goToLose();
void lose();

// random
void srand();

// game state
enum
{
    START,
    INSTRUCTIONS,
    GAME,
    PAUSE,
    WIN,
    LOSE
};
int state;

OBJ_ATTR shadowOAM[128];

unsigned short oldButtons;
unsigned short buttons;

int rSeed;
int parallelVOff = 0;
int parallelHOff = 0;
int parallelBG1VOff = 0;

int main() {

    initialize();

    while (1)
    {
        // update button variables
        oldButtons = buttons;
        buttons = REG_BUTTONS;

        // state machine
        switch (state) {
            case START:
                start();
                break;
            case GAME:
                game();
                break;
            case INSTRUCTIONS:
                instructions();
                break;
            case PAUSE:
                pause();
                break;
            case WIN:
                win();
                break;
            case LOSE:
                lose();
                break;
        }
    }
}

void initialize() {
 
    REG_DISPCTL = MODE(0) | BG_ENABLE(0) | BG_ENABLE(1) | BG_ENABLE(2) | BG_ENABLE(3);
    
    mgba_open();

    goToStart();
}

void goToStart() {

    REG_DISPCTL = MODE(0) | BG_ENABLE(2);
    REG_BG2CNT = BG_4BPP | BG_CHARBLOCK(2) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL;

    DMANow(3, &startBGTiles, &CHARBLOCK[2], startBGTilesLen / 2);
    DMANow(3, &startBGMap, &SCREENBLOCK[26], startBGMapLen / 2);
    DMANow(3 ,&startBGPal, BG_PALETTE, startBGPalLen / 2);


    hideSprites();
    DMANow(3, shadowOAM, OAM, 128 * 4);

    setupSounds();

    playSoundA(backgroundMusic_data, backgroundMusic_length, 1);

    waitForVBlank();

    state = START;

    rSeed = 0;
}

void start() {
     rSeed++;

     score = 0;

    waitForVBlank();
    if (BUTTON_PRESSED(BUTTON_START)) {
        srand(rSeed);
        goToInstructions();
    }
}

void goToInstructions() {
    
    int parallelVOff = 0;
    int parallelBG1VOff = 0;

    REG_DISPCTL = MODE(0) | BG_ENABLE(1) | BG_ENABLE(3);

    // text
    REG_BG1CNT = BG_4BPP | BG_CHARBLOCK(0) | BG_SCREENBLOCK(26) | BG_SIZE_TALL;
    DMANow(3, finalInstructionTiles, &CHARBLOCK[0], finalInstructionTilesLen/2);
	DMANow(3, finalInstructionMap, &SCREENBLOCK[26], finalInstructionMapLen/2);
    DMANow(3 ,&movingPal, BG_PALETTE, movingPalLen / 2);

    // grass
    REG_BG3CNT = BG_4BPP | BG_CHARBLOCK(1) | BG_SCREENBLOCK(30) | BG_SIZE_TALL;
    DMANow(3, movingTiles, &CHARBLOCK[1], movingTilesLen/2);
	DMANow(3, movingMap, &SCREENBLOCK[30], movingMapLen/2);

    waitForVBlank();

    state = INSTRUCTIONS;
}

void instructions() {
    
    REG_BG3VOFF = parallelVOff++;

    waitForVBlank();
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToGame();
        initGame();
    }
    if (BUTTON_PRESSED(BUTTON_SELECT)) {
        goToStart();
    }

    if (BUTTON_HELD(BUTTON_DOWN)) {

        if (parallelBG1VOff < INSTRUCTIONHEIGHT) {
            REG_BG1VOFF = parallelBG1VOff++;
        }
        REG_BG3VOFF = parallelVOff++;
    } else if (BUTTON_HELD(BUTTON_UP)) {

        if (parallelBG1VOff > -10) {
            REG_BG1VOFF = parallelBG1VOff--;
        }
        REG_BG3VOFF = parallelVOff++;
    }
}

void goToGame() {

    REG_DISPCTL = MODE(0) | BG_ENABLE(0) | SPRITE_ENABLE;
    REG_BG0CNT = BG_4BPP | BG_CHARBLOCK(0) | BG_SCREENBLOCK(16) | BG_SIZE_LARGE;
    waitForVBlank();

    DMANow(3, &gardenBGTiles, &CHARBLOCK[0], gardenBGTilesLen / 2);
    DMANow(3, &gardenBGMap, &SCREENBLOCK[16], gardenBGMapLen / 2);
    DMANow(3 ,&gardenBGPal, BG_PALETTE, gardenBGPalLen / 2);

    DMANow(3, finalSpriteSheetTiles, &CHARBLOCK[4], finalSpriteSheetTilesLen / 2);
    DMANow(3, finalSpriteSheetPal, SPRITE_PALETTE, finalSpriteSheetPalLen / 2);

    hideSprites();
    DMANow(3, shadowOAM, OAM, 128 * 4);


    state = GAME;
}

void game() {
    
    updateGame();
    drawGame();

    waitForVBlank();
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToPause();
    }
    if (score == 5) {
        goToWin();
    } 
    if (score == -1) {
        goToLose();
        score = 0;
    } 
}

void goToPause() {

    REG_DISPCTL = MODE(0) | BG_ENABLE(2);
    REG_BG2CNT = BG_4BPP | BG_CHARBLOCK(2) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL;

    DMANow(3, &pausedBGTiles, &CHARBLOCK[2], pausedBGTilesLen / 2);
    DMANow(3, &pausedBGMap, &SCREENBLOCK[26], pausedBGMapLen / 2);
    DMANow(3 ,&pausedBGPal, BG_PALETTE, pausedBGPalLen);

    waitForVBlank();
    hideSprites();

    state = PAUSE;
}

void pause() {

    waitForVBlank();
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToGame();
    }
}

void goToWin() {

    REG_DISPCTL = MODE(0) | BG_ENABLE(2);
    REG_BG2CNT = BG_4BPP | BG_CHARBLOCK(2) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL;

    DMANow(3, &winBGTiles, &CHARBLOCK[2], winBGTilesLen / 2);
    DMANow(3, &winBGMap, &SCREENBLOCK[26], winBGMapLen / 2);
    DMANow(3 ,&winBGPal, BG_PALETTE, winBGPalLen);

    waitForVBlank();
    hideSprites();

    state = WIN;
}

void win() {

    waitForVBlank();
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToStart();
    }
}

void goToLose() {
    
    REG_DISPCTL = MODE(0) | BG_ENABLE(2); 
    REG_BG2CNT = BG_4BPP | BG_CHARBLOCK(2) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL;

    DMANow(3, &loseBGTiles, &CHARBLOCK[2], loseBGTilesLen / 2);
    DMANow(3, &loseBGMap, &SCREENBLOCK[26], loseBGMapLen / 2);
    DMANow(3 ,&loseBGPal, BG_PALETTE, loseBGPalLen);

    waitForVBlank();
    hideSprites();

    state = LOSE;
}

void lose() {

    waitForVBlank();
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToStart();
    }
}