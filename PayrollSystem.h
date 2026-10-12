#ifndef PAYROLL_SYSTEM_H
#define PAYROLL_SYSTEM_H

#include <memory>
#include <vector>

#include "Payroll.h"

using namespace std;

class PayrollSystem {
private:
    vector<unique_ptr<Payroll> > employees;
    bool storageReady;

    int findEmployeeIndex(int employeeId) const;
    void loadEmployees();
    bool saveEmployees() const;

public:
    PayrollSystem();
    ~PayrollSystem();

    void addEmployee();
    void viewEmployees() const;
    void searchEmployee() const;
    void calculateSalary() const;
    void generatePayslip() const;
    void deleteEmployee();
    void run();
};

#endif
