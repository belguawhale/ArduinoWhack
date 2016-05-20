/*  _       _   _     _   _______   ________   _     __       _______           _         _______   _______   _     _   _______
 * | |     | | | |   | | |  ___  | |  ____  | | |   / /      |  ___  |         | |       |__   __| |  ___  | | |   | | |__   __|
 * | |     | | | |   | | | |   | | | |    |_| | |  / /       | |   | |         | |          | |    | |   |_| | |   | |    | |   
 * | |  _  | | | |___| | | |___| | | |        | |_/ / _____  | |___| |  _____  | |          | |    | |       | |___| |    | |   
 * | | | | | | |  ___  | |  ___  | | |        |  _ | |_____| |  ___  | |_____| | |          | |    | |  ___  |  ___  |    | |   
 * | | | | | | | |   | | | |   | | | |     _  | | \ \        | |   | |         | |          | |    | | |_  | | |   | |    | |   
 * | |_| |_| | | |   | | | |   | | | |____| | | |  \ \       | |   | |         | |_____   __| |__  | |___| | | |   | |    | |   
 * |_________| |_|   |_| |_|   |_| |________| |_|   \_\      |_|   |_|         |_______| |_______| |_______| |_|   |_|    |_|  
 * 
 * A game written in Arduino.
 * By: belguawhale 
 */

#include "notes.h" // include pitches
//moomoomoo

// CHECK VARIABLES
int L[] = {
  3, 5, 6, 9, 10, 11
}; // init LED pins; note that leds go from 0 to 5 in program, but 1 - 6 in manual
int B[] = {
  2, 4, 7, 8, 12, 13
}; // init button pins; note that buttons go from 0 to 5 in program, but 1 - 6 in manual
int LL[] = {
  A3, A2, A1
}; // init Life LEDs; note that leds go from 0 to 2 in program, but 1 - 3 in manual
int dlay = 2; // delay for LED testing
int endcheck = 0; // exit check
int choice = 0; // button choice
int bincheck = 0; // for binary-ing
int chromatic[] = {
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4,
  NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  NOTE_C5, NOTE_B4, NOTE_AS4, NOTE_A4, NOTE_GS4, NOTE_G4,
  NOTE_FS4, NOTE_F4, NOTE_E4, NOTE_DS4, NOTE_D4, NOTE_CS4,
  NOTE_C4
}; // notes in scale

// MENU VARIABLES
int ingame = 0; // playing a game?
int difficulty = 0; // difficulty selection
int brightness[] = {
  0, 0, 0, 0, 0, 0
}; // LED brightness
int fademultiplier[] = {
  5, 5, 5, 5, 5, 5
}; // Fading stuff
unsigned long prevtime = 0; // timer for leds going on and off during menu
unsigned long time = 0; // timer

// GET READY VARIABLES
int devmode = 0; // TOP SECRET
int leveldebug[] = {
  0, 0, 0, 0, 0, 0
}; // Level selection in binary for devmode 2

// GAME VARIABLES
int lives = 4 - difficulty; // lives
int pressed[] = {
  0, 0, 0, 0, 0, 0
}; // buttons read yet?
int level = 0; // current level
int whacks = 0; // number of successful whacks
unsigned long LEDtime[] = {
  0, 0, 0, 0, 0, 0
}; // Time each LED started on
unsigned long levelttl = 0; // TTL for current level
unsigned long leveldelay = 0; // Amount of time between new led

unsigned long delaytime = 0; // time since last led on
unsigned long currenttime = 0; // current time

int antilag = 1; // prevents debug() from lagging the game

void setup() {
  // Set up pins
  for (int i = 0; i <= 5; i++) {
    pinMode(L[i], OUTPUT);
    pinMode(B[i], INPUT);
  } // init buttons and leds
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT); // buzzer and life LEDs
  Serial.begin(9600);

  while (endcheck == 0) {
    // read buttons and indicate in binary on Life LEDs
    choice = readbuttons();
    indicatechoice (choice);
    Serial.println(choice);

    switch (choice) {
      case 1: // fade two at a time from left to write
        for (int i = 0; i <= 10; i++) {
          for (int j = 0; j <= 255; j += 2) {
            analogWrite(L[i % 6], 255 - j);
            analogWrite(L[(i + 1) % 6], j);
            delay(dlay);
          }
        }
        analogWrite(L[0], 0);
        analogWrite(L[5], 0);
        break;
      case 2: // Binary from 0 to 63
        for (int i = 0; i <= 63; i++) { // convert each number from 1 to 63 into binary
          bincheck = i;
          if (bincheck >= 32) { // take out 32
            digitalWrite (L[0], HIGH);
            bincheck -= 32;
          }
          else {
            digitalWrite (L[0], LOW);
          }
          if (bincheck >= 16) { // take out 16
            digitalWrite (L[1], HIGH);
            bincheck -= 16;
          }
          else {
            digitalWrite (L[1], LOW);
          }
          if (bincheck >= 8) { // take out 8
            digitalWrite (L[2], HIGH);
            bincheck -= 8;
          }
          else {
            digitalWrite (L[2], LOW);
          }
          if (bincheck >= 4) { // take out 4
            digitalWrite (L[3], HIGH);
            bincheck -= 4;
          }
          else {
            digitalWrite (L[3], LOW);
          }
          if (bincheck >= 2) { // take out 2
            digitalWrite (L[4], HIGH);
            bincheck -= 2;
          }
          else {
            digitalWrite (L[4], LOW);
          }
          if (bincheck >= 1) { // take out 1
            digitalWrite (L[5], HIGH);
            bincheck -= 1;
          }
          else {
            digitalWrite (L[5], LOW);
          }
          delay(100);
        }
        for (int i = 0; i <= 5; i++) {
          digitalWrite (L[i], LOW);
        }
        break;
      case 3: // chromatic scale
        // iterate over the notes of the melody:
        for (int thisNote = 0; thisNote < 25; thisNote++) {
          // to calculate the note duration, take one second
          // divided by the note type.
          //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
          tone(A0, chromatic[thisNote], 1000 / 8);
          // to distinguish the notes, set a minimum time between them.
          // the note's duration + 30% seems to work well:
          int pauseBetweenNotes = 125 * 1.30;
          delay(pauseBetweenNotes);
          // stop the tone playing:
          noTone(A0);
        }
        digitalWrite(A0, LOW);
        break;
      case 4: // testing serial connection
        Serial.println("TESTING SERIAL");
        break;
      case 5: // knight-rider with tones
        for (int i = 0; i <= 8; i++) {
          tone(A0, i * 500 + 1000);
          if (i > 4) { // reversing
            digitalWrite(L[10 - i], LOW); // off on on off -> on on off off
            digitalWrite(L[8 - i], HIGH);
            digitalWrite(L[9 - i], HIGH);
          }
          else {
            digitalWrite(L[i - 1], LOW); // off on on off -> off off on on
            digitalWrite(L[i], HIGH);
            digitalWrite(L[i + 1], HIGH);
          }
          delay(250);
          noTone(A0);
        }
        digitalWrite(L[0], 0);
        digitalWrite(L[1], 0);
        break;
      case 6: // exit the checking sequence
        endcheck = 1;
        break;
    }
    delay(100); // so it doesn't go crazy
  }
}

void loop() {
  // MENU
  ingame = 0; // reset game variables
  difficulty = 0;
  level = 0;
  while (ingame == 0 || digitalRead(B[3]) == HIGH) { // while game not started or game started but user has not let go of the submit button
    // read buttons and indicate in binary on Life LEDs
    choice = readbuttons();
    Serial.println(choice);
    switch (choice) {
      case 1: // EASY
        if (difficulty != 1) {
          difficulty = 1;
          resetBrightness();
          brightness[0] = 6;
          fademultiplier[0] = 5;
          tone(A0, NOTE_C4, 100);
          delay(100);
          noTone(A0);
        }
        break;
      case 2: // MEDIUM
        if (difficulty != 2) {
          difficulty = 2;
          resetBrightness();
          brightness[1] = 6;
          fademultiplier[1] = 5;
          tone(A0, NOTE_E4, 100);
          delay(100);
          noTone(A0);
        }
        break;
      case 3: // HARD
        if (difficulty != 3) {
          difficulty = 3;
          resetBrightness();
          brightness[2] = 6;
          fademultiplier[2] = 5;
          tone(A0, NOTE_G4, 100);
          delay(100);
          noTone(A0);
        }
        break;
      case 4:
        if (difficulty != 0 && ingame != 1) { // if difficulty was selected and not yet in game
          ingame = 1;
        }
        break;
      case 5: // reset difficulty
        if (difficulty != 0) {
          difficulty = 0;
          resetBrightness();
          tone(A0, NOTE_G3, 100);
          delay(100);
          noTone(A0);
        }
        break;
      case 6: // left empty since user enters menu by pressing button 6 (B[5])
        break;
    }

    switch (difficulty) { // display difficulty and fade corresponding LED on and off (e.g. easy selected, so fade easy led)
      case 1:
        if (brightness[0] < 5 || brightness[0] > 250) {
          fademultiplier[0] *= -1;
        }
        brightness[0] += fademultiplier[0];
        digitalWrite(LL[2], HIGH);
        digitalWrite(LL[1], HIGH);
        digitalWrite(LL[0], HIGH);
        break;
      case 2:
        if (brightness[1] < 5 || brightness[1] > 250) {
          fademultiplier[1] *= -1;
        }
        brightness[1] += fademultiplier[1];
        digitalWrite(LL[2], HIGH);
        digitalWrite(LL[1], HIGH);
        digitalWrite(LL[0], LOW);
        break;
      case 3:
        if (brightness[2] < 5 || brightness[2] > 250) {
          fademultiplier[2] *= -1;
        }
        brightness[2] += fademultiplier[2];
        digitalWrite(LL[2], HIGH);
        digitalWrite(LL[1], LOW);
        digitalWrite(LL[0], LOW);
        break;
      case 0: // blink all three difficulty leds if none selected
        time = millis();
        if (prevtime + 500 < time) {
          prevtime = time;
          for (int i = 0; i <= 2; i++) {
            brightness[i] = 255 - brightness [i];
            Serial.println(brightness[i]);
          }
        }
        digitalWrite(LL[2], LOW);
        digitalWrite(LL[1], LOW);
        digitalWrite(LL[0], LOW);
        break;
    }
    if (difficulty != 0) { // blink the submit led if a difficulty was selected
      time = millis();
      if (prevtime + 500 < time) {
        prevtime = time;
        brightness[3] = 255 - brightness [3];
        Serial.println(brightness[3]);
      }
    }
    writeBrightness(); // write the brightness array to the leds
    Serial.print("Difficulty: ");
    Serial.println(difficulty);
    delay(10); // so it doesn't go crazy
  }
  resetBrightness(); // leds off

  // GET READY
  for (int i = 0; i <= 2; i++) { // countdown three times
    tone(A0, NOTE_C5, 500);
    for (int i = 0; i <= 11; i++) {
      brightness[i % 6] = 255 - brightness[i % 6]; // turn on all leds from left to right, then off from left to right
      writeBrightness();
      if (digitalRead(B[0]) == HIGH && digitalRead(B[2]) == HIGH && digitalRead(B[5]) == HIGH) { // activate piano mode
        devmode = 1; // moomoomoo
      }
      if (digitalRead(B[0]) == HIGH && digitalRead(B[3]) == HIGH && digitalRead(B[5]) == HIGH) { // activate level select
        devmode = 2; // moomoomoo
      }
      delay(100);
    }
    noTone(A0);
  }
  tone(A0, NOTE_C6, 1000); // game about to start!
  delay(1000);

  if (devmode == 1) {
    while (digitalRead(B[0]) == LOW || digitalRead(B[1]) == LOW || digitalRead(B[4]) == LOW || digitalRead(B[5]) == LOW) { // press buttons 1, 2, 5, 6 to exit
      choice = readbuttons();
      if (digitalRead(B[0]) == HIGH && digitalRead(B[5]) == HIGH) { // make a high-pitched sound that only students can hear
        tone (A0, 10000);
      }
      else if (digitalRead(B[0]) == HIGH && digitalRead(B[1]) == HIGH) { // C#
        tone (A0, NOTE_CS5);
      }
      else if (digitalRead(B[0]) == HIGH && digitalRead(B[2]) == HIGH) { // low B
        tone (A0, NOTE_B4);
      }
      else if (digitalRead(B[0]) == HIGH && digitalRead(B[3]) == HIGH) { // low A#
        tone (A0, NOTE_AS4);
      }
      else if (digitalRead(B[0]) == HIGH && digitalRead(B[4]) == HIGH) { // low A
        tone (A0, NOTE_A4);
      }
      else if (digitalRead(B[1]) == HIGH && digitalRead(B[2]) == HIGH) { // D#
        tone (A0, NOTE_DS5);
      }
      else if (digitalRead(B[1]) == HIGH && digitalRead(B[5]) == HIGH) { // high C
        tone (A0, NOTE_C6);
      }
      else if (digitalRead(B[2]) == HIGH && digitalRead(B[5]) == HIGH) { // B
        tone (A0, NOTE_B5);
      }
      else if (digitalRead(B[3]) == HIGH && digitalRead(B[4]) == HIGH) { // F#
        tone (A0, NOTE_FS5);
      }
      else if (digitalRead(B[3]) == HIGH && digitalRead(B[5]) == HIGH) { // A#
        tone (A0, NOTE_AS5);
      }
      else if (digitalRead(B[4]) == HIGH && digitalRead(B[5]) == HIGH) { // G#
        tone (A0, NOTE_GS5);
      }
      else {
        switch (choice) {
          case 1:
            tone (A0, NOTE_C5); // C
            break;
          case 2:
            tone (A0, NOTE_D5); // D
            break;
          case 3:
            tone (A0, NOTE_E5); // E
            break;
          case 4:
            tone (A0, NOTE_F5); // F
            break;
          case 5:
            tone (A0, NOTE_G5); // G
            break;
          case 6:
            tone (A0, NOTE_A5); // A
            break;
          case 0:
            noTone(A0); // turn off sound if no key is pressed
        }
      }
    }
  }
  if (devmode == 2) {
    while (digitalRead(B[0]) == LOW) { // press button 1 to confirm selection
      level = 0;
      for (int i = 1; i <= 5; i++) { // check all buttons
        if (digitalRead(B[i]) == HIGH && pressed[i] == 0) { // if button is pressed but not checked yet (toggle)
          pressed[i] = 1;
          if (leveldebug[i] == 0) { // since pow(base, exponent) screws up
            switch (5 - i) { // chose binary options if off
              case 0:
                leveldebug[i] = 1;
                break;
              case 1:
                leveldebug[i] = 2;
                break;
              case 2:
                leveldebug[i] = 4;
                break;
              case 3:
                leveldebug[i] = 8;
                break;
              case 4:
                leveldebug[i] = 16;
                break;
            }

          }
          else {
            leveldebug[i] = 0; // chose 0 if on before
          }
        }
        else if (digitalRead(B[i]) == LOW) { // if not pressed, set read to 0
          pressed[i] = 0;
        }
        level += leveldebug[i]; // change level
        Serial.print(leveldebug[i]);
        Serial.print(" ");
      }
      Serial.println("");
      displaybin(level, 255); // display selected level in binary
    }
  }
  // out of devmode, actual game
  noTone(A0); // turn off all sound from before
  // GAME
  lives = 4 - difficulty; // set lives
  for (int i = 0; i <= 5; i++) { // init LED times (0 == led not active)
    LEDtime[i] = 0;
  }
  while (lives > 0) { // New level; loop until no lives left
    whacks = 0; // init level stuff
    level++;
    levelttl = (3 - difficulty / 2) * 1000.0 / sqrt(level); // time before led dies
    leveldelay = (3 - difficulty / 2) * 1000.0 / (sqrt(level - 1) + 1); // time before new led is spawned
    Serial.print("levelttl: ");
    Serial.println(levelttl);
    Serial.print("leveldelay: ");
    Serial.println(leveldelay);
    displaylives(); // show lives
    for (int i = 0; i <= 255; i++) { // show current level fading on and off
      displaybin(level, i);
      delay(5);
    }
    for (int i = 255; i >= 0; i--) {
      displaybin(level, i);
      delay(5);
    }
    resetBrightness();
    delay(10);
    currenttime = millis(); // Set current time
    Serial.print("Current time: ");
    Serial.println(currenttime);
    delaytime = currenttime;
    newled(); // spawn first LED
    while (whacks < level + 5 && lives > 0) { // Current Level Shtuff
      Serial.print("Entering level ");
      Serial.println(level);
      updateLEDs(); // update LED brightnesses
      debug(); // since the game part REALLY needed debugging
      currenttime = millis();
      while (delaytimeup() == -1 && not leddelayover() && buttonpressed() == 0) { // update led brightness until event occurs
        currenttime = millis(); // update current time
        debug();
        updateLEDs();
        Serial.print("LEDtimes: "); // debug stuff
        for (int i = 0; i <= 5; i++) {
          Serial.print(LEDtime[i]);
          Serial.print(" ");
        }
        Serial.println("");
      }
      Serial.println("~~~~~EXITED WAIT LOOP~~~~~"); // event happened
      if (delaytimeup() != -1) { // if led ran out of time
        Serial.print("~~~~LED ");
        Serial.print(delaytimeup());
        Serial.println(" ran out of time~~~~");
        LEDtime[delaytimeup()] = 0; // disable that led
        tone(A0, NOTE_B5, 50); // error blip
        lives--; // :(
        displaylives();
      }
      if (leddelayover()) { // if time for new led
        Serial.println("~~~~SPAWNING NEW LED~~~~");
        delaytime = currenttime;
        newled();
      }
      if (buttonpressed() != 0) { // if something was pressed
        Serial.print("~~~~Buttons pressed: "); // debugging
        for (int i = 0; i <= 5; i++) {
          if (digitalRead(B[i]) == HIGH) {
            Serial.print(i);
            Serial.print(" ");
          }
        }
        Serial.println("~~~~");
        for (int i = 0; i <= 5; i++) { // note that this kind of does debouncing: check if button event, then is it still pressed, then which one is actually pressed?
          if (digitalRead(B[i]) == HIGH && pressed[i] == 0) { // if button is pressed and it was not read yet
            Serial.print("Button ");
            Serial.print(i);
            Serial.print(" was pressed");
            pressed[i] = 1; // so you can't press a button twice without letting go of it first
            if (LEDtime[i] != 0) { // led was an active one
              Serial.println(" and the LED was whacked!");
              tone(A0, NOTE_G5, 50); // whack blip
              whacks++; // :)
              LEDtime[i] = 0;
            }
            else {
              Serial.println(" but whacked nothing!"); // led was not active (pressed wrong button)
              tone(A0, NOTE_B5, 50); // error blip
              lives--; // :(
              LEDtime[i] = 0;
              displaylives();
            }
          }
        }
      }
    }
    if (whacks > level + 4) { // level complete
      for (int i = 0; i <= 5; i++) { // leds turn on from left to right, then off from left to right
        brightness[i] = 255;
        writeBrightness();
        delay(100);
      }
      for (int i = 0; i <= 5; i++) {
        brightness[i] = 0;
        writeBrightness();
        delay(100);
      }
      tone(A0, NOTE_C5, 250); // victory music :D
      delay(275);
      tone(A0, NOTE_G4, 250);
      delay(275);
      tone(A0, NOTE_C5, 250);
      delay(275);
      tone(A0, NOTE_E5, 250);
      delay(275);
      tone(A0, NOTE_G5, 1000);
      delay(1100);
    }
  }
  // DEAD
  Serial.println("~~~~~RAN OUT OF LIVES~~~~~");
  resetBrightness();
  displaybin(level - 1, 255); // show last completed level (level is current level, which user died on)
  lives = 4 - difficulty;
  displaylives(); // show difficulty
  tone(A0, NOTE_C5, 500); // defeat music :'(
  delay(550);
  tone(A0, NOTE_B4, 500);
  delay(550);
  tone(A0, NOTE_AS4, 1000);
  delay(1100);

  while (buttonpressed()) { // wait for all buttons to be released
  }
  while (not buttonpressed()) { // wait for a button to be pressed
  }
  while (buttonpressed()) { // wait for all buttons to be released
  }
  // END GAME SECTION
  resetBrightness();
  writeBrightness();
  Serial.println("bye");
  devmode = 0; // reset devmode and loop back to menu
}

int readbuttons() { // read buttons and output corresponding result. Designed so multiple buttons cannot be pressed.
  if (digitalRead(B[0]) == HIGH) {
    return 1;
  }
  else if (digitalRead(B[1]) == HIGH) {
    return 2;
  }
  else if (digitalRead(B[2]) == HIGH) {
    return 3;
  }
  else if (digitalRead(B[3]) == HIGH) {
    return 4;
  }
  else if (digitalRead(B[4]) == HIGH) {
    return 5;
  }
  else if (digitalRead(B[5]) == HIGH) {
    return 6;
  }
  else {
    return 0;
  }
}

void indicatechoice (int chosen) { // Lights the Life LEDs depending on choice
  switch (chosen) {
    case 1:
      digitalWrite (A1, HIGH);
      digitalWrite (A2, LOW);
      digitalWrite (A3, LOW);
      break;
    case 2:
      digitalWrite (A1, LOW);
      digitalWrite (A2, HIGH);
      digitalWrite (A3, LOW);
      break;
    case 3:
      digitalWrite (A1, HIGH);
      digitalWrite (A2, HIGH);
      digitalWrite (A3, LOW);
      break;
    case 4:
      digitalWrite (A1, LOW);
      digitalWrite (A2, LOW);
      digitalWrite (A3, HIGH);
      break;
    case 5:
      digitalWrite (A1, HIGH);
      digitalWrite (A2, LOW);
      digitalWrite (A3, HIGH);
      break;
    case 6:
      digitalWrite (A1, LOW);
      digitalWrite (A2, LOW);
      digitalWrite (A3, LOW);
      break;
    default:
      digitalWrite (A1, HIGH);
      digitalWrite (A2, HIGH);
      digitalWrite (A3, HIGH);
      break;
  }
}

void resetBrightness() { // sets all brightnesses to 0 but DOES NOT write them to the leds
  for (int i = 0; i <= 5; i++) {
    brightness[i] = 0;
  }
}

void writeBrightness() { // update brightnesses based on the array
  for (int i = 0; i <= 5; i++) {
    analogWrite(L[i], brightness[i]);
  }
}

// Game functions (a lot :O)

boolean buttonpressed() { // is a button pressed?
  for (int i = 0; i <= 5; i++) {
    if (digitalRead(B[i]) == HIGH) {
      return true; // program exits if true, so it won't reach the "return false;"
    }
    if (digitalRead(B[i]) == LOW && pressed[i] == 1) { // if it is depressed, we don't have to worry about it HIGH being read twice
      pressed[i] = 0;
    }
  }
  return false; // nothing was pressed
}

void newled() { // Spawn new LED
  int unusedleds = 0; // how many leds are not used yet
  int randomled = 0; // randomly chosen LED
  tone(A0, NOTE_C5, 50); // spawn blip
  for (int i = 0; i <= 5; i++) {
    if (LEDtime[i] == 0) {
      unusedleds++; // count how many leds are not taken yet
    }
  }
  if (unusedleds != 0) { // if all leds have timers, no need to spawn a new one
    randomled = random(1, unusedleds + 1); // choose a random led out of the unused ones
    for (int i = 0; i <= 5; i++) {
      if (LEDtime[i] == 0) {
        randomled--; // choose the randomled-th place in unused leds (if it is 3, chose the third unused led)
      }
      if (randomled == 0) {
        LEDtime[i] = currenttime; // led is active
        Serial.print("Chose LED "); // debugging
        Serial.println(i);
        Serial.print("Set LEDtime of ");
        Serial.print(i);
        Serial.print(" to ");
        Serial.println(currenttime);
      }
    }
  }
}

int delaytimeup() { // led timed out?
  for (int i = 0; i <= 5; i++) {
    if (LEDtime[i] != 0) {
      if (currenttime - LEDtime[i] > levelttl) { // is an led expired?
        return i; // which led?
      }
    }
  }
  return -1; // nope
}

boolean leddelayover() { // time for new led?
  if (currenttime - delaytime > leveldelay) { // yes
    return true;
  }
  else { // no
    return false;
  }
}

void updateLEDs() { // update LED brightness based on the remaining time
  for (int i = 0; i <= 5; i++) {
    if (LEDtime[i] == 0) {
      brightness[i] = 0; // if led is inactive, don't turn it on
    }
    else {
      brightness[i] = map (currenttime - LEDtime[i], 0, levelttl, 255, 0); // map time elapsed inversely to brightness, so as time elapsed goes higher, brightness decreases
    }
  }
  writeBrightness(); // update brightness
}

void displaybin(int binary, int brightness) { // convert decimal to binary and displays it according to brightness
  int bin = binary; // for binary-ing
  if (bin >= 32) { // same as before
    analogWrite (L[0], brightness);
    bin -= 32;
  }
  else {
    analogWrite (L[0], LOW);
  }
  if (bin >= 16) {
    analogWrite (L[1], brightness);
    bin -= 16;
  }
  else {
    analogWrite (L[1], LOW);
  }
  if (bin >= 8) {
    analogWrite (L[2], brightness);
    bin -= 8;
  }
  else {
    analogWrite (L[2], LOW);
  }
  if (bin >= 4) {
    analogWrite (L[3], brightness);
    bin -= 4;
  }
  else {
    analogWrite (L[3], LOW);
  }
  if (bin >= 2) {
    analogWrite (L[4], brightness);
    bin -= 2;
  }
  else {
    analogWrite (L[4], LOW);
  }
  if (bin >= 1) {
    analogWrite (L[5], brightness);
    bin -= 1;
  }
  else {
    analogWrite (L[5], LOW);
  }
}

void displaylives() { // show lives
  for (int i; i <= 2; i++) { // turn all Life LEDs off
    digitalWrite(LL[i], LOW);
  }
  if (lives >= 1) { // at least 1?
    digitalWrite(LL[2], HIGH);
  }
  if (lives >= 2) { // at least 2?
    digitalWrite(LL[1], HIGH);
  }
  if (lives >= 3) { // at least 3?
    digitalWrite(LL[0], HIGH);
  }
}

void debug() { // giant debug function that spews out a load of data
  if (antilag == 0) { // toggles neutralization of your magical hammer of human flesh (your finger) by the evil power of lag
    Serial.print("Lives: ");
    Serial.println(lives);
    Serial.print("Whacks: ");
    Serial.println(whacks);
    Serial.print("delaytimeup: ");
    Serial.println(delaytimeup());
    Serial.print("leddelayover: ");
    Serial.println(leddelayover());
    Serial.print("buttonpressed: ");
    Serial.println(buttonpressed());
    Serial.print("currenttime: ");
    Serial.println(currenttime);
    Serial.print("delaytime: ");
    Serial.println(delaytime);
    Serial.print("Buttons pressed: ");
    for (int i = 0; i <= 5; i++) { // displays readings of the buttons
      if (digitalRead(B[i]) == HIGH) {
        Serial.print(i);
        Serial.print(" ");
      }
    }
    Serial.println("");
    Serial.print("Brightnesses: ");
    for (int i = 0; i <= 5; i++) {
      Serial.print(brightness[i]);
      Serial.print(" ");
    }
    Serial.println("");
  } // this should be self-explanatory
}
