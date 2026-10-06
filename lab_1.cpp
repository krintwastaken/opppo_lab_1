#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// Базовый класс для всех исторических событий
class HistoricalEvent {
   private:
    std::string name;
    std::string date;

   public:
    explicit HistoricalEvent(std::string n, std::string d)
        : name(std::move(n)), date(std::move(d)) {}

    virtual ~HistoricalEvent() = default;

    virtual void print() const = 0;

    // Устранение замечания Cppcheck: возврат по константной ссылке
    const std::string& getName() const noexcept { return name; }
    const std::string& getDate() const noexcept { return date; }
};

// Производный класс: Битва
class Battle : public HistoricalEvent {
   private:
    std::string location;

   public:
    explicit Battle(std::string n, std::string d, std::string loc)
        : HistoricalEvent(std::move(n), std::move(d)), location(std::move(loc)) {}

    void print() const override {
        std::cout << "[Битва] " << getName() << " | Дата: " << getDate() << " | Место: " << location
                  << "\n";
    }
};

// Производный класс: Договор
class Treaty : public HistoricalEvent {
   private:
    std::string parties;

   public:
    explicit Treaty(std::string n, std::string d, std::string p)
        : HistoricalEvent(std::move(n), std::move(d)), parties(std::move(p)) {}

    void print() const override {
        std::cout << "[Договор] " << getName() << " | Дата: " << getDate()
                  << " | Стороны: " << parties << "\n";
    }
};

// Вспомогательная функция для удаления пробелов по краям строки
std::string trim(const std::string& str) {
    const size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Обработка команды ADD
void handleAdd(const std::string& args, std::vector<std::unique_ptr<HistoricalEvent>>& events) {
    std::stringstream ss(args);
    std::string type, name, date, extra;

    if (std::getline(ss, type, ';') && std::getline(ss, name, ';') && std::getline(ss, date, ';') &&
        std::getline(ss, extra)) {
        type = trim(type);
        name = trim(name);
        date = trim(date);
        extra = trim(extra);

        if (type == "Battle") {
            events.push_back(
                std::make_unique<Battle>(std::move(name), std::move(date), std::move(extra)));
        } else if (type == "Treaty") {
            events.push_back(
                std::make_unique<Treaty>(std::move(name), std::move(date), std::move(extra)));
        } else {
            std::cerr << "Предупреждение: Неизвестный тип события '" << type << "'\n";
        }
    } else {
        std::cerr << "Ошибка: Неверный формат аргументов команды ADD\n";
    }
}

// Обработка команды REM с использованием алгоритма erase_if
void handleRem(const std::string& condition,
               std::vector<std::unique_ptr<HistoricalEvent>>& events) {
    std::stringstream ss(condition);
    std::string field, eq, val;

    ss >> field >> eq;
    std::getline(ss, val);
    val = trim(val);

    if (eq != "=") {
        std::cerr << "Ошибка: Неверный формат условия REM. Ожидается: <поле> = <значение>\n";
        return;
    }

    // Использование std::erase_if (C++20) вместо ручного цикла со сдвигом итератора
    std::erase_if(events, [&](const std::unique_ptr<HistoricalEvent>& item) {
        if (!item) return false;
        if (field == "date") return item->getDate() == val;
        if (field == "name") return item->getName() == val;
        return false;
    });
}

// Обработка команды PRINT
void handlePrint(const std::vector<std::unique_ptr<HistoricalEvent>>& events) {
    std::cout << "--- Список событий (" << events.size() << ") ---\n";
    if (events.empty()) {
        std::cout << "(список пуст)\n";
        std::cout << "---------------------------\n";
        return;
    }
    for (const auto& ev : events) {
        if (ev) {
            ev->print();
        }
    }
    std::cout << "---------------------------\n";
}

int main() {
    setlocale(LC_ALL, "");

    const std::string filename = "input.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << "\n";
        return 1;
    }

    std::vector<std::unique_ptr<HistoricalEvent>> events;
    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string command;
        ss >> command;

        if (command == "ADD") {
            std::string rest;
            std::getline(ss, rest);
            handleAdd(rest, events);
        } else if (command == "REM") {
            std::string condition;
            std::getline(ss, condition);
            handleRem(condition, events);
        } else if (command == "PRINT") {
            handlePrint(events);
        } else {
            std::cerr << "Неизвестная команда: " << command << "\n";
        }
    }

    return 0;
}