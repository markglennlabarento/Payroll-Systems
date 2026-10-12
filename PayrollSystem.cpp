#include "PayrollSystem.h"

#include <cerrno>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

using namespace std;

namespace {
const char* employeeDataFile = "employees.txt";
const char* employeeDataHeader = "PAYROLL_DATA_V1";
const double defaultDailyRate = 505.00;
const int defaultDaysWorked = 13;

template <typename T>
bool parseLine(const string& line, T& value) {
    istringstream parser(line);
    string extra;

    if (!(parser >> value)) {
        return false;
    }
    return !(parser >> extra);
}
}

PayrollSystem::PayrollSystem() : storageReady(true) {
    loadEmployees();
}

PayrollSystem::~PayrollSystem() {
    if (storageReady && !saveEmployees()) {
        cerr << "Error: could not save employee data to "
             << employeeDataFile << ".\n";
    }
}

void PayrollSystem::loadEmployees() {
    errno = 0;
    ifstream input(employeeDataFile);
    if (!input.is_open()) {
        if (errno != ENOENT) {
            cerr << "Error: could not open " << employeeDataFile
                 << " for reading. Employee changes will not be saved.\n";
            storageReady = false;
        }
        return;
    }

    string line;
    if (!getline(input, line) || line != employeeDataHeader) {
        cerr << "Error: " << employeeDataFile
             << " has an invalid format. Employee changes will not be saved.\n";
        storageReady = false;
        return;
    }

    size_t employeeCount;
    if (!getline(input, line) || !parseLine(line, employeeCount)) {
        cerr << "Error: " << employeeDataFile
             << " has an invalid employee count. Employee changes will not be saved.\n";
        storageReady = false;
        return;
    }

    vector<unique_ptr<Payroll> > loadedEmployees;
    for (size_t index = 0; index < employeeCount; ++index) {
        int employeeId;
        double dailyRate;
        int daysWorked;
        string employeeName;

        if (!getline(input, line) || !parseLine(line, employeeId)
            || !getline(input, line) || !parseLine(line, dailyRate)
            || !getline(input, line) || !parseLine(line, daysWorked)
            || !getline(input, employeeName)
            || employeeId <= 0 || !isfinite(dailyRate) || dailyRate <= 0.0
            || daysWorked < 0 || employeeName.empty()) {
            cerr << "Error: invalid employee record in " << employeeDataFile
                 << ". Employee changes will not be saved.\n";
            storageReady = false;
            return;
        }

        for (size_t existing = 0; existing < loadedEmployees.size(); ++existing) {
            int existingId;
            string existingName;
            loadedEmployees[existing]->getDetails(existingId, existingName);
            if (existingId == employeeId) {
                cerr << "Error: duplicate employee ID in " << employeeDataFile
                     << ". Employee changes will not be saved.\n";
                storageReady = false;
                return;
            }
        }

        loadedEmployees.push_back(unique_ptr<Payroll>(
            new Payroll(employeeId, employeeName, dailyRate, daysWorked)));
    }

    while (getline(input, line)) {
        if (!line.empty()) {
            cerr << "Error: unexpected data in " << employeeDataFile
                 << ". Employee changes will not be saved.\n";
            storageReady = false;
            return;
        }
    }

    if (input.bad()) {
        cerr << "Error: failed while reading " << employeeDataFile
             << ". Employee changes will not be saved.\n";
        storageReady = false;
        return;
    }

    employees.swap(loadedEmployees);
    cout << "Loaded " << employees.size() << " employee(s) from "
         << employeeDataFile << ".\n";
}

bool PayrollSystem::saveEmployees() const {
    ofstream output(employeeDataFile, ios::trunc);
    if (!output.is_open()) {
        return false;
    }

    output << employeeDataHeader << '\n' << employees.size() << '\n';
    for (size_t index = 0; index < employees.size(); ++index) {
        int employeeId;
        string employeeName;
        employees[index]->getDetails(employeeId, employeeName);
        output << employeeId << '\n'
               << employees[index]->getDailyRate() << '\n'
               << employees[index]->getDaysWorked() << '\n'
               << employeeName << '\n';
    }

    output.close();
    return output.good();
}

int PayrollSystem::findEmployeeIndex(int employeeId) const {
    for (size_t index = 0; index < employees.size(); ++index) {
        int currentId;
        string currentName;
        employees[index]->getDetails(currentId, currentName);

        if (currentId == employeeId) {
            return static_cast<int>(index);
        }
    }

    return -1;
}

void PayrollSystem::addEmployee() {
    if (!storageReady) {
        cout << "Cannot add employees because employee data storage is unavailable.\n";
        return;
    }

    int employeeId;
    cout << "Enter employee ID: ";
    if (!(cin >> employeeId)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid employee ID.\n";
        return;
    }

    if (findEmployeeIndex(employeeId) != -1) {
        cout << "An employee with that ID already exists.\n";
        return;
    }

    string employeeName;
    cout << "Enter employee name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, employeeName);
    if (employeeName.empty()) {
        cout << "Employee name cannot be empty.\n";
        return;
    }

    employees.emplace_back(
        new Payroll(employeeId, employeeName, defaultDailyRate, defaultDaysWorked));
    if (saveEmployees()) {
        cout << "Employee added and saved successfully.\n";
    } else {
        cerr << "Error: employee was added for this session but could not be saved.\n";
    }
}

void PayrollSystem::viewEmployees() const {
    if (employees.empty()) {
        cout << "No employees have been added.\n";
        return;
    }

    cout << "\n=== Employee List ===\n";
    for (size_t index = 0; index < employees.size(); ++index) {
        int employeeId;
        string employeeName;
        employees[index]->getDetails(employeeId, employeeName);

        cout << "--------------------------\n"
             << "Employee ID: " << employeeId << '\n'
             << "Name: " << employeeName << '\n'
             << "Daily Rate: PHP " << employees[index]->getDailyRate() << '\n'
             << "Days Worked: " << employees[index]->getDaysWorked() << '\n';
    }
}

void PayrollSystem::searchEmployee() const {
    int employeeId;
    cout << "Enter employee ID to search: ";
    if (!(cin >> employeeId)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid employee ID.\n";
        return;
    }

    const int index = findEmployeeIndex(employeeId);
    if (index == -1) {
        cout << "Employee not found.\n";
        return;
    }

    employees[static_cast<size_t>(index)]->displayDetails();
}

void PayrollSystem::calculateSalary() const {
    int employeeId;
    cout << "Enter employee ID to calculate salary: ";
    if (!(cin >> employeeId)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid employee ID.\n";
        return;
    }

    const int index = findEmployeeIndex(employeeId);
    if (index == -1) {
        cout << "Employee not found.\n";
        return;
    }

    const Payroll& employee = *employees[static_cast<size_t>(index)];
    const double basicSalary = employee.calculateBasicSalary(
        employee.getDailyRate(), employee.getDaysWorked());
    const double deductions = employee.calculateDeductions(basicSalary);
    const double netSalary = employee.calculateNetSalary(basicSalary, deductions);

    cout << fixed << setprecision(2)
         << "Basic Salary: PHP " << basicSalary << '\n'
         << "Deductions: PHP " << deductions << '\n'
         << "Net Salary: PHP " << netSalary << '\n';
}

void PayrollSystem::generatePayslip() const {
    int employeeId;
    cout << "Enter employee ID to generate payslip: ";
    if (!(cin >> employeeId)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid employee ID.\n";
        return;
    }

    const int index = findEmployeeIndex(employeeId);
    if (index == -1) {
        cout << "Employee not found.\n";
        return;
    }

    employees[static_cast<size_t>(index)]->generatePayslip();
}

void PayrollSystem::deleteEmployee() {
    if (!storageReady) {
        cout << "Cannot delete employees because employee data storage is unavailable.\n";
        return;
    }

    int employeeId;
    cout << "Enter employee ID to delete: ";
    if (!(cin >> employeeId)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid employee ID.\n";
        return;
    }

    const int index = findEmployeeIndex(employeeId);
    if (index == -1) {
        cout << "Employee not found.\n";
        return;
    }

    employees.erase(employees.begin() + index);
    if (saveEmployees()) {
        cout << "Employee deleted and changes saved successfully.\n";
    } else {
        cerr << "Error: employee was deleted for this session but the change could not be saved.\n";
    }
}

void PayrollSystem::run() {
    int choice = -1;

    do {
        cout << "\n=== Payroll System ===\n"
             << "1. Add employee\n"
             << "2. View employees\n"
             << "3. Search employee\n"
             << "4. Calculate salary\n"
             << "5. Generate payslip\n"
             << "6. Delete employee\n"
             << "0. Exit\n"
             << "Select an option: ";

        if (!(cin >> choice)) {
            if (cin.eof()) {
                cout << "\nExiting payroll system.\n";
                return;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Enter a number from 0 to 6.\n";
            continue;
        }

        switch (choice) {
        case 1:
            addEmployee();
            break;
        case 2:
            viewEmployees();
            break;
        case 3:
            searchEmployee();
            break;
        case 4:
            calculateSalary();
            break;
        case 5:
            generatePayslip();
            break;
        case 6:
            deleteEmployee();
            break;
        case 0:
            cout << "Exiting payroll system.\n";
            break;
        default:
            cout << "Invalid option. Enter a number from 0 to 6.\n";
            break;
        }
    } while (choice != 0);
}
