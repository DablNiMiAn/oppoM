#include <sstream>  
#include <iostream>
#include <fstream>  
#include <string>  
#include <ctime>  
#include <iomanip>  
#include <vector>  
#include <algorithm>  

using namespace std;

class files {
private:
    string file_name;
    tm creation_date; // Используем tm для хранения даты  
    int size;

public:
    files(const string& filename, const tm& creation_date, int size)
        : file_name(filename), creation_date(creation_date), size(size) {}

    void display(ostream& os) const { // Используем ostream для переменной вывода  
        os << "Имя файла: " << file_name << "\t";
        os << "Дата создания: " << put_time(&creation_date, "%Y.%m.%d") << "\t";
        os << "Размер: " << size << " Байт" << endl;
    }


    const string& getFilename() const {
        return file_name;
    }

    const tm& getCreationDate() const {
        return creation_date;
    }

    int getSize() const {
        return size;
    }
};

tm parseDate(const string& dateStr) {
    tm date = {};
    istringstream ss(dateStr);
    ss >> get_time(&date, "%Y.%m.%d");
    if (ss.fail()) {
        throw runtime_error("Неправильный формат даты");
    }
    return date;
}

files createfiles(const string& description) {
    istringstream stream(description);
    string filename;
    string dateStr;
    int size = 0;
    string temp;

    bool filenameSet = false;
    bool dateSet = false;
    bool sizeSet = false;

    while (stream >> temp) {
        if (temp.front() == '\"') {
            filename = temp.substr(1, temp.size() - 2); // Удаляем кавычки   
            filenameSet = true;
        }
        else if (temp.find('.') != string::npos && temp.size() <= 10) {
            dateStr = temp;
            dateSet = true;
        }
        else if (isdigit(temp[0])) {
            size = stoi(temp);
            sizeSet = true;
        }
    }

    if (!filenameSet or !dateSet or !sizeSet or size <= 0) {
        throw runtime_error("Invalid object description");
    }

    tm creation_date = parseDate(dateStr);
    return files(filename, creation_date, size);
}

int main() {
    setlocale(0, "ru");
    ifstream inputFile("in.txt");
    if (!inputFile.is_open()) {
        cerr << "Error: Не удалось открыть файл 'in.txt'.\n";
        return 1;
    }

    vector<files> fileList;
    string description;

    while (getline(inputFile, description)) {
        try {
            files obj = createfiles(description);
            fileList.push_back(obj);
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << "\n";
        }
    }

    inputFile.close();

    int choice;
    cout << "Выберите метод сортировки:\n";
    cout << "1)По размеру файла (largest to smallest)\n";
    cout << "2) По дате создания\n";
    cout << "3) Без сортировки\n";
    cout << "4) Вывод по временному промежутку\n";
    cout << "5) Вывод по промежутку размеров\n";
    cout << "Выберите цифру (1-5): ";
    cin >> choice;

    switch (choice) {
    case 1:
        sort(fileList.begin(), fileList.end(), [](const files& a, const files& b) {
            return a.getSize() < b.getSize(); // Сортировка по размеру от большего к меньшему  
            });
        for (const auto& file : fileList) {
            file.display(cout); // Выводим на экран  
        }
        break;
    case 2:
        sort(fileList.begin(), fileList.end(), [](const files& a, const files& b) {
            return mktime(const_cast<tm*>(&a.getCreationDate())) < mktime(const_cast<tm*>(&b.getCreationDate())); // Сортировка по дате создания  
            });
        for (const auto& file : fileList) {
            file.display(cout); // Выводим на экран  
        }
        break;
    case 3:
        for (const auto& file : fileList) {
            file.display(cout); // Выводим на экран  
        }
        // Выводим в том порядке, в каком есть  
        break;
    case 4:
    {
        string start_date_str, end_date_str;
        cout << "Введите начальную дату (ГГГГ.ММ.ДД): ";
        cin >> start_date_str;
        cout << "Введите конечную дату (ГГГГ.ММ.ДД): ";
        cin >> end_date_str;

        tm start_date = parseDate(start_date_str);
        tm end_date = parseDate(end_date_str);

        // Фильтрация файлов по дате
        vector<files> filteredFileList;
        for (const auto& file : fileList) {
            tm fileDate = file.getCreationDate();
            if (mktime(&fileDate) >= mktime(&start_date) && mktime(&fileDate) <= mktime(&end_date)) {
                filteredFileList.push_back(file);
            }
        }

        // Вывод отфильтрованных файлов
        if (filteredFileList.empty()) {
            cout << "Файлов, созданных в указанном диапазоне дат, нет.\n";
        }
        else {
            cout << "Список файлов, созданных в диапазоне дат " << start_date_str << " - " << end_date_str << ":" << endl;
            for (const auto& file : filteredFileList) {
                file.display(cout);
            }
        }
        break;
    }
    case 5:
    {
        int start_size, end_size;
        cout << "Введите начальный размер файла: ";
        cin >> start_size;
        cout << "Введите конечный размер файла: ";
        cin >> end_size;

        // Фильтрация файлов по дате
        vector<files> filteredFileList;
        for (const auto& file : fileList) {
            int size = file.getSize();
            if ((size) >= (start_size) && (size) <= (end_size)) {
                filteredFileList.push_back(file);
            }
        }

        // Вывод отфильтрованных файлов
        if (filteredFileList.empty()) {
            cout << "Файлов, созданных в указанном диапазоне размеров, нет.\n";
        }
        else {
            cout << "Список файлов, созданных в диапазоне размеров " << start_size << " - " << end_size << ":" << endl;
            for (const auto& file : filteredFileList) {
                file.display(cout);
            }
        }
        break;
    }
    default:
        cout << "Такого числа нет в списке.\n";
        return 1;
    }
    return 0;
}