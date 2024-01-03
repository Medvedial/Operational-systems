#include <fstream> 
#include <iostream> 
#include <windows.h> 
#include <string> 

using namespace std;

void CreateSenderProcesses(const std::string& file_name, int number_of_senders, HANDLE* hEventStarted) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    for (int i = 0; i < number_of_senders; ++i) {
        std::string sender_cmd = "Sender.exe " + file_name;

        LPSTR CommandLine = const_cast<LPSTR>(sender_cmd.c_str());
        const char* сcommand = (const char*)CommandLine;
        int len1 = strlen(сcommand) + 1;
        int size1 = MultiByteToWideChar(CP_UTF8, 0, сcommand, len1, NULL, 0);
        wchar_t* wCommand = new wchar_t[size1];
        MultiByteToWideChar(CP_UTF8, 0, сcommand, len1, wCommand, size1);

        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        if (!CreateProcess(NULL, wCommand, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            cout << "The Sender Process is not started." << endl;
        }
        hEventStarted[i] = CreateEvent(NULL, FALSE, FALSE, L"StartPocess");
        CloseHandle(pi.hProcess);
    }
}

void HandleMessages(const std::string& file_name, HANDLE hInputReadySemaphore, HANDLE hOutputReadySemaphore, HANDLE hMutex) {
    std::fstream file;
    cout << endl << "Enter 1 to read message"
        << endl << "Enter 0 to exit" << endl;
    int key;
    cin >> key;
    file.open(file_name, std::ios::in);

    while (true) {
        if (key == 1) {
            std::string message;
            WaitForSingleObject(hInputReadySemaphore, INFINITE); //блокирует reciever покак семафор не ьудет освобожден
            WaitForSingleObject(hMutex, INFINITE);               //блокирует доступ к файлу
            std::getline(file, message);
            cout << message;
            ReleaseSemaphore(hOutputReadySemaphore, 1, NULL);    //освобождает семафор и говорит что reciver может дальше принимать сообщения
            ReleaseMutex(hMutex);                                //осфобождает мьютекс (дает доступ к файлам)
            cout << endl << "Enter 1 to read next message"
                << endl << "Enter 0 to exit" << endl;
            cin >> key;
        }
        if (key != 0 && key != 1) {
            cout << endl << "Invalid Value"
                << endl << "Enter 1 to read the next message"
                << endl << "Enter 0 to exit" << endl;
            cin >> key;
        }
        if (key == 0) {
            cout << "Process ended";
            break;
        }
    }
    file.close();
}

int main() {
    std::string file_name;
    int number_of_notes;
    std::fstream file;
    int number_of_senders;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    cout << "Enter binary file name: ";
    cin >> file_name;
    cout << "Number of notes: ";
    cin >> number_of_notes;
    file.open(file_name, std::ios::out);
    file.close();

    cout << "Number of processes: ";
    cin >> number_of_senders;

    HANDLE inputSemaphore = CreateSemaphore(NULL, 0, number_of_notes, L"EnterSemaphoreStarted");
    HANDLE outputSemaphore = CreateSemaphore(NULL, 0, number_of_notes, L"OutputSemaphoreStarted");
    HANDLE hMutex = CreateMutex(NULL, 0, L"mut ex");
    HANDLE* hEventStarted = new HANDLE[number_of_senders];

    CreateSenderProcesses(file_name, number_of_senders, hEventStarted);
    WaitForMultipleObjects(number_of_senders, hEventStarted, TRUE, INFINITE); //ждет пока не запустятся все процессы sender
    ReleaseMutex(hMutex); // дает доступ к файлу
    HandleMessages(file_name, inputSemaphore, outputSemaphore, hMutex);

    CloseHandle(inputSemaphore);
    CloseHandle(outputSemaphore);
    for (int i = 0; i < number_of_senders; i++) {
        CloseHandle(hEventStarted[i]);
    }
    delete[] hEventStarted;
}