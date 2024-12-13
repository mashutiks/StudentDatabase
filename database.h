#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <set> 
#include <unordered_map> 

// Структура записи в БД
struct Student {
    int id = 0;                // Ключевое поле (целое число)
    std::string name = "";     // Имя студента (строка)
    int age = 0;               // Возраст студента (целое число)
    std::string department = ""; // Департамент (строка)
    bool is_full_time = true;  // Дневная форма обучения (логический тип)

    // Преобразование записи в строку для хранения в файле
    std::string to_string() const {
        return std::to_string(id) + ";" + name + ";" + std::to_string(age) + ";" +
            department + ";" + (is_full_time ? "1" : "0");
    }

    // Преобразование строки из файла в объект Student
    static Student from_string(const std::string& str);
};

class Database {
public:
    Database(); // Конструктор без имени файла

    void create(const std::string& db_name); // Создание новой базы данных
    void open(const std::string& db_name);   // Открытие существующей базы данных
    void save();                             // Сохранение текущей базы данных
    void clear();                            // Очистка текущей базы данных
    void remove(const std::string& db_name); // Удаление базы данных

    void add_record(const Student& student); // Добавление записи
    void delete_record_by_key(int key);      // Удаление записи по ключу
    void delete_record_by_field(const std::string& field, const std::string& value); // Удаление по полю
    std::vector<Student> search_by_key(int key) const; // Поиск по ключу
    std::vector<Student> search_by_field(const std::string& field, const std::string& value) const; // Поиск по полю

    void create_backup(const std::string& backup_filename); // Создание резервной копии
    void restore_from_backup(const std::string& backup_filename); // Восстановление из резервной копии

    void export_to_csv(const std::string& csv_filename); // Экспорт в CSV
    void current_database() const; // Просмотр текущей базы данных


    void show_menu(); // Меню управления через консоль

private:
    std::string filename; // Имя файла текущей базы данных
    std::unordered_map<int, Student> student_map; // Основная структура хранения записей

    std::unordered_map<std::string, std::set<int>> name_index;      // Имя -> множество ID
    std::unordered_map<std::string, std::set<int>> department_index; // Департамент -> множество ID

    void print_students() const; // Вывод всех записей на экран
};

#endif
