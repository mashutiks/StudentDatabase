// database.cpp
#include "database.h"
#include <sstream>
#include <algorithm>
#include <filesystem>


Student Student::from_string(const std::string& str) {
    std::istringstream ss(str);
    std::string token;

    Student student;

    std::getline(ss, token, ';');
    student.id = std::stoi(token); // Преобразуем ID в целое число

    std::getline(ss, token, ';');
    student.name = token; // Имя студента

    std::getline(ss, token, ';');
    student.age = std::stoi(token); // Возраст студента

    std::getline(ss, token, ';');
    student.department = token; // Департамент студента

    std::getline(ss, token, ';');
    student.is_full_time = (token == "1"); // Преобразуем флаг дневной формы обучения

    return student;
}


Database::Database() : filename("") {}
// Создание базы данных
void Database::create(const std::string& db_name) {

    filename = db_name + ".db";

    // Проверяем, существует ли файл с таким именем
    if (std::filesystem::exists(filename)) {
        throw std::runtime_error("Database '" + db_name + "' already exists.");
    }

    student_map.clear();
    std::ofstream file(filename, std::ios::trunc);
    if (!file) {
        throw std::runtime_error("Failed to create database file.");
    }
    file.close();
    std::cout << "Database '" << db_name << "' created successfully.\n";
}


// Открытие базы данных
void Database::open(const std::string& db_name) {
    filename = db_name + ".db";
    student_map.clear();
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Database '" + db_name + "' does not exist.");
    }
    std::string line;
    while (std::getline(file, line)) {
        Student student = Student::from_string(line);
        student_map[student.id] = student;
    }
    file.close();
    std::cout << "Database '" << db_name << "' opened successfully.\n";
}

// Сохранение базы данных
void Database::save() {
    if (filename.empty()) {
        throw std::runtime_error("No database is currently opened to save.");
    }
    std::ofstream file(filename, std::ios::trunc);
    if (!file) {
        throw std::runtime_error("Failed to save database file.");
    }
    for (const auto& entry : student_map) {
        const auto& student = entry.second;
        file << student.to_string() << "\n";
    }
    file.close();
    std::cout << "Database '" << filename << "' saved successfully.\n";
}
// Очистка базы данных
void Database::clear() {
    student_map.clear();
    save();
    std::cout << "Database cleared successfully.\n";
}

// Удаление базы данных
void Database::remove(const std::string& db_name) {
    std::string db_file = db_name + ".db";
    if (std::remove(db_file.c_str()) != 0) {
        throw std::runtime_error("Failed to delete database '" + db_name + "'.");
    }
    std::cout << "Database '" << db_name << "' deleted successfully.\n";
}

// Добавление записи
void Database::add_record(const Student& student) {
    if (student_map.find(student.id) != student_map.end()) {
        throw std::runtime_error("Record with the same key already exists.");
    }
    student_map[student.id] = student;

    // Обновление индексов
    name_index[student.name].insert(student.id);
    department_index[student.department].insert(student.id);

    std::cout << "Record added successfully.\n";
}



// Удаление записи по ключу
void Database::delete_record_by_key(int key) {
    auto it = student_map.find(key);
    if (it == student_map.end()) {
        std::cout << "No record found with the given key: " << key << ".\n";
        return;
    }

    // Удаляем запись из индексов
    name_index[it->second.name].erase(key);
    if (name_index[it->second.name].empty()) {
        name_index.erase(it->second.name);
    }

    department_index[it->second.department].erase(key);
    if (department_index[it->second.department].empty()) {
        department_index.erase(it->second.department);
    }

    // Удаляем запись из основной структуры
    student_map.erase(it);
    std::cout << "Record with key " << key << " deleted successfully.\n";
}



// Удаление записей по полю
void Database::delete_record_by_field(const std::string& field, const std::string& value) {
    size_t deleted_count = 0;

    auto it = student_map.begin();
    while (it != student_map.end()) {
        const auto& student = it->second;
        bool match = false;

        if (field == "name" && student.name == value) {
            match = true;
        }
        else if (field == "department" && student.department == value) {
            match = true;
        }
        else if (field == "is_full_time" && ((value == "1") == student.is_full_time)) {
            match = true;
        }

        if (match) {
            it = student_map.erase(it); // Удаляем запись и обновляем итератор
            ++deleted_count;
        }
        else {
            ++it;
        }
    }

    if (deleted_count == 0) {
        std::cout << "No records found matching field '" << field << "' with value '" << value << "'.\n";
    }
    else {
        std::cout << deleted_count << " record(s) matching field '" << field << "' with value '" << value << "' deleted successfully.\n";
    }
}


// Поиск записей по ключу
std::vector<Student> Database::search_by_key(int key) const {
    std::vector<Student> results;
    auto it = student_map.find(key);
    if (it != student_map.end()) {
        results.push_back(it->second);
    }
    else {
        std::cout << "No record found with the given key: " << key << ".\n";
    }
    return results;
}
//Поиск записей по полю

std::vector<Student> Database::search_by_field(const std::string& field, const std::string& value) const {
    std::vector<Student> results;

    if (field == "name") {
        auto it = name_index.find(value);
        if (it != name_index.end()) {
            for (int id : it->second) {
                results.push_back(student_map.at(id));
            }
        }
    }
    else if (field == "department") {
        auto it = department_index.find(value);
        if (it != department_index.end()) {
            for (int id : it->second) {
                results.push_back(student_map.at(id));
            }
        }
    }

    if (results.empty()) {
        std::cout << "No records found matching field '" << field << "' with value '" << value << "'.\n";
    }

    return results;
}



// Вывод записей на экран
void Database::print_students() const {
    if (student_map.empty()) {
        std::cout << "No records found in the database.\n";
        return;
    }
    std::cout << "ID\tName\tAge\tDepartment\tIs Full-Time\n";
    for (const auto& entry : student_map) {
        const auto& student = entry.second;
        std::cout << student.id << "\t" << student.name << "\t" << student.age
            << "\t" << student.department << "\t"
            << (student.is_full_time ? "Yes" : "No") << "\n";
    }
}

// Реализация функций резервного копирования и восстановления

void Database::create_backup(const std::string& backup_filename) {
    // Проверяем, существует ли файл с таким именем
    if (std::filesystem::exists(backup_filename)) {
        throw std::runtime_error("Backup file '" + backup_filename + "' already exists.");
    }

    std::ofstream backup_file(backup_filename, std::ios::trunc);
    if (!backup_file) {
        throw std::runtime_error("Failed to create backup file.");
    }
    for (const auto& entry : student_map) {
        const auto& student = entry.second;
        backup_file << student.to_string() << "\n";
    }
    backup_file.close();
    std::cout << "Backup created successfully: " << backup_filename << "\n";
}


void Database::restore_from_backup(const std::string& backup_filename) {
    std::ifstream backup_file(backup_filename); // Открываем бэкап-файл для чтения
    if (!backup_file) {
        throw std::runtime_error("Backup file does not exist.");
    }

    student_map.clear(); // Очищаем текущие данные
    std::string line;

    // Чтение файла построчно
    while (std::getline(backup_file, line)) {
        Student student = Student::from_string(line); // Преобразуем строку обратно в объект Student
        student_map[student.id] = student; // Добавляем студента в student_map
    }

    backup_file.close(); // Закрываем файл после чтения

    // Обновляем имя текущего файла базы данных на имя бэкапа
    filename = backup_filename;
    std::cout << "Database restored successfully from backup: " << backup_filename << "\n";
}
void Database::export_to_csv(const std::string& csv_filename) {
    // Проверяем, существует ли файл с таким именем
    if (std::filesystem::exists(csv_filename)) {
        throw std::runtime_error("CSV file '" + csv_filename + "' already exists.");
    }

    std::ofstream csv_file(csv_filename, std::ios::trunc);
    if (!csv_file) {
        throw std::runtime_error("Failed to create CSV file.");
    }

    // Записываем заголовки столбцов
    csv_file << "ID;Name;Age;Department;Is Full-Time\n";

    // Записываем данные базы данных
    for (const auto& entry : student_map) {
        const auto& student = entry.second;
        csv_file << student.to_string() << "\n";
    }

    csv_file.close();
    std::cout << "Database exported successfully to CSV: " << csv_filename << "\n";
}
void Database::current_database() const {
    if (filename.empty()) {
        std::cout << "No database is currently open.\n";
    }
    else {
        std::cout << "Current database: " << filename << "\n";
    }
}


void Database::show_menu() {
    while (true) {
        std::cout << "\nDatabase Menu:\n";
        std::cout << "1. Create Database\n";
        std::cout << "2. Open Database\n";
        std::cout << "3. Save Database\n";
        std::cout << "4. Clear Database\n";
        std::cout << "5. Delete Database\n";
        std::cout << "6. Add Record\n";
        std::cout << "7. Delete Record by Key\n";
        std::cout << "8. Delete Record by Field\n";
        std::cout << "9. Search by Key\n";
        std::cout << "10. Search by Field\n";
        std::cout << "11. Show Records\n";
        std::cout << "12. Create Backup\n";
        std::cout << "13. Restore from Backup\n";
        std::cout << "14. Export to CSV\n";
        std::cout << "15. Exit\n";
        std::cout << "16. View Current Database\n"; // Новый пункт
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a valid number.\n";
            continue;
        }

        try {
            switch (choice) {
            case 1: {
                std::string db_name;
                std::cout << "Enter database name to create: ";
                std::cin >> db_name;
                create(db_name);
                break;
            }
            case 2: {
                std::string db_name;
                std::cout << "Enter database name to open: ";
                std::cin >> db_name;
                open(db_name);
                break;
            }
            case 3:
                save();
                break;
            case 4:
                clear();
                break;
            case 5: {
                std::string db_name;
                std::cout << "Enter database name to delete: ";
                std::cin >> db_name;
                remove(db_name);
                break;
            }
            case 6: {
                Student student;
                std::cout << "Enter ID, Name, Age, Department, Is Full-Time (1 for Yes, 0 for No): ";
                std::cin >> student.id >> student.name >> student.age >> student.department >> student.is_full_time;

                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid input. Record not added.\n";
                    break;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                add_record(student);
                break;
            }
            case 7: {
                int key;
                std::cout << "Enter Key (ID) to delete: ";
                std::cin >> key;
                delete_record_by_key(key);
                break;
            }
            case 8: {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::string field, value;
                std::cout << "Enter Field (name/department): ";
                std::getline(std::cin, field);
                std::cout << "Enter Value: ";
                std::getline(std::cin, value);
                delete_record_by_field(field, value);
                break;
            }
            case 9: {
                int key;
                std::cout << "Enter Key (ID) to search: ";
                std::cin >> key;
                auto results = search_by_key(key);
                for (const auto& student : results) {
                    std::cout << student.to_string() << "\n";
                }
                break;
            }
            case 10: {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::string field, value;
                std::cout << "Enter Field (name/department): ";
                std::getline(std::cin, field);
                std::cout << "Enter Value: ";
                std::getline(std::cin, value);
                auto results = search_by_field(field, value);
                if (results.empty()) {
                    std::cout << "No records found.\n";
                }
                else {
                    for (const auto& student : results) {
                        std::cout << student.to_string() << "\n";
                    }
                }
                break;
            }
            case 11:
                print_students();
                break;
            case 12: {
                std::string backup_filename;
                std::cout << "Enter backup filename: ";
                std::cin >> backup_filename;
                create_backup(backup_filename);
                break;
            }
            case 13: {
                std::string backup_filename;
                std::cout << "Enter backup filename to restore and work with: ";
                std::cin >> backup_filename;
                restore_from_backup(backup_filename);
                break;
            }
            case 14: {
                std::string csv_filename;
                std::cout << "Enter CSV filename: ";
                std::cin >> csv_filename;
                export_to_csv(csv_filename);
                break;
            }
            case 15:
                return;
            case 16: // Новый пункт
                current_database();
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
}
