#include <iomanip>
#include <iostream>
#include <algorithm>
#include <cmath>

#include "Payroll.h"

namespace {
const double weeksPerYear = 52.0;
const double monthsPerYear = 12.0;
const double weeklyProration = monthsPerYear / weeksPerYear;

double roundToCents(double amount) {
    return std::round(amount * 100.0) / 100.0;
}

double calculateAnnualIncomeTax(double taxableIncome) {
    if (taxableIncome <= 250000.0) {
        return 0.0;
    }
    if (taxableIncome <= 400000.0) {
        return (taxableIncome - 250000.0) * 0.15;
    }
    if (taxableIncome <= 800000.0) {
        return 22500.0 + (taxableIncome - 400000.0) * 0.20;
    }
    if (taxableIncome <= 2000000.0) {
        return 102500.0 + (taxableIncome - 800000.0) * 0.25;
    }
    if (taxableIncome <= 8000000.0) {
        return 402500.0 + (taxableIncome - 2000000.0) * 0.30;
    }
    return 2202500.0 + (taxableIncome - 8000000.0) * 0.35;
}
}

double Payroll::calculateBasicSalary(double dailyRate, int daysWorked) const {
    return dailyRate * daysWorked;
}

double Payroll::calculateSssDeduction(double weeklySalary) const {
    const double monthlySalary = weeklySalary * weeksPerYear / monthsPerYear;
    const double salaryCredit = monthlySalary < 5250.0
        ? 5000.0
        : std::min(35000.0,
            5500.0 + std::floor((monthlySalary - 5250.0) / 500.0) * 500.0);
    return roundToCents(salaryCredit * 0.05 * weeklyProration);
}

double Payroll::calculatePhilHealthDeduction(double weeklySalary) const {
    const double monthlySalary = weeklySalary * weeksPerYear / monthsPerYear;
    const double contributionBase = std::max(10000.0, std::min(100000.0, monthlySalary));
    return roundToCents(contributionBase * 0.025 * weeklyProration);
}

double Payroll::calculatePagIbigDeduction(double weeklySalary) const {
    const double monthlySalary = weeklySalary * weeksPerYear / monthsPerYear;
    const double contributionBase = std::min(10000.0, monthlySalary);
    const double contributionRate = monthlySalary <= 1500.0 ? 0.01 : 0.02;
    return roundToCents(contributionBase * contributionRate * weeklyProration);
}

double Payroll::calculateIncomeTax(double weeklySalary, double otherDeductions) const {
    const double annualTaxableIncome = std::max(
        0.0, weeklySalary * weeksPerYear - otherDeductions * weeksPerYear);
    return roundToCents(calculateAnnualIncomeTax(annualTaxableIncome) / weeksPerYear);
}

double Payroll::calculateDeductions(double basicSalary) const {
    const double sss = calculateSssDeduction(basicSalary);
    const double philHealth = calculatePhilHealthDeduction(basicSalary);
    const double pagIbig = calculatePagIbigDeduction(basicSalary);
    const double otherDeductions = sss + philHealth + pagIbig;
    const double incomeTax = calculateIncomeTax(basicSalary, otherDeductions);
    return otherDeductions + incomeTax;
}

double Payroll::calculateNetSalary(double basicSalary, double deductions) const {
    return basicSalary - deductions;
}

void Payroll::generatePayslip() const {
    int employeeId;
    std::string employeeName;
    getDetails(employeeId, employeeName);

    const double dailyRate = getDailyRate();
    const int daysWorked = getDaysWorked();
    const double basicSalary = calculateBasicSalary(dailyRate, daysWorked);
    const double sss = calculateSssDeduction(basicSalary);
    const double philHealth = calculatePhilHealthDeduction(basicSalary);
    const double pagIbig = calculatePagIbigDeduction(basicSalary);
    const double otherDeductions = sss + philHealth + pagIbig;
    const double incomeTax = calculateIncomeTax(basicSalary, otherDeductions);
    const double deductions = otherDeductions + incomeTax;
    const double netSalary = calculateNetSalary(basicSalary, deductions);

    std::cout << "--------------------------\n"
              << "Employee ID: " << employeeId << '\n'
              << "Name: " << employeeName << '\n'
              << "Daily Rate: PHP " << std::fixed << std::setprecision(2) << dailyRate << '\n'
              << "Days Worked: " << daysWorked << '\n'
              << "Basic Salary: PHP " << basicSalary << '\n'
              << "SSS: PHP " << sss << '\n'
              << "PhilHealth: PHP " << philHealth << '\n'
              << "Pag-IBIG: PHP " << pagIbig << '\n'
              << "Withholding Tax: PHP " << incomeTax << '\n'
              << "Deductions: PHP " << deductions << '\n'
              << "Net Salary: PHP " << netSalary << '\n';
}
