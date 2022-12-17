#include "emulator.h"
#include <time.h>
#include <cstdint>
#include <memory.h>
#include <random>

// -- Predefinition --
namespace mastermind {
    const uint8_t button1 = 48, button2 = 42, button3 = 34, button4 = 26;
    void init();
    namespace lines {
        void setting();
    }
    namespace game {
        void start();
        void loop();
        void colorNext();
        void colorPrevious();
        char* getAbsoluteLocation();
        void setAnswer();
        void showHint();
        void end(bool isSuccess);
    }
    namespace color {
        struct colorType {
            unsigned char R;
            unsigned char G;
            unsigned char B;
        };
        colorType getColorType(int numeric);
    }
    namespace ui {
        void menu();
        void loop();
        void game();
    }
    namespace draw {
        void point(char x, char y, color::colorType color);
        void line(char startX, char startY, char endX, char endY, color::colorType color);
        void square(char startX, char startY, char endX, char endY, color::colorType color);
        void pointAbsolute(char* point, color::colorType color);
    }
    namespace util {
        uint8_t waitInput();
    }
}

// -- Main --
void setup() {
    Serial.begin(9600);
    RGBMatrixInit();
    mastermind::init();
}

void loop() {
    while (true) {
        switch (mastermind::util::waitInput()) {
        case mastermind::button1: mastermind::game::start(); break;
        case mastermind::button2: mastermind::lines::setting(); break;
        }
    }
}

namespace mastermind {
    void init() {
        ui::menu();
    }

    namespace color {
        const colorType NONE = { 0, 0, 0 };
        const colorType WHITE = { 255, 255, 255 };
        const colorType RED = { 255, 0, 0 };
        const colorType GREEN = { 0, 255, 0 };
        const colorType BLUE = { 0, 0, 255 };
        const colorType YELLOW = { 255, 255, 0 };
        const colorType ORANGE = { 255, 127, 0 };
        const colorType PURPLE = { 127, 0, 255 };
        const colorType MAGENTA = { 255, 0, 255 };

        colorType getColorType(int numeric) {
            switch (numeric) {
            case 0: return RED;
            case 1: return ORANGE;
            case 2: return YELLOW;
            case 3: return GREEN;
            case 4: return BLUE;
            case 5: return PURPLE;
            case 6: return MAGENTA;
            default: return NONE;
            }
        }
    }

    namespace lines {
        char line = 6;

        void setting() {
            switch (util::waitInput()) {
            case button1: line = 6; break;
                //case button2: line = 7; break;
                //case button3: line = 8; break;
            case button4: break;
            }
        }
    }

    namespace game {
        char current[2];  // {line, row} == {attempt(1~line), cell(0~3)}
        char current_color;       // 0 ~ 5
        int attempts;
        char colors[4], answer[4];

        void start() {
            ui::game();
            attempts = lines::line;
            current[0] = 1, current[1] = 0;
            current_color = 0;
            memset(colors, -1, 4);
            setAnswer();
            loop();
        }

        void loop() {
            draw::pointAbsolute(getAbsoluteLocation(), color::getColorType(current_color));
            while (true) {
                switch (util::waitInput()) {
                case button1: {
                    if (current[1] > 0) {
                        current[1]--;
                        current_color = colors[current[1]];
                        draw::pointAbsolute(getAbsoluteLocation(), color::getColorType(current_color));
                    }
                    break;
                }
                case button2: colorPrevious(); break;
                case button3: colorNext(); break;
                case button4: {
                    colors[current[1]] = current_color;
                    if (current[1] < 3) {
                        current[1]++;
                        current_color = 0;
                        draw::pointAbsolute(getAbsoluteLocation(), color::getColorType(current_color));
                    }
                    else {
                        showHint();
                        for (int i = 0; i < 4; i++) {
                            if (colors[i] != answer[i]) break;
                            if (i == 3) {
                                end(true);
                                return;
                            }
                        }
                        current[0]++;
                        if (current[0] > attempts) {
                            end(false);
                            return;
                        }
                        current[1] = 0;
                        current_color = 0;
                        memset(colors, -1, 4);
                        draw::pointAbsolute(getAbsoluteLocation(), color::getColorType(current_color));
                    }
                    break;
                }
                }
            }
        }

        void colorNext() {
            if (current_color < 6) current_color++;
            else return;
            draw::pointAbsolute(getAbsoluteLocation(), color::getColorType(current_color));
        }

        void colorPrevious() {
            if (current_color > 0) current_color--;
            else return;
            draw::pointAbsolute(getAbsoluteLocation(), color::getColorType(current_color));
        }

        char* getAbsoluteLocation() {
            char* location = new char[2] {1, 7};
            location[1] -= current[1];
            location[0] += current[0] - 1;
            return location;
        }

#pragma warning(push)
#pragma warning(disable:4244)
        void setAnswer() {
            srand(time(NULL));
            for (int i = 0; i < 4; i++) {
                answer[i] = rand() % 7;
            }
        }
#pragma warning(pop)

        void showHint() {
            int red = 0, white = 0;
            char answerMark[4] = { 0, 0, 0, 0 };    // red = 1, white = 2
            bool colorsMark[4] = { false, false, false, false };
            for (int i = 0; i < 4; i++) {
                if (colors[i] == answer[i]) {
                    red++;
                    answerMark[i] = 1;
                    colorsMark[i] = true;
                }
            }
            for (int i = 0; i < 4; i++) {       // colors loop
                if (colorsMark[i]) continue;
                for (int j = 0; j < 4; j++) {   // answer loop
                    if (answer[j] == colors[i]) {
                        if (answerMark[j] != 0) continue;
                        white++;
                        answerMark[j] = 2;
                        colorsMark[i] = true;
                        break;
                    }
                }
            }
            char location[2] = { 1, 3 };
            location[0] += current[0] - 1;
            for (int i = 0; i < red; i++) {
                draw::pointAbsolute(location , color::RED);
                location[1]--;
            }
            for (int i = 0; i < white; i++) {
                draw::pointAbsolute(location, color::WHITE);
                location[1]--;
            }
        }

        void end(bool isSuccess) {
            if (isSuccess) draw::line(7, 7, 7, 0, color::GREEN);
            else draw::line(7, 7, 7, 0, color::RED);
        }
    }


    namespace ui {
        void menu() {
            draw::square(0, 0, 7, 7, color::NONE);
        }

        void game() {
            draw::square(0, 0, 7, 7, color::NONE);
            draw::square(1, 0, 6, 7, color::NONE);
            draw::line(0, 4, 0, 7, color::WHITE);
            draw::line(0, 0, 0, 3, color::RED);
            draw::line(7, 7, 7, 0, color::PURPLE);
        }
    }

    namespace draw {
        void point(char x, char y, color::colorType color) {
            char point[2] = { x, y };
            draw_point(point, color.R, color.G, color.B);
            image();
        }

        void line(char startX, char startY, char endX, char endY, color::colorType color) {
            char line[4] = { startX, startY, endX, endY };
            draw_line(line, color.R, color.G, color.B);
            image();
        }

        void square(char startX, char startY, char endX, char endY, color::colorType color) {
            char square[4] = { startX, startY, endX, endY };
            draw_rectangle(square, color.R, color.G, color.B);
            image();
        }

        void pointAbsolute(char* point, color::colorType color) {
            draw_point(point, color.R, color.G, color.B);
            image();
        }
    }

    namespace util {
        uint8_t waitInput() {
            while (true) {
                char c = _getch();
                if (c == '1') return button1;
                else if (c == '2') return button2;
                else if (c == '3') return button3;
                else if (c == '4') return button4;
                /*if (digitalRead(button1) == HIGH)
                    return button1;
                else if (digitalRead(button2) == HIGH)
                    return button2;
                else if (digitalRead(button3) == HIGH)
                    return button3;
                else if (digitalRead(button4) == HIGH)
                    return button4;*/
            }
        }
    }
}

int main() {
    setup();
    while (true) loop();
    return 0;
}