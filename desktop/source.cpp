#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <string>
#include "COM.h"
#include "windowsx.h"
#include <d2d1.h>
#include "GUI.h"
#include <sstream>
#include "Interval.h"

TextArea* textarea;

TextArea* txtarea_day;
TextArea* txtarea_month;
TextArea* txtarea_year;
TextArea* txtarea_hours;
TextArea* txtarea_minutes;
TextArea* txtarea_seconds;

int tm_ping;
bool device_connected = false;

int to;
int counter = 0;
bool receiver_enabled = false;

void read_callback(char c) {

    string time_out;

    SYSTEMTIME lt;
    GetLocalTime(&lt);
    stringstream ss;
    ss << (lt.wHour < 10 ? "0" : "") << lt.wHour << ":" << (lt.wMinute<10 ? "0" : "") <<lt.wMinute << ":" << (lt.wSecond < 10 ? "0" : "") << lt.wSecond;

    if (receiver_enabled) {

        to |= (c << (counter * 8));
        counter++;

        if (counter == 4) {
            
            receiver_enabled = false;

            stringstream ss_;
            ss_ << to;

            textarea->SetText(textarea->GetText() + "Data MCU: " + ss_.str() + "\r\n");

            counter = 0;
            to = 0;

        }

        return;
    }

    if ((int)c == 100) {

        textarea->SetText(textarea->GetText() + "Alarm wlaczony " + ss.str() + "\r\n");

    }
    if ((int)c == 101) {

        textarea->SetText(textarea->GetText() + "Drzwi otwarte " + ss.str() + "\r\n");

    }
    if ((int)c == 102) {

        textarea->SetText(textarea->GetText() + "Drzwi zamkniete " + ss.str() + "\r\n");

    }

    if ((int)c == 103) {

        if (device_connected == false) {

            textarea->SetText(textarea->GetText() + "Urzadzenie polaczone " + ss.str() + "\r\n");

            device_connected = true;

        }

        tm_ping = lt.wSecond + (60 * lt.wMinute) + (60 * 60 * lt.wHour);

    }

    if ((int)c == 104) {

        receiver_enabled = true;

    }

}

void callback_1() {

    WritePort((char)25);

}
void callback_2() {

    WritePort((char)26);

}

void callback_interval() {

    SYSTEMTIME lt;
    GetLocalTime(&lt);
    stringstream ss;
    ss << (lt.wHour < 10 ? "0" : "") << lt.wHour << ":" << (lt.wMinute < 10 ? "0" : "") << lt.wMinute << ":" << (lt.wSecond < 10 ? "0" : "") << lt.wSecond;
    
    if (device_connected == false) {
        return;
    }

    int tm_now = lt.wSecond + (60 * lt.wMinute) + (60 * 60 * lt.wHour);

    if (tm_now - tm_ping > 2) {

        textarea->SetText(textarea->GetText() + "Urzadzenie rozlaczone " + ss.str() + "\r\n");

        device_connected = false;

    }

}

void callback_3() {

    SYSTEMTIME lt;
    GetLocalTime(&lt);

    int secs_out = 0;

    int day_ = 60 * 60 * 24;

    int cur_time_sec = lt.wSecond + (60 * lt.wMinute) + (60 * 60 * lt.wHour);
    int alarm_time = txtarea_seconds->GetNumber() + (txtarea_minutes->GetNumber() * 60) + (txtarea_hours->GetNumber() * 60 * 60);

    if (txtarea_day->GetNumber() == lt.wDay + 1) {

        secs_out = alarm_time + (day_ - cur_time_sec);

    }
    else {

        secs_out = alarm_time - cur_time_sec;

    }

    stringstream ss;

    int h_ = secs_out / 60 / 60;
    int m_ = (secs_out - (h_ * 60 * 60)) / 60;
    int s_ = (secs_out - (h_ * 60 * 60) - (m_ * 60));

    if (h_ > 0) {

        ss << h_ << " godzin ";

    }
    if (m_ > 0) {

        ss << m_ << " minut ";

    }
    
    ss << s_<<" sekund ";

    textarea->SetText(textarea->GetText() + "Alarm uruchomiony za " + ss.str() + "\r\n");

    WritePort((char)30);
    Sleep(100);
    WritePort((char)secs_out & 255);
    Sleep(100);
    WritePort((char)(secs_out>>8) & 255);
    Sleep(100);
    WritePort((char)(secs_out>>16) & 255);
    Sleep(100);
    WritePort((char)(secs_out>>24) & 255);

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    GUI gui;


    gui.NewButton("Wlacz alarm drzwi", 20, 20, 150, 20, callback_1);
    gui.NewButton("Wylacz alarm drzwi", 180, 20, 150, 20, callback_2);
    gui.NewButton("Nastaw alarm", 340, 20, 150, 20, callback_3);

    textarea = gui.NewTextArea("", 20, 45, 200, 400);
    textarea->readonly = true;
    textarea->multiline = true;
    
    txtarea_day = gui.NewTextArea("", 20, 460, 50, 20);
    txtarea_day->number = true;
    txtarea_month = gui.NewTextArea("", 80, 460, 50, 20);
    txtarea_month->number = true;
    txtarea_year = gui.NewTextArea("", 140, 460, 50, 20);
    txtarea_year->number = true;
    txtarea_hours = gui.NewTextArea("", 200, 460, 50, 20);
    txtarea_hours->number = true;
    txtarea_minutes = gui.NewTextArea("", 260, 460, 50, 20);
    txtarea_minutes->number = true;
    txtarea_seconds = gui.NewTextArea("", 320, 460, 50, 20);
    txtarea_seconds->number = true;
    
    setInterval(callback_interval, 1000);

    gui.start(hInstance);

    OpenPort("COM3", 1200, 8, NOPARITY, TWOSTOPBITS, read_callback);

    gui.loop();

    return 0;
}

