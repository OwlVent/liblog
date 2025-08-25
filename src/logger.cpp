#include "liblog/logger.h"
#include <iostream>
#include <ctime>
#include <string>
#include <limits>

using namespace std;

// ===================================================================
// Вспомогательная функция для преобразования enum в строку
// ===================================================================

string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:    return "INFO   ";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR  ";
        default:                return "UNKNOWN";
    }
}

// ===================================================================
// Реализация методов класса Logger
// ===================================================================

/**
 * @brief Конструктор класса Logger.
 * @param filename Имя файла для записи логов.
 * @param default_level Минимальный уровень важности для записи сообщений.
 */
Logger::Logger(const string& filename, LogLevel default_level) {
    default_level = default_level;
    log_file.open(filename, ios::app);
    if (!log_file.is_open()) {
        cerr << "CRITICAL ERROR: Could not open log file: " << filename << endl;
    }
}

/**
 * @brief Деструктор класса Logger.
 * Гарантирует, что файл будет закрыт при уничтожении объекта.
 */
Logger::~Logger() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

/**
 * @brief Записывает сообщение в лог, если его уровень не ниже установленного по умолчанию.
 * @param message Текст сообщения для записи.
 * @param level Уровень важности данного сообщения.
 */
void Logger::log(const string& message, LogLevel level) {
    if (!log_file.is_open() || static_cast<int>(level) < static_cast<int>(default_level)) {
        return;
    }

    string entry = format_log_entry(message, level);
    log_file << entry << endl;
}

/**
 * @brief Устанавливает новый минимальный уровень важности для записи.
 * @param new_level Новый уровень важности по умолчанию.
 */
void Logger::set_default_level(LogLevel new_level) {
    default_level = new_level;
}

/**
 * @brief [Приватный] Форматирует итоговую строку для записи в лог.
 * @param message Текст сообщения.
 * @param level Уровень важности сообщения.
 * @return Готовая к записи строка вида "ВРЕМЯ | УРОВЕНЬ | СООБЩЕНИЕ".
 */
string Logger::format_log_entry(const string& message, LogLevel level) {
    string level_str = logLevelToString(level);
    return get_current_time_str() + " | " + level_str + " | " + message;
}

/**
 * @brief [Приватный] Получает текущее системное время в виде строки.
 * @return Отформатированная строка с датой и временем.
 */
string Logger::get_current_time_str() {
    time_t now = time(nullptr);
    string time_str = ctime(&now);

    if (!time_str.empty() && time_str.back() == '\n') {
        time_str.pop_back();
    }
    return time_str;
}

// ===================================================================
// Реализация глобальной функции-обертки log_message
// ===================================================================

/**
 * @brief Утилита для однократной записи в лог. Запрашивает текст у пользователя.
 * @param filename Имя файла для записи.
 * @param severity_level Уровень важности сообщения (1=INFO, 2=WARNING, 3=ERROR).
 */
void log_message(const string& filename, int severity_level) {
    Logger logger(filename, LogLevel::INFO);

    LogLevel message_level;
    if (severity_level >= 1 && severity_level <= 3) {
        message_level = static_cast<LogLevel>(severity_level);
    } else {
        cout << "Warning: Unknown severity level " << severity_level
                  << ". Using INFO." << endl;
        message_level = LogLevel::INFO;
    }

    cout << "Enter a message to log: ";
    string message;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, message);

    logger.log(message, message_level);

    cout << "Message logged successfully." << endl;
}