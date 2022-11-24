#include "rgbMatrix.h"
#include <time.h>

// -- Predefinition --
namespace mastermind {
  void init();
  namespace lines {
    void setting();
  }
  namespace game {
    void start();
    void loop();
    void colorNext();
    void colorPrevious();
    byte* getAbsoluteLocation();
    void setAnswer();
    void showHint();
    void end(bool isSuccess);
  }
  namespace color {
    struct colorType {
      byte R;
      byte G;
      byte B;
      uint32_t toHex() {
        return R << 16 + G << 8 + B;
      }
    };
    colorType getColorType(int numeric);
  }
  namespace ui {
    void menu();
    void loop();
    void game();
  }
  namespace draw {
    void point(byte x, byte y, color::colorType color);
    void line(byte startX, byte startY, byte endX, byte endY, color::colorType color);
    void square(byte startX, byte startY, byte endX, byte endY, color::colorType color);
    void pointAbsolute(byte* point, color::colorType color);
  }
  namespace util {
    uint8_t waitInput();
  }
}

// -- Main --
void setup() {
  Serial.begin(9600);
  RGBMatrixInit();
}

void loop() {
  mastermind::init();
}

namespace mastermind {
  const uint8_t button1 = 48, button2 = 42, button3 = 34, button4 = 26;

  void init() {
    ui::menu();
  }

  namespace color {
    const colorType NONE = {0, 0, 0};
    const colorType WHITE = {255, 255, 255};
    const colorType RED = {255, 0, 0};
    const colorType GREEN = {0, 255, 0};
    const colorType BLUE = {0, 0, 255};
    const colorType YELLOW = {255, 255, 0};
    const colorType ORANGE = {255, 128, 0};
    const colorType PURPLE = {127, 0, 255};
    const colorType MAGENTA = {255, 0, 255};

    colorType getColorType(int numeric) {
      switch(numeric) {
        case 0: return RED;
        case 1: return YELLOW;
        case 2: return GREEN;
        case 4: return BLUE;
        case 5: return PURPLE;
        case 6: return MAGENTA;
        default: return NONE;
      }
    }
  }

  namespace lines {
    byte line = 6;

    void setting() {
      switch(util::waitInput()) {
        case button1: line = 6; break;
        //case button2: line = 7; break;
        //case button3: line = 8; break;
        case button4: break;
      }
    }
  }
  
  namespace game {
    byte current[2];  // {line, row} == {attempt, cell}
    byte color;       // 0 ~ 5
    int attempts;
    byte colors[4], answer[4];

    void start() {
      ui::game();
      attempts = lines::line;
      memset(current, 1, 2);
      color = 0;
      memset(colors, -1, 4);
      setAnswer();
      loop();
    }

    void loop() {
      while(true) {
        switch(util::waitInput()) {
          case button1: {
            if(current[1] > 1) current[1]--;
          }
          case button2: if(color > 0) color--;
          case button3: if(color < 6) color++;
          case button4: {
            if(current[1] < 4) current[1]++;
            else {
             if(current[0] < attempts) {
               for(int i = 0; i < 4; i++) {
                 if(colors[i] != answer[i]) break;
                 end(true);
                 return;
               }
               showHint();
               current[0]++;
               current[1] = 1;
              }
              else end(false);
            }
          }
        }
      }
    }

    void colorNext() {
      if(color < 6) color++;
      else return;
      draw::pointAbsolute(getAbsoluteLocation(), color::getColorType(color));
    }

    void colorPrevious() {
      if(color > 0) color--;
      else return;
      draw::pointAbsolute(getAbsoluteLocation(), color::getColorType(color));
    }

    byte* getAbsoluteLocation() {
      byte location[2] = {1, 7};
      location[0] += current[0] - 1;
      location[1] -= current[1] - 1;
      return location;
    }

    void setAnswer() {
      srand(time(NULL));
      for(int i = 0; i < 4; i++) {
        answer[i] = rand() % 4;
      }
    }

    void showHint() {
      int red = 0, white = 0;
      byte answerMark[4] = {0, 0, 0, 0};  // 0: unmarked, 1: red, 2: white  1 1 3 5 ans
      bool colorsMark[4] = {false, false, false, false};                     //5 1 4 5 colors
      for(int i = 0; i < 4; i++) {
        if(colors[i] == answer[i]) {
          red++;
          answerMark[i] = 1;
          colorsMark[i] = true;
        }
      }
      for(int i = 0; i < 4; i++) {    // colors loop
        if(colorsMark[i] == 1) continue;
        for(int j = 0; j < 4; j++) {  // answer loop
          if(answer[j] == colors[i]) {
            if(answerMark[j] != 0) continue;
            white++;
            answerMark[j] = 2;
            colorsMark[i] = true;
          }
        }
      }
      current[1]++;
      for(int i = 0; i < red; i++) {
        if(current[1] > 7) {
          Serial.write("Row out of range: ");
          Serial.write(current[1]);
          Serial.write("\n");
          return;
        }
        draw::pointAbsolute(getAbsoluteLocation(), color::RED);
        current[1]++;
      }
      for(int i = 0; i < white; i++) {
        if(current[1] > 7) {
          Serial.write("Row out of range: ");
          Serial.write(current[1]);
          Serial.write("\n");
          return;
        }
        draw::pointAbsolute(getAbsoluteLocation(), color::WHITE);
        current[1]++;
      }
    }

    void end(bool isSuccess) {
      if(isSuccess) draw::line(0, 0, 7, 0, color::GREEN);
      else draw::line(0, 0, 7, 0, color::RED);
    }
  }


  namespace ui {
    void menu() {
      draw::square(0, 0, 7, 7, color::NONE);
      while(true) {
        switch (util::waitInput()) {
          case button1: game::start(); break;
          case button2: lines::setting(); break;
        }
      }
    }

    void game() {
      draw::square(0, 1, 7, 6, color::NONE);
      draw::line(0, 7, 3, 7, color::WHITE);
      draw::line(4, 7, 7, 7, color::RED);
      draw::line(0, 0, 7, 0, color::PURPLE);
    }
  }

  namespace draw {
    // Coordinates start from bottom left (0, 0). X is to the right, Y is to the top.

    void point(byte x, byte y, color::colorType color) {
      byte point[2] = {y, x};
      draw_point(point, color.toHex());
      image();
    }

    void line(byte startX, byte startY, byte endX, byte endY, color::colorType color){
      byte line[4] = {startY, startX, endY, endX};
      draw_line(line, color.toHex());
      image();
    }

    void square(byte startX, byte startY, byte endX, byte endY, color::colorType color) {
      byte square[4] = {startY, startX, endY, endX};
      draw_rectangle(square, color.toHex());
      image();
    }

    void pointAbsolute(byte* point, color::colorType color) {
      draw_point(point, color.toHex());
      image();
    }
  }

  namespace util {
    uint8_t waitInput() {
      while(true) {
        if(digitalRead(button1) == HIGH)
          return button1;
        else if(digitalRead(button2) == HIGH)
          return button2;
        else if(digitalRead(button3) == HIGH)
          return button3;
        else if(digitalRead(button4) == HIGH)
          return button4;
      }
    }
  }
}