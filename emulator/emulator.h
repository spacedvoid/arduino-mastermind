#pragma once

#include <string>
#include <Windows.h>
#include <conio.h>
#include <iostream>

const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

struct colorType {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	bool equals(colorType other) {
		if (this->R == other.R && this->G == other.G && this->B == other.B) return true;
		return false;
	}
	static const colorType fromRGB(unsigned char r, unsigned char g, unsigned char b) {
		if (r == 0) {
			if (g == 0) {
				if (b == 0) return { 0, 0, 0 };
				else if (b == 255) return { 0, 0, 255 };
			}
			else if (g == 255) {
				if (b == 0) return { 0, 255, 0 };
			}
		}
		else if (r == 127) return { 127, 0, 255 };
		else if (r == 255) {
			if (g == 0) {
				if (b == 0) return { 255, 0, 0 };
				else if (b == 255) return { 255, 0, 255 };
			}
			else if (g == 127 && b == 0) return { 255, 127, 0 };
			else if (g == 255) {
				if (b == 255) return { 255, 255, 255 };
				else if (b == 0) return { 255, 255, 0 };
			}
		}
		return { 0, 0, 0 };
	}
};

const colorType NONE = { 0, 0, 0 };
const colorType WHITE = { 255, 255, 255 };
const colorType RED = { 255, 0, 0 };
const colorType GREEN = { 0, 255, 0 };
const colorType BLUE = { 0, 0, 255 };
const colorType YELLOW = { 255, 255, 0 };
const colorType ORANGE = { 255, 127, 0 };
const colorType PURPLE = { 127, 0, 255 };
const colorType MAGENTA = { 255, 0, 255 };


void RGBMatrixInit(void) {}

COORD toCOORD(char point[2]) {
	return COORD{ (short)((7 - point[1]) * 2), point[0] };
}

void setColor(unsigned char r, unsigned char g, unsigned char b) {
	colorType type = colorType::fromRGB(r, g, b);
	if (type.equals(WHITE)) SetConsoleTextAttribute(hConsole, 15);
	else if(type.equals(RED)) SetConsoleTextAttribute(hConsole, 12);
	else if(type.equals(GREEN)) SetConsoleTextAttribute(hConsole, 10);
	else if (type.equals(BLUE)) SetConsoleTextAttribute(hConsole, 9);
	else if (type.equals(YELLOW)) SetConsoleTextAttribute(hConsole, 14);
	else if (type.equals(ORANGE)) SetConsoleTextAttribute(hConsole, 6);
	else if (type.equals(PURPLE)) SetConsoleTextAttribute(hConsole, 5);
	else if (type.equals(MAGENTA)) SetConsoleTextAttribute(hConsole, 13);
	else SetConsoleTextAttribute(hConsole, 8);
}

void draw_point(char point[2], unsigned char r, unsigned char g, unsigned char b) {
	SetConsoleCursorPosition(hConsole, toCOORD(point));
	setColor(r, g, b);
	std::cout << "\u2588\u2588";
}

void draw_line(char point[4], unsigned char r, unsigned char g, unsigned char b) {
	if (point[0] == point[2]) {
		if (point[1] >= point[3]) {
			for (char i = point[1]; i >= point[3]; i--) {
				char p[2] = { point[0], i };
				draw_point(p, r, g, b);
			}
		}
		else {
			for (char i = point[1]; i <= point[3]; i++) {
				char p[2] = {point[0], i};
				draw_point(p, r, g, b);
			}
		}
	}
	else if (point[1] == point[3]) {
		if (point[0] >= point[2]) {
			for (char i = point[0]; i >= point[2]; i--) {
				char p[2] = { i, point[1] };
				draw_point(p, r, g, b);
			}
		}
		else {
			for (char i = point[0]; i <= point[2]; i++) {
				char p[2] = { i, point[1] };
				draw_point(p, r, g, b);
			}
		}
	}
}

void draw_rectangle(char point[4], unsigned char r, unsigned char g, unsigned char b) {
	if (point[0] >= point[2]) {
		for (char i = point[0]; i >= point[2]; i--) {
			char l[4] = { i, point[1], i, point[3]};
			draw_line(l, r, g, b);
		}
	}
	else {
		for (char i = point[0]; i <= point[2]; i++) {
			char l[4] = { i, point[1], i, point[3] };
			draw_line(l, r, g, b);
		}
	}
}

void image(void) {}

class serial {
public:
	void begin(int freq) {}
	void write(std::string msg) {}
	void write(int num) {}
};

serial Serial;

unsigned char HIGH = 255;
unsigned char LOW = 0;

unsigned char digitalRead(uint8_t pin) {
	return 0;
}