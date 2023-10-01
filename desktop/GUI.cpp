#include "GUI.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    GUI *gui = reinterpret_cast<GUI*>(GetWindowLongPtrA(hwnd, 0));

    switch (uMsg)
    {
    case 1024:
    {

        break;
    }
    case WM_CREATE:

        break;

    case WM_COMMAND: {

        for (int i = 0; i < gui->buttons.size(); i++) {

            Button& btn = *(gui->buttons[i]);

            if (LOWORD(wParam) == btn.id) {

                btn.callback();

            }

        }

        break;
    }
    case WM_LBUTTONDOWN: {

        if (gui->cb_lbtndown != NULL) {
            gui->cb_lbtndown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }

        break;
    }

    case WM_LBUTTONUP: {

        if (gui->cb_lbtnup != NULL) {
            gui->cb_lbtnup(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }

        break;
    }
    
    case WM_KEYDOWN: {

        if (gui->cb_keydown != NULL) {
            gui->cb_keydown(wParam);
        }

        break;
    }

    case WM_MOUSEMOVE: {

        if (gui->cb_mousemove != NULL) {
            gui->cb_mousemove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
        }

        break;
    }

    case WM_MOUSEWHEEL: {

        if (gui->cb_mousewheel != NULL) {
            gui->cb_mousewheel(GET_WHEEL_DELTA_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }

        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);  

        if (gui->cb_paint != NULL) {
            gui->cb_paint(hdc);
        }

        EndPaint(hwnd, &ps);

        break;
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void GUI::NewButton(string label, int x, int y, int width, int height, void (*callback)()) {

    Button* wsk = new Button(label, x, y, width, height, callback, this->next_id++);

    this->buttons.push_back( wsk );

}

TextArea* GUI::NewTextArea(string label, int x, int y, int width, int height) {

    TextArea* wsk = new TextArea(label, x, y, width, height, this->next_id++);

    this->textareas.push_back(wsk);

    return wsk;

}

void GUI::start(HINSTANCE &hInstance) {

    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.cbWndExtra = 200;
    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return;
    }

    this->m_hwnd = hwnd;

    int ok = SetWindowLongPtrA(hwnd, 0, reinterpret_cast<LONG_PTR>(this));
    int err;
    if (!ok) {
        err = GetLastError();
    }

    GUI* gui_ = reinterpret_cast<GUI*>(GetWindowLongPtrA(hwnd, 0));

    ShowWindow(hwnd, SW_SHOW);

    for (int i = 0; i < this->buttons.size(); i++) {

        Button& btn = *(this->buttons[i]);

        HWND hwnd_ = CreateWindowA("Button", btn.label.c_str(),
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            btn.x, btn.y, btn.width, btn.height, hwnd, (HMENU)btn.id,
            NULL, NULL);

        btn.hwnd = hwnd_;

    }

    for (int i = 0; i < this->textareas.size(); i++) {

        TextArea& textarea = *(this->textareas[i]);

        long long flags = WS_CHILD | WS_VISIBLE | WS_BORDER;

        if(textarea.readonly){
            flags |= ES_READONLY;
        }
        if (textarea.multiline) {
            flags |= ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL;
        }
        if (textarea.number) {
            flags |= ES_NUMBER;
        }

        HWND hwnd_ = CreateWindowA("EDIT", textarea.label.c_str(),
            flags,
            textarea.x, textarea.y, textarea.width, textarea.height, hwnd, (HMENU)textarea.id,
            NULL, NULL);

        textarea.hwnd = hwnd_;

    }


}

void GUI::loop() {

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);

        if (msg.message == WM_KEYDOWN) {

            if (msg.wParam == VK_ESCAPE) {
                msg.hwnd = this->m_hwnd;
                MessageBeep(MB_OK);
            }

        }

        if (msg.message == 1024) {
            msg.hwnd = this->m_hwnd;
        }

        DispatchMessage(&msg);
    }

}

