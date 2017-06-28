#include <LiquidCrystal.h>

//Impostazioni schermo LCD
#define LCD_WIDTH 16
#define LCD_EMPTY 0
#define LCD_DOWN 1

//Impostazioni shuttle
#define SHUTTLE_POSITION 7

//Pin bottoni sparo
#define FIRE_BUTTON_RIGHT 13
#define FIRE_BUTTON_LEFT 12

//Numero totale sprite
#define TOTAL_SPRITE 8

//Sprite shuttle
#define SHUTTLE_SPRITE1 1
#define SHUTTLE_SPRITE2 2
#define SHUTTLE_SPRITE1_LEFT 7
#define SHUTTLE_SPRITE2_LEFT 8
#define SHUTTLE_FIRE1 3
#define SHUTTLE_FIRE1_AFTER 4
#define SHUTTLE_FIRE1_BEFORE 5

//Sprite enemy
#define SPRITE_ENEMY 6

//Sprite casella vuota
#define SPRITE_BLOCK_EMPTY ' '

//Variabili globali
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
char gameArea[LCD_WIDTH + 1];
bool buttonPushed = false;
bool dead = false;
int nKill = 0;
int level = 0;
int pushed = 0;
int fireDirection = FIRE_BUTTON_RIGHT;

/**
 * Funzione che gestisce la creazione dei caratteri sprite ed il reset dello schermo LCD
 */
void drawLCD(){
  static byte sprites[] = {
    //Primo sprite navicella
    0b01100,
    0b00100,
    0b11110,
    0b10101,
    0b11110,
    0b00100,
    0b01100,
    0b00000,
    //Secondo sprite navicella
    0b10100,
    0b00100,
    0b11110,
    0b10101,
    0b11110,
    0b00100,
    0b10100,
    0b00000,
    //Sparo
    0b00000,
    0b00000,
    0b00000,
    0b00100,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    //Sparo sinistra
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    //Sparo destra
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    //Navicella nemica
    0b00100,
    0b01010,
    0b01110,
    0b10001,
    0b10001,
    0b01110,
    0b00000,
    0b00000,
    //Primo sprite navicella verso sinistra
    0b00110,
    0b00100,
    0b01111,
    0b10101,
    0b01111,
    0b00100,
    0b00110,
    0b00000,
    //Secondo sprite navicella verso sinistra
    0b00101,
    0b00100,
    0b01111,
    0b10101,
    0b01111,
    0b00100,
    0b00101,
    0b00000
  };

  //Creazione sprite
  for (int i = 0; i < TOTAL_SPRITE; i++) {
    lcd.createChar(i + 1, &sprites[i * 8]);
  }

  //Reset LCD
  for (int i = 0; i < LCD_WIDTH; i++) {
    lcd.setCursor(i, 0);
    lcd.print(" ");
    
    gameArea[i] = SPRITE_BLOCK_EMPTY;
  }
}

/**
 * Funziona che disegna il nostro shuttle
 * Input:
 *      -gameArea: vettore area di gioco
 *      -currentSprite: sprite usato al momento per lo shuttle
 * Output:
 *      -collision: comunica se c'è stata una collisione
 */
bool drawShuttle(char* gameArea, byte currentSprite){
  byte digits = 1;  //Quantità cifre punteggio
  bool collision = false;
  char oldPosition = gameArea[SHUTTLE_POSITION];

  if(fireDirection == FIRE_BUTTON_RIGHT){
    if(currentSprite == SHUTTLE_SPRITE1 || currentSprite == SHUTTLE_SPRITE2_LEFT){
      gameArea[SHUTTLE_POSITION] = SHUTTLE_SPRITE1;
    }else{
      gameArea[SHUTTLE_POSITION] = SHUTTLE_SPRITE2;
    }
  }else{
    if (currentSprite == SHUTTLE_SPRITE1 || currentSprite == SHUTTLE_SPRITE2 || currentSprite == SHUTTLE_SPRITE2_LEFT) {
        gameArea[SHUTTLE_POSITION] = SHUTTLE_SPRITE1_LEFT;
      }else{
        gameArea[SHUTTLE_POSITION] = SHUTTLE_SPRITE2_LEFT;
      }
  }

  //Gestione collisione
  if(oldPosition != SPRITE_BLOCK_EMPTY){
    collision = true;
  }

  if(nKill > 9999){
    digits = 5;
  }else if(nKill > 999){
    digits = 4;
  }else if(nKill > 99){
    digits = 3;
  }else if(nKill > 9){
    digits = 2;
  }

  //Stampa area di gioco
  lcd.setCursor(0, 1);
  lcd.print(gameArea);

  //Stampa numero kill
  lcd.setCursor(LCD_WIDTH - digits, 0);
  lcd.print(nKill);
  
  //Stampa numero livello
  if(!dead){
    lcd.setCursor(0, 0);
    lcd.print("Livello ");
    lcd.print(level);
  }

  gameArea[SHUTTLE_POSITION] = oldPosition;
  
  return collision;
}

/**
 * Funzione creazione e avanzamento area di gioco
 * Input: 
 *        -gameArea: vettore area di gioco
 *        -newSprite: sprite da aggiungere
 *        -toRight: ci indica se il nuovo sprite va aggiunto a destra o a sinistra
*/
void updateArea(char* gameArea, byte newSprite, _Bool toRight){
  char current;
  char next;
  char prev;
  
  //Aggiornamento parte destra dello shuttle
  for (int i = SHUTTLE_POSITION; i < LCD_WIDTH; i++) {
    current = gameArea[i];

    //Viene aggiunto il nuovo sprite alla fine
    if(i == LCD_WIDTH-1){
      if(toRight){
        next = newSprite;
      }else{
        next = SPRITE_BLOCK_EMPTY;
      }
    }else{
      next = gameArea[i+1];
    }
    
    switch (current){
      //Casella vuota
      case SPRITE_BLOCK_EMPTY:
        if(next == SPRITE_ENEMY){
          //La prossima casella contiene un nemico
          gameArea[i] = next;
        }else if(next == SHUTTLE_FIRE1){
          //La prossima casella contiene uno sparo   
          if(gameArea[i+2] != SPRITE_ENEMY){
            gameArea[i+2] = SHUTTLE_FIRE1;
          }else{
            //Collisione sparo-ufo
            Serial.println("Collisione3");
            gameArea[i+2] = SPRITE_BLOCK_EMPTY;
            gameArea[i+1] = SPRITE_BLOCK_EMPTY;
            nKill++;
          }
          gameArea[i+1] = SHUTTLE_FIRE1_BEFORE;
        }else{
            gameArea[i] = SPRITE_BLOCK_EMPTY;
        }
        break;

      //Nemico
      case SPRITE_ENEMY:
        if(next == SPRITE_BLOCK_EMPTY){
          //C'è solo una navicella
          gameArea[i] = next;
        }else{
          //Sono presenti più navicelle attaccate
          gameArea[i] = SPRITE_ENEMY;
        }
        break;

      //Sparo shuttle
      case SHUTTLE_FIRE1:
        if(next == SPRITE_BLOCK_EMPTY){
          //La casella dopo lo sparo è vuota
          gameArea[i] = SHUTTLE_FIRE1_AFTER;
        }else if(next == SPRITE_ENEMY){
          //Collisione sparo-ufo
          Serial.println("Collisione");
          gameArea[i] = SPRITE_BLOCK_EMPTY;
          gameArea[i+1] = SPRITE_BLOCK_EMPTY;
          nKill++;
        }else{
          gameArea[i] = SHUTTLE_FIRE1;
        }
        break;
      case SHUTTLE_FIRE1_AFTER:
        if(gameArea[i+1] == SPRITE_ENEMY){
          //Collisione sparo-ufo
          Serial.println("Collisione2");
          gameArea[i] = SPRITE_BLOCK_EMPTY;
          gameArea[i+1] = SPRITE_BLOCK_EMPTY;
          nKill++;
        }else{
          gameArea[i] = SHUTTLE_FIRE1;
        }
        break;
      
      case SHUTTLE_FIRE1_BEFORE:
        gameArea[i] = SPRITE_BLOCK_EMPTY;
        break;
    }
  }
  
  //Aggiornamento parte sinistra
  for (int i = SHUTTLE_POSITION-1; i > 0; i--) {
    current = gameArea[i];

    //Viene aggiunto il nuovo sprite a sinistra
    if(i == 1){
      if(!toRight){
        gameArea[0] = newSprite;
      }else{
        gameArea[0] = SPRITE_BLOCK_EMPTY;
      }
    }
    
    prev = gameArea[i-1];
    
    switch (current){
      //Casella vuota
      case SPRITE_BLOCK_EMPTY:
        if(prev == SPRITE_ENEMY){
          //La casella precedente contiene un nemico
          gameArea[prev] = SPRITE_BLOCK_EMPTY;
          gameArea[i] = SPRITE_ENEMY;
        }else{
          gameArea[i] = SPRITE_BLOCK_EMPTY;
        }
        break;

      //Nemico
      case SPRITE_ENEMY:
        gameArea[i+1] = SPRITE_ENEMY;
        if(prev == SPRITE_BLOCK_EMPTY){
          //C'è solo una navicella
          gameArea[i] = SPRITE_BLOCK_EMPTY;
        }else{
          //Sono presenti più navicelle attaccate
          gameArea[i] = SPRITE_ENEMY;
        }
        break;

      //Sparo shuttle
      case SHUTTLE_FIRE1:
        if(prev == SPRITE_BLOCK_EMPTY){
          //La casella preima dello sparo è vuota
          gameArea[i] = SHUTTLE_FIRE1_BEFORE;
          gameArea[i-1] = SHUTTLE_FIRE1_AFTER;
        }else if(prev == SPRITE_ENEMY){
          //Collisione sparo-ufo
          Serial.println("Collisione");
          gameArea[i] = SPRITE_BLOCK_EMPTY;
          gameArea[i-1] = SPRITE_BLOCK_EMPTY;
          nKill++;
        }else{
          gameArea[i] = SHUTTLE_FIRE1;
        }
        break;
      case SHUTTLE_FIRE1_AFTER:
        if(gameArea[i-1] == SPRITE_ENEMY){
          //Collisione sparo-ufo
          Serial.println("Collisione2");
          gameArea[i] = SPRITE_BLOCK_EMPTY;
          gameArea[i-1] = SPRITE_BLOCK_EMPTY;
          nKill++;
        }else{
          gameArea[i] = SHUTTLE_FIRE1;
        }
        break;
      
      case SHUTTLE_FIRE1_BEFORE:
        gameArea[i] = SPRITE_BLOCK_EMPTY;
        break;
    }
  }
}

/**
 * Funzione gestione liveli
 * INPUT:
 *      -playing: variabile che fa terminare il gioco
 *      -nEnemy: possibiltà dell'uscita di un nemico
 */
void newLevel(_Bool *playing, int *nEnemy){
  if(nKill == 15 || nKill == 25){
    lcd.setCursor(0,0);
    lcd.print("Level UP!");
    level++;
    nKill++;
    delay(2000);
    nEnemy -= 3;
  }

  if(nKill >= 35){
    *playing = false;
  }
}

/**
 * Funzione sparo shuttle
 * Input:
 *      -gameArea: vettore campo di gioco
 */
void shuttleFire(char *gameArea){
  if(fireDirection == FIRE_BUTTON_RIGHT){
    gameArea[SHUTTLE_POSITION+1] = SHUTTLE_FIRE1;
  }else{
    gameArea[SHUTTLE_POSITION-1] = SHUTTLE_FIRE1;
  }
  
}

void setup(){
  lcd.begin(16, 2);
  pinMode(FIRE_BUTTON_RIGHT, INPUT);
  pinMode(FIRE_BUTTON_LEFT, INPUT);
  
  Serial.begin(9600);
  drawLCD();
}

void loop(){
  static byte currentSprite = SHUTTLE_SPRITE1;
  static unsigned int distance = 0;
  static bool playing = false;
  static bool blinking = false;
  static int nFire = 0;
  static int nEnemy = 7;
  

  pushed = digitalRead(FIRE_BUTTON_RIGHT);
  
  if (!playing){
    if(pushed == HIGH){
      buttonPushed = true;
    }
      
    if (blinking) {
      lcd.setCursor(0,0);
      if(!dead && nKill < 35){
        lcd.print("Missione 'ARE'");
      }else if(dead){
        lcd.print("Abbattuto!");
        lcd.setCursor(0, 1);
        lcd.print("Score: ");
        lcd.print((unsigned int)((nKill+distance)-nFire));
      }else if(nKill >= 35){
        lcd.print("Vittoria!Kill:");
        lcd.print(nKill);
        lcd.setCursor(0, 1);
        lcd.print("Score: ");
        lcd.print((unsigned int)((nKill+distance)-nFire));
      }
    }
    
    delay(250);
    blinking = !blinking;

    //Reset riga superiore
    for(int i = 0; i < LCD_WIDTH; i++){
      lcd.setCursor(i, 0);
      lcd.print(" ");
    }
    
    if (buttonPushed) {
      drawLCD();
      currentSprite = SHUTTLE_SPRITE1;
      playing = true;
      buttonPushed = false;
      dead = false;
      nKill = 0;
      nFire = 0;
      level = 0;
      distance = 0;
    }
    return;
  }

  if(random(0, nEnemy) == 1){
    //Aggiunta nuovo nemico
    updateArea(gameArea, SPRITE_ENEMY, random(2));
  }else{
    //Aggiunta nuovo blocco vuoto
    updateArea(gameArea, SPRITE_BLOCK_EMPTY, random(2));
  }
  
  newLevel(&playing, &nEnemy);

  if((pushed = digitalRead(FIRE_BUTTON_RIGHT)) == HIGH){
    fireDirection = FIRE_BUTTON_RIGHT;
  }else if((pushed = digitalRead(FIRE_BUTTON_LEFT)) == HIGH){
    fireDirection = FIRE_BUTTON_LEFT;
  }

  if (pushed == HIGH) {
    shuttleFire(gameArea);
    nFire++;
    buttonPushed = false;
    pushed = LOW;
  }

  if (drawShuttle(gameArea, currentSprite)) {
    //Morte giocatore, collisione shuttle-navicella
    playing = false;
    dead = true;
  }else{
    if(fireDirection == FIRE_BUTTON_RIGHT){
      if (currentSprite == SHUTTLE_SPRITE2 || currentSprite == SHUTTLE_SPRITE1_LEFT || currentSprite == SHUTTLE_SPRITE2_LEFT) {
        currentSprite = SHUTTLE_SPRITE1;
      }else{
        currentSprite++;
      }
    }else{
      if (currentSprite == SHUTTLE_SPRITE1 || currentSprite == SHUTTLE_SPRITE2 || currentSprite == SHUTTLE_SPRITE2_LEFT) {
        currentSprite = currentSprite = SHUTTLE_SPRITE1_LEFT;
      }else{
        currentSprite++;
      }
    }
    
    distance++;
  }

  //Velocità di gioco
  if(level == 0){
    delay(250);
  }else if(level == 1){
    delay(180);
  }else if(level == 2){
    delay(120);
  }
}
