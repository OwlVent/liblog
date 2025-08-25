#ifndef LIBLOG_LOGGER_H
#define LIBLOG_LOGGER_H

#include <string>
#include <fstream>

using namespace std;
/**
 * @brief Перечисление для уровней важности сообщений.
 * Использование enum class обеспечивает строгую типизацию и предотвращает ошибки.
 */
enum class LogLevel {
    INFO    = 1,
    WARNING = 2,
    ERROR   = 3
};

/**
 * @class Logger
 * @brief Основной класс для управления логированием в файл.
 */
class Logger {
public:
    /**
     * @brief Конструктор логгера.
     * @param filename Путь к файлу журнала.
     * @param default_level Минимальный уровень важности, при котором сообщения будут записываться.
     */
    explicit Logger(const string& filename, LogLevel default_level = LogLevel::INFO);

    /**
     * @brief Деструктор логгера.
     */
    ~Logger();

    /**
     * @brief Записывает сообщение в журнал.
     * Сообщение будет записано только в том случае, если его уровень (level)
     * не ниже уровня по умолчанию, установленного в логгере.
     * @param message Текст сообщения.
     * @param level Уровень важности текущего сообщения.
     */
    void log(const string& message, LogLevel level);

    /**
     * @brief Изменяет минимальный уровень важности "на лету".
     * Позволяет менять детализацию логов во время работы программы.
     * @param new_level Новый минимальный уровень важности.
     */
    void set_default_level(LogLevel new_level);

private:
    /**
     * @brief [Приватный] Получает текущее системное время в виде строки.
     */
    string get_current_time_str();

    /**
     * @brief [Приватный] Собирает финальную строку для записи в файл.
     */
    string format_log_entry(const string& message, LogLevel level);
    

    ofstream log_file;   // Поток для записи в файл
    LogLevel default_level;   // Текущий минимальный уровень для записи
};


/**
 * @brief Функция-утилита для простой однократной записи в лог.
 * 
 * Эта функция является "фасадом" для класса Logger. Она создает временный
 * объект логгера, запрашивает у пользователя текст сообщения в консоли
 * и записывает его в файл. Удобна для простых приложений или скриптов.
 * @param filename Путь к файлу журнала.
 * @param severity_level Уровень важности сообщения (1=INFO, 2=WARNING, 3=ERROR).
 */
void log_message(const string& filename, int severity_level);


#endif