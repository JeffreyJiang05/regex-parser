#include "parser/error.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/signal.h>

#define DEFAULT_OUTPUT_FD STDOUT_FILENO
#ifdef COLOR
    #define DEFAULT_COLOR_DIAGNOSTIC 1
#else
    #define DEFAULT_COLOR_DIAGNOSTIC 0
#endif
#define DEFAULT_HIDE_WARNING 0
#define DEFAULT_SILENT_SUCCESS 0

enum error_type
{
    ERROR_TYPE,
    WARNING_TYPE,
    SENTINEL_TYPE
};

struct error_node
{
    struct error_node *next;
    struct error_node *prev;
    enum error_type type;
    const char *message;
};

struct error_logs
{
    struct error_node sentinel;
    size_t log_sz;
    size_t warning_sz;
    size_t error_sz;
    ERRLOGS_CONFIG config;
};

static struct error_logs logs = {
    .sentinel = { NULL, NULL, SENTINEL_TYPE, "" },
    .log_sz = 0,
    .warning_sz = 0,
    .error_sz = 0,
    .config = {
        .output_fd                = DEFAULT_OUTPUT_FD,
        .color_diagnostic_enabled = DEFAULT_COLOR_DIAGNOSTIC,
        .hide_warning_enabled     = DEFAULT_HIDE_WARNING,
        .silent_success_enabled   = DEFAULT_SILENT_SUCCESS,
    }
};

static void handle_signal(int sig)
{
    int ret = write(logs.config.output_fd, "\n", 1);
    (void) ret;
    errlogs_display();
    exit(1);
}

int errlogs_install()
{
    int ret;
    if ((ret = atexit(errlogs_display)) != 0)
    {
        perror("unable to install errlogs_display for atexit");
        return ret; 
    }

    struct sigaction sighandler = { 0 };
    sighandler.sa_handler = handle_signal;
    
    if ((ret = sigaction(SIGINT, &sighandler, NULL)) != 0)
    {
        perror("Unabled to install errlogs_display for SIGINT signal.");
        return ret;
    }

    return 0;
}

int errlogs_get_output_fd()
{
    return logs.config.output_fd;
}

int errlogs_set_output_fd(int output_fd)
{
    int old_fd = logs.config.output_fd;
    logs.config.output_fd = output_fd;
    return old_fd;
}

void errlogs_enable_color_diagnostics()
{
    logs.config.color_diagnostic_enabled = 1;
}

void errlogs_disable_color_diagnostics()
{
    logs.config.color_diagnostic_enabled = 0;
}

void errlogs_hide_warnings()
{
    logs.config.hide_warning_enabled = 1;
}

void errlogs_show_warnings()
{
    logs.config.hide_warning_enabled = 0;
}

void errlogs_enable_silent_success()
{
    logs.config.silent_success_enabled = 1;
}

void errlogs_disable_silent_success()
{
    logs.config.silent_success_enabled = 0;
}

void errlogs_get_config(ERRLOGS_CONFIG *config)
{
    logs.config = *config;
}

void errlogs_set_config(ERRLOGS_CONFIG *new_config, ERRLOGS_CONFIG *old_config)
{
    *old_config = logs.config;
    logs.config = *new_config;
}

// utility functions

static void push_log(struct error_node *node)
{
    if (node->type == SENTINEL_TYPE) return;

    node->next = logs.sentinel.next;
    node->prev = &logs.sentinel;
    if (logs.sentinel.next) logs.sentinel.next->prev = node;
    else logs.sentinel.prev = node;
    logs.sentinel.next = node;

    if (node->type == ERROR_TYPE)
    {
        ++logs.error_sz;
        ++logs.log_sz;
    }
    else if (node->type == WARNING_TYPE)
    {
        ++logs.warning_sz;
        ++logs.log_sz;
    }
}

static struct error_node *pop_log()
{
    struct error_node *node = logs.sentinel.prev;

    if (logs.log_sz != 0)
    {
        logs.sentinel.prev = node->prev;
        node->prev->next = &logs.sentinel;
        node->prev = node->next = NULL;
    }

    switch (node->type)
    {
    case ERROR_TYPE:
        logs.error_sz--;
        logs.log_sz--;
        break;
    case WARNING_TYPE:
        logs.warning_sz--;
        logs.log_sz--;
        break;
    default:
    }
    return node;
}

// REPORT FUNCTIONS

void errlogs_display()
{
    struct error_node *node = NULL;
    size_t len = 0;
    while (logs.log_sz)
    {
        node = pop_log();
        len = strlen(node->message);
        if (logs.config.hide_warning_enabled && node->type != WARNING_TYPE)
        {
            len = write(logs.config.output_fd, node->message, len);
        }
        else if (!logs.config.hide_warning_enabled)
        {
            len = write(logs.config.output_fd, node->message, len);
        }
        free((void*) node->message);
        free(node);
    }
}

static void errlogs_report_warning_no_color(const char *regex, LOC loc, const char *warn_msg)
{
    char char_range_buf[10] = { 0 };
    snprintf(char_range_buf, sizeof(char_range_buf), "[%lu:%lu]", loc.begin, loc.end - 1);

    size_t regex_len = strlen(regex);
    size_t warn_msg_len = strlen(warn_msg);
    size_t range_len = strlen(char_range_buf);

    // 10 for " warning: "
    size_t warn_line_1_len = range_len + 11 + warn_msg_len;
    // 1 for "\t" and 1 for "\n"
    size_t warn_line_2_len = 2 + regex_len;
    // "\t" + loc.end - loc.start + "\n"
    size_t warn_line_3_len = loc.end + 2;
    // +1 for null terminator 
    size_t total_string_len = warn_line_1_len + warn_line_2_len + warn_line_3_len + 1;

    char *warning = calloc(total_string_len, sizeof(char));
    sprintf(warning + 0, "%s warning: %s\n", char_range_buf, warn_msg);
    sprintf(warning + warn_line_1_len, "\t%s\n", regex);

    char *offset = warning + warn_line_1_len + warn_line_2_len;
    *offset = '\t';
    ++offset;
    for (size_t i = 0; i < loc.end; ++i)
    {
        if (i >= loc.begin && i < loc.end) *offset = '^';
        else *offset = ' ';
        offset++;
    }
    *offset = '\n';

    struct error_node *node = malloc(sizeof(struct error_node));
    node->message = warning;
    node->next = node->prev = NULL;
    node->type = WARNING_TYPE;

    push_log(node);
}

static void errlogs_report_warning_color(const char *regex, LOC loc, const char *warn_msg)
{
    char char_range_buf[10] = { 0 };
    snprintf(char_range_buf, sizeof(char_range_buf), "[%lu:%lu]", loc.begin, loc.end - 1);

    size_t regex_len = strlen(regex);
    size_t warn_msg_len = strlen(warn_msg);
    size_t range_len = strlen(char_range_buf);

    // 10 for " warning: "
    size_t warn_line_1_len = range_len + 20 + warn_msg_len;
    // 1 for "\t" and 1 for "\n"
    size_t warn_line_2_len = 2 + regex_len;
    // "\t" + loc.end - loc.start + "\n"
    size_t warn_line_3_len = loc.end + 2;
    // +1 for null terminator 
    size_t total_string_len = warn_line_1_len + warn_line_2_len + warn_line_3_len + 1;

    char *warning = calloc(total_string_len, sizeof(char));
    sprintf(warning + 0, "%s \033[33mwarning\033[0m: %s\n", char_range_buf, warn_msg);
    sprintf(warning + warn_line_1_len, "\t%s\n", regex);

    char *offset = warning + warn_line_1_len + warn_line_2_len;
    *offset = '\t';
    ++offset;
    for (size_t i = 0; i < loc.end; ++i)
    {
        if (i >= loc.begin && i < loc.end) *offset = '^';
        else *offset = ' ';
        offset++;
    }
    *offset = '\n';

    struct error_node *node = malloc(sizeof(struct error_node));
    node->message = warning;
    node->next = node->prev = NULL;
    node->type = WARNING_TYPE;

    push_log(node);
}

static void errlogs_report_error_no_color(const char *regex, LOC loc, const char *err_msg)
{
    char char_range_buf[10] = { 0 };
    snprintf(char_range_buf, sizeof(char_range_buf), "[%lu:%lu]", loc.begin, loc.end - 1);

    size_t regex_len = strlen(regex);
    size_t err_msg_len = strlen(err_msg);
    size_t range_len = strlen(char_range_buf);

    // 8 for " error: "
    size_t err_line_1_len = range_len + 9 + err_msg_len;
    // 1 for "\t" and 1 for "\n"
    size_t err_line_2_len = 2 + regex_len;
    // "\t" + loc.end - loc.start + "\n"
    size_t err_line_3_len = loc.end + 2;
    // +1 for null terminator 
    size_t total_string_len = err_line_1_len + err_line_2_len + err_line_3_len + 1;

    char *error_txt = calloc(total_string_len, sizeof(char));
    sprintf(error_txt + 0, "%s error: %s\n", char_range_buf, err_msg);
    sprintf(error_txt + err_line_1_len, "\t%s\n", regex);

    char *offset = error_txt + err_line_1_len + err_line_2_len;
    *offset = '\t';
    ++offset;
    for (size_t i = 0; i < loc.end; ++i)
    {
        if (i >= loc.begin && i < loc.end) *offset = '^';
        else *offset = ' ';
        offset++;
    }
    *offset = '\n';

    struct error_node *node = malloc(sizeof(struct error_node));
    node->message = error_txt;
    node->next = node->prev = NULL;
    node->type = ERROR_TYPE;

    push_log(node);
}

static void errlogs_report_error_color(const char *regex, LOC loc, const char *err_msg)
{
    char char_range_buf[10] = { 0 };
    snprintf(char_range_buf, sizeof(char_range_buf), "[%lu:%lu]", loc.begin, loc.end - 1);

    size_t regex_len = strlen(regex);
    size_t err_msg_len = strlen(err_msg);
    size_t range_len = strlen(char_range_buf);

    // 8 for " error: "
    size_t err_line_1_len = range_len + 18 + err_msg_len;
    // 1 for "\t" and 1 for "\n"
    size_t err_line_2_len = 2 + regex_len;
    // "\t" + loc.end - loc.start + "\n"
    size_t err_line_3_len = loc.end + 2;
    // +1 for null terminator 
    size_t total_string_len = err_line_1_len + err_line_2_len + err_line_3_len + 1;

    char *error_txt = calloc(total_string_len, sizeof(char));
    sprintf(error_txt + 0, "%s \033[31merror\033[0m: %s\n", char_range_buf, err_msg);
    sprintf(error_txt + err_line_1_len, "\t%s\n", regex);

    char *offset = error_txt + err_line_1_len + err_line_2_len;
    *offset = '\t';
    ++offset;
    for (size_t i = 0; i < loc.end; ++i)
    {
        if (i >= loc.begin && i < loc.end) *offset = '^';
        else *offset = ' ';
        offset++;
    }
    *offset = '\n';

    struct error_node *node = malloc(sizeof(struct error_node));
    node->message = error_txt;
    node->next = node->prev = NULL;
    node->type = ERROR_TYPE;

    push_log(node);
}

// LOG FUNCTIONS

// TODO: FIX THIS:

// 1:17 | warning: This is an error message.\n
// \t"REGEX REGEX REGEX"
//          ^^^^^
void errlogs_report_warning(const char *regex, LOC loc, const char *warn_msg)
{
    if (logs.config.color_diagnostic_enabled) errlogs_report_warning_color(regex, loc, warn_msg);
    else errlogs_report_warning_no_color(regex, loc, warn_msg);
}

void errlogs_report_error(const char *regex, LOC loc, const char *err_msg)
{
    if (logs.config.color_diagnostic_enabled) errlogs_report_error_color(regex, loc, err_msg);
    else errlogs_report_error_no_color(regex, loc, err_msg);
}
