
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>
int replaceByValue(T* arr, int size, const T& oldValue, const T& newValue) {
    if (arr == nullptr || size <= 0) {
        throw std::invalid_argument("Некоректний вказівник на масив або його розмір.");
    }
    int replacedCount = 0;
    for (int i = 0; i < size; ++i) {
        if (arr[i] == oldValue) {
            arr[i] = newValue;
            ++replacedCount;
        }
    }
    return replacedCount;
}

template <typename T>
void replaceAtIndex(T* arr, int size, int index, const T& newValue) {
    if (arr == nullptr || size <= 0) {
        throw std::invalid_argument("Некоректний вказівник на масив або його розмір.");
    }
    if (index < 0 || index >= size) {
        throw std::out_of_range("Індекс елемента виходить за межі масиву.");
    }
    arr[index] = newValue;
}

template <typename T>
void printArray(const T* arr, int size, const std::string& label = "") {
    if (!label.empty()) {
        std::cout << label << ": ";
    }
    std::cout << "[ ";
    for (int i = 0; i < size; ++i) {
        std::cout << arr[i] << (i < size - 1 ? ", " : " ");
    }
    std::cout << "]\n";
}

template <typename T>
class FileHandler {
protected:
    std::fstream fileStream;
    std::string fileName;

public:
    FileHandler() = default;
    explicit FileHandler(const std::string& name) : fileName(name) {}

    virtual ~FileHandler() {
        close();
    }

    virtual bool open(const std::string& name, std::ios::openmode mode) {
        close();
        fileName = name;
        fileStream.open(fileName, mode);
        return fileStream.is_open();
    }

    virtual bool write(const std::vector<T>& data) = 0;

    virtual bool read(std::vector<T>& data) = 0;

    void close() {
        if (fileStream.is_open()) {
            fileStream.close();
        }
    }

    std::string getFileName() const {
        return fileName;
    }
};

template <typename T>
class TextFileHandler : public FileHandler<T> {
public:
    using FileHandler<T>::FileHandler;

    bool open(const std::string& name, std::ios::openmode mode) override {
        return FileHandler<T>::open(name, mode);
    }

    bool write(const std::vector<T>& data) override {
        if (!this->fileStream.is_open()) {
            return false;
        }
        for (const auto& item : data) {
            this->fileStream << item << "\n";
        }
        this->fileStream.flush();
        return !this->fileStream.fail();
    }

    bool read(std::vector<T>& data) override {
        if (!this->fileStream.is_open()) {
            return false;
        }
        data.clear();
        T item;
        while (this->fileStream >> item) {
            data.push_back(item);
        }
        return this->fileStream.eof() || !this->fileStream.fail();
    }
};

template <typename T>
class BinaryFileHandler : public FileHandler<T> {
    static_assert(std::is_trivially_copyable<T>::value,
                  "BinaryFileHandler підтримує лише тривіально копійовані типи.");

public:
    using FileHandler<T>::FileHandler;

    bool open(const std::string& name, std::ios::openmode mode) override {
        return FileHandler<T>::open(name, mode | std::ios::binary);
    }

    bool write(const std::vector<T>& data) override {
        if (!this->fileStream.is_open()) {
            return false;
        }
        std::size_t count = data.size();
        this->fileStream.write(reinterpret_cast<const char*>(&count), sizeof(count));
        if (count > 0) {
            this->fileStream.write(reinterpret_cast<const char*>(data.data()),
                                   static_cast<std::streamsize>(count * sizeof(T)));
        }
        this->fileStream.flush();
        return !this->fileStream.fail();
    }

    bool read(std::vector<T>& data) override {
        if (!this->fileStream.is_open()) {
            return false;
        }
        data.clear();
        std::size_t count = 0;
        this->fileStream.read(reinterpret_cast<char*>(&count), sizeof(count));
        if (this->fileStream.fail()) {
            return false;
        }
        data.resize(count);
        if (count > 0) {
            this->fileStream.read(reinterpret_cast<char*>(data.data()),
                                  static_cast<std::streamsize>(count * sizeof(T)));
        }
        return !this->fileStream.fail();
    }
};

template <typename T>
void demonstrateFileOperations(FileHandler<T>& handler,
                               const std::string& filename,
                               const std::vector<T>& sourceData,
                               const std::string& modeDescription) {
    std::cout << "--- " << modeDescription << " (" << filename << ") ---\n";

    if (!handler.open(filename, std::ios::out | std::ios::trunc)) {
        std::cerr << "Помилка відкривання файла для запису!\n";
        return;
    }
    if (handler.write(sourceData)) {
        std::cout << "Дані успішно записано у файл.\n";
    }
    handler.close();

    std::vector<T> loadedData;
    if (!handler.open(filename, std::ios::in)) {
        std::cerr << "Помилка відкривання файла для читання!\n";
        return;
    }
    if (handler.read(loadedData)) {
        std::cout << "Зчитані з файла дані: [ ";
        for (std::size_t i = 0; i < loadedData.size(); ++i) {
            std::cout << loadedData[i] << (i < loadedData.size() - 1 ? ", " : " ");
        }
        std::cout << "]\n";
    }
    handler.close();
}

int main() {
    try {
        std::cout << "========================================================\n";
        std::cout << "КОМПЛЕКТ ВХІДНИХ ДАНИХ №1 (Тип: int)\n";
        std::cout << "========================================================\n";

        const int sizeInt = 6;
        int arrInt[sizeInt] = {12, 45, 7, 45, 89, 23};

        printArray(arrInt, sizeInt, "Початковий масив");
        int countInt = replaceByValue(arrInt, sizeInt, 45, 99);
        std::cout << "Заміна значення 45 -> 99 (виконано замін: " << countInt << ")\n";
        printArray(arrInt, sizeInt, "Після заміни за значенням");

        replaceAtIndex(arrInt, sizeInt, 2, 50);
        printArray(arrInt, sizeInt, "Після заміни індексу [2] на 50");

        std::vector<int> vecInt(arrInt, arrInt + sizeInt);
        TextFileHandler<int> textIntHandler;
        BinaryFileHandler<int> binIntHandler;

        demonstrateFileOperations(textIntHandler, "data_int.txt", vecInt, "Текстовий файл");
        demonstrateFileOperations(binIntHandler, "data_int.bin", vecInt, "Бінарний файл");

        std::cout << "\n========================================================\n";
        std::cout << "КОМПЛЕКТ ВХІДНИХ ДАНИХ №2 (Тип: double)\n";
        std::cout << "========================================================\n";

        const int sizeDouble = 5;
        double arrDouble[sizeDouble] = {36.6, 37.2, 36.6, 38.0, 36.9};

        printArray(arrDouble, sizeDouble, "Початковий масив");
        int countDouble = replaceByValue(arrDouble, sizeDouble, 36.6, 36.5);
        std::cout << "Заміна значення 36.6 -> 36.5 (виконано замін: " << countDouble << ")\n";
        printArray(arrDouble, sizeDouble, "Після заміни за значенням");

        replaceAtIndex(arrDouble, sizeDouble, 3, 37.5);
        printArray(arrDouble, sizeDouble, "Після заміни індексу [3] на 37.5");

        std::vector<double> vecDouble(arrDouble, arrDouble + sizeDouble);
        TextFileHandler<double> textDoubleHandler;
        BinaryFileHandler<double> binDoubleHandler;

        demonstrateFileOperations(textDoubleHandler, "data_double.txt", vecDouble, "Текстовий файл");
        demonstrateFileOperations(binDoubleHandler, "data_double.bin", vecDouble, "Бінарний файл");

    } catch (const std::exception& ex) {
        std::cerr << "Виняток під час виконання: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
