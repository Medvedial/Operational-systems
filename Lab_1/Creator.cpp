#include <iostream>
#include <fstream>
#include <string>

struct Employee
{
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: Creator <file_name> <num_records>\n";
        return 1;
    }

    std::string file_name = argv[1];
    int num_records = std::stoi(argv[2]);

    std::ofstream file(file_name, std::ios::binary);

    if (!file)
    {
        std::cout << "Error creating file.\n";
        return 1;
    }

    Employee emp;

    for (int i = 0; i < num_records; i++)
    {
        std::cout << "Enter employee number, name, hours: ";
        std::cin >> emp.num >> emp.name >> emp.hours;
        file.write(reinterpret_cast<char*>(&emp), sizeof(Employee));
    }

    return 0;
}