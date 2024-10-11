#include <iostream>  
#include <sstream>  
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
    cout << "Выберите цифру (1-3): ";
    cin >> choice;

    switch (choice) {
    case 1:
        sort(fileList.begin(), fileList.end(), [](const files& a, const files& b) {
            return a.getSize() < b.getSize(); // Сортировка по размеру от большего к меньшему  
            });
        break;
    case 2:
        sort(fileList.begin(), fileList.end(), [](const files& a, const files& b) {
            return mktime(const_cast<tm*>(&a.getCreationDate())) < mktime(const_cast<tm*>(&b.getCreationDate())); // Сортировка по дате создания  
            });
        break;
    case 3:
        // Выводим в том порядке, в каком есть  
        break;

    default:
        cout << "Такого числа нет в списке.\n";
            return 1;
        }


      for (const auto& file : fileList) {
            file.display(cout); // Выводим на экран  
        }
        return 0;
}
