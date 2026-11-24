#include <iostream>
#include <limits>
#include <clocale>
#ifdef _WIN32
#include <Windows.h>
#endif
using namespace std;

class List
{
    struct Node
    {
        int data;
        Node* next;
        Node(int d, Node* n) : data(d), next(n) {}
    };
    Node* head;
    int count;

    void copyFrom(const List& other)
    {
        Node* tail = nullptr;
        for (Node* p = other.head; p != nullptr; p = p->next)
        {
            Node* n = new Node(p->data, nullptr);
            if (tail) tail->next = n; else head = n;
            tail = n;
            count++;
        }
    }
public:
    List() : head(nullptr), count(0) {}
    List(const List& other) : head(nullptr), count(0) { copyFrom(other); }
    List& operator=(const List& other)
    {
        if (this != &other) { clear(); copyFrom(other); }
        return *this;
    }
    ~List() { clear(); }

    void addFirst(int value)
    {
        head = new Node(value, head);
        count++;
    }
    bool removeFirst(int& value)
    {
        if (!head) return false;
        Node* tmp = head;
        value = tmp->data;
        head = head->next;
        delete tmp;
        count--;
        return true;
    }
    bool getFirst(int& value) const
    {
        if (!head) return false;
        value = head->data;
        return true;
    }
    bool isEmpty() const { return head == nullptr; }
    int getCount() const { return count; }
    void clear()
    {
        int tmp;
        while (removeFirst(tmp)) {}
    }
    void show() const
    {
        if (!head) { cout << "\t[порожньо]" << endl; return; }
        for (Node* p = head; p != nullptr; p = p->next)
            cout << "\t" << p->data << endl;
    }
};

class Stack
{
    List items;
public:
    Stack() { cout << "\t(створено порожній стек)" << endl; }
    ~Stack() { cout << "\t(стек знищено)" << endl; }

    void push(int value) { items.addFirst(value); }
    bool pop(int& value) { return items.removeFirst(value); }
    bool peek(int& value) const { return items.getFirst(value); }
    bool isEmpty() const { return items.isEmpty(); }
    int size() const { return items.getCount(); }
    void show() const
    {
        cout << "\nВміст стека (вершина зверху), елементів: " << size() << endl;
        items.show();
    }
};

int readInt(const char* prompt)
{
    int v;
    while (true)
    {
        cout << prompt;
        if (cin >> v) break;
        cout << "\tНЕКОРЕКТНИЙ ВВІД! Очікується ціле число." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return v;
}

int main()
{
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#else
    setlocale(LC_ALL, "");
#endif
    Stack st;
    int choice;
    do
    {
        cout << "\n===== МЕНЮ =====" << endl;
        cout << "1 - Занести елемент у стек" << endl;
        cout << "2 - Прочитати елемент зі стека (з видаленням)" << endl;
        cout << "3 - Переглянути вершину стека" << endl;
        cout << "4 - Вивести вміст стека" << endl;
        cout << "0 - Вихід" << endl;
        choice = readInt("Ваш вибір: ");

        int v;
        switch (choice)
        {
        case 1:
            v = readInt("\tВведіть число: ");
            st.push(v);
            break;
        case 2:
            if (st.pop(v)) cout << "\tПрочитано: " << v << endl;
            else cout << "\tСтек порожній!" << endl;
            break;
        case 3:
            if (st.peek(v)) cout << "\tВершина: " << v << endl;
            else cout << "\tСтек порожній!" << endl;
            break;
        case 4:
            st.show();
            break;
        case 0:
            break;
        default:
            cout << "\tНевірний пункт меню!" << endl;
        }
    } while (choice != 0);
    return 0;
}
