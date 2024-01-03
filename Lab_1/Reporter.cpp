#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

struct Employee
{
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cout << "Usage: Reporter <input_file> <output_file> <hourly_rate>\n";
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];
    double hourly_rate = std::stod(argv[3]);

    std::ifstream in_file(input_file, std::ios::binary);
    std::ofstream out_file(output_file, std::ios::out);

    if (!in_file || !out_file)
    {
        std::cout << "Error opening files.\n";
        return 1;
    }

    Employee emp;

    out_file << "Report for file \"" << input_file << "\"\n";
    out_file << "Number\tName\tHours\tSalary\n";

    while (in_file.read(reinterpret_cast<char*>(&emp), sizeof(Employee)))
    {
        double salary = emp.hours * hourly_rate;
        out_file << emp.num << "\t" << emp.name << "\t" << emp.hours << "\t" << std::fixed << std::setprecision(2) << salary << "\n";
    }

    return 0;
}