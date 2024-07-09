#include "game.h"
#include "sprites.h"
#include "gba.h"
#include "mode0.h"
#include "gardenBGCollision.h"
#include "musicCollision.h"
#include "bell.h"
#include "digitalSound.h"

#define MAPWIDTH 512
#define MAPHEIGHT 400

OBJ_ATTR shadowOAM[128];
SPRITE player;
SPRITE fruit[5];
SPRITE carrot[30];
SPRITE turtle;
SPRITE worm;

typedef enum {UP, RIGHT, DOWN, LEFT, DIG} DIRECTION;

int digCount = 40;
int hurtCount;
int score = 0;
int pressed;
int carrotNum;
int currTileX;
int currTileY;
int cheat = 0;
int glow = 0;
int tileFrame = 1;

int hOff;
int vOff;

unsigned char* collisionMap;
unsigned char* musicCollisionMap;

inline unsigned char colorAt(int x, int y){
    return collisionMap[OFFSET(x, y, MAPWIDTH)];
}

inline unsigned char musicAt(int x, int y){
    return musicCollisionMap[OFFSET(x, y, MAPWIDTH)];
}

void initGame() {
    collisionMap = (unsigned char*) gardenBGCollisionBitmap;
    musicCollisionMap = (unsigned char*) musicCollisionBitmap;
    score = 0;
    cheat = 0;

    initPlayer();
    initFruit();
    initCarrot();
    initTurtle();
    turtle.active = 0;
    turtle.hide = 1;
    initWorm();
    worm.active = 0;
    worm.hide = 1;
    
}

void updateGame() {

    updatePlayer();

    for (int i = 0; i < 5; i++) {
        updateFruit(&fruit[i]);
    }

    if (BUTTON_PRESSED(BUTTON_A)) {
        spawnCarrot();
    }

    if (BUTTON_PRESSED(BUTTON_LSHOULDER)) {
        cheat = !cheat;
    }

    updateCarrot();

    updateTurtle();

    updateWorm();
}

void drawGame() {

    drawPlayer();

    for (int i = 0; i < 5; i++) {
        drawFruit(&fruit[i]);
    }

    for (int i = 0; i < 30; i++) {
        drawCarrot(&carrot[i]);
    }

    drawTurtle();
    drawWorm();

    waitForVBlank();
    REG_BG0HOFF = hOff;
    REG_BG0VOFF = vOff;
    DMANow(3, shadowOAM, OAM, 128 * 4);
}

// initalizing
void initPlayer() {
    player.x = 132;
    player.y = 9;
    player.direction = DOWN;
    player.xVel = 4; 
    player.yVel = 3; 
    player.width = 25; 
    player.height = 27; 
    player.oamIndex = 0;
    player.hide = 0;
    player.timeUntilNextFrame = 5;
    player.health = 100;
}

void initFruit() {
    for (int i = 0; i < 5; i++) {
        fruit[i].oamIndex = i + 1;
        fruit[i].hide = 1;
    }
}

void initCarrot() {
    for (int i = 0; i < 30; i++) {
        carrot[i].hide = 1; 
        carrot[i].oamIndex = i + 7;
    }
}

void initTurtle() {
    turtle.hide = 1;
    turtle.oamIndex = 38;
    turtle.spawn = 0;
    turtle.active = 0;
}

void initWorm() {
    worm.hide = 1;
    worm.oamIndex = 39;
}

// spawning
void spawnFruit() {

    int usedNumbers[5] = {0};

    for (int i = 0; i < 5; i++) {

        if (fruit[i].hide) {
            int fruitPicker;

            do {
                fruitPicker = rand() % 5;
            } while (usedNumbers[fruitPicker]); 

            usedNumbers[fruitPicker] = 1;
            
            switch (fruitPicker) {
                case 0:
                    fruit[i].tile = 0;
                    fruit[i].width = 10;
                    fruit[i].height = 11;
                    spawnTurtle();
                    break;
                case 1:
                    fruit[i].tile = 2; 
                    fruit[i].width = 16;
                    fruit[i].height = 16;
                    spawnTurtle();
                    break;
                case 2:
                    fruit[i].tile = 4;
                    fruit[i].width = 13;
                    fruit[i].height = 16;
                    spawnTurtle();
                    break;
                case 3:
                    fruit[i].tile = 6;
                    fruit[i].width = 16;
                    fruit[i].height = 16;
                    spawnTurtle();
                    break;
                case 4:
                    fruit[i].tile = 8;
                    fruit[i].width = 11;
                    fruit[i].height = 13;
                    spawnTurtle();
                    break; 
            }

            fruit[i].x = player.x + 5;
            fruit[i].y = player.y - 20;
            if (fruit[i].x < 0) {
                fruit[i].x = 0;
            }
            if (fruit[i].y < 0) {
                fruit[i].y = 0;
            }
            fruit[i].hide = 0;
            fruit[i].active = 1;

            if (score == -1) {
                fruit[i].hide = 1;
            }

            break;
        }
    }
}

void spawnCarrot() {
    for (int i = 0; i < 30; i++) {
        if (carrot[i].hide) {
            
            carrot[i].hide = 0;
            carrot[i].active = 1;
            carrot[i].isAnimating = 1;

            if (player.direction == RIGHT) {
                carrot[i].direction = RIGHT;
                carrot[i].x = player.x + 25;
                carrot[i].y = player.y + 10;
            } else if (player.direction == LEFT) {
                carrot[i].direction = LEFT;
                carrot[i].x = player.x;
                carrot[i].y = player.y + 10;
            } else if (player.direction == UP) {
                carrot[i].direction = UP;
                carrot[i].x = player.x + 8;
                carrot[i].y = player.y - 5;
            } else if (player.direction == DOWN) {
                carrot[i].direction = DOWN;
                carrot[i].x = player.x + 8;
                carrot[i].y = player.y + 28;
            } 

            carrot[i].timeUntilNextFrame = 2;
            carrot[i].currentFrame = 0; 
            carrot[i].numFrames = 3; 
            carrot[i].xVel = 4; 
            carrot[i].yVel = 4; 

            break; 
        }
    }
}

void spawnTurtle() {
    turtle.x = player.x - 100;
    turtle.y = player.y - 60;
    if (turtle.x < 0) {
        turtle.x = 0;
    }
    if (turtle.y < 0) {
        turtle.y = 0;
    }

    turtle.xVel = (rand() % 3) + 1; 
    turtle.yVel = (rand() % 2) + 1; ; 
    turtle.width = 21; 
    turtle.height = 12; 
    turtle.oamIndex = 38;
    turtle.hide = 0;
    turtle.timeUntilNextFrame = 4;
    turtle.numFrames = 4;
    turtle.currentFrame = 0;
    turtle.health = 100;
    turtle.active = 1;

    if (cheat) {
        spawnWorm();
    }
}

void spawnWorm() {
    worm.x = player.x;
    worm.y = player.y - 60;
    worm.xVel = 4; 
    worm.yVel = 4; 
    worm.width = 8; 
    worm.height = 2; 
    worm.hide = 0;
    worm.timeUntilNextFrame = 4;
    worm.numFrames = 2;
    worm.currentFrame = 0;
    worm.active = 1;
}

// updating
void updatePlayer() {
    
    // palette mod
    glow++;
    if (glow > 10) {
        glow == 0;

        tileFrame ++;
        if ((tileFrame % 10 == 0) && (tileFrame % 20 != 0)) {
            BG_PALETTE[15] = RGB(3, 15, 22); 
        } else if ((tileFrame % 10 == 0) && (tileFrame % 20 == 0)){
            BG_PALETTE[15] = RGB(23, 15, 22);   
        }
    } 
    
    player.isAnimating = 0;
    int leftX = player.x;
    int rightX = player.x + player.width - 1;
    int topY = player.y;
    int bottomY = player.y + player.height - 1;

    // button controls
    if (BUTTON_HELD(BUTTON_UP)) {
        digCount = 50;
        player.isAnimating = 1;
        player.numFrames = 2;
        player.width = 19;
        player.height = 28;
        player.direction = UP;
        if (colorAt(leftX, topY - player.yVel) && colorAt(rightX, topY - player.yVel)) {
            player.y -= player.yVel;
        }
    } else if (BUTTON_HELD(BUTTON_DOWN)) {
        digCount = 50;
        player.isAnimating = 1;
        player.numFrames = 2;
        player.width = 19;
        player.height = 25;
        player.direction = DOWN;
        if (colorAt(leftX, bottomY + player.yVel) && colorAt(rightX, bottomY + player.yVel)) {
            player.y += player.yVel;
        }
    } else if (BUTTON_HELD(BUTTON_LEFT)) {
        digCount = 50;
        player.isAnimating = 1;
        player.numFrames = 4;
        player.width = 25; 
        player.height = 27; 
        player.direction = LEFT;
        if (colorAt(leftX - player.xVel, bottomY)) {
            player.x -= player.xVel;
        }
    } else if (BUTTON_HELD(BUTTON_RIGHT)) {
        digCount = 50;
        player.isAnimating = 1;
        player.numFrames = 4;
        player.width = 25; 
        player.height = 27; 
        player.direction = RIGHT;
        if (colorAt(rightX + player.xVel + 2, bottomY)) {
            player.x += player.xVel;
        } 
    } else if(!turtle.active) {
        if ((83 <= ((player.x))) && ((player.x) <=109) && (78 <= (player.y + player.height)) && ((player.y + player.height) <= 90)) {
            if (BUTTON_HELD(BUTTON_B)) {
                digCount--;
                player.isAnimating = 1;
                player.numFrames = 2;
                player.direction = DIG;
            }
        } else if ((203 <= ((player.x))) && ((player.x) <= 229) && (134 <= (player.y + player.height)) && ((player.y + player.height) <= 146)) {
            if (BUTTON_HELD(BUTTON_B)) {
                digCount--;
                player.isAnimating = 1;
                player.numFrames = 2;
                player.direction = DIG;
            }
        } else if ((299 <= ((player.x))) && ((player.x) <= 325) && (238 <= (player.y + player.height)) && ((player.y + player.height) <= 250)) {
            if (BUTTON_HELD(BUTTON_B)) {
                digCount--;
                player.isAnimating = 1;
                player.numFrames = 2;
                player.direction = DIG;
            }
        } else if ((339 <= ((player.x))) && ((player.x) <= 365) && (94 <= (player.y + player.height)) && ((player.y + player.height) <= 106)) {
            if (BUTTON_HELD(BUTTON_B)) {
                digCount--;
                player.isAnimating = 1;
                player.numFrames = 2;
                player.direction = DIG;
            }
        } else if ((435 <= ((player.x))) && ((player.x) <= 461) && (54 <= (player.y + player.height)) && ((player.y + player.height) <= 66)) {
            if (BUTTON_HELD(BUTTON_B)) {
                digCount--;
                player.isAnimating = 1;
                player.numFrames = 2;
                player.direction = DIG;
            }
        }  

        if (digCount <= 0) {
            if (digCount == 0) {
                spawnFruit();

                // tile mod
                if ((83 <= ((player.x))) && ((player.x) <=109) && (78 <= (player.y + player.height)) && ((player.y + player.height) <= 90)) {
                   SCREENBLOCK[16].tilemap[OFFSET((96) / 8, (80) / 8, 32)] = TILEMAP_ENTRY_TILEID(1);
                } else if ((203 <= ((player.x))) && ((player.x) <= 229) && (134 <= (player.y + player.height)) && ((player.y + player.height) <= 146)) {
                    SCREENBLOCK[16].tilemap[OFFSET((216) / 8, (136) / 8, 32)] = TILEMAP_ENTRY_TILEID(1);
                }  else if ((299 <= ((player.x))) && ((player.x) <= 325) && (238 <= (player.y + player.height)) && ((player.y + player.height) <= 250)) {
                    SCREENBLOCK[17].tilemap[OFFSET((312) / 8, 232 / 8, 32)] = TILEMAP_ENTRY_TILEID(1);
                }  else if ((339 <= ((player.x))) && ((player.x) <= 365) && (94 <= (player.y + player.height)) && ((player.y + player.height) <= 106)) {
                    SCREENBLOCK[17].tilemap[OFFSET((352) / 8, ((88) / 8), 32)] = TILEMAP_ENTRY_TILEID(1);
                } else if ((435 <= ((player.x))) && ((player.x) <= 461) && (54 <= (player.y + player.height)) && ((player.y + player.height) <= 66)) {
                    SCREENBLOCK[17].tilemap[OFFSET((448) / 8, ((48) / 8), 32)] = TILEMAP_ENTRY_TILEID(1);
                }  
            }
             player.isAnimating = 0;
             player.direction = DOWN;
             player.food = 1;
        }
    } 

    if (player.isAnimating) {
        player.timeUntilNextFrame--;
        if (player.timeUntilNextFrame == 0) {
            player.timeUntilNextFrame = 5;
            player.currentFrame = (player.currentFrame + 1) % player.numFrames; 
        }
    } else {
        player.currentFrame = 0;
        player.timeUntilNextFrame = 5;
    }

    // hoff voff
    hOff = player.x - (SCREENWIDTH - player.width) / 2;
    vOff = player.y - (SCREENHEIGHT - player.height) / 2;

    if (player.x < 0) {
        player.x = 0;
    }
    if (player.y < 0) {
        player.y = 0;
    }
    if (player.x > MAPWIDTH - player.width) {
        player.x = MAPWIDTH - player.width;
    }
    if (player.y > MAPHEIGHT - player.height) {
        player.y = MAPHEIGHT - player.height;
    }

    if (hOff < 0) {
        hOff = 0;
    }
    if (vOff < 0) {
        vOff = 0;
    }
    if (hOff > MAPWIDTH - SCREENWIDTH){
        hOff = MAPWIDTH - SCREENWIDTH;
    } 
    if (vOff > MAPHEIGHT - SCREENHEIGHT){
        vOff = MAPHEIGHT - SCREENHEIGHT;
    } 
}


void updateFruit(SPRITE* fruit) {
    if (score == -1) {
        fruit->hide = 1;
        fruit->active = 0;
    }

    if (!turtle.active) {
        if (collision(fruit->x, fruit->y, fruit->width, fruit->height, player.x, player.y, player.width, player.height) && fruit->active) {
            fruit->hide = 1;
            if (fruit->active) {
                playSoundB(bell_data, bell_length, 0);
                score++;
                fruit->active = 0;
            }
        }
    }
}

void updateCarrot() {
    if (turtle.hurt && hurtCount > 0) {
        hurtCount--;
    } else {
        turtle.hurt = 0;
    }
    for (int i = 0; i < 30; i++) {
        if (carrot[i].active) {
            if (carrot[i].direction == RIGHT) {
                carrot[i].x += carrot[i].xVel;
            } else if (carrot[i].direction == LEFT) {
                carrot[i].x -= carrot[i].xVel;
            } else if (carrot[i].direction == UP) {
                carrot[i].y -= carrot[i].yVel;
            } else if (carrot[i].direction == DOWN) {
                carrot[i].y += carrot[i].yVel;
            }

         // boundary checks
        if (carrot[i].x < 0) {
            carrot[i].active = 0;
            carrot[i].hide = 1;
        }
        if (carrot[i].y < 0) {
            carrot[i].active = 0;
            carrot[i].hide = 1;
        }
        if (carrot[i].x > MAPWIDTH - carrot[i].width) {
            carrot[i].active = 0;
            carrot[i].hide = 1;
        }
        if (carrot[i].y > MAPHEIGHT - carrot[i].height) {
            carrot[i].active = 0;
            carrot[i].hide = 1;
        } 

        // turtle collision 
        if (collision(carrot[i].x, carrot[i].y, carrot[i].width, carrot[i].height, turtle.x, turtle.y, turtle.width, turtle.height)) {
            carrot[i].active = 0;
            carrot[i].hide = 1;
            turtle.health -= 25;
            turtle.hurt = 1;
            hurtCount = 5;
        }
        
        if (score == -1) {
            carrot[i].hide;
        }

            if (carrot[i].isAnimating) {
                carrot[i].timeUntilNextFrame--;
                if (carrot[i].timeUntilNextFrame == 0) {
                    carrot[i].timeUntilNextFrame = 2;
                    carrot[i].currentFrame = (carrot[i].currentFrame + 1) % carrot[i].numFrames;
                }
            }
        }
    }
}

void updateTurtle() {
    turtle.isAnimating = 1;

    if (turtle.hurt) {
        turtle.x -= 1;
        turtle.y = turtle.y;
    } else {
        if (turtle.active) {
            if (player.x - hOff > turtle.x - hOff){
                turtle.x += turtle.xVel;
                turtle.direction = RIGHT;
            }
            if (player.x - hOff < turtle.x - hOff){
                turtle.x -= turtle.xVel;
                turtle.direction = LEFT;
            }
            if (player.y - vOff < turtle.y - vOff){
                turtle.y -= turtle.yVel;
            }
            if (player.y - vOff > turtle.y - vOff){
                turtle.y += turtle.yVel;
            }
        }
    }

    // player dead cause of collision
    if (collision(turtle.x, turtle.y, turtle.width, turtle.height, player.x, player.y, player.width, player.height) && turtle.active) {
        score = -1;
        player.hide = 1;
        turtle.hide = 1;
    }

        // turtle collision
    if (collision(turtle.x, turtle.y, turtle.width, turtle.height, worm.x, worm.y, worm.width, worm.height) && turtle.active && worm.active) {
        worm.active = 0;
        worm.hide = 1;
        turtle.xVel = 0;
    }

    // dead turtle
    if (turtle.health == 0 || score == -1) {
        turtle.hide = 1;
        turtle.active = 0;
    }

    // if (score == -1) {
    //     turtle.hide = 1;
    //     turtle.active = 0;
    // }

    if (turtle.isAnimating) {
        turtle.timeUntilNextFrame--;
        if (turtle.timeUntilNextFrame == 0) {
            turtle.timeUntilNextFrame = 4;
            turtle.currentFrame = (turtle.currentFrame + 1) % turtle.numFrames; 
        }
    }
}

updateWorm() {
    worm.isAnimating = 1;

    // chase turtle
    if (turtle.active && worm.active) {
        if (turtle.x - hOff > worm.x - hOff){
            worm.x += worm.xVel;
            worm.direction = RIGHT;
        }
        if (turtle.x - hOff < worm.x - hOff){
                worm.x -= worm.xVel;
                worm.direction = LEFT;
        }
        if (turtle.y - vOff < worm.y - vOff){
                worm.y -= worm.yVel;
        }
        if (turtle.y - vOff > worm.y - vOff){
                worm.y += worm.yVel;
        }
    } else {
        worm.hide = 1;
    }

    if (worm.isAnimating) {
        worm.timeUntilNextFrame--;
        if (worm.timeUntilNextFrame == 0) {
            worm.timeUntilNextFrame = 4;
            worm.currentFrame = (worm.currentFrame + 1) % worm.numFrames; 
        }
    }
}

// drawing
void drawPlayer() {
    shadowOAM[player.oamIndex].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(player.y - vOff);
    shadowOAM[player.oamIndex].attr1 = ATTR1_MEDIUM | ATTR1_X(player.x - hOff);
    if (player.direction == RIGHT || player.direction == LEFT) {
        shadowOAM[player.oamIndex].attr2 = ATTR2_PALROW(0) | ATTR2_PRIORITY(0) | ATTR2_TILEID(player.currentFrame * 4,0);
    } else if (player.direction == DOWN) {
       shadowOAM[player.oamIndex].attr2 = ATTR2_PALROW(0) | ATTR2_PRIORITY(0) | ATTR2_TILEID(16 + (player.currentFrame * 4),0); 
    } else if (player.direction == UP) {
        shadowOAM[player.oamIndex].attr2 = ATTR2_PALROW(0) | ATTR2_PRIORITY(0) | ATTR2_TILEID(24 + (player.currentFrame * 4),0); 
    } else if (player.direction == DIG) {
        shadowOAM[player.oamIndex].attr2 = ATTR2_PALROW(0) | ATTR2_PRIORITY(0) | ATTR2_TILEID(16 + (player.currentFrame * 4),4); 
    }

    if (player.hide || score == 5) {
        shadowOAM[player.oamIndex].attr0 |= ATTR0_HIDE;
    }
    if (player.direction == LEFT) {
        shadowOAM[player.oamIndex].attr1 |= ATTR1_HFLIP;
    }
}

void drawFruit(SPRITE* fruit) {
    shadowOAM[fruit->oamIndex].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(fruit->y - vOff);
    if (fruit->hide || !collision(fruit->x, fruit->y, fruit->width, fruit->height, hOff, vOff, SCREENWIDTH, SCREENHEIGHT)) {
        shadowOAM[fruit->oamIndex].attr0 |= ATTR0_HIDE;
    }
    shadowOAM[fruit->oamIndex].attr1 = ATTR1_SMALL | ATTR1_X(fruit->x - hOff);
    shadowOAM[fruit->oamIndex].attr2 = ATTR2_PALROW(1) | ATTR2_PRIORITY(0) | ATTR2_TILEID(fruit->tile, 4); 
}

void drawCarrot(SPRITE* carrot) {
    if (carrot->active) {
        shadowOAM[carrot->oamIndex].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(carrot->y - vOff);
        shadowOAM[carrot->oamIndex].attr1 = ATTR1_TINY | ATTR1_X(carrot->x - hOff);
        shadowOAM[carrot->oamIndex].attr2 = ATTR2_PALROW(1) | ATTR2_PRIORITY(0) | ATTR2_TILEID(10 + carrot->currentFrame, 4); 
    }
    if (carrot->hide) {
        shadowOAM[carrot->oamIndex].attr0 |= ATTR0_HIDE;
    }
}

void drawTurtle() {
    shadowOAM[turtle.oamIndex].attr0 = ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(turtle.y - vOff);
    shadowOAM[turtle.oamIndex].attr1 = ATTR1_MEDIUM | ATTR1_X(turtle.x - hOff);
    shadowOAM[turtle.oamIndex].attr2 = ATTR2_PALROW(2) | ATTR2_PRIORITY(0) | ATTR2_TILEID(turtle.currentFrame * 4, 8); 
    
    if (turtle.hurt) {
        shadowOAM[turtle.oamIndex].attr2 = ATTR2_PALROW(2) | ATTR2_PRIORITY(0) | ATTR2_TILEID(16, 8); 
    }
    if (turtle.hide || !turtle.active || !collision(turtle.x, turtle.y, turtle.width, turtle.height, hOff, vOff, SCREENWIDTH, SCREENHEIGHT)) {
        shadowOAM[turtle.oamIndex].attr0 |= ATTR0_HIDE;
    }
    if (turtle.direction == LEFT) {
        shadowOAM[turtle.oamIndex].attr1 |= ATTR1_HFLIP;
    }
    
}

void drawWorm() {
    shadowOAM[worm.oamIndex].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(worm.y - vOff);
    shadowOAM[worm.oamIndex].attr1 = ATTR1_TINY | ATTR1_X(worm.x - hOff);
    shadowOAM[worm.oamIndex].attr2 = ATTR2_PALROW(0) | ATTR2_PRIORITY(0) | ATTR2_TILEID(worm.currentFrame, 6); 

    if (worm.hide || !collision(worm.x, worm.y, worm.width, worm.height, hOff, vOff, SCREENWIDTH, SCREENHEIGHT)) {
        shadowOAM[worm.oamIndex].attr0 |= ATTR0_HIDE;
    }
    if (worm.direction == RIGHT) {
        shadowOAM[worm.oamIndex].attr1 |= ATTR1_HFLIP;
    }
    
}