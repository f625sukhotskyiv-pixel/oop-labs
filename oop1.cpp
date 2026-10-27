#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

class ZavdClass {
private:
    double a, b;
public:
    ZavdClass() { a = 1; b = 1; }

    double Faktr(int n) {
        double f = 1;
        if (n > 1) {
            for (int i = 2; i <= n; i++) f *= i;
        }
        return f;
    }

    void Fn_b(double x, double y, double z) {
        double term1 = x / Faktr(1);
        double term2 = pow(y, 2) / Faktr(3);
        double term3 = pow(z, 3) / Faktr(5);
        double term4 = sin(x - z) / (y - z + 1.23);

        b = 1.0 - term1 + term2 - term3 + term4;
    }

    void Fn_a(double x, double y, double z) {
        double num = 2.0 * cos(x) - (b / 6.0);
        double den = 1.0 + x * b * sin(y);
        double log_part = (1.0 / 3.0) * log(fabs((x + 1.0) / (pow(z, 2) + 1.2)));

        a = (num / den) + log_part;
    }

    double geta() { return a; }
    double getb() { return b; }
};

int main() {
    int No = 12;
    double x = 0.48 * No;
    double y = 0.47 * No;
    double z = -1.32 * No;

    ZavdClass Zavd;

    cout << "--- Завдання 1: Обчислення a та b ---" << endl;
    Zavd.Fn_b(x, y, z);
    Zavd.Fn_a(x, y, z);

    cout << "Вхiднi данi: x = " << x << ", y = " << y << ", z = " << z << endl;
    cout << "Результат a = " << Zavd.geta() << endl;
    cout << "Результат b = " << Zavd.getb() << endl << endl;

    cout << "--- Завдання 2: Табулювання (x вiд -1 до 1, крок 0.2) ---" << endl;
    cout << "x\t\ta\t\tb" << endl;
    cout << "--------------------------------------------------" << endl;

    for (double temp_x = -1.0; temp_x <= 1.01; temp_x += 0.25) {
        Zavd.Fn_b(temp_x, y, z);
        Zavd.Fn_a(temp_x, y, z);

        cout << fixed << setprecision(1) << temp_x << "\t\t"
            << setprecision(4) << Zavd.geta() << "\t\t"
            << setprecision(4) << Zavd.getb() << endl;
    }

    return 0;
}