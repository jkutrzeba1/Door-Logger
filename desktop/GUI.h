#pragma once

#include<windows.h>
#include<string>
#include<vector>
#include "windowsx.h"
#include <uxtheme.h>
#pragma comment (lib, "uxtheme.lib")
#define ID_BTN_1 1
#define ID_BTN_2 4
#define ID_EDIT 2
#define ID_EDIT_2 3

using namespace std;

class Button {
public:
	Button(string label, int x, int y, int width, int height, void (*callback)(), int id) : label(label), x(x), y(y), width(width), height(height), callback(callback), id(id) {}

	int id;
	void (*callback)();
	int x, y, width, height;
	string label;

	HWND hwnd;
};

class TextArea {
public:
	TextArea(string label, int x, int y, int width, int height, int id) : label(label), x(x), y(y), width(width), height(height), readonly(false), multiline(false), number(false), id(id) {}

	string label;
private:
	string buf;
public:
	int x;
	int y;
	int width;
	int height;
	int id;

	bool readonly;
	bool multiline;
	bool number;

	HWND hwnd;

	string GetText() {

		int len = GetWindowTextLengthA(this->hwnd) + 1;

		char* buf_ = new char[len];
		GetWindowTextA(this->hwnd, buf_, len);

		this->buf = buf_;

		delete[] buf_;


		return this->buf;
	}

	int GetNumber() {
		
		string txt = this->GetText();
		int pow = 1;
		int out = 0;
		for (int i = txt.size() - 1; i >= 0; i--) {

			int x = (int)txt[i] - (int)'0';

			out += (x*pow);
			pow *= 10;

		}

		return out;
	}

	void SetText(string text) {

		int ok = SetWindowTextA(this->hwnd, text.c_str());
		if (!ok) {
			int err = GetLastError();

			return;
		}

		//SendMessage(this->hwnd, WM_SETTEXT, 0, (LPARAM)text.c_str());

		this->buf = text;

	}



};



class GUI {
public:
	GUI() : next_id(1), cb_lbtndown(NULL), cb_lbtnup(NULL), cb_paint(NULL), cb_mousemove(NULL), cb_mousewheel(NULL){}
	void start(HINSTANCE &hInstance);
	void loop();
	void NewButton(string label, int x, int y, int width, int height, void (*callback)());
	TextArea* NewTextArea(string label, int x, int y, int width, int height);
	vector<Button*> buttons;
	vector<TextArea*> textareas;
	int next_id;
	void (*cb_lbtndown)(int x, int y);
	void (*cb_lbtnup)(int x, int y);
	void (*cb_paint)(HDC &hdc);
	void (*cb_keydown)(int key);
	void (*cb_mousemove)(int x, int y);
	void (*cb_mousewheel)(int delta, int x, int y);

	HWND m_hwnd;
};