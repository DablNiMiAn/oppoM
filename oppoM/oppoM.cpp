#include <sstream>  
#include <iostream>
#include <fstream>  
#include <string>  
#include <ctime>  
#include <iomanip>  
#include <vector>  
#include <algorithm>

class files {
private:
    std::string file_name;
    tm creation_date; // Используем tm для хранения даты  
    int size;

public:
    files(const std::string& filename, const tm& creation_date, int size)
        : file_name(filename), creation_date(creation_date), size(size) {}

    void display(std::ostream& os) const { // Используем ostream для переменной вывода  
        os << "Имя файла: " << file_name << "\t";
        os << "Дата создания: " << std::put_time(&creation_date, "%Y.%m.%d") << "\t";
        os << "Размер: " << size << " Байт" << std::endl;
    }


    const std::string& getFilename() const {
        return file_name;
    }

    const tm& getCreationDate() const {
        return creation_date;
    }

    int getSize() const {
        return size;
    }
};

tm parseDate(const std::string& dateStr) {
    tm date = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&date, "%Y.%m.%d");
    if (ss.fail()) {
        throw std::runtime_error("Неправильный формат даты");
    }
    return date;
}

files createfiles(const std::string& description) {
    std::istringstream stream(description);
    std::string filename;
    std::string dateStr;
    int size = 0;
    std::string temp;

    bool filenameSet = false;
    bool dateSet = false;
    bool sizeSet = false;

    while (stream >> temp) {
        if (temp.front() == '\"') {
            filename = temp.substr(1, temp.size() - 2); // Удаляем кавычки   
            filenameSet = true;
        }
        else if (temp.find('.') != std::string::npos && temp.size() <= 10) {
            dateStr = temp;
            dateSet = true;
        }
        else if (isdigit(temp[0])) {
            size = stoi(temp);
            sizeSet = true;
        }
    }

    if (!filenameSet or !dateSet or !sizeSet or size <= 0) {
        throw std::runtime_error("Invalid object description");
    }

    tm creation_date = parseDate(dateStr);
    return files(filename, creation_date, size);
}

int main() {
    setlocale(0, "ru");
    std::ifstream inputFile("in.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Error: Не удалось открыть файл 'in.txt'.\n";
        return 1;
    }

    std::vector<files> fileList;
    std::string description;

    while (getline(inputFile, description)) {
        try {
            files obj = createfiles(description);
            fileList.push_back(obj);
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    inputFile.close();

    int choice;
    std::cout << "Выберите метод сортировки:\n"
        << "1)По размеру файла (largest to smallest)\n"
        << "2) По дате создания\n"
        << "3) Без сортировки\n"
        << "4) Вывод по временному промежутку\n"
        << "5) Вывод по промежутку размеров\n"
        << "Выберите цифру (1-5): ";
    std::cin >> choice;

    switch (choice) {
    case 1:
        sort(fileList.begin(), fileList.end(), [](const files& a, const files& b) {
            return a.getSize() < b.getSize(); // Сортировка по размеру от большего к меньшему  
            });
        for (const auto& file : fileList) {
            file.display(std::cout); // Выводим на экран  
        }
        break;
    case 2:
        sort(fileList.begin(), fileList.end(), [](const files& a, const files& b) {
            return mktime(const_cast<tm*>(&a.getCreationDate())) < mktime(const_cast<tm*>(&b.getCreationDate())); // Сортировка по дате создания  
            });
        for (const auto& file : fileList) {
            file.display(std::cout); // Выводим на экран  
        }
        break;
    case 3:
        for (const auto& file : fileList) {
            file.display(std::cout); // Выводим на экран  
        }
        // Выводим в том порядке, в каком есть  
        break;
    case 4:
    {
        std::string start_date_str, end_date_str;
        std::cout << "Введите начальную дату (ГГГГ.ММ.ДД): ";
        std::cin >> start_date_str;
        std::cout << "Введите конечную дату (ГГГГ.ММ.ДД): ";
        std::cin >> end_date_str;

        tm start_date = parseDate(start_date_str);
        tm end_date = parseDate(end_date_str);

        // Фильтрация файлов по дате
        std::vector<files> filteredFileList;
        for (const auto& file : fileList) {
            tm fileDate = file.getCreationDate();
            if (mktime(&fileDate) >= mktime(&start_date) && mktime(&fileDate) <= mktime(&end_date)) {
                filteredFileList.push_back(file);
            }
        }

        // Вывод отфильтрованных файлов
        if (filteredFileList.empty()) {
            std::cout << "Файлов, созданных в указанном диапазоне дат, нет.\n";
        }
        else {
            std::cout << "Список файлов, созданных в диапазоне дат " << start_date_str << " - " << end_date_str << ":" << std::endl;
            for (const auto& file : filteredFileList) {
                file.display(std::cout);
            }
        }
        break;
    }
    case 5:
    {
        int start_size, end_size;
        std::cout << "Введите начальный размер файла: ";
        std::cin >> start_size;
        std::cout << "Введите конечный размер файла: ";
        std::cin >> end_size;

        // Фильтрация файлов по дате
        std::vector<files> filteredFileList;
        for (const auto& file : fileList) {
            int size = file.getSize();
            if ((size) >= (start_size) && (size) <= (end_size)) {
                filteredFileList.push_back(file);
            }
        }

        // Вывод отфильтрованных файлов
        if (filteredFileList.empty()) {
            std::cout << "Файлов, созданных в указанном диапазоне размеров, нет.\n";
        }
        else {
            std::cout << "Список файлов, созданных в диапазоне размеров " << start_size << " - " << end_size << ":" << std::endl;
            for (const auto& file : filteredFileList) {
                file.display(std::cout);
            }
        }
        break;
    }
    default:
        std::cout << "Такого числа нет в списке.\n";
        return 1;
    }
    return 0;
}
