#include "awry.h"

static void log_format(int log_level, const char* tag, const char* message, va_list args) {
  if (log_level >= Awry.log_level) {
    printf("[%s]: ", tag);
    vprintf(message, args);
    printf("\n");
  }
}

static void log_dev(const char* message, ...) {
  va_list args;
  va_start(args, message);
  log_format(AWRY_LOG_DEVELOPMENT, "DEV", message, args);
  va_end(args);
}

static void log_fatal(const char* message, ...) {
  va_list args;
  va_start(args, message);
  log_format(AWRY_LOG_FATAL, "FATAL", message, args);
  va_end(args);
}

static void log_error(const char* message, ...) {
  va_list args;
  va_start(args, message);
  log_format(AWRY_LOG_ERROR, "ERROR", message, args);
  va_end(args);
}

static void log_warn(const char* message, ...) {
  va_list args;
  va_start(args, message);
  log_format(AWRY_LOG_WARN, "WARNING", message, args);
  va_end(args);
}

static void log_info(const char* message, ...) {
  va_list args;
  va_start(args, message);
  log_format(AWRY_LOG_INFO, "INFO", message, args);
  va_end(args);
}

static void log_debug(const char* message, ...) {
  va_list args;
  va_start(args, message);
  log_format(AWRY_LOG_DEBUG, "DEBUG", message, args);
  va_end(args);
}

AwryLoggerModule AwryLogger = {
  .log_debug = log_debug,
  .log_info = log_info,
  .log_warn = log_warn,
  .log_error = log_error,
  .log_fatal = log_fatal,
  .log_dev = log_dev
};