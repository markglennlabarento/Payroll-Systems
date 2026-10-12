#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
using namespace std;

class Employee {
private:
    int id;
    string name;
    const double dailyRate = 505.00; // Default daily rate
    int daysWorked = 13; // Default number of days worked

public:
    Employee(int employeeId, string employeeName, const double dailyRate = 505.00, const int daysWorked = 13);
    void getDetails(int& employeeId, string& employeeName) const;
    void displayDetails() const;
    double getDailyRate() const;
    int getDaysWorked() const;
};

#endif