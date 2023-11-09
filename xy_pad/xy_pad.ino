#include <Elegoo_GFX.h> // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <MIDI.h>

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900
#define TS_MINY 70
#define TS_MAXY 920

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4 // optional
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define WHITE   0xFFFF

MIDI_CREATE_DEFAULT_INSTANCE();
int midix, midiy, midiz;
int midiCCX, midiCCY, midiCCZ;
int toggle, ccValue;

void handleNoteOn(byte channel, byte pitch, byte velocity) {
  MIDI.sendNoteOn(channel, pitch, velocity);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  MIDI.sendNoteOff(channel, pitch, velocity);
}

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Start"));
  tft.reset();
  uint16_t identifier = tft.readID();
  if (identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if (identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if (identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  } else if (identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if (identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if (identifier == 0x0101) {
    identifier = 0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    identifier = 0x9341;
  }
  
  midiCCX = 43; 
  midiCCY = 44; 
  midiCCZ = 45;
  // 1 modulation
  // 2 breath controller
  // 4 foot pedal
  // 5 portamento time
  // 7 volume
  // 11 expression
  // 71 resonance
  // 74 cutoff
  // 84 portamento amount
  // 91 reverb
  // 94 detune
  // 24 Korg Minilogue LFO rate
  // 26 Korg Minilogue LFO depth
  // 27 Korg Minilogue voice depth
  // 29 Korg Minilogue delay hi-pass
  // 30 Korg Minilogue delay time
  // 31 Korg Minilogue delay feedback
  // 33 Korg Minilogue noise level
  // 34 Korg Minilogue VCO1 pitch
  // 35 Korg Minilogue VCO2 pitch
  // 36 Korg Minilogue VCO1 shape
  // 37 Korg Minilogue VCO2 shape
  // 39 Korg Minilogue VCO1 level
  // 40 Korg Minilogue VCO2 level
  // 41 Korg Minilogue cross mod depth
  // 42 Korg Minilogue pitch EG intensity
  // 43 Korg Minilogue filter cutoff
  // 44 Korg Minilogue filter resonance
  // 45 Korg Minilogue filter EG intensity

  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  tft.drawRect(0, 0, 240, 240, WHITE); // 240 x 320
  tft.setTextColor(WHITE, WHITE);
  tft.setTextSize(1);
  tft.setCursor(250, 10);
  tft.print("X: ");
  tft.setCursor(250, 30);
  tft.print("CC#");
  tft.print(midiCCX);
  tft.setCursor(250, 90);
  tft.print("Y: ");
  tft.setCursor(250, 110);
  tft.print("CC#");
  tft.print(midiCCY);
  tft.setCursor(250, 170);
  tft.print("Z: ");
  tft.setCursor(250, 190);
  tft.print("CC#");
  tft.print(midiCCZ);

  pinMode(13, OUTPUT);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if ( p.z > 0 && p.z < 1024 ) {
    p.x = ( tft.height() - map(p.x, TS_MINX, TS_MAXX, tft.height(), 0) );
    p.y = ( tft.width() - map(p.y, TS_MINY, TS_MAXY, tft.width(), 0) );
    p.z = ( 1024 - p.z );
    
    if ( p.y > 0 && p.y < 240 && p.x > 0 && p.x < 240 ) {
      if ( toggle != 0 ) {
        toggle = 0;
        tft.setTextColor(WHITE, WHITE);
        tft.fillRect(245, 25, 45, 20, BLACK);
        tft.setCursor(250, 30);
        tft.print("CC#");
        tft.print(midiCCX);        
        tft.setTextColor(WHITE, WHITE);
        tft.fillRect(245, 105, 45, 20, BLACK);
        tft.setCursor(250, 110);
        tft.print("CC#");
        tft.print(midiCCY);
        tft.setTextColor(WHITE, WHITE);
        tft.fillRect(245, 185, 45, 20, BLACK);
        tft.setCursor(250, 190);
        tft.print("CC#");
        tft.print(midiCCZ);
      }
      // tft.drawFastHLine(1, p.x, 238, randomColor);
      // tft.drawFastVLine(p.y, 1, 238, randomColor);
      midix = map(p.y, 240, 0, 127, 0);
      midiy = map(p.x, 240, 0, 0, 127);
      midiz = map(p.z, 1023, 0, 127, 0);
      tft.fillRect(250, 10, 70, 10, BLACK);
      tft.setCursor(250, 10);
      tft.print("X: ");
      tft.print(midix);
      tft.fillRect(250, 90, 70, 10, BLACK);
      tft.setCursor(250, 90);
      tft.print("Y: ");
      tft.print(midiy);
      tft.fillRect(250, 170, 70, 10, BLACK);
      tft.setCursor(250, 170);
      tft.print("Z: ");
      tft.print(midiz);
      MIDI.sendControlChange(midiCCX, midix, 1);
      MIDI.sendControlChange(midiCCY, midiy, 1);
      MIDI.sendControlChange(midiCCZ, midiz, 1);
    } else if ( p.x > 0 && p.x < 240 && p.y > 240 && p.y < 320 ) {
      if ( p.x > 10 && p.x < 230 && p.y > 290 && p.y < 320 ) {
        ccValue = map(p.x, 230, 10, 127, 0);
        if ( toggle == 1 && ccValue != midiCCY && ccValue != midiCCZ ) {
          midiCCX = ccValue;
        } else if ( toggle == 2 && ccValue != midiCCX && ccValue != midiCCZ ) {
          midiCCY = ccValue;
        } else if ( toggle == 3 && ccValue != midiCCX && ccValue != midiCCY ) {
          midiCCZ = ccValue;
        }
      }
      if ( p.x > 25 && p.x < 45 && p.y > 245 && p.y < 285 ) {
        toggle = 1;
      } else if ( p.x > 105 && p.x < 125 && p.y > 245 && p.y < 285 ) {
        toggle = 2;
      } else if ( p.x > 185 && p.x < 205 && p.y > 245 && p.y < 285 ) {
        toggle = 3;
      }
      if ( toggle == 1 ) {
        tft.setTextColor(BLACK, BLACK);
        tft.fillRect(245, 25, 45, 20, WHITE);
        tft.setCursor(250, 30);
        tft.print("CC#");
        tft.print(midiCCX);        
        tft.setTextColor(WHITE, WHITE);
        tft.fillRect(245, 105, 45, 20, BLACK);
        tft.setCursor(250, 110);
        tft.print("CC#");
        tft.print(midiCCY);
        tft.setTextColor(WHITE, WHITE);
        tft.fillRect(245, 185, 45, 20, BLACK);
        tft.setCursor(250, 190);
        tft.print("CC#");
        tft.print(midiCCZ);
      } else if ( toggle == 2 ) {
        tft.setTextColor(WHITE, WHITE);
        tft.fillRect(245, 25, 45, 20, BLACK);
        tft.setCursor(250, 30);
        tft.print("CC#");
        tft.print(midiCCX);        
        tft.setTextColor(BLACK, BLACK);
        tft.fillRect(245, 105, 45, 20, WHITE);
        tft.setCursor(250, 110);
        tft.print("CC#");
        tft.print(midiCCY);
        tft.setTextColor(WHITE, WHITE);
        tft.fillRect(245, 185, 45, 20, BLACK);
        tft.setCursor(250, 190);
        tft.print("CC#");
        tft.print(midiCCZ);
      } else if ( toggle == 3 ) {
        tft.setTextColor(WHITE, WHITE);
        tft.fillRect(245, 25, 45, 20, BLACK);
        tft.setCursor(250, 30);
        tft.print("CC#");
        tft.print(midiCCX);        
        tft.setTextColor(WHITE, WHITE);
        tft.fillRect(245, 105, 45, 20, BLACK);
        tft.setCursor(250, 110);
        tft.print("CC#");
        tft.print(midiCCY);
        tft.setTextColor(BLACK, BLACK);
        tft.fillRect(245, 185, 45, 20, WHITE);
        tft.setCursor(250, 190);
        tft.print("CC#");
        tft.print(midiCCZ);
      }
    }
  }
  
  MIDI.read();
}
