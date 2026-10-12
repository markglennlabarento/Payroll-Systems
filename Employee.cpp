#include <iostream>
#include "Employee.h"
using namespace std;

Employee::Employee(int employeeId, string employeeName, const double dailyRate, const int daysWorked) : id(employeeId), name(employeeName), dailyRate(dailyRate), daysWorked(daysWorked) {
}

void Employee::getDetails(int& employeeId, string& employeeName) const {
    employeeId = id;
    employeeName = name;
}

void Employee::displayDetails() const {
    cout << "--------------------------" << endl;
    cout << "Employee ID: " << id << endl;
    cout << "Name: " << name << endl;
    cout << "Daily Rate: PHP " << dailyRate << endl;
    cout << "Days Worked: " << daysWorked << " days" << endl;
}

double Employee::getDailyRate() const {
    return dailyRate;
}

int Employee::getDaysWorked() const {
    return daysWorked;
}