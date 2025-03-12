#include "logger.h"
#include "dataStructures.h"
#include <stdio.h>
#include <time.h>

void logMessage(const char* message) {
    FILE* logFile = fopen(LOG_FILE, "a");
    if (logFile) {
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

        fprintf(logFile, "[%s] : %s\n", timestamp, message);
        fclose(logFile);
    }
}