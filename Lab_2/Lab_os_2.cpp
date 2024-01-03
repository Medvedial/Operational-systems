#include <iostream>
#include <thread>
#include <vector>
#include <Windows.h>

CRITICAL_SECTION cs;

void find_min_max(const std::vector<double>& arr) {
    double min_val = arr[0];
    double max_val = arr[0];
    for (int i = 1; i < arr.size(); ++i) {
        if (arr[i] < min_val) {
            min_val = arr[i];
        }

        if (arr[i] > max_val) {
            max_val = arr[i];
        }

        Sleep(7);
    }
    TryEnterCriticalSection(&cs);
    std::cout << "Min: " << min_val << ", Max: " << max_val << std::endl;
    LeaveCriticalSection(&cs);
}

void find_average(const std::vector<double>& arr) {
    double sum = 0;
    for (int i = 0; i < arr.size(); ++i) {
        sum += arr[i];
        Sleep(12);
    }
    double avg = sum / arr.size();
    TryEnterCriticalSection(&cs);
    std::cout << "Average: " << avg << std::endl;
    LeaveCriticalSection(&cs);
}

int main() {
    int size;
    std::cout << "Enter the size of the array: ";
    std::cin >> size;
    std::vector<double> arr(size);
    std::cout << "Enter the elements of the array: ";
    for (int i = 0; i < size; ++i) {
        std::cin >> arr[i];
    }
    InitializeCriticalSection(&cs);
    std::thread min_max_thread(find_min_max, arr);
    std::thread average_thread(find_average, arr);
    min_max_thread.join();
    average_thread.join();

    double min_val = arr[0];
    double max_val = arr[0];
    double sum = arr[0];
    for (int i = 1; i < arr.size(); ++i) {
        if (arr[i] < min_val) {
            min_val = arr[i];
        }

        if (arr[i] > max_val) {
            max_val = arr[i];
        }

        sum += arr[i];
    }
    double avg = sum / arr.size();
    for (int i = 0; i < arr.size(); ++i) {
        if (arr[i] == min_val) {
            arr[i] = avg;
        }

        if (arr[i] == max_val) {
            arr[i] = avg;
        }
    }
    TryEnterCriticalSection(&cs);
    std::cout << "Modified array: ";
    for (int i = 0; i < arr.size(); ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
    LeaveCriticalSection(&cs);
    DeleteCriticalSection(&cs);
    return 0;
}