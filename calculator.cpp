#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <ctype.h>

#define ID_EDIT1 1
#define ID_EDIT2 2
#define ID_BUTTON_ADD 3
#define ID_BUTTON_SUB 4
#define ID_BUTTON_MUL 5
#define ID_BUTTON_DIV 6

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void PerformCalculation(HWND, int);
BOOL IsValidNumber(const char*);

HWND hEdit1, hEdit2, hStatic;
HFONT hFont;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int nCmdShow) {
    WNDCLASS wc = {};
    wc.hInstance = hInst;
    wc.lpszClassName = "CalculatorWindow";
    wc.lpfnWndProc = WindowProcedure;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 255, 127));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("CalculatorWindow", "My Calculator",
        WS_OVERLAPPED | WS_SYSMENU, 100, 100, 250, 200,
        NULL, NULL, hInst, NULL);

    ShowWindow(hwnd, nCmdShow);
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        hFont = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

        hStatic = CreateWindow("STATIC", "Please input two numbers", WS_VISIBLE | WS_CHILD | SS_CENTER,
                               20, 10, 200, 20, hwnd, NULL, NULL, NULL);
        SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Removed ES_NUMBER to allow decimal points and negative numbers
        hEdit1 = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                              70, 40, 100, 20, hwnd, (HMENU)ID_EDIT1, NULL, NULL);
        hEdit2 = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                              70, 70, 100, 20, hwnd, (HMENU)ID_EDIT2, NULL, NULL);

        CreateWindow("BUTTON", "+", WS_VISIBLE | WS_CHILD | WS_BORDER,
                     40, 110, 40, 30, hwnd, (HMENU)ID_BUTTON_ADD, NULL, NULL);
        CreateWindow("BUTTON", "-", WS_VISIBLE | WS_CHILD | WS_BORDER,
                     90, 110, 40, 30, hwnd, (HMENU)ID_BUTTON_SUB, NULL, NULL);
        CreateWindow("BUTTON", "*", WS_VISIBLE | WS_CHILD | WS_BORDER,
                     140, 110, 40, 30, hwnd, (HMENU)ID_BUTTON_MUL, NULL, NULL);
        CreateWindow("BUTTON", "/", WS_VISIBLE | WS_CHILD | WS_BORDER,
                     190, 110, 40, 30, hwnd, (HMENU)ID_BUTTON_DIV, NULL, NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case ID_BUTTON_ADD: PerformCalculation(hwnd, ID_BUTTON_ADD); break;
        case ID_BUTTON_SUB: PerformCalculation(hwnd, ID_BUTTON_SUB); break;
        case ID_BUTTON_MUL: PerformCalculation(hwnd, ID_BUTTON_MUL); break;
        case ID_BUTTON_DIV: PerformCalculation(hwnd, ID_BUTTON_DIV); break;
        }
        break;

    case WM_CTLCOLORSTATIC:
        SetBkMode((HDC)wp, TRANSPARENT);
        return (LRESULT)GetStockObject(NULL_BRUSH);

    case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

// Function to validate if input is a valid floating-point number
BOOL IsValidNumber(const char* str) {
    int dotCount = 0, signCount = 0;

    while (*str) {
        if (*str == '.') {
            if (++dotCount > 1) return FALSE;  // Only one decimal allowed
        }
        else if (*str == '-' || *str == '+') {
            if (signCount > 0 || (str != str + 1)) return FALSE;  // Only one sign at the beginning
            signCount++;
        }
        else if (!isdigit((unsigned char)*str)) {
            return FALSE;  // Not a number
        }
        str++;
    }
    return TRUE;
}

void PerformCalculation(HWND hwnd, int operation) {
    char buffer1[100], buffer2[100], resultText[100];
    GetWindowText(hEdit1, buffer1, 100);
    GetWindowText(hEdit2, buffer2, 100);

    // Validate input
    if (!IsValidNumber(buffer1) || !IsValidNumber(buffer2)) {
        MessageBox(hwnd, "Invalid input! Please enter valid numbers.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    double num1 = atof(buffer1);
    double num2 = atof(buffer2);
    double result = 0.0;

    switch (operation) {
    case ID_BUTTON_ADD: result = num1 + num2; break;
    case ID_BUTTON_SUB: result = num1 - num2; break;
    case ID_BUTTON_MUL: result = num1 * num2; break;
    case ID_BUTTON_DIV:
        if (num2 != 0)
            result = num1 / num2;
        else {
            MessageBox(hwnd, "Cannot divide by zero!", "Error", MB_OK | MB_ICONERROR);
            return;
        }
        break;
    }

    snprintf(resultText, sizeof(resultText), "Result: %.2f", result);
    MessageBox(hwnd, resultText, "Result", MB_OK);
}

