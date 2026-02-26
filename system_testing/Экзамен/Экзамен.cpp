#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <functional>
#include <cctype>
#include <iomanip>
#include <direct.h>

using namespace std;



string simpleEncrypt(const string& input)
{
    string result = input;
    for (char& c : result)
    {
        c = c ^ 0x5A;
    }
    return result;
}
struct User
{
    string login;
    string password;
    string fullName;
    string address;
    string phone;
    bool isAdmin;

    User() : isAdmin(false) {}
};
struct Question
{
    int id;
    string text;
    vector<string> options;
    int correctOption;

    Question() : id(0), correctOption(0) {}
};
struct Test
{
    int id;
    string category;
    string name;
    vector<Question> questions;
    int timeLimit;

    Test() : id(0), timeLimit(0) {}
};
struct TestResult
{
    int userId;
    int testId;
    string testName;
    string category;
    int totalQuestions;
    int correctAnswers;
    double percentage;
    string grade;
    time_t startTime;
    time_t endTime;
    bool completed;
    vector<int> userAnswers;

    TestResult() : userId(0), testId(0), totalQuestions(0), correctAnswers(0), percentage(0), completed(false)
    {
        startTime = time(nullptr);
        endTime = 0;
    }
};


class TestingSystem
{
private:
    map<string, User> users;
    vector<Test> tests;
    vector<TestResult> results;
    User* currentUser;

    //char dataDir = "\\test_data\\";

    void loadData()
    {
        loadUsers();
        loadTests();
        loadResults();
    }
    void saveData()
    {
        saveUsers();
        saveTests();
        saveResults();
    }
    void loadUsers()
    {
        ifstream file( "users.txt");
        if (!file.is_open()) return;

        users.clear();
        string line;
        while (getline(file, line))
        {
            istringstream iss(line);
            User u;
            string encryptedPass;

            getline(iss, u.login, '|');
            getline(iss, encryptedPass, '|');
            getline(iss, u.fullName, '|');
            getline(iss, u.address, '|');
            getline(iss, u.phone, '|');
            iss >> u.isAdmin;

            u.password = simpleEncrypt(encryptedPass);
            users[u.login] = u;
        }
        file.close();
    }

    void saveUsers()
    {
        ofstream file( "users.txt");
        for (const auto& pair : users)
        {
            const User& u = pair.second;
            file << u.login << "|"
                << simpleEncrypt(u.password) << "|"
                << u.fullName << "|"
                << u.address << "|"
                << u.phone << "|"
                << u.isAdmin << "\n";
        }
        file.close();
    }

    void loadTests()
    {
        ifstream file( "tests.txt");
        if (!file.is_open()) return;

        tests.clear();
        string line;
        while (getline(file, line))
        {
            if (line.empty()) continue;

            Test t;
            istringstream iss(line);
            iss >> t.id;
            iss.ignore();
            getline(iss, t.category, '|');
            getline(iss, t.name, '|');
            iss >> t.timeLimit;

            int questionCount;
            iss >> questionCount;

            for (int i = 0; i < questionCount; i++)
            {
                if (!getline(file, line)) break;

                Question q;
                istringstream qiss(line);
                qiss >> q.id;
                qiss.ignore();
                getline(qiss, q.text, '|');

                int optionCount;
                qiss >> optionCount;

                for (int j = 0; j < optionCount; j++)
                {
                    string option;
                    getline(qiss, option, '|');
                    q.options.push_back(option);
                }

                qiss >> q.correctOption;
                t.questions.push_back(q);
            }

            tests.push_back(t);
        }
        file.close();
    }

    void saveTests()
    {
        ofstream file( "tests.txt");
        for (const Test& t : tests)
        {
            file << t.id << "|" << t.category << "|" << t.name << "|" << t.timeLimit << " " << t.questions.size() << "\n";

            for (const Question& q : t.questions)
            {
                file << q.id << "|" << q.text << "|" << q.options.size();
                for (const std::string& opt : q.options)
                {
                    file << "|" << opt;
                }
                file << "|" << q.correctOption << "\n";
            }
        }
        file.close();
    }

    void loadResults()
    {
        ifstream file( "results.txt");
        if (!file.is_open()) return;

        results.clear();
        string line;
        while (getline(file, line))
        {
            TestResult r;
            istringstream iss(line);

            iss >> r.userId;
            iss.ignore();
            iss >> r.testId;
            iss.ignore();
            getline(iss, r.testName, '|');
            getline(iss, r.category, '|');
            iss >> r.totalQuestions;
            iss >> r.correctAnswers;
            iss >> r.percentage;
            iss.ignore();
            getline(iss, r.grade, '|');
            iss >> r.startTime;
            iss >> r.endTime;
            iss >> r.completed;

            int answersCount;
            iss >> answersCount;
            for (int i = 0; i < answersCount; i++)
            {
                int answer;
                iss >> answer;
                r.userAnswers.push_back(answer);
            }

            results.push_back(r);
        }
        file.close();
    }

    void saveResults()
    {
        ofstream file( "results.txt");
        for (const TestResult& r : results)
        {
            file << r.userId << "|" << r.testId << "|" << r.testName << "|" << r.category << "|" << r.totalQuestions << " "
                << r.correctAnswers << " " << r.percentage << "|" << r.grade << "|" << r.startTime << " " << r.endTime << " "
                << r.completed << " " << r.userAnswers.size();

            for (int answer : r.userAnswers)
            {
                file << " " << answer;
            }
            file << "\n";
        }
        file.close();
    }

    bool loginExists(const string& login)
    {
        return users.find(login) != users.end();
    }

    string getGrade(double percentage)
    {
        if (percentage >= 90) return "5 (Отлично)";
        if (percentage >= 75) return "4 (Хорошо)";
        if (percentage >= 60) return "3 (Удовлетворительно)";
        return "2 (Неудовлетворительно)";
    }

    void clearScreen()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void waitForEnter()
    {
        cout << "\nНажмите Enter для продолжения...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

public:
    TestingSystem() : currentUser(nullptr)
    {
       /* if (dataDir.back() != '\\' || dataDir.back() != '/')
        {
            dataDir += '\\';
        }*/
    /*TestingSystem() :currentUser(nullptr)
    {*/
        loadData();
        //C:\Users\Student\P - 41\ООП\Экзамен\testing_system\system_testing\Экзамен
        if (users.empty())
        {
            User admin;
            admin.login = "admin";
            admin.password = simpleEncrypt("admin123");
            admin.fullName = "System Administrator";
            admin.address = "System";
            admin.phone = "000-000-000";
            admin.isAdmin = true;
            users[admin.login] = admin;
            saveUsers();
        }
    }

    ~TestingSystem()
    {
        saveData();
    }

    void run()
    {
        while (true)
        {
            clearScreen();
            cout << "=== СИСТЕМА ТЕСТИРОВАНИЯ ===\n";
            cout << "1. Вход в систему\n";
            cout << "2. Регистрация\n";
            cout << "3. Выход\n";
            cout << "Выберите действие: ";

            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                login();
                break;
            case 2:
                registerUser();
                break;
            case 3:
                cout << "До свидания!\n";
                return;
            default:
                cout << "Неверный выбор!\n";
                waitForEnter();
            }
        }
    }

    void login()
    {
        clearScreen();
        cout << "=== ВХОД В СИСТЕМУ ===\n";

        string login, password;
        cout << "Логин: ";
        cin >> login;
        cout << "Пароль: ";
        cin >> password;

        auto it = users.find(login);
        if (it != users.end() && it->second.password == simpleEncrypt(password))
        {
            currentUser = &it->second;
            cout << "Добро пожаловать, " << currentUser->fullName << "!\n";
            waitForEnter();

            if (currentUser->isAdmin)
            {
                adminMenu();
            }
            else
            {
                userMenu();
            }
        }
        else
        {
            cout << "Неверный логин или пароль!\n";
            waitForEnter();
        }
    }

    void registerUser()
    {
        clearScreen();
        cout << "=== РЕГИСТРАЦИЯ НОВОГО ПОЛЬЗОВАТЕЛЯ ===\n";

        User newUser;

        cout << "Введите логин: ";
        cin >> newUser.login;

        if (loginExists(newUser.login))
        {
            cout << "Пользователь с таким логином уже существует!\n";
            waitForEnter();
            return;
        }

        string password;
        cout << "Введите пароль: ";
        cin >> password;
        newUser.password = simpleEncrypt(password);

        cout << "Введите ФИО: ";
        cin.ignore();
        getline(cin, newUser.fullName);

        cout << "Введите домашний адрес: ";
        getline(cin, newUser.address);

        cout << "Введите телефон: ";
        getline(cin, newUser.phone);

        newUser.isAdmin = false;

        users[newUser.login] = newUser;
        saveUsers();

        cout << "Регистрация успешно завершена!\n";
        waitForEnter();
    }

    void userMenu()
    {
        while (currentUser)
        {
            clearScreen();
            cout << "=== МЕНЮ ПОЛЬЗОВАТЕЛЯ ===\n";
            cout << "Пользователь: " << currentUser->fullName << "\n\n";
            cout << "1. Мои результаты тестирования\n";
            cout << "2. Пройти новое тестирование\n";
            cout << "3. Продолжить прерванный тест\n";
            cout << "4. Выйти из аккаунта\n";
            cout << "Выберите действие: ";

            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                viewMyResults();
                break;
            case 2:
                takeNewTest();
                break;
            case 3:
                continueTest();
                break;
            case 4:
                currentUser = nullptr;
                break;
            default:
                cout << "Неверный выбор!\n";
                waitForEnter();
            }
        }
    }

    void adminMenu() {
        while (currentUser) {
            clearScreen();
            cout << "=== МЕНЮ АДМИНИСТРАТОРА ===\n";
            cout << "Администратор: " << currentUser->fullName << "\n\n";
            cout << "1. Управление пользователями\n";
            cout << "2. Просмотр статистики\n";
            cout << "3. Управление тестами\n";
            cout << "4. Выйти из аккаунта\n";
            cout << "Выберите действие: ";

            int choice;
            cin >> choice;

            switch (choice) {
            case 1:
                manageUsers();
                break;
            case 2:
                viewStatistics();
                break;
            case 3:
                manageTests();
                break;
            case 4:
                currentUser = nullptr;
                break;
            default:
                cout << "Неверный выбор!\n";
                waitForEnter();
            }
        }
    }

    void viewMyResults()
    {
        clearScreen();
        cout << "=== МОИ РЕЗУЛЬТАТЫ ===\n\n";

        bool hasResults = false;
        for (const auto& result : results)
        {
            if (result.userId == hash<string>{}(currentUser->login) && result.completed)
            {
                hasResults = true;
                cout << "Тест: " << result.testName << "\n";
                cout << "Категория: " << result.category << "\n";
                cout << "Результат: " << result.correctAnswers << "/" << result.totalQuestions << "\n";
                cout << "Процент: " << fixed << setprecision(1) << result.percentage << "%\n";
                cout << "Оценка: " << result.grade << "\n";
                cout << "Дата: " << ctime(&result.startTime);
                cout << "------------------------\n";
            }
        }

        if (!hasResults)
        {
            cout << "У вас пока нет завершенных тестов.\n";
        }

        waitForEnter();
    }

    void takeNewTest()
    {
        clearScreen();
        cout << "=== ДОСТУПНЫЕ ТЕСТЫ ===\n\n";

        if (tests.empty())
        {
            cout << "Тестов пока нет.\n";
            waitForEnter();
            return;
        }

        map<string, vector<Test>> testsByCategory;
        for (const auto& test : tests)
        {
            testsByCategory[test.category].push_back(test);
        }

        int index = 1;
        map<int, Test> testMap;

        for (const auto& category : testsByCategory)
        {
            cout << "\n=== " << category.first << " ===\n";
            for (const auto& test : category.second)
            {
                cout << index << ". " << test.name << " (" << test.questions.size() << " вопросов)\n";
                testMap[index] = test;
                index++;
            }
        }

        cout << "\n0. Назад\n";
        cout << "Выберите тест: ";

        int choice;
        cin >> choice;

        if (choice == 0) return;

        auto it = testMap.find(choice);
        if (it != testMap.end())
        {
            startTest(it->second);
        }
        else
        {
            cout << "Неверный выбор!\n";
            waitForEnter();
        }
    }

    void startTest(const Test& test)
    {
        clearScreen();
        cout << "=== ТЕСТ: " << test.name << " ===\n\n";
        cout << "Категория: " << test.category << "\n";
        cout << "Количество вопросов: " << test.questions.size() << "\n";
        if (test.timeLimit > 0)
        {
            cout << "Время на выполнение: " << test.timeLimit << " минут\n";
        }

        cout << "\n1. Начать тест\n";
        cout << "2. Вернуться к выбору\n";
        cout << "Выберите действие: ";

        int choice;
        cin >> choice;

        if (choice != 1) return;

        TestResult result;
        result.userId = hash<string>{}(currentUser->login);
        result.testId = test.id;
        result.testName = test.name;
        result.category = test.category;
        result.totalQuestions = test.questions.size();
        result.userAnswers.resize(test.questions.size(), -1);
        result.startTime = time(nullptr);

        for (size_t i = 0; i < test.questions.size(); i++)
        {
            const auto& q = test.questions[i];

            clearScreen();
            cout << "Вопрос " << (i + 1) << " из " << test.questions.size() << "\n";
            cout << "====================\n\n";
            cout << q.text << "\n\n";

            for (size_t j = 0; j < q.options.size(); j++)
            {
                cout << j + 1 << ". " << q.options[j] << "\n";
            }

            cout << "\n0. Прервать тест\n";
            cout << "Ваш ответ (номер варианта): ";

            int answer;
            cin >> answer;

            if (answer == 0)
            {
                // Прерываем тест
                results.push_back(result);
                saveResults();
                cout << "\nТест прерван. Вы можете продолжить его позже.\n";
                waitForEnter();
                return;
            }

            if (answer >= 1 && answer <= (int)q.options.size())
            {
                result.userAnswers[i] = answer - 1;
                if (result.userAnswers[i] == q.correctOption)
                {
                    result.correctAnswers++;
                }
            }
        }

        result.completed = true;
        result.endTime = time(nullptr);
        result.percentage = (100.0 * result.correctAnswers) / result.totalQuestions;
        result.grade = getGrade(result.percentage);

        results.push_back(result);
        saveResults();

        clearScreen();
        cout << "=== РЕЗУЛЬТАТ ТЕСТИРОВАНИЯ ===\n\n";
        cout << "Правильных ответов: " << result.correctAnswers << " из " << result.totalQuestions << "\n";
        cout << "Процент правильных: " << fixed << setprecision(1) << result.percentage << "%\n";
        cout << "Оценка: " << result.grade << "\n";

        waitForEnter();
    }

    void continueTest()
    {
        clearScreen();
        cout << "=== ПРЕРВАННЫЕ ТЕСТЫ ===\n\n";

        vector<TestResult> incompleteTests;
        int userId = hash<string>{}(currentUser->login);

        for (const auto& result : results)
        {
            if (result.userId == userId && !result.completed)
            {
                incompleteTests.push_back(result);
            }
        }

        if (incompleteTests.empty())
        {
            cout << "У вас нет прерванных тестов.\n";
            waitForEnter();
            return;
        }

        for (size_t i = 0; i < incompleteTests.size(); i++)
        {
            cout << i + 1 << ". " << incompleteTests[i].testName
                << " (прерван: " << ctime(&incompleteTests[i].startTime) << ")";
        }

        cout << "\n0. Назад\n";
        cout << "Выберите тест для продолжения: ";

        int choice;
        cin >> choice;

        if (choice == 0 || choice > (int)incompleteTests.size()) return;

        Test originalTest;
        bool found = false;
        for (const auto& test : tests)
        {
            if (test.id == incompleteTests[choice - 1].testId)
            {
                originalTest = test;
                found = true;
                break;
            }
        }

        if (!found)
        {
            cout << "Ошибка: тест не найден!\n";
            waitForEnter();
            return;
        }

        TestResult& result = incompleteTests[choice - 1];

        for (size_t i = 0; i < originalTest.questions.size(); i++)
        {
            if (result.userAnswers[i] != -1) continue;

            const auto& q = originalTest.questions[i];

            clearScreen();
            cout << "Вопрос " << (i + 1) << " из " << originalTest.questions.size() << "\n";
            cout << "====================\n\n";
            cout << q.text << "\n\n";

            for (size_t j = 0; j < q.options.size(); j++)
            {
                cout << j + 1 << ". " << q.options[j] << "\n";
            }

            cout << "\n0. Прервать тест\n";
            cout << "Ваш ответ (номер варианта): ";

            int answer;
            cin >> answer;

            if (answer == 0)
            {
                saveResults();
                cout << "\nТест снова прерван.\n";
                waitForEnter();
                return;
            }

            if (answer >= 1 && answer <= (int)q.options.size())
            {
                result.userAnswers[i] = answer - 1;
                if (result.userAnswers[i] == q.correctOption)
                {
                    result.correctAnswers++;
                }
            }
        }

        result.completed = true;
        result.endTime = time(nullptr);
        result.percentage = (100.0 * result.correctAnswers) / result.totalQuestions;
        result.grade = getGrade(result.percentage);

        saveResults();

        clearScreen();
        cout << "=== РЕЗУЛЬТАТ ТЕСТИРОВАНИЯ ===\n\n";
        cout << "Правильных ответов: " << result.correctAnswers << " из " << result.totalQuestions << "\n";
        cout << "Процент правильных: " << fixed << setprecision(1) << result.percentage << "%\n";
        cout << "Оценка: " << result.grade << "\n";

        waitForEnter();
    }

    void manageUsers()
    {
        while (true)
        {
            clearScreen();
            cout << "=== УПРАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯМИ ===\n\n";
            cout << "1. Просмотреть всех пользователей\n";
            cout << "2. Добавить пользователя\n";
            cout << "3. Редактировать пользователя\n";
            cout << "4. Удалить пользователя\n";
            cout << "5. Назад\n";
            cout << "Выберите действие: ";

            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                viewAllUsers();
                break;
            case 2:
                addUser();
                break;
            case 3:
                editUser();
                break;
            case 4:
                deleteUser();
                break;
            case 5:
                return;
            default:
                cout << "Неверный выбор!\n";
                waitForEnter();
            }
        }
    }

    void viewAllUsers()
    {
        clearScreen();
        cout << "=== ВСЕ ПОЛЬЗОВАТЕЛИ ===\n\n";

        for (const auto& pair : users)
        {
            const User& u = pair.second;
            cout << "Логин: " << u.login << "\n";
            cout << "ФИО: " << u.fullName << "\n";
            cout << "Адрес: " << u.address << "\n";
            cout << "Телефон: " << u.phone << "\n";
            cout << "Роль: " << (u.isAdmin ? "Администратор" : "Тестируемый") << "\n";
            cout << "------------------------\n";
        }

        waitForEnter();
    }

    void addUser()
    {
        clearScreen();
        cout << "=== ДОБАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯ ===\n\n";

        User newUser;

        cout << "Логин: ";
        cin >> newUser.login;

        if (loginExists(newUser.login))
        {
            cout << "Пользователь с таким логином уже существует!\n";
            waitForEnter();
            return;
        }

        string password;
        cout << "Пароль: ";
        cin >> password;
        newUser.password = simpleEncrypt(password);

        cout << "ФИО: ";
        cin.ignore();
        getline(cin, newUser.fullName);

        cout << "Адрес: ";
        getline(cin, newUser.address);

        cout << "Телефон: ";
        getline(cin, newUser.phone);

        cout << "Администратор? (1-да, 0-нет): ";
        cin >> newUser.isAdmin;

        users[newUser.login] = newUser;
        saveUsers();

        cout << "Пользователь добавлен!\n";
        waitForEnter();
    }

    void editUser()
    {
        clearScreen();
        cout << "=== РЕДАКТИРОВАНИЕ ПОЛЬЗОВАТЕЛЯ ===\n\n";

        string login;
        cout << "Введите логин пользователя: ";
        cin >> login;

        auto it = users.find(login);
        if (it == users.end())
        {
            cout << "Пользователь не найден!\n";
            waitForEnter();
            return;
        }

        User& u = it->second;

        cout << "\nТекущие данные:\n";
        cout << "ФИО: " << u.fullName << "\n";
        cout << "Адрес: " << u.address << "\n";
        cout << "Телефон: " << u.phone << "\n";
        cout << "Роль: " << (u.isAdmin ? "Администратор" : "Тестируемый") << "\n\n";

        cout << "Введите новые данные (оставьте пустым для сохранения текущего):\n";

        cout << "ФИО: ";
        cin.ignore();
        string input;
        getline(cin, input);
        if (!input.empty()) u.fullName = input;

        cout << "Адрес: ";
        getline(cin, input);
        if (!input.empty()) u.address = input;

        cout << "Телефон: ";
        getline(cin, input);
        if (!input.empty()) u.phone = input;

        cout << "Изменить пароль? (1-да, 0-нет): ";
        int changePass;
        cin >> changePass;

        if (changePass)
        {
            cout << "Новый пароль: ";
            cin >> input;
            u.password = simpleEncrypt(input);
        }

        cout << "Администратор? (1-да, 0-нет): ";
        cin >> u.isAdmin;

        saveUsers();
        cout << "Данные обновлены!\n";
        waitForEnter();
    }

    void deleteUser()
    {
        clearScreen();
        cout << "=== УДАЛЕНИЕ ПОЛЬЗОВАТЕЛЯ ===\n\n";

        string login;
        cout << "Введите логин пользователя для удаления: ";
        cin >> login;

        if (login == "admin")
        {
            cout << "Нельзя удалить главного администратора!\n";
            waitForEnter();
            return;
        }

        auto it = users.find(login);
        if (it == users.end())
        {
            cout << "Пользователь не найден!\n";
            waitForEnter();
            return;
        }

        cout << "Вы уверены? (1-да, 0-нет): ";
        int confirm;
        cin >> confirm;

        if (confirm)
        {
            users.erase(it);
            saveUsers();
            cout << "Пользователь удален!\n";
        }

        waitForEnter();
    }

    void viewStatistics()
    {
        while (true)
        {
            clearScreen();
            cout << "=== СТАТИСТИКА ===\n\n";
            cout << "1. Общая статистика по тестам\n";
            cout << "2. Статистика по пользователям\n";
            cout << "3. Статистика по категориям\n";
            cout << "4. Назад\n";
            cout << "Выберите действие: ";

            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                viewTestStatistics();
                break;
            case 2:
                viewUserStatistics();
                break;
            case 3:
                viewCategoryStatistics();
                break;
            case 4:
                return;
            default:
                cout << "Неверный выбор!\n";
                waitForEnter();
            }
        }
    }

    void viewTestStatistics()
    {
        clearScreen();
        cout << "=== СТАТИСТИКА ПО ТЕСТАМ ===\n\n";

        map<int, vector<TestResult>> resultsByTest;
        for (const auto& result : results)
        {
            if (result.completed)
            {
                resultsByTest[result.testId].push_back(result);
            }
        }

        for (const auto& test : tests)
        {
            cout << "Тест: " << test.name << "\n";
            cout << "Категория: " << test.category << "\n";

            auto it = resultsByTest.find(test.id);
            if (it != resultsByTest.end()) {
                int total = it->second.size();
                int totalCorrect = 0;
                double avgPercentage = 0;

                for (const auto& r : it->second)
                {
                    totalCorrect += r.correctAnswers;
                    avgPercentage += r.percentage;
                }

                avgPercentage /= total;

                cout << "Количество прохождений: " << total << "\n";
                cout << "Средний процент: " << fixed << setprecision(1) << avgPercentage << "%\n";
                cout << "Среднее количество правильных: " << fixed << setprecision(1) << (double)totalCorrect / total << "\n";
            }
            else
            {
                cout << "Еще не проходили\n";
            }
            cout << "------------------------\n";
        }

        waitForEnter();
    }

    void viewUserStatistics()
    {
        clearScreen();
        cout << "=== СТАТИСТИКА ПО ПОЛЬЗОВАТЕЛЯМ ===\n\n";

        for (const auto& pair : users)
        {
            if (pair.second.isAdmin) continue;

            cout << "Пользователь: " << pair.second.fullName << " (" << pair.first << ")\n";

            int userId = hash<string>{}(pair.first);
            int completedTests = 0;
            int totalCorrect = 0;
            int totalQuestions = 0;

            for (const auto& result : results)
            {
                if (result.userId == userId && result.completed)
                {
                    completedTests++;
                    totalCorrect += result.correctAnswers;
                    totalQuestions += result.totalQuestions;
                }
            }

            if (completedTests > 0)
            {
                cout << "Пройдено тестов: " << completedTests << "\n";
                cout << "Всего вопросов: " << totalQuestions << "\n";
                cout << "Правильных ответов: " << totalCorrect << "\n";
                cout << "Процент успеха: " << fixed << setprecision(1) << (100.0 * totalCorrect / totalQuestions) << "%\n";
            }
            else
            {
                cout << "Еще не проходил тесты\n";
            }
            cout << "------------------------\n";
        }

        waitForEnter();
    }

    void viewCategoryStatistics()
    {
        clearScreen();
        cout << "=== СТАТИСТИКА ПО КАТЕГОРИЯМ ===\n\n";

        map<string, vector<TestResult>> resultsByCategory;

        for (const auto& result : results)
        {
            if (result.completed)
            {
                resultsByCategory[result.category].push_back(result);
            }
        }

        for (const auto& category : resultsByCategory)
        {
            cout << "Категория: " << category.first << "\n";

            int totalTests = category.second.size();
            int totalCorrect = 0;
            int totalQuestions = 0;

            for (const auto& r : category.second)
            {
                totalCorrect += r.correctAnswers;
                totalQuestions += r.totalQuestions;
            }

            cout << "Количество прохождений: " << totalTests << "\n";
            cout << "Всего вопросов: " << totalQuestions << "\n";
            cout << "Правильных ответов: " << totalCorrect << "\n";
            cout << "Процент успеха: " << fixed << setprecision(1) << (100.0 * totalCorrect / totalQuestions) << "%\n";
            cout << "------------------------\n";
        }

        waitForEnter();
    }

    void manageTests()
    {
        while (true)
        {
            clearScreen();
            cout << "=== УПРАВЛЕНИЕ ТЕСТАМИ ===\n\n";
            cout << "1. Просмотреть все тесты\n";
            cout << "2. Добавить тест\n";
            cout << "3. Редактировать тест\n";
            cout << "4. Удалить тест\n";
            cout << "5. Назад\n";
            cout << "Выберите действие: ";

            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                viewAllTests();
                break;
            case 2:
                addTest();
                break;
            case 3:
                editTest();
                break;
            case 4:
                deleteTest();
                break;
            case 5:
                return;
            default:
                cout << "Неверный выбор!\n";
                waitForEnter();
            }
        }
    }

    void viewAllTests()
    {
        clearScreen();
        cout << "=== ВСЕ ТЕСТЫ ===\n\n";

        for (const auto& test : tests)
        {
            cout << "ID: " << test.id << "\n";
            cout << "Название: " << test.name << "\n";
            cout << "Категория: " << test.category << "\n";
            cout << "Вопросов: " << test.questions.size() << "\n";
            if (test.timeLimit > 0)
            {
                cout << "Лимит времени: " << test.timeLimit << " минут\n";
            }

            cout << "Вопросы:\n";
            for (size_t i = 0; i < min(test.questions.size(), size_t(3)); i++)
            {
                cout << "  " << (i + 1) << ". " << test.questions[i].text << "\n";
            }
            if (test.questions.size() > 3)
            {
                cout << "  ... и еще " << (test.questions.size() - 3) << " вопросов\n";
            }
            cout << "------------------------\n";
        }

        waitForEnter();
    }

    void addTest()
    {
        clearScreen();
        cout << "=== ДОБАВЛЕНИЕ ТЕСТА ===\n\n";

        Test newTest;
        newTest.id = tests.empty() ? 1 : tests.back().id + 1;

        cout << "Название теста: ";
        cin.ignore();
        getline(cin, newTest.name);

        cout << "Категория: ";
        getline(cin, newTest.category);

        cout << "Лимит времени (минут, 0 - без лимита): ";
        cin >> newTest.timeLimit;

        cout << "Количество вопросов: ";
        int qCount;
        cin >> qCount;

        for (int i = 0; i < qCount; i++)
        {
            cout << "\n--- Вопрос " << (i + 1) << " ---\n";

            Question q;
            q.id = i + 1;

            cout << "Текст вопроса: ";
            cin.ignore();
            getline(cin, q.text);

            cout << "Количество вариантов ответа: ";
            int optCount;
            cin >> optCount;

            for (int j = 0; j < optCount; j++)
            {
                cin.ignore();
                cout << "Вариант " << (j + 1) << ": ";
                string opt;
                getline(cin, opt);
                q.options.push_back(opt);
            }

            cout << "Номер правильного ответа (1-" << optCount << "): ";
            cin >> q.correctOption;
            q.correctOption--;

            newTest.questions.push_back(q);
        }

        tests.push_back(newTest);
        saveTests();

        cout << "\nТест успешно добавлен!\n";
        waitForEnter();
    }

    void editTest()
    {
        clearScreen();
        cout << "=== РЕДАКТИРОВАНИЕ ТЕСТА ===\n\n";

        cout << "Введите ID теста: ";
        int id;
        cin >> id;

        auto it = find_if(tests.begin(), tests.end(), [id](const Test& t) { return t.id == id; });

        if (it == tests.end())
        {
            cout << "Тест не найден!\n";
            waitForEnter();
            return;
        }

        Test& test = *it;

        cout << "\nТекущее название: " << test.name << "\n";
        cout << "Новое название (Enter - оставить): ";
        cin.ignore();
        string input;
        getline(cin, input);
        if (!input.empty()) test.name = input;

        cout << "Текущая категория: " << test.category << "\n";
        cout << "Новая категория (Enter - оставить): ";
        getline(cin, input);
        if (!input.empty()) test.category = input;

        cout << "Текущий лимит времени: " << test.timeLimit << "\n";
        cout << "Новый лимит (Enter - оставить): ";
        getline(cin, input);
        if (!input.empty())
        {
            test.timeLimit = stoi(input);
        }

        cout << "\nХотите редактировать вопросы? (1-да, 0-нет): ";
        int editQuestions;
        cin >> editQuestions;

        if (editQuestions)
        {
            for (size_t i = 0; i < test.questions.size(); i++)
            {
                cout << "\n--- Вопрос " << (i + 1) << " ---\n";
                cout << "Текущий текст: " << test.questions[i].text << "\n";
                cout << "Новый текст (Enter - оставить): ";
                cin.ignore();
                getline(cin, input);
                if (!input.empty()) test.questions[i].text = input;

                cout << "Варианты ответа:\n";
                for (size_t j = 0; j < test.questions[i].options.size(); j++)
                {
                    cout << "  " << (j + 1) << ". " << test.questions[i].options[j] << "\n";
                }

                cout << "Текущий правильный ответ: " << (test.questions[i].correctOption + 1) << "\n";
                cout << "Новый номер правильного ответа (0 - оставить): ";
                int correct;
                cin >> correct;
                if (correct > 0 && correct <= (int)test.questions[i].options.size())
                {
                    test.questions[i].correctOption = correct - 1;
                }
            }
        }

        saveTests();
        cout << "\nТест обновлен!\n";
        waitForEnter();
    }

    void deleteTest()
    {
        clearScreen();
        cout << "=== УДАЛЕНИЕ ТЕСТА ===\n\n";

        cout << "Введите ID теста: ";
        int id;
        cin >> id;

        auto it = find_if(tests.begin(), tests.end(), [id](const Test& t) { return t.id == id; });

        if (it == tests.end())
        {
            cout << "Тест не найден!\n";
            waitForEnter();
            return;
        }

        cout << "Тест: " << it->name << "\n";
        cout << "Вы уверены? (1-да, 0-нет): ";
        int confirm;
        cin >> confirm;

        if (confirm)
        {
            tests.erase(it);
            saveTests();
            cout << "Тест удален!\n";
        }

        waitForEnter();
    }
};
int main()
{
    setlocale(LC_ALL, "Russian");

    TestingSystem system;
    system.run();

    return 0;
}