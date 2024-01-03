#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <Windows.h>

const int MAX_ARRAY_SIZE = 100;
const int MARKER_THREADS = 5;
const int MAX_RANDOM_NUMBER = 100;

std::vector<int> data;
HANDLE threadHandles[MARKER_THREADS];
HANDLE mainEvent;
HANDLE markerEvents[MARKER_THREADS];
CRITICAL_SECTION cs;

DWORD WINAPI markerThread(LPVOID lpParam) {
    int threadID = reinterpret_cast<int>(lpParam);
    std::srand(static_cast<unsigned int>(std::time(nullptr)) + threadID);

    while (true) {
        WaitForSingleObject(mainEvent, INFINITE);

        int randomValue = std::rand() % data.size();

        EnterCriticalSection(&cs);
        if (data[randomValue] == 0) {
            Sleep(5);
            data[randomValue] = threadID;
            Sleep(5);
        }
        else {
            int unmarkedElement = randomValue;
            SetEvent(mainEvent);
            WaitForSingleObject(markerEvents[threadID], INFINITE);
        }
        LeaveCriticalSection(&cs);

        SetEvent(mainEvent);

        if (WaitForSingleObject(markerEvents[threadID], 0) == WAIT_OBJECT_0) {
            break;
        }
    }

    return 0;
}

int main() {
    setlocale(LC_ALL, "rus");
    int arraySize;
    int markedCount = 0;

    std::cout << "Введите размер массива: ";
    std::cin >> arraySize;

    if (arraySize <= 0 || arraySize > MAX_ARRAY_SIZE) {
        std::cerr << "Неверный размер массива." << std::endl;
        return 1;
    }

    data.resize(arraySize, 0);

    int numMarkerThreads;
    std::cout << "Введите количество потоков marker: ";
    std::cin >> numMarkerThreads;

    if (numMarkerThreads <= 0 || numMarkerThreads > MARKER_THREADS) {
        std::cerr << "Неверное количество потоков marker." << std::endl;
        return 1;
    }

    mainEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    InitializeCriticalSection(&cs);

    for (int i = 0; i < numMarkerThreads; ++i) {
        markerEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        threadHandles[i] = CreateThread(NULL, 0, markerThread, (LPVOID)i, 0, NULL);
    }

    std::cout << "Запуск потоков marker..." << std::endl;

    SetEvent(mainEvent);

    while (true) {
        WaitForSingleObject(mainEvent, INFINITE);

        if (WaitForSingleObject(mainEvent, 0) == WAIT_OBJECT_0) {
            break;
        }

        bool fullyMarked = true;
        for (int i = 0; i < data.size(); ++i) {
            if (data[i] == 0) {
                fullyMarked = false;
                break;
            }
        }

        if (fullyMarked) {
            std::cout << "Массив полностью помечен." << std::endl;
        }
        else {
            std::cout << "Не все элементы помечены." << std::endl;
        }

        int markerToExit;
        std::cout << "Введите порядковый номер потока marker, которому следует завершить работу: ";
        std::cin >> markerToExit;

        if (markerToExit >= 0 && markerToExit < numMarkerThreads) {
            SetEvent(markerEvents[markerToExit]);
            WaitForSingleObject(threadHandles[markerToExit], INFINITE);
            CloseHandle(threadHandles[markerToExit]);
        }
    }

    for (int i = 0; i < numMarkerThreads; ++i) {
        CloseHandle(markerEvents[i]);
    }

    DeleteCriticalSection(&cs);
    CloseHandle(mainEvent);

    return 0;
}
