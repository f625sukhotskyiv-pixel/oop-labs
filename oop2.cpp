#include <iostream>
#include <string>
using namespace std;

class ConsolIO
{
public:
    static int GetInt(istream& is)
    {
        int value;
        while (true)
        {
            is >> value;
            if (is.good() && is.peek() == '\n' && value >= 0)
            {
                is.get();
                return value;
            }
            cout << "\n\tНЕКОРЕКТНИЙ ВВІД! Очікується ціле число >= 0. Повторіть: ";
            is.clear();
            is.ignore(10000, '\n');
        }
    }

    static string GetText(istream& is)
    {
        string s;
        while (true)
        {
            getline(is, s);
            if (s.find_first_not_of(' ') != string::npos)
                return s;
            cout << "\n\tНЕКОРЕКТНИЙ ВВІД! Очікується текст. Повторіть: ";
        }
    }
};

class Department
{
    string headName;
    int employees;
public:
    Department() : headName("Unknown"), employees(0) {}
    Department(string head, int count) : headName(head), employees(count) {}
    ~Department() {}

    string getHeadName() const { return headName; }
    void setHeadName(const string& value) { headName = value; }
    int getEmployees() const { return employees; }
    void setEmployees(int value) { employees = value; }

    void input()
    {
        cout << "\t\tПрізвище завідувача кафедри: ";
        headName = ConsolIO::GetText(cin);
        cout << "\t\tКількість співробітників: ";
        employees = ConsolIO::GetInt(cin);
    }

    void show() const
    {
        cout << "\t\tЗавідувач: " << headName
             << ", співробітників: " << employees << endl;
    }
};

class Institute
{
    string directorName;   
    int depCount;
    Department* departments;
public:
    Institute() : directorName("Unknown"), depCount(0), departments(nullptr) {}

    Institute(const Institute& other)
        : directorName(other.directorName), depCount(other.depCount), departments(nullptr)
    {
        if (depCount > 0)
        {
            departments = new Department[depCount];
            for (int i = 0; i < depCount; i++)
                departments[i] = other.departments[i];
        }
    }

    Institute& operator=(const Institute& other)
    {
        if (this != &other)
        {
            delete[] departments;
            directorName = other.directorName;
            depCount = other.depCount;
            departments = nullptr;
            if (depCount > 0)
            {
                departments = new Department[depCount];
                for (int i = 0; i < depCount; i++)
                    departments[i] = other.departments[i];
            }
        }
        return *this;
    }

    ~Institute() { delete[] departments; }

    string getDirectorName() const { return directorName; }
    void setDirectorName(const string& value) { directorName = value; }
    int getDepCount() const { return depCount; }

    void input()
    {
        cout << "\tПрізвище директора інституту: ";
        directorName = ConsolIO::GetText(cin);
        cout << "\tКількість кафедр: ";
        int n = ConsolIO::GetInt(cin);

        delete[] departments;
        depCount = n;
        departments = (n > 0) ? new Department[n] : nullptr;

        for (int i = 0; i < depCount; i++)
        {
            cout << "\t  Кафедра №" << i + 1 << ":" << endl;
            departments[i].input();
        }
    }

    void show() const
    {
        cout << "\tДиректор: " << directorName
             << ", кафедр: " << depCount << endl;
        for (int i = 0; i < depCount; i++)
            departments[i].show();
    }

    int totalEmployees() const
    {
        int sum = 0;
        for (int i = 0; i < depCount; i++)
            sum += departments[i].getEmployees();
        return sum;
    }
};

class University
{
    string rectorName;
    int instCount;
    Institute* institutes;
public:
    University() : rectorName("Unknown"), instCount(0), institutes(nullptr) {}
    ~University() { delete[] institutes; }
    University(const University&) = delete;
    University& operator=(const University&) = delete;

    string getRectorName() const { return rectorName; }
    void setRectorName(const string& value) { rectorName = value; }

    void input()
    {
        cout << "Прізвище ректора: ";
        rectorName = ConsolIO::GetText(cin);
        cout << "Кількість інститутів: ";
        int n = ConsolIO::GetInt(cin);

        delete[] institutes;
        instCount = n;
        institutes = (n > 0) ? new Institute[n] : nullptr;

        for (int i = 0; i < instCount; i++)
        {
            cout << "\nІнститут №" << i + 1 << ":" << endl;
            institutes[i].input();
        }
    }

    void show() const
    {
        cout << "\n===== УНІВЕРСИТЕТ =====" << endl;
        cout << "Ректор: " << rectorName << endl;
        for (int i = 0; i < instCount; i++)
        {
            cout << "\nІнститут №" << i + 1 << ":" << endl;
            institutes[i].show();
        }
    }

    int totalEmployees() const
    {
        int sum = 0;
        for (int i = 0; i < instCount; i++)
            sum += institutes[i].totalEmployees();
        return sum;
    }
};

int main()
{
    University univ;
    univ.input();
    univ.show();
    cout << "\nЗагальна кількість співробітників університету: "
         << univ.totalEmployees() << endl;
    return 0;
}