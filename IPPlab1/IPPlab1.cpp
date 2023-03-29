#include <windows.h>
#include <fstream>
#include <string>
#include <chrono>

// Глобальні змінні 
HINSTANCE hInst;
HWND hWnd;
void SaveExecutionTimeToFile(HWND hWnd, long long time);

// Функція потоку (малювання)
DWORD WINAPI DrawThread(LPVOID lpParam) {
    auto start_time = std::chrono::high_resolution_clock::now();
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    int width = rcClient.right - rcClient.left;
    int height = rcClient.bottom - rcClient.top;

    HDC hdc = GetDC(hWnd);

    // Заповнення області вікна різнокольоровими прямокутниками
    for (int y = 0; y < height; y += 50) {
        for (int x = 0; x < width; x += 40) {
            int r = rand() % 256;
            int g = rand() % 256;
            int b = rand() % 256;
            HBRUSH hBrush = CreateSolidBrush(RGB(r, g, b));
            RECT rcSquare = { x, y, x + 40, y + 50 };
            FillRect(hdc, &rcSquare, hBrush);
            DeleteObject(hBrush);
        }
    }

    ReleaseDC(hWnd, hdc);
    auto end_time = std::chrono::high_resolution_clock::now();
    long long execution_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    SaveExecutionTimeToFile(hWnd, execution_time);

    return 0;
}

// Вікона процедура
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {
    case WM_SIZE: {
        // Потік для малювання різнокольорових квадратів
        HANDLE hThread = CreateThread(NULL, 0, DrawThread, NULL, 0, NULL);
       
        CloseHandle(hThread);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Точка входу
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;

    // Регістрація класу вікна
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"MyClass";
    RegisterClass(&wc);

    // Створення основного вікна
    hWnd = CreateWindow(L"MyClass", L"My Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);

    // Цикл повідомлень
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
void SaveExecutionTimeToFile(HWND hWnd, long long time)
{
    // Отримання розмірів вікна
    RECT rect;
    GetWindowRect(hWnd, &rect);
    int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;

    // Створення рядка для запису у файл
    std::string str = std::to_string(window_width) + "x" + std::to_string(window_height) + ": " + std::to_string(time) + " microseconds\n";

    // Запис рядка у файл
    std::ofstream file("execution_times.txt", std::ios::app);

    if (file.is_open())
    {
        file << str;
        file.close();
    }
}
