#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// Базовый класс для всех исторических событий
class HistoricalEvent {
   protected:
    std::string name;
    std::string date;

   public:
    HistoricalEvent(const std::string& n, const std::string& d) : name(n), date(d) {}

    virtual ~HistoricalEvent() = default;

    virtual void print() const = 0;

    std::string getName() const { return name; }
    std::string getDate() const { return date; }
};

// Производный класс: Битва
class Battle : public HistoricalEvent {
   private:
    std::string location;  // Место битвы

   public:
    Battle(const std::string& n, const std::string& d, const std::string& loc)
        : HistoricalEvent(n, d), location(loc) {}

    void print() const override {
        std::cout << "[Битва] " << name << " | Дата: " << date << " | Место: " << location << "\n";
    }
};

// Производный класс: Договор
class Treaty : public HistoricalEvent {
   private:
    std::string parties;  // Стороны договора

   public:
    Treaty(const std::string& n, const std::string& d, const std::string& p)
        : HistoricalEvent(n, d), parties(p) {}

    void print() const override {
        std::cout << "[Договор] " << name << " | Дата: " << date << " | Стороны: " << parties
                  << "\n";
    }
};

// Вспомогательная функция для удаления пробелов по краям строки
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Обработка команды ADD
void handleAdd(const std::string& args, std::vector<std::unique_ptr<HistoricalEvent>>& events) {
    std::stringstream ss(args);
    std::string type, name, date, extra;

    // Считываем поля, разделенные ';'
    if (std::getline(ss, type, ';') && std::getline(ss, name, ';') && std::getline(ss, date, ';') &&
        std::getline(ss, extra)) {
        type = trim(type);
        name = trim(name);
        date = trim(date);
        extra = trim(extra);

        if (type == "Battle") {
            events.push_back(std::make_unique<Battle>(name, date, extra));
        } else if (type == "Treaty") {
            events.push_back(std::make_unique<Treaty>(name, date, extra));
        } else {
            std::cout << "Неизвестный тип события: " << type << "\n";
        }
    }
}

// Обработка команды REM (условие вида: поле = значение)
void handleRem(const std::string& condition,
               std::vector<std::unique_ptr<HistoricalEvent>>& events) {
    std::stringstream ss(condition);
    std::string field, eq, val;

    ss >> field >> eq;
    std::getline(ss, val);
    val = trim(val);

    if (eq != "=") {
        std::cout << "Неверный формат условия. Ожидается: поле = значение\n";
        return;
    }

    for (auto it = events.begin(); it != events.end();) {
        bool match = false;

        if (field == "date" && (*it)->getDate() == val) {
            match = true;
        } else if (field == "name" && (*it)->getName() == val) {
            match = true;
        }

        if (match) {
            it = events.erase(it);  // Удаление и переход к следующему
        } else {
            ++it;
        }
    }
}

// Обработка команды PRINT
void handlePrint(const std::vector<std::unique_ptr<HistoricalEvent>>& events) {
    std::cout << "--- Список событий (" << events.size() << ") ---\n";
    if (events.empty()) {
        std::cout << "(список пуст)\n";
        return;
    }
    for (const auto& ev : events) {
        ev->print();
    }
    std::cout << "---------------------------\n";
}

int main() {
    setlocale(LC_ALL, "");

    std::string filename = "input.txt";
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
        }
    }

    file.close();
    return 0;
}