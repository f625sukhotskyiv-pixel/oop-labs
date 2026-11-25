#define NOMINMAX
#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

class DictException : public std::exception {
protected:
    std::string message_;
public:
    explicit DictException(const std::string& msg) : message_(msg) {}
    const char* what() const noexcept override { return message_.c_str(); }
};

// Звернення до поля-вказівника, який не ініціалізований
class NullPointerException : public DictException {
public:
    explicit NullPointerException(const std::string& field)
        : DictException("Звернення до неініціалізованого поля-вказівника: " + field) {}
};

class WordNotFoundException : public DictException {
public:
    explicit WordNotFoundException(const std::string& word)
        : DictException("Слово \"" + word + "\" не знайдено у словнику") {}
};

class DuplicateWordException : public DictException {
public:
    explicit DuplicateWordException(const std::string& word)
        : DictException("Слово \"" + word + "\" вже є у словнику") {}
};

class EmptyWordException : public DictException {
public:
    EmptyWordException()
        : DictException("Слово або переклад не може бути порожнім рядком") {}
};

int compareAlphabetical(const std::string& a, const std::string& b);

class Slovo {
private:
    std::string* ukr_;
    std::string* eng_;

public:
    Slovo();
    Slovo(const std::string& ukr, const std::string& eng);
    Slovo(const Slovo& other);
    Slovo& operator=(const Slovo& other);
    ~Slovo();

    bool isInitialized() const;

    const std::string& getUkr() const;
    const std::string& getEng() const;

    void setUkr(const std::string& ukr);
    void setEng(const std::string& eng);

    bool operator<(const Slovo& other) const;
    bool operator>=(const Slovo& other) const;
    bool operator==(const Slovo& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Slovo& w);
};

enum class Language { Ukrainian, English };

class Slovnyk {
private:
    std::vector<Slovo> words_;
    int indexOf(const std::string& ukr) const;

public:
    Slovnyk() = default;

    Slovnyk operator+(const Slovo& w) const;
    Slovnyk operator-(const Slovo& w) const;

    void sort();
    void sort(Language lang);

    const Slovo& search(const std::string& word,
                        Language lang = Language::Ukrainian) const;

    void print() const;

    std::size_t size() const;
};

static std::u32string decodeUtf8(const std::string& s) {
    std::u32string result;
    for (std::size_t i = 0; i < s.size();) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        char32_t cp;
        int len;
        if (c < 0x80)              { cp = c;        len = 1; }
        else if ((c >> 5) == 0x06) { cp = c & 0x1F; len = 2; }
        else if ((c >> 4) == 0x0E) { cp = c & 0x0F; len = 3; }
        else if ((c >> 3) == 0x1E) { cp = c & 0x07; len = 4; }
        else                       { cp = c;        len = 1; }
        for (int k = 1; k < len && i + k < s.size(); ++k)
            cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3F);
        result.push_back(cp);
        i += len;
    }
    return result;
}

static char32_t toLowerChar(char32_t c) {
    if (c >= U'A' && c <= U'Z')     return c + 32;
    if (c >= 0x0410 && c <= 0x042F) return c + 32;
    switch (c) {
        case 0x0490: return 0x0491;
        case 0x0404: return 0x0454;
        case 0x0406: return 0x0456;
        case 0x0407: return 0x0457;
    }
    return c;
}

static const std::u32string UKR_ALPHABET =
    U"\u0430\u0431\u0432\u0433\u0491\u0434\u0435\u0454\u0436\u0437\u0438"
    U"\u0456\u0457\u0439\u043A\u043B\u043C\u043D\u043E\u043F\u0440\u0441"
    U"\u0442\u0443\u0444\u0445\u0446\u0447\u0448\u0449\u044C\u044E\u044F";

static int charRank(char32_t c) {
    c = toLowerChar(c);
    if (c < 0x80) return static_cast<int>(c);
    std::size_t pos = UKR_ALPHABET.find(c);
    if (pos != std::u32string::npos) return 0x100 + static_cast<int>(pos);
    return 0x200 + static_cast<int>(c);
}

int compareAlphabetical(const std::string& a, const std::string& b) {
    std::u32string ua = decodeUtf8(a);
    std::u32string ub = decodeUtf8(b);
    std::size_t n = std::min(ua.size(), ub.size());
    for (std::size_t i = 0; i < n; ++i) {
        int ra = charRank(ua[i]);
        int rb = charRank(ub[i]);
        if (ra != rb) return ra < rb ? -1 : 1;
    }
    if (ua.size() == ub.size()) return 0;
    return ua.size() < ub.size() ? -1 : 1;
}


Slovo::Slovo() : ukr_(nullptr), eng_(nullptr) {}

Slovo::Slovo(const std::string& ukr, const std::string& eng)
    : ukr_(nullptr), eng_(nullptr) {
    try {
        setUkr(ukr);
        setEng(eng);
    }
    catch (...) {
        delete ukr_;
        delete eng_;
        throw;
    }
}

Slovo::Slovo(const Slovo& other)
    : ukr_(other.ukr_ ? new std::string(*other.ukr_) : nullptr),
      eng_(other.eng_ ? new std::string(*other.eng_) : nullptr) {}

Slovo& Slovo::operator=(const Slovo& other) {
    if (this != &other) {
        std::string* u = other.ukr_ ? new std::string(*other.ukr_) : nullptr;
        std::string* e = other.eng_ ? new std::string(*other.eng_) : nullptr;
        delete ukr_;
        delete eng_;
        ukr_ = u;
        eng_ = e;
    }
    return *this;
}

Slovo::~Slovo() {
    delete ukr_;
    delete eng_;
}

bool Slovo::isInitialized() const {
    return ukr_ != nullptr && eng_ != nullptr;
}

const std::string& Slovo::getUkr() const {
    if (ukr_ == nullptr) throw NullPointerException("українське слово");
    return *ukr_;
}

const std::string& Slovo::getEng() const {
    if (eng_ == nullptr) throw NullPointerException("англійський переклад");
    return *eng_;
}

void Slovo::setUkr(const std::string& ukr) {
    if (ukr.empty()) throw EmptyWordException();
    if (ukr_) *ukr_ = ukr;
    else      ukr_ = new std::string(ukr);
}

void Slovo::setEng(const std::string& eng) {
    if (eng.empty()) throw EmptyWordException();
    if (eng_) *eng_ = eng;
    else      eng_ = new std::string(eng);
}

bool Slovo::operator<(const Slovo& other) const {
    return compareAlphabetical(getUkr(), other.getUkr()) < 0;
}

bool Slovo::operator>=(const Slovo& other) const {
    return compareAlphabetical(getUkr(), other.getUkr()) >= 0;
}

bool Slovo::operator==(const Slovo& other) const {
    return compareAlphabetical(getUkr(), other.getUkr()) == 0;
}

static std::string show(const std::string* p) {
    return p ? *p : std::string("<не ініціалізовано>");
}

std::ostream& operator<<(std::ostream& os, const Slovo& w) {
    os << show(w.ukr_) << " - " << show(w.eng_);
    return os;
}


int Slovnyk::indexOf(const std::string& ukr) const {
    for (std::size_t i = 0; i < words_.size(); ++i)
        if (compareAlphabetical(words_[i].getUkr(), ukr) == 0)
            return static_cast<int>(i);
    return -1;
}

Slovnyk Slovnyk::operator+(const Slovo& w) const {
    const std::string& key = w.getUkr();
    (void)w.getEng();
    if (indexOf(key) != -1) throw DuplicateWordException(key);

    Slovnyk result(*this);
    result.words_.push_back(w);
    return result;
}

Slovnyk Slovnyk::operator-(const Slovo& w) const {
    const std::string& key = w.getUkr();
    int idx = indexOf(key);
    if (idx < 0) throw WordNotFoundException(key);

    Slovnyk result(*this);
    result.words_.erase(result.words_.begin() + idx);
    return result;
}

void Slovnyk::sort() {
    sort(Language::Ukrainian);
}

void Slovnyk::sort(Language lang) {
    std::sort(words_.begin(), words_.end(),
        [lang](const Slovo& a, const Slovo& b) {
            if (lang == Language::Ukrainian) return a < b;
            return compareAlphabetical(a.getEng(), b.getEng()) < 0;
        });
}

const Slovo& Slovnyk::search(const std::string& word, Language lang) const {
    for (const Slovo& w : words_) {
        const std::string& key = (lang == Language::Ukrainian) ? w.getUkr() : w.getEng();
        if (compareAlphabetical(key, word) == 0) return w;
    }
    throw WordNotFoundException(word);
}

void Slovnyk::print() const {
    if (words_.empty()) {
        std::cout << "  (словник порожній)\n";
        return;
    }
    int n = 1;
    for (const Slovo& w : words_)
        std::cout << "  " << n++ << ". " << w << '\n';
}

std::size_t Slovnyk::size() const {
    return words_.size();
}


using namespace std;

template <typename Action>
void tryRun(const string& title, Action action) {
    cout << "-> " << title << '\n';
    try {
        action();
    }
    catch (const NullPointerException& e)   { cout << "   [NullPointerException] "   << e.what() << '\n'; }
    catch (const WordNotFoundException& e)  { cout << "   [WordNotFoundException] "  << e.what() << '\n'; }
    catch (const DuplicateWordException& e) { cout << "   [DuplicateWordException] " << e.what() << '\n'; }
    catch (const EmptyWordException& e)     { cout << "   [EmptyWordException] "     << e.what() << '\n'; }
    catch (const DictException& e)          { cout << "   [DictException] "          << e.what() << '\n'; }
    catch (const exception& e)              { cout << "   [std::exception] "         << e.what() << '\n'; }
}


void processDataSet(const string& title, Slovnyk dict,
                    const string& findUkr, const string& findEng,
                    const Slovo& removeWord) {
    cout << "\n===== " << title << " =====\n";
    cout << "Словник до сортування:\n";
    dict.print();

    dict.sort();
    cout << "\nСортування за українським словом:\n";
    dict.print();

    dict.sort(Language::English);
    cout << "\nСортування за англійським перекладом:\n";
    dict.print();

    cout << '\n';
    tryRun("Пошук за українським словом \"" + findUkr + "\"", [&] {
        cout << "   Знайдено: " << dict.search(findUkr) << '\n';
    });
    tryRun("Пошук за англійським перекладом \"" + findEng + "\"", [&] {
        cout << "   Знайдено: " << dict.search(findEng, Language::English) << '\n';
    });
    tryRun("Видалення слова (операція -)", [&] {
        dict = dict - removeWord;
        cout << "   Видалено: " << removeWord << "\n   Слів у словнику: " << dict.size() << '\n';
    });
    dict.print();
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    Slovnyk set1;
    set1 = set1 + Slovo("яблуко", "apple")
                + Slovo("їжак", "hedgehog")
                + Slovo("банан", "banana")
                + Slovo("ґанок", "porch")
                + Slovo("дім", "house")
                + Slovo("єнот", "raccoon")
                + Slovo("гриб", "mushroom");
    processDataSet("Комплект даних 1", set1, "гриб", "house", Slovo("банан", "banana"));

    Slovnyk set2;
    set2 = set2 + Slovo("книга", "book")
                + Slovo("ліс", "forest")
                + Slovo("вода", "water")
                + Slovo("сонце", "sun")
                + Slovo("небо", "sky")
                + Slovo("мова", "language");
    processDataSet("Комплект даних 2", set2, "небо", "water", Slovo("ліс", "forest"));

    cout << "\n===== Порівняння слів (<, >=) =====\n" << boolalpha;
    Slovo a("ґанок", "porch"), b("дім", "house"), c("єнот", "raccoon"), d("їжак", "hedgehog");
    cout << a.getUkr() << " < "  << b.getUkr() << " : " << (a < b)  << '\n';
    cout << c.getUkr() << " >= " << d.getUkr() << " : " << (c >= d) << '\n';
    cout << d.getUkr() << " >= " << c.getUkr() << " : " << (d >= c) << '\n';

    cout << "\n===== Виняткові ситуації =====\n";
    Slovnyk dict = set1;

    tryRun("Читання неініціалізованого поля-вказівника", [] {
        Slovo empty;
        cout << empty.getUkr();
    });
    tryRun("Слово без перекладу (eng_ = nullptr)", [] {
        Slovo half;
        half.setUkr("вікно");
        cout << half.getEng();
    });
    tryRun("Порожній переклад у конструкторі", [] {
        Slovo bad("кіт", "");
        (void)bad;
    });
    tryRun("Додавання слова, яке вже є у словнику", [&] {
        dict = dict + Slovo("дім", "house");
    });
    tryRun("Додавання неініціалізованого слова", [&] {
        dict = dict + Slovo();
    });
    tryRun("Видалення відсутнього слова", [&] {
        dict = dict - Slovo("кіт", "cat");
    });
    tryRun("Пошук відсутнього слова", [&] {
        cout << dict.search("зебра");
    });

    return 0;
}
