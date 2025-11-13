#include <Wire.h>
#include <U8g2lib.h>
#include <LiquidCrystal.h>

// === Display Setup ===
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// === Pin Configuration ===
const int buzzerPin      = 8;
const int happyButton    = A1;
const int sadButton      = A2;
const int calmButton     = A3;
const int soundSensorPin = A0;
const int pirPin         = 9;   // PIR motion sensor output pin

// === Motion + Timer Variables ===
int pirState = LOW;                 // Keeps track of previous PIR state
unsigned long lastGreetTime = 0;    // Last time the bot greeted you
const unsigned long greetCooldown = 900000;   // 15 min (in ms) (this is for water reminder)
unsigned long lastStretchTime = 0;
const unsigned long stretchInterval = 1800000UL; // 30 min (in ms)

// === Sound Detection ===
int loudLevel = 0;                   // Latest sound sensor reading
const int loudThreshold = 600;       // Adjust based on sensor sensitivity (600–800 typical)
unsigned long lastLoudTime = 0;      // Last time sound was loud
const unsigned long loudCooldown = 0; // Cooldown between loud warnings (ms)

// === Function Prototypes ===
void reactHappy();
void reactSad();
void reactCalm();
void drawIdleFace();
void drawHappyFace();
void drawSadFace();
void drawCalmFace();
void wakeUpBuddy();
void showStretchReminder();
void tooLoudResponse();

void setup() {
  u8g2.begin();
  lcd.begin(16, 2);

  pinMode(buzzerPin, OUTPUT);
  pinMode(happyButton, INPUT_PULLUP);
  pinMode(sadButton, INPUT_PULLUP);
  pinMode(calmButton, INPUT_PULLUP);
  pinMode(soundSensorPin, INPUT);
  pinMode(pirPin, INPUT);
  randomSeed(analogRead(0));

  lastStretchTime = millis();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mood Buddy Ready");
  lcd.setCursor(0, 1);
  lcd.print("Press a button!");
  delay(1500);
  lcd.clear();
}

void loop() {
  int motion = digitalRead(pirPin);
  unsigned long now = millis();  // Central time reference

  // === Sound Detection ===
  loudLevel = analogRead(soundSensorPin);
  if (loudLevel > loudThreshold && (now - lastLoudTime > loudCooldown)) {
    lastLoudTime = now;
    tooLoudResponse();
  }

  // === Motion Greeting ===
  if (motion == HIGH && (now - lastGreetTime > greetCooldown)) {
    lastGreetTime = now;
    wakeUpBuddy(); // Friendly greeting
  }

  // === Stretch Reminder ===
  if (now - lastStretchTime >= stretchInterval) {
    showStretchReminder();
    lastStretchTime = now;
  }

  // === Button + Sound-Based Mood Reactions ===
  int soundLevel = analogRead(soundSensorPin);
  if (soundLevel > loudThreshold) {
    reactCalm();
    delay(1000);
  } 
  else if (digitalRead(happyButton) == LOW) {
    reactHappy();
  } 
  else if (digitalRead(sadButton) == LOW) {
    reactSad();
  } 
  else if (digitalRead(calmButton) == LOW) {
    reactCalm();
  } 
  else {
    drawIdleFace();
  }

  delay(200);
}

// ====================================================
//                    FACE DRAWING
// ====================================================

void drawIdleFace() {
  u8g2.clearBuffer();

  static int eyeOffsetX = 0, eyeOffsetY = 0;
  static int dirX = 1, dirY = 1;
  static unsigned long lastBlink = 0;
  static bool eyesOpen = true;

  // Make eyes wander around slightly
  eyeOffsetX += dirX;
  eyeOffsetY += dirY;
  if (eyeOffsetX > 5 || eyeOffsetX < -5) dirX *= -1;
  if (eyeOffsetY > 2 || eyeOffsetY < -2) dirY *= -1;

  // Blink every few seconds
  if (millis() - lastBlink > 3500) {
    eyesOpen = !eyesOpen;
    lastBlink = millis();
  }

  // Eyebrows
  u8g2.drawLine(30, 18, 50, 18);
  u8g2.drawLine(78, 18, 98, 18);

  // Eyes
  if (eyesOpen) {
    u8g2.drawCircle(40 + eyeOffsetX, 32 + eyeOffsetY, 6);
    u8g2.drawCircle(88 + eyeOffsetX, 32 + eyeOffsetY, 6);
    u8g2.drawDisc(40 + eyeOffsetX, 32 + eyeOffsetY, 2);
    u8g2.drawDisc(88 + eyeOffsetX, 32 + eyeOffsetY, 2);
  } else {
    u8g2.drawLine(34 + eyeOffsetX, 32 + eyeOffsetY, 46 + eyeOffsetX, 32 + eyeOffsetY);
    u8g2.drawLine(82 + eyeOffsetX, 32 + eyeOffsetY, 94 + eyeOffsetX, 32 + eyeOffsetY);
  }

  // Smile
  u8g2.drawArc(64, 55, 15, 100, 160);
  u8g2.sendBuffer();
}

void drawHappyFace() {
  u8g2.clearBuffer();
  u8g2.drawLine(30, 17, 50, 15);
  u8g2.drawLine(78, 15, 98, 17);
  u8g2.drawCircle(40, 28, 7);
  u8g2.drawCircle(88, 28, 7);
  u8g2.drawDisc(40, 28, 2);
  u8g2.drawDisc(88, 28, 2);
  u8g2.drawArc(64, 55, 6, 100, 285);
  u8g2.sendBuffer();
}

void drawSadFace() {
  u8g2.clearBuffer();
  u8g2.drawLine(30, 20, 50, 18);
  u8g2.drawLine(78, 18, 98, 20);
  u8g2.drawCircle(40, 32, 6);
  u8g2.drawCircle(88, 32, 6);
  u8g2.drawDisc(40, 32, 2);
  u8g2.drawDisc(88, 32, 2);
  u8g2.drawArc(64, 52, 14, 0, 160); // Gentle smile, not frown
  u8g2.sendBuffer();
}

void drawCalmFace() {
  u8g2.clearBuffer();
  u8g2.drawLine(30, 20, 50, 20);
  u8g2.drawLine(78, 20, 98, 20);
  u8g2.drawLine(34, 30, 46, 30);
  u8g2.drawLine(82, 30, 94, 30);
  u8g2.drawArc(64, 50, 12, 100, 300);
  u8g2.sendBuffer();
}

// ====================================================
//                     REACTIONS
// ====================================================

void reactHappy() {
  drawHappyFace();
  int melody[] = {659, 784, 988, 784, 988, 1046};
  int beat[]   = {150, 150, 200, 150, 150, 250};

  for (int i = 0; i < 6; i++) {
    tone(buzzerPin, melody[i], beat[i]);
    delay(beat[i] * 1.2);
  }
  noTone(buzzerPin);

  const char* happyMsgs[][2] = {
    {"Hi Prabhav! :)", "You rock today!"},
    {"Keep shining *", "You got this!"},
    {"Smile wide ^_^", "Spread joy!"},
    {"Life is good!", "Celebrate YOU!"},
    {"Keep glowing!", "Stay awesome!"}
  };

  for (int i = 0; i < 5; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(happyMsgs[i][0]);
    lcd.setCursor(0, 1);
    lcd.print(happyMsgs[i][1]);
    delay(2500);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("~ Mood Buddy ~");
  lcd.setCursor(0, 1);
  lcd.print("Stay happy :)");
  delay(3000);
}

void reactSad() {
  drawSadFace();
  int melody[] = {440, 392, 349, 330, 294};
  int beat[]   = {350, 350, 400, 400, 600};

  for (int i = 0; i < 5; i++) {
    tone(buzzerPin, melody[i], beat[i]);
    delay(beat[i] * 1.1);
  }
  noTone(buzzerPin);

  const char* sadMsgs[][2] = {
    {"Hey Prabhav <3", "You matter!"},
    {"It's okay :)", "Take it slow"},
    {"You are loved", "Always enough"},
    {"Your light *", "Still shines"},
    {"You'll be ok", "Breathe easy"}
  };

  for (int i = 0; i < 5; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(sadMsgs[i][0]);
    lcd.setCursor(0, 1);
    lcd.print(sadMsgs[i][1]);
    delay(2500);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("~ Mood Buddy ~");
  lcd.setCursor(0, 1);
  lcd.print("You matter <3");
  delay(3000);
}

void reactCalm() {
  drawCalmFace();
  int calmNotes[] = {262, 294, 330, 294, 262};
  int calmDurations[] = {500, 600, 700, 600, 500};

  for (int i = 0; i < 5; i++) {
    tone(buzzerPin, calmNotes[i], calmDurations[i]);
    delay(calmDurations[i] * 1.3);
  }
  noTone(buzzerPin);

  const char* calmMsgs[][2] = {
    {"Breathe in...", "Hold for 2..."},
    {"Breathe out...", "Let go..."},
    {"Stay calm *", "Feel peace"},
    {"You got this", "All is okay"},
    {"Rest easy", "You're safe"}
  };

  for (int i = 0; i < 5; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(calmMsgs[i][0]);
    lcd.setCursor(0, 1);
    lcd.print(calmMsgs[i][1]);
    delay(3000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("~ Mood Buddy ~");
  lcd.setCursor(0, 1);
  lcd.print("Breathe easy");
  delay(3000);
}

// ====================================================
//              SPECIAL AUTOMATED EVENTS
// ====================================================

void wakeUpBuddy() {
  drawHappyFace();

  int tune[] = {523, 659, 784, 1046};
  int timeDur[] = {200, 200, 250, 400};

  for (int i = 0; i < 4; i++) {
    tone(buzzerPin, tune[i], timeDur[i]);
    delay(timeDur[i] * 1.3);
  }
  noTone(buzzerPin);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Heyyy :)");
  lcd.setCursor(0, 1);
  lcd.print("Pls Hydrate!!");
  delay(8000);
  lcd.clear();

  // Alternate between happy and idle faces
  for (int i = 0; i < 3; i++) {
    drawHappyFace();
    delay(500);
    drawIdleFace();
    delay(500);
  }
}

void showStretchReminder() {
  drawIdleFace();

  // Gentle chime reminder
  tone(buzzerPin, 523, 150); delay(180);
  tone(buzzerPin, 659, 150); delay(180);
  noTone(buzzerPin);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time to stretch");
  lcd.setCursor(0, 1);
  lcd.print("Take 1 min break");
  delay(4000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("~ Mood Buddy ~");
  lcd.setCursor(0, 1);
  lcd.print("You got this!");
  delay(2000);
}

void tooLoudResponse() {
  drawCalmFace();

  tone(buzzerPin, 440, 200);
  delay(250);
  tone(buzzerPin, 392, 300);
  noTone(buzzerPin);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Too loud, friend");
  lcd.setCursor(0, 1);
  lcd.print("Take deep breaths");
  delay(4000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("~ Mood Buddy ~");
  lcd.setCursor(0, 1);
  lcd.print("All is calm now");
  delay(3000);
}
