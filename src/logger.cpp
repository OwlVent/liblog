// ВНИМАНИЕ: Для предотвращения конфликта макросов min/max из Windows.h
#define NOMINMAX

#include "liblog/logger.h"
#include <iostream>
#include <ctime>
#include <string>
#include <limits>
#include <locale>

// --- Блок кросс-платформенной совместимости для сокетов ---
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int SOCKET;
    const int INVALID_SOCKET = -1;
    const int SOCKET_ERROR = -1;
    #define closesocket close
#endif

// ===================================================================
// Вспомогательная функция для преобразования enum в строку
// ===================================================================

/**
 * @brief Преобразует значение LogLevel в отформатированную строку.
 * @param level Уровень лога типа LogLevel.
 * @return Строка с названием уровня, дополненная пробелами для выравнивания.
 */
string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:      return "INFO    ";
        case LogLevel::WARNING:   return "WARNING ";
        case LogLevel::LOG_ERROR: return "ERROR   ";
        default:                  return "UNKNOWN ";
    }
}

// ===================================================================
// 1. РЕАЛИЗАЦИЯ БАЗОВОГО КЛАССА LoggerBase
// ===================================================================

// Конструктор просто инициализирует поле
LoggerBase::LoggerBase(LogLevel default_level) : m_default_level(default_level) {}

// Общий метод для всех
void LoggerBase::set_default_level(LogLevel new_level) {
    m_default_level = new_level;
}

// РЕАЛИЗАЦИЯ ОБЩИХ ФУНКЦИЙ (перемещена сюда)
std::string LoggerBase::format_log_entry(const std::string& message, LogLevel level) {
    std::string level_str = logLevelToString(level);
    return get_current_time_str() + " | " + level_str + " | " + message;
}

std::string LoggerBase::get_current_time_str() {
    time_t now = time(nullptr);
    std::string time_str = ctime(&now);
    if (!time_str.empty() && time_str.back() == '\n') {
        time_str.pop_back();
    }
    return time_str;
}

// ===================================================================
// Реализация методов класса FileLogger
// ===================================================================

/**
 * @brief Конструктор класса FileLogger.
 * @param filename Имя файла для записи логов.
 * @param default_level Минимальный уровень важности для записи сообщений.
 */
FileLogger::FileLogger(const string& filename, LogLevel default_level)
    : LoggerBase(default_level) {
    log_file.open(filename, ios::app);
    if (!log_file.is_open()) {
        cerr << "CRITICAL ERROR: Could not open log file: " << filename << endl;
    }
}

/**
 * @brief Деструктор класса FileLogger.
 * Гарантирует, что файл будет закрыт при уничтожении объекта (принцип RAII).
 */
FileLogger::~FileLogger() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

/**
 * @brief Записывает сообщение в лог, если его уровень не ниже установленного по умолчанию.
 * @param message Текст сообщения для записи.
 * @param level Уровень важности данного сообщения.
 */
void FileLogger::log(const string& message, LogLevel level) {
    // Проверяем, открыт ли файл и достаточен ли уровень важности
    if (!log_file.is_open() || static_cast<int>(level) < static_cast<int>(m_default_level)) {
        return;
    }

    string entry = format_log_entry(message, level);
    log_file << entry << endl;
}

// ===================================================================
// Реализация методов класса SocketLogger
// ===================================================================

/**
 * @brief Конструктор класса SocketLogger.
 * @param host IP-адрес или доменное имя сервера логов.
 * @param port Порт сервера логов.
 * @param default_level Минимальный уровень важности для отправки сообщений.
 */
SocketLogger::SocketLogger(const std::string& host, int port, LogLevel default_level)
    : LoggerBase(default_level), m_socket(INVALID_SOCKET), is_connected(false)
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed for SocketLogger." << std::endl;
        return;
    }
#endif

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed for SocketLogger." << std::endl;
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);

    if (connect(m_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to log server at " << host << ":" << port << std::endl;
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return;
    }

    is_connected = true;
    std::cout << "SocketLogger connected to " << host << ":" << port << std::endl;
}

/**
 * @brief Деструктор класса SocketLogger.
 * Корректно закрывает сокет и освобождает ресурсы.
 */
SocketLogger::~SocketLogger() {
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
    }
#ifdef _WIN32
    // Для Windows освобождаем ресурсы, инициализированные в WSAStartup
    WSACleanup();
#endif
}

/**
 * @brief Отправляет сообщение на сервер логов.
 * @param message Текст сообщения.
 * @param level Уровень важности сообщения.
 */
void SocketLogger::log(const std::string& message, LogLevel level) {
    if (!is_connected || static_cast<int>(level) < static_cast<int>(m_default_level)) {
        return;
    }

    std::string entry = format_log_entry(message, level) + "\n";
    int result = send(m_socket, entry.c_str(), entry.length(), 0);

    if (result == SOCKET_ERROR) {
        std::cerr << "Failed to send log message. Connection may be lost." << std::endl;
        is_connected = false;
    }
}

// ===================================================================
// Реализация глобальной функции-обертки log_message
// ===================================================================

/**
 * @brief Утилита для однократной записи в лог. Запрашивает текст у пользователя.
 * @param filename Имя файла для записи.
 * @param severity_level Уровень важности сообщения (1=INFO, 2=WARNING, 3=LOG_ERROR).
 */
void log_message(const string& filename, int severity_level) {
    FileLogger logger(filename, LogLevel::INFO);

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

    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    getline(cin, message);

    logger.log(message, message_level);

    cout << "Message logged successfully." << endl;
}