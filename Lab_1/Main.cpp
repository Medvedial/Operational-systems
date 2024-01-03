#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>

struct Employee
{
    int num;
    char name[10];
    double hours;
};

int main()
{
    std::string input_file;
    int num_records;

    std::cout << "Enter input file name: ";
    std::cin >> input_file;

    std::cout << "Enter number of records: ";
    std::cin >> num_records;

    std::string creator_command = "Creator.exe " + input_file + " " + std::to_string(num_records);
    const char* сcommand = creator_command.c_str();
    int len1 = strlen(сcommand) + 1;
    int size1 = MultiByteToWideChar(CP_UTF8, 0, сcommand, len1, NULL, 0);
    wchar_t* wCommand1 = new wchar_t[size1];
    MultiByteToWideChar(CP_UTF8, 0, сcommand, len1, wCommand1, size1);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(nullptr, (LPWSTR)wCommand1, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
    {
        std::cout << "Error creating Creator process.\n";
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    std::cout << "Contents of " << input_file << ":\n";
    std::ifstream file(input_file, std::ios::binary);

    if (!file)
    {
        std::cout << "Error opening file.\n";
        return 1;
    }

    Employee emp;

    while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee)))
    {
        std::cout << emp.num << "\t" << emp.name << "\t" << emp.hours << "\n";
    }




    std::string output_file;
    double hourly_rate;

    std::cout << "Enter output file name: ";
    std::cin >> output_file;

    std::cout << "Enter hourly rate: ";
    std::cin >> hourly_rate;

    std::string reporter_command = "Reporter.exe " + input_file + " " + output_file + " " + std::to_string(hourly_rate);

    const char* rcommand = reporter_command.c_str();
    int len2 = strlen(rcommand) + 1;
    int size2 = MultiByteToWideChar(CP_UTF8, 0, rcommand, len2, NULL, 0);
    wchar_t* wCommand2 = new wchar_t[size2];
    MultiByteToWideChar(CP_UTF8, 0, rcommand, len2, wCommand2, size2);

    if (!CreateProcess(NULL, (LPWSTR)wCommand2, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        std::cout << "Error creating Reporter process.\n";
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    std::ifstream report_file(output_file);
    if (!report_file)
    {
        std::cout << "Error opening report file.\n";
        return 1;
    }
    std::string line;
    while (std::getline(report_file, line))
    {
        std::cout << line << "\n";
    }

    return 0;
}