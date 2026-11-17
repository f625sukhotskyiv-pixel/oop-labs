
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <clocale>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

class Employee {
protected:
    int id;
    string fullName;
    string position;

public:
    Employee(int id = 0, const string& fullName = "Невідомо", const string& position = "Працівник")
        : id(id), fullName(fullName), position(position) {}

    virtual ~Employee() = default;

    virtual double calculateSalary() const {
        return 0.0;
    }

    virtual void printInfo() const {
        cout << "Табельний №: " << id << "\n"
             << "ПІБ:         " << fullName << "\n"
             << "Посада:      " << position << "\n"
             << "Нараховано:  " << fixed << setprecision(2)
             << calculateSalary() << " грн\n";
    }
};

class HourlyEmployee : public Employee {
private:
    double hourlyRate;
    double hoursWorked;

public:
    HourlyEmployee(int id, const string& fullName, const string& position,
                   double hourlyRate, double hoursWorked)
        : Employee(id, fullName, position),
          hourlyRate(hourlyRate >= 0.0 ? hourlyRate : 0.0),
          hoursWorked(hoursWorked >= 0.0 ? hoursWorked : 0.0) {}

    double calculateSalary() const override {
        return hourlyRate * hoursWorked;
    }

    void printInfo() const override {
        cout << "[Службовець з погодинною оплатою]\n";
        Employee::printInfo();
        cout << fixed << setprecision(2)
             << "Деталі:      " << hoursWorked << " год * "
             << hourlyRate << " грн/год\n";
    }
};

class SalariedEmployee : public Employee {
private:
    double monthlySalary;
    double bonusPercent;

public:
    SalariedEmployee(int id, const string& fullName, const string& position,
                     double monthlySalary, double bonusPercent = 0.0)
        : Employee(id, fullName, position),
          monthlySalary(monthlySalary >= 0.0 ? monthlySalary : 0.0),
          bonusPercent(bonusPercent >= 0.0 ? bonusPercent : 0.0) {}

    double calculateSalary() const override {
        return monthlySalary * (1.0 + bonusPercent / 100.0);
    }

    void printInfo() const override {
        cout << "[Службовець з окладом]\n";
        Employee::printInfo();
        cout << fixed << setprecision(2)
             << "Деталі:      оклад " << monthlySalary << " грн, премія "
             << bonusPercent << "%\n";
    }
};

void processPayroll(const string& setTitle, const vector<Employee*>& staff) {
    cout << "========================================================\n";
    cout << setTitle << "\n";
    cout << "========================================================\n";

    double totalPayroll = 0.0;
    for (const Employee* emp : staff) {
        emp->printInfo();
        totalPayroll += emp->calculateSalary();
        cout << "--------------------------------------------------------\n";
    }

    cout << fixed << setprecision(2);
    cout << "Загальний фонд оплати праці: " << totalPayroll << " грн\n\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    setlocale(LC_ALL, "");

    HourlyEmployee demoHourly(100, "Демо Працівник", "Розробник", 300.0, 10.0);
    Employee* basePtr = &demoHourly;
    Employee& baseRef = demoHourly;

    cout << "=== Демонстрація механізму зв'язування ===\n" << fixed << setprecision(2);
    cout << "Виклик через об'єкт (раннє зв'язування):     " << demoHourly.calculateSalary() << " грн\n";
    cout << "Виклик через вказівник (пізнє зв'язування):  " << basePtr->calculateSalary() << " грн\n";
    cout << "Виклик через посилання (пізнє зв'язування):  " << baseRef.calculateSalary() << " грн\n\n";

    vector<Employee*> dataset1 = {
        new HourlyEmployee(101, "Коваленко Олександр Ігорович", "Junior C++ Developer", 320.0, 140.0),
        new SalariedEmployee(102, "Мельник Ірина Василівна", "Project Manager", 52000.0, 15.0)
    };
    processPayroll("КОМПЛЕКТ ВХІДНИХ ДАНИХ №1 (Відділ розробки)", dataset1);

    vector<Employee*> dataset2 = {
        new HourlyEmployee(201, "Шевченко Тарас Андрійович", "QA Automation Engineer", 450.0, 168.0),
        new SalariedEmployee(202, "Бондаренко Софія Олегівна", "System Analyst", 48000.0, 10.0),
        new HourlyEmployee(203, "Ткаченко Максим Петрович", "Technical Support Specialist", 250.0, 96.0)
    };
    processPayroll("КОМПЛЕКТ ВХІДНИХ ДАНИХ №2 (Відділ тестування та підтримки)", dataset2);

    for (Employee* emp : dataset1) delete emp;
    for (Employee* emp : dataset2) delete emp;

    return 0;
}
