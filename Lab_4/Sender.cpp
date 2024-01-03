#include <fstream> 
#include <Windows.h> 
#include <conio.h> 
#include <iostream> 

using namespace std;

void processMessages(const std::string& file_name, HANDLE hStartEvent, HANDLE hInputReadySemaphore, HANDLE hOutputReadySemaphore, HANDLE hMutex) {
    std::fstream file;
    cout << "Press 1 to write the message"
        << endl << "Write 0 to end the process" << endl;
    std::string key;
    cin >> key;

    while (true) {
        if (key == "1") {
            WaitForSingleObject(hMutex, INFINITE); //блокирует текущий sender пока не будет получен доступ к файлу
            file.open(file_name, std::ios::out | std::ios::app);

            std::string msg;
            cout << "Type your message: ";
            cin >> msg;

            char message[21];
            for (int i = 0; i < msg.length(); i++) {
                message[i] = msg[i];
            }

            for (int i = msg.length(); i < 21; i++) {
                message[i] = '\0';
            }

            message[20] = '\n';
            ReleaseMutex(hMutex); //дает доступ к файлу
            ReleaseSemaphore(hOutputReadySemaphore, 1, NULL); //говорит reciever-у что sender записал сообщение и готов принять следующее

            if (ReleaseSemaphore(hInputReadySemaphore, 1, NULL) != 1) {
                ReleaseSemaphore(hOutputReadySemaphore, 1, NULL);
                cout << "you have broken the limit of numbers of messages";
                WaitForSingleObject(hOutputReadySemaphore, INFINITE); //позволяет reciever-у дождаться готовности sender
                ReleaseSemaphore(hOutputReadySemaphore, 1, NULL); //сообщает reciever что его ждет сообщение
                ReleaseSemaphore(hInputReadySemaphore, 1, NULL); //говорит reciever что sender готов отправлять сообщение

                for (int i = 0; i < 21; i++) {
                    file << message[i];
                }
            }
            else {
                for (int i = 0; i < 21; i++) {
                    file << message[i];
                }
            }

            file.close();
            cout << endl << "Write 1 to write next message"
                << endl << "Write 0 to end the process" << endl;
            cin >> key;
        }

        if (key != "0" && key != "1") {
            cout << endl << "Incorrect value"
                << endl << "Write 1 to write next message"
                << endl << "Write 0 to end the process" << endl;
            cin >> key;
        }

        if (key == "0") {
            cout << "Process ended";
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <file_name>" << endl;
        return 1;
    }

    std::string file_name = argv[1];
    std::fstream file;
    HANDLE hStartEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, "StartPocess");
    HANDLE hInputReadySemaphore = OpenSemaphore(EVENT_ALL_ACCESS, FALSE, L"EnterSemaphoreStarted");
    HANDLE hOutputReadySemaphore = OpenSemaphore(EVENT_ALL_ACCESS, FALSE, L"OutputSemaphoreStarted");
    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"mut ex");

    SetEvent(hStartEvent);
    cout << "Event was started" << endl;

    processMessages(file_name, hStartEvent, hInputReadySemaphore, hOutputReadySemaphore, hMutex);

    CloseHandle(hInputReadySemaphore);
    CloseHandle(hOutputReadySemaphore);
    CloseHandle(hMutex);
}