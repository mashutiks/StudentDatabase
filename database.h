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

// ��������� ������ � ��
struct Student {
    int id = 0;                // �������� ���� (����� �����)
    std::string name = "";     // ��� �������� (������)
    int age = 0;               // ������� �������� (����� �����)
    std::string department = ""; // ����������� (������)
    bool is_full_time = true;  // ������� ����� �������� (���������� ���)

    // �������������� ������ � ������ ��� �������� � �����
    std::string to_string() const {
        return std::to_string(id) + ";" + name + ";" + std::to_string(age) + ";" +
            department + ";" + (is_full_time ? "1" : "0");
    }

    // �������������� ������ �� ����� � ������ Student
    static Student from_string(const std::string& str);
};

class Database {
public:
    Database(); // ����������� ��� ����� �����

    void create(const std::string& db_name); // �������� ����� ���� ������
    void open(const std::string& db_name);   // �������� ������������ ���� ������
    void save();                             // ���������� ������� ���� ������
    void clear();                            // ������� ������� ���� ������
    void remove(const std::string& db_name); // �������� ���� ������

    void add_record(const Student& student); // ���������� ������
    void delete_record_by_key(int key);      // �������� ������ �� �����
    void delete_record_by_field(const std::string& field, const std::string& value); // �������� �� ����
    std::vector<Student> search_by_key(int key) const; // ����� �� �����
    std::vector<Student> search_by_field(const std::string& field, const std::string& value) const; // ����� �� ����

    void create_backup(const std::string& backup_filename); // �������� ��������� �����
    void restore_from_backup(const std::string& backup_filename); // �������������� �� ��������� �����

    void export_to_csv(const std::string& csv_filename); // ������� � CSV
    void current_database() const; // �������� ������� ���� ������


    void show_menu(); // ���� ���������� ����� �������

private:
    std::string filename; // ��� ����� ������� ���� ������
    std::unordered_map<int, Student> student_map; // �������� ��������� �������� �������

    std::unordered_map<std::string, std::set<int>> name_index;      // ��� -> ��������� ID
    std::unordered_map<std::string, std::set<int>> department_index; // ����������� -> ��������� ID

    void print_students() const; // ����� ���� ������� �� �����
};

#endif
