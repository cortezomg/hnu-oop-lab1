#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <string>

using namespace std;

// Структура для зберігання даних про студента
struct Student {
    char surname[50];
    char name[50];
    int course;
    char group[20];
    int grades[4];
};

// Режим файлу: бінарний або текстовий
enum FileType {
    BINARY_FILE,
    TEXT_FILE
};

// --- Оголошення функцій ---
void RW_dat(const char* filename, Student* students, int count);
void RD_dat(const char* filename);

void RW_txt(const char* filename, Student* students, int count);
void RD_txt(const char* filename);

// Перевантажені функції RW() та RD()
void RW(const char* filename, Student* students, int count, FileType type = BINARY_FILE);
void RD(const char* filename, FileType type = BINARY_FILE);

// Допоміжні функції
Student* inputStudents(int& count);
Student* createSampleStudents(int& count);

// Обчислення видимої довжини UTF-8 рядка (для коректного вирівнювання кирилиці)
int utf8_len(const char* s) {
    int len = 0;
    while (*s) {
        if ((*s & 0xC0) != 0x80) len++;
        s++;
    }
    return len;
}

// Друк комірки таблиці з урахуванням UTF-8
void printCell(const char* str, int width) {
    cout << str;
    int len = utf8_len(str);
    for (int i = len; i < width; ++i) {
        cout << ' ';
    }
}

void printCell(int val, int width) {
    string s = to_string(val);
    cout << s;
    for (int i = (int)s.length(); i < width; ++i) {
        cout << ' ';
    }
}

void printTableHeader() {
    printCell("№", 4);
    printCell("Прізвище", 18);
    printCell("Ім'я", 14);
    printCell("Курс", 8);
    printCell("Група", 12);
    printCell("Іспит 1", 10);
    printCell("Іспит 2", 10);
    printCell("Іспит 3", 10);
    printCell("Іспит 4", 10);
    cout << endl;
    cout << string(86, '-') << endl;
}

// ==========================================
// 1.1 Запис у типізований (бінарний) файл INFO.DAT
// ==========================================
void RW_dat(const char* filename, Student* students, int count) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Помилка відкриття файлу для запису: " << filename << endl;
        return;
    }

    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    file.write(reinterpret_cast<const char*>(students), sizeof(Student) * count);
    file.close();

    cout << "[RW_dat] Успішно записано " << count << " студентів у бінарний файл: " << filename << endl;
}

// ==========================================
// 1.2 Читання з типізованого файлу INFO.DAT та виведення середнього балу
// ==========================================
void RD_dat(const char* filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Помилка відкриття файлу для читання: " << filename << endl;
        return;
    }

    int count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    if (file.fail() || count <= 0) {
        cerr << "Помилка читання або файл порожній: " << filename << endl;
        file.close();
        return;
    }

    Student* students = new Student[count];
    file.read(reinterpret_cast<char*>(students), sizeof(Student) * count);
    file.close();

    cout << "\n=================== ВМІСТ БІНАРНОГО ФАЙЛУ (" << filename << ") ===================\n";
    printTableHeader();

    double sumGrades[4] = {0.0, 0.0, 0.0, 0.0};

    for (int i = 0; i < count; ++i) {
        printCell(i + 1, 4);
        printCell(students[i].surname, 18);
        printCell(students[i].name, 14);
        printCell(students[i].course, 8);
        printCell(students[i].group, 12);
        for (int j = 0; j < 4; ++j) {
            printCell(students[i].grades[j], 10);
            sumGrades[j] += students[i].grades[j];
        }
        cout << endl;
    }
    cout << string(86, '-') << endl;

    cout << "\n[INFO.DAT] Середній бал по кожному іспиту:\n";
    for (int j = 0; j < 4; ++j) {
        cout << "  - Іспит №" << (j + 1) << ": " 
             << fixed << setprecision(2) << (sumGrades[j] / count) << endl;
    }

    delete[] students;
}

// ==========================================
// 1.3 Запис у текстовий файл INFO.TXT
// ==========================================
void RW_txt(const char* filename, Student* students, int count) {
    ofstream file(filename);
    if (!file) {
        cerr << "Помилка відкриття текстового файлу для запису: " << filename << endl;
        return;
    }

    file << count << "\n";
    for (int i = 0; i < count; ++i) {
        file << students[i].surname << " "
             << students[i].name << " "
             << students[i].course << " "
             << students[i].group << " "
             << students[i].grades[0] << " "
             << students[i].grades[1] << " "
             << students[i].grades[2] << " "
             << students[i].grades[3] << "\n";
    }
    file.close();

    cout << "[RW_txt] Успішно записано " << count << " студентів у текстовий файл: " << filename << endl;
}

// ==========================================
// 1.4 Читання з текстового файлу INFO.TXT та виведення середнього балу
// ==========================================
void RD_txt(const char* filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Помилка відкриття текстового файлу для читання: " << filename << endl;
        return;
    }

    int count = 0;
    if (!(file >> count) || count <= 0) {
        cerr << "Помилка читання кількості студентів з " << filename << endl;
        file.close();
        return;
    }

    Student* students = new Student[count];
    for (int i = 0; i < count; ++i) {
        file >> students[i].surname
             >> students[i].name
             >> students[i].course
             >> students[i].group
             >> students[i].grades[0]
             >> students[i].grades[1]
             >> students[i].grades[2]
             >> students[i].grades[3];
    }
    file.close();

    cout << "\n=================== ВМІСТ ТЕКСТОВОГО ФАЙЛУ (" << filename << ") ===================\n";
    printTableHeader();

    double sumGrades[4] = {0.0, 0.0, 0.0, 0.0};

    for (int i = 0; i < count; ++i) {
        printCell(i + 1, 4);
        printCell(students[i].surname, 18);
        printCell(students[i].name, 14);
        printCell(students[i].course, 8);
        printCell(students[i].group, 12);
        for (int j = 0; j < 4; ++j) {
            printCell(students[i].grades[j], 10);
            sumGrades[j] += students[i].grades[j];
        }
        cout << endl;
    }
    cout << string(86, '-') << endl;

    cout << "\n[INFO.TXT] Середній бал по кожному іспиту:\n";
    for (int j = 0; j < 4; ++j) {
        cout << "  - Іспит №" << (j + 1) << ": " 
             << fixed << setprecision(2) << (sumGrades[j] / count) << endl;
    }

    delete[] students;
}

// Перевантажені функції RW і RD
void RW(const char* filename, Student* students, int count, FileType type) {
    if (type == BINARY_FILE) {
        RW_dat(filename, students, count);
    } else {
        RW_txt(filename, students, count);
    }
}

void RD(const char* filename, FileType type) {
    if (type == BINARY_FILE) {
        RD_dat(filename);
    } else {
        RD_txt(filename);
    }
}

// Функція ручного введення студентів з клавіатури
Student* inputStudents(int& count) {
    cout << "Введіть кількість студентів N: ";
    cin >> count;

    if (count <= 0) {
        cout << "Кількість студентів повинна бути більше 0." << endl;
        return nullptr;
    }

    Student* students = new Student[count];

    for (int i = 0; i < count; ++i) {
        cout << "\n--- Введення даних для студента №" << (i + 1) << " ---\n";
        cout << "Прізвище: ";
        cin >> students[i].surname;
        cout << "Ім'я: ";
        cin >> students[i].name;
        cout << "Курс: ";
        cin >> students[i].course;
        cout << "Група: ";
        cin >> students[i].group;
        cout << "Оцінки з 4 предметів (через пробіл): ";
        for (int j = 0; j < 4; ++j) {
            cin >> students[i].grades[j];
        }
    }

    return students;
}

// Зразок готових даних для демонстрації
Student* createSampleStudents(int& count) {
    count = 4;
    Student* students = new Student[count];

    strcpy(students[0].surname, "Коваленко");
    strcpy(students[0].name, "Олександр");
    students[0].course = 2;
    strcpy(students[0].group, "ІПЗ-21");
    students[0].grades[0] = 85; students[0].grades[1] = 90; students[0].grades[2] = 88; students[0].grades[3] = 92;

    strcpy(students[1].surname, "Мельник");
    strcpy(students[1].name, "Марія");
    students[1].course = 2;
    strcpy(students[1].group, "ІПЗ-21");
    students[1].grades[0] = 95; students[1].grades[1] = 92; students[1].grades[2] = 98; students[1].grades[3] = 100;

    strcpy(students[2].surname, "Шевченко");
    strcpy(students[2].name, "Дмитро");
    students[2].course = 3;
    strcpy(students[2].group, "КН-32");
    students[2].grades[0] = 74; students[2].grades[1] = 80; students[2].grades[2] = 68; students[2].grades[3] = 85;

    strcpy(students[3].surname, "Бойко");
    strcpy(students[3].name, "Анна");
    students[3].course = 1;
    strcpy(students[3].group, "КН-11");
    students[3].grades[0] = 88; students[3].grades[1] = 84; students[3].grades[2] = 90; students[3].grades[3] = 79;

    return students;
}

int main() {
    setlocale(LC_ALL, "");

    int choice = 0;
    cout << "========================================================\n";
    cout << "        ЛАБОРАТОРНА РОБОТА №1 - ВАРІАНТ 9               \n";
    cout << "========================================================\n";
    cout << "1. Використати демонстраційні дані (4 студенти)\n";
    cout << "2. Ввести дані студентів вручну з клавіатури\n";
    cout << "Оберіть режим (1 або 2): ";
    cin >> choice;

    int count = 0;
    Student* students = nullptr;

    if (choice == 2) {
        students = inputStudents(count);
    } else {
        students = createSampleStudents(count);
        cout << "\nЗавантажено " << count << " демонстраційних записів.\n";
    }

    if (!students || count <= 0) {
        cout << "Немає даних для обробки. Завершення програми." << endl;
        return 1;
    }

    const char* datFilename = "INFO.DAT";
    const char* txtFilename = "INFO.TXT";

    cout << "\n------------------ РОБОТА З БІНАРНИМ ФАЙЛОМ (INFO.DAT) ------------------\n";
    // 1.1 Запис у INFO.DAT
    RW(datFilename, students, count, BINARY_FILE);

    // 1.2 Читання з INFO.DAT
    RD(datFilename, BINARY_FILE);

    cout << "\n------------------ РОБОТА З ТЕКСТОВИМ ФАЙЛОМ (INFO.TXT) ------------------\n";
    // 1.3 Запис у INFO.TXT
    RW(txtFilename, students, count, TEXT_FILE);

    // 1.4 Читання з INFO.TXT
    RD(txtFilename, TEXT_FILE);

    // Очищення пам'яті
    delete[] students;

    cout << "\nПрограму успішно виконано!\n";
    return 0;
}
