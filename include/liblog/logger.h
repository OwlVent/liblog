#ifndef LIBLOG_LOGGER_H
#define LIBLOG_LOGGER_H

#ifdef _WIN32
    #include <winsock2.h>
#else
    typedef int SOCKET;
#endif


#include <string>
#include <fstream>

using namespace std;
/**
 * @brief Перечисление для уровней важности сообщений.
 * Использование enum class обеспечивает строгую типизацию и предотвращает ошибки.
 */
enum class LogLevel { INFO = 1, WARNING, LOG_ERROR };

// ===================================================================
// 1. АБСТРАКТНЫЙ БАЗОВЫЙ КЛАСС
// ===================================================================

class LoggerBase {
    public:
    // Виртуальный деструктор для базовых классов
    virtual ~LoggerBase() = default;
    // "Чистый виртуальный" метод log - это контракт для дочерних классов
    virtual void log(const std::string& message, LogLevel level) = 0;
    // Общий для всех метод
    void set_default_level(LogLevel level);

    protected:
    // Конструктор будет вызываться дочерними классами
    explicit LoggerBase(LogLevel default_level);

    /**
     * @brief [Приватный] Получает текущее системное время в виде строки.
     */
    string get_current_time_str();
    /**
    * @brief [Приватный] Собирает финальную строку для записи в файл.
    */
    string format_log_entry(const std::string& message, LogLevel level);

    LogLevel m_default_level;
};

// ===================================================================
// 2. ДОЧЕРНИЕ КЛАССЫ
// ===================================================================

/**
 * @class FileLogger
 * @brief Основной класс для управления логированием в файл.
 */
class FileLogger : public LoggerBase {
public:
    /**
     * @brief Конструктор логгера.
     * @param filename Путь к файлу журнала.
     * @param default_level Минимальный уровень важности, при котором сообщения будут записываться.
     */
    explicit FileLogger(const string& filename, LogLevel default_level = LogLevel::INFO);

    /**
     * @brief Деструктор логгера.
     */
    ~FileLogger() override;

    /**
     * @brief Записывает сообщение в журнал.
     * Сообщение будет записано только в том случае, если его уровень (level)
     * не ниже уровня по умолчанию, установленного в логгере.
     * @param message Текст сообщения.
     * @param level Уровень важности текущего сообщения.
     */
    void log(const string& message, LogLevel level) override;
private:
    ofstream log_file;   // Поток для записи в файл
};

class SocketLogger : public LoggerBase {
    public:
    explicit SocketLogger(const string& host, int port, LogLevel default_level = LogLevel::INFO);
    ~SocketLogger() override;

    void log(const string& message, LogLevel level) override;

    private:
    SOCKET m_socket;
    bool is_connected = false;
};

/**
 * @brief Функция-утилита для простой однократной записи в лог.
 *
 * Эта функция является "фасадом" для класса FileLogger. Она создает временный
 * объект логгера, запрашивает у пользователя текст сообщения в консоли
 * и записывает его в файл. Удобна для простых приложений или скриптов.
 * @param filename Путь к файлу журнала.
 * @param severity_level Уровень важности сообщения (1=INFO, 2=WARNING, 3=ERROR).
 */
void log_message(const string& filename, int severity_level);
#endif