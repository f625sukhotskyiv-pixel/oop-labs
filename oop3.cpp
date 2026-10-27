#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>
using namespace std;

class SalaryException
{
protected:
    string msg;
public:
    SalaryException(const string& m) : msg(m) {}
    virtual ~SalaryException() {}
    virtual string what() const { return msg; }
};

class InvalidSalaryException : public SalaryException
{
public:
    InvalidSalaryException(const string& m) : SalaryException("Помилка зарплати: " + m) {}
};

class InvalidPhoneException : public SalaryException
{
public:
    InvalidPhoneException(const string& m) : SalaryException("Помилка телефону: " + m) {}
};

class EmptyFieldException : public SalaryException
{
public:
    EmptyFieldException(const string& m) : SalaryException("Порожнє поле: " + m) {}
};

class NotFoundException : public SalaryException
{
public:
    NotFoundException(const string& m) : SalaryException("Не знайдено: " + m) {}
};

class NumberFormatException : public SalaryException
{
public:
    NumberFormatException(const string& m) : SalaryException("Некоректне число: " + m) {}
};

class Employee
{
    string lastName;
    string firstName;
    string middleName;
    string phone;
    string position;
    double salary;
public:
    Employee() : lastName("Unknown"), firstName("Unknown"), middleName("Unknown"),
                 phone("0000000000"), position("Unknown"), salary(0) {}

    Employee(const string& ln, const string& fn, const string& mn,
             const string& ph, const string& pos, double sal)
    {
        setLastName(ln);
        setFirstName(fn);
        setMiddleName(mn);
        setPhone(ph);
        setPosition(pos);
        setSalary(sal);
    }

    void setLastName(const string& v)   { checkText(v, "прізвище");    lastName = v; }
    void setFirstName(const string& v)  { checkText(v, "ім'я");        firstName = v; }
    void setMiddleName(const string& v) { checkText(v, "по батькові"); middleName = v; }
    void setPosition(const string& v)   { checkText(v, "посада");      position = v; }

    void setPhone(const string& v)
    {
        if (v.empty())
            throw InvalidPhoneException("номер не може бути порожнім");
        size_t start = (v[0] == '+') ? 1 : 0;
        if (v.size() - start < 10 || v.size() - start > 12)
            throw InvalidPhoneException("кількість цифр має бути від 10 до 12");
        for (size_t i = start; i < v.size(); i++)
            if (v[i] < '0' || v[i] > '9')
                throw InvalidPhoneException("номер має містити лише цифри");
        phone = v;
    }

    void setSalary(double v)
    {
        if (std::isnan(v) || std::isinf(v))
            throw InvalidSalaryException("значення не є скінченним числом");
        if (v < 0)
            throw InvalidSalaryException("зарплата не може бути від'ємною");
        if (v > 10000000)
            throw InvalidSalaryException("зарплата перевищує допустиму межу 10 000 000");
        salary = v;
    }

    string getLastName() const   { return lastName; }
    string getFirstName() const  { return firstName; }
    string getMiddleName() const { return middleName; }
    string getPosition() const   { return position; }

    string getPhone() const
    {
        if (phone.empty())
            throw InvalidPhoneException("номер не встановлено");
        return phone;
    }

    double getSalary() const
    {
        if (salary < 0)
            throw InvalidSalaryException("збережене значення некоректне");
        return salary;
    }

    void show() const
    {
        cout << "  " << lastName << " " << firstName << " " << middleName
             << " | тел: " << getPhone()
             << " | посада: " << position
             << " | зарплата: " << fixed << setprecision(2) << getSalary() << endl;
    }

private:
    static void checkText(const string& v, const string& field)
    {
        if (v.find_first_not_of(' ') == string::npos)
            throw EmptyFieldException(field);
    }
};

struct Bonus
{
    string lastName;
    double amount;
};

class Salary
{
    vector<Employee> staff;

    int indexOf(const string& lastName) const
    {
        for (size_t i = 0; i < staff.size(); i++)
            if (staff[i].getLastName() == lastName)
                return (int)i;
        return -1;
    }
public:
    Salary() {}
    ~Salary() {}

    int count() const { return (int)staff.size(); }

    Salary& operator+=(const Bonus& b)
    {
        int i = indexOf(b.lastName);
        if (i < 0)
            throw NotFoundException("співробітник " + b.lastName);
        if (b.amount <= 0)
            throw InvalidSalaryException("премія має бути додатною");
        staff[i].setSalary(staff[i].getSalary() + b.amount);
        return *this;
    }

    Salary operator+(const Employee& e) const
    {
        Salary result(*this);
        result.staff.push_back(e);
        return result;
    }

    Salary operator-(const string& lastName) const
    {
        Salary result(*this);
        int i = result.indexOf(lastName);
        if (i < 0)
            throw NotFoundException("співробітник " + lastName);
        result.staff.erase(result.staff.begin() + i);
        return result;
    }


    vector<Employee> find(const string& lastName) const
    {
        vector<Employee> res;
        for (size_t i = 0; i < staff.size(); i++)
            if (staff[i].getLastName() == lastName)
                res.push_back(staff[i]);
        return res;
    }

    vector<Employee> find(const string& lastName, const string& firstName) const
    {
        vector<Employee> res;
        for (size_t i = 0; i < staff.size(); i++)
            if (staff[i].getLastName() == lastName && staff[i].getFirstName() == firstName)
                res.push_back(staff[i]);
        return res;
    }

    vector<Employee> find(double minSalary, double maxSalary) const
    {
        if (minSalary > maxSalary)
            throw InvalidSalaryException("мінімум більший за максимум");
        vector<Employee> res;
        for (size_t i = 0; i < staff.size(); i++)
        {
            double s = staff[i].getSalary();
            if (s >= minSalary && s <= maxSalary)
                res.push_back(staff[i]);
        }
        return res;
    }

    vector<Employee> find(const string& position, double minSalary) const
    {
        vector<Employee> res;
        for (size_t i = 0; i < staff.size(); i++)
            if (staff[i].getPosition() == position && staff[i].getSalary() >= minSalary)
                res.push_back(staff[i]);
        return res;
    }

    void show() const
    {
        cout << "\n--- Список співробітників (" << staff.size() << ") ---" << endl;
        if (staff.empty())
            cout << "  (порожньо)" << endl;
        for (size_t i = 0; i < staff.size(); i++)
            staff[i].show();
    }
};

string readText(const string& prompt)
{
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

double readDouble(const string& prompt)
{
    string s = readText(prompt);
    try
    {
        size_t pos = 0;
        double v = stod(s, &pos);
        if (pos != s.size())
            throw NumberFormatException("'" + s + "'");
        return v;
    }
    catch (const invalid_argument&)
    {
        throw NumberFormatException("'" + s + "'");
    }
    catch (const out_of_range&)
    {
        throw NumberFormatException("'" + s + "' (занадто велике)");
    }
}

void showResult(const vector<Employee>& r)
{
    if (r.empty())
    {
        cout << "  Нікого не знайдено." << endl;
        return;
    }
    cout << "  Знайдено: " << r.size() << endl;
    for (size_t i = 0; i < r.size(); i++)
        r[i].show();
}

void printMenu()
{
    cout << "\n===== МЕНЮ =====" << endl;
    cout << "1 - Показати список" << endl;
    cout << "2 - Прийняти співробітника   (оператор +)" << endl;
    cout << "3 - Видати премію            (оператор +=)" << endl;
    cout << "4 - Звільнити співробітника  (оператор -)" << endl;
    cout << "5 - Пошук за прізвищем" << endl;
    cout << "6 - Пошук за прізвищем та ім'ям" << endl;
    cout << "7 - Пошук за діапазоном зарплати" << endl;
    cout << "8 - Пошук за посадою та мін. зарплатою" << endl;
    cout << "0 - Вихід" << endl;
    cout << "Ваш вибір: ";
}

int main()
{
    Salary payroll;

    try
    {
        payroll = payroll + Employee("Коваленко", "Іван", "Петрович", "0501234567", "Програміст", 30000);
        payroll = payroll + Employee("Шевченко", "Олена", "Миколаївна", "0677654321", "Бухгалтер", 22000);
    }
    catch (const SalaryException& e)
    {
        cout << e.what() << endl;
    }

    int choice = -1;
    while (choice != 0)
    {
        printMenu();
        string line;
        getline(cin, line);
        choice = (line.size() == 1 && line[0] >= '0' && line[0] <= '8') ? line[0] - '0' : -1;

        try
        {
            switch (choice)
            {
            case 1:
                payroll.show();
                break;

            case 2:
            {
                string ln = readText("Прізвище: ");
                string fn = readText("Ім'я: ");
                string mn = readText("По батькові: ");
                string ph = readText("Телефон (10-12 цифр): ");
                string pos = readText("Посада: ");
                double sal = readDouble("Зарплата: ");
                try
                {
                    Employee e(ln, fn, mn, ph, pos, sal);
                    payroll = payroll + e;
                    cout << "Співробітника прийнято." << endl;
                }
                catch (const InvalidPhoneException& e)
                {
                    cout << "[Внутрішній обробник] " << e.what() << endl;
                    throw;
                }
                break;
            }

            case 3:
            {
                Bonus b;
                b.lastName = readText("Прізвище: ");
                b.amount = readDouble("Сума премії: ");
                payroll += b;
                cout << "Премію нараховано." << endl;
                break;
            }

            case 4:
                payroll = payroll - readText("Прізвище звільненого: ");
                cout << "Співробітника звільнено." << endl;
                break;

            case 5:
                showResult(payroll.find(readText("Прізвище: ")));
                break;

            case 6:
            {
                string ln = readText("Прізвище: ");
                string fn = readText("Ім'я: ");
                showResult(payroll.find(ln, fn));
                break;
            }

            case 7:
            {
                double mn = readDouble("Від: ");
                double mx = readDouble("До: ");
                showResult(payroll.find(mn, mx));
                break;
            }

            case 8:
            {
                string pos = readText("Посада: ");
                double mn = readDouble("Мінімальна зарплата: ");
                showResult(payroll.find(pos, mn));
                break;
            }

            case 0:
                cout << "Завершення роботи." << endl;
                break;

            default:
                cout << "Невірний пункт меню." << endl;
            }
        }
        catch (const InvalidSalaryException& e)  { cout << "[Зовнішній обробник] " << e.what() << endl; }
        catch (const InvalidPhoneException& e)   { cout << "[Зовнішній обробник] " << e.what() << endl; }
        catch (const NotFoundException& e)       { cout << "[Зовнішній обробник] " << e.what() << endl; }
        catch (const SalaryException& e)         { cout << "[Зовнішній обробник] " << e.what() << endl; }
    }
    return 0;
}