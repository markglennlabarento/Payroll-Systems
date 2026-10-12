#ifndef PAYROLL_H
#define PAYROLL_H

#include "Employee.h"

class Payroll : public Employee {
private:
    double calculateSssDeduction(double weeklySalary) const;
    double calculatePhilHealthDeduction(double weeklySalary) const;
    double calculatePagIbigDeduction(double weeklySalary) const;
    double calculateIncomeTax(double weeklySalary, double otherDeductions) const;

public:
    using Employee::Employee;

    double calculateBasicSalary(double dailyRate, int daysWorked) const;
    double calculateDeductions(double basicSalary) const;
    double calculateNetSalary(double basicSalary, double deductions) const;
    void generatePayslip() const;
};

#endif
