#include "liblog/logger.h"
#include <iostream>
#include <string>

using namespace std;

void demonstrate_logging(Logger& logger) {
    cout << "  - Logging an INFO message..." << endl;
    logger.log("This is an info message.", LogLevel::INFO);

    cout << "  - Logging a WARNING message..." << endl;
    logger.log("This is a warning message.", LogLevel::WARNING);

    cout << "  - Logging an ERROR message..." << endl;
    logger.log("This is an error message.", LogLevel::ERROR);
}

int main() {
    string filename = "C:\\Users\\VentO\\CLionProjects\\liblog\\examples\\log.txt";
    
    cout << "--- DEMONSTRATION OF THE LOGGER CLASS ---" << endl;
    cout << "Log file used: \"" << filename << "\"" << endl << endl;
    
    Logger logger(filename, LogLevel::INFO);
    cout << "[1] Logger created. Initial default level is INFO." << endl;
    logger.log("--- Logger session started with INFO level ---", LogLevel::INFO);
    
    demonstrate_logging(logger);
    cout << "    -> All messages should be written to the file." << endl << endl;
    
    cout << "[2] Changing default level to WARNING..." << endl;
    logger.set_default_level(LogLevel::WARNING);
    logger.log("--- Default log level changed to WARNING ---", LogLevel::WARNING);

    demonstrate_logging(logger);
    cout << "    -> Now, INFO messages should be ignored." << endl << endl;
    
    cout << "[3] Changing default level to ERROR..." << endl;
    logger.set_default_level(LogLevel::ERROR);
    logger.log("--- Default log level changed to ERROR ---", LogLevel::ERROR);
    
    demonstrate_logging(logger);
    cout << "    -> Now, INFO and WARNING messages should be ignored." << endl << endl;
    
    cout << "--- DEMONSTRATION OF THE log_message UTILITY FUNCTION ---" << endl;
    int severity_level;
    cout << "Enter the severity level for a new one-time message (1:INFO, 2:WARN, 3:ERROR): ";
    cin >> severity_level;
    
    log_message(filename, severity_level);
    cout << "\nDemonstration complete. Check the content of \"" << filename << "\"." << endl;

    return 0;
}