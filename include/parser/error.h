#ifndef PARSER_ERROR_H
#define PARSER_ERROR_H

#include "loc.h"

// CONFIGURATION FUNCTIONS

typedef struct errlogs_config
{
    int output_fd;
    char color_diagnostic_enabled;
    char hide_warning_enabled;
    char silent_success_enabled;
    char context_frame;
} ERRLOGS_CONFIG;

/**
 * installs a signal handler so that logs are outputted on SIGINT
 * installs an atexit handler so that logs are ouputted on exit
 */
int errlogs_install();

/**
 * returns the current output file descriptor set
 * 
 * @return the output fd in the config
 */
int errlogs_get_output_fd();

/**
 * sets the output file descriptor for the configuration
 * 
 * @return the old output file descriptor
 */
int errlogs_set_output_fd(int output_fd);

/**
 * enables color diagnostic for the logger
 */
void errlogs_enable_color_diagnostics();

/**
 * disables color diagnostic for the logger
 */
void errlogs_disable_color_diagnostics();

/**
 * will hide warnings upon when the logs are displayed
 */
void errlogs_hide_warnings();

/**
 * will show warnings upon when the logs are displayed
 */
void errlogs_show_warnings();

/**
 * enable silent successes meaning if there is no errors,
 * the logger does not emit diagnostics.
 */
void errlogs_enable_silent_success();

/**
 * disables silent success meaning if there is no errors,
 * the logger will still emit diagnostics
 */
void errlogs_disable_silent_success();

/**
 * retrieves the handle for the logging configs
 * 
 * @param config the address to store the config in
 */
void errlogs_get_config(ERRLOGS_CONFIG *config);

/**
 * replaces the config in the logger with the new config,
 * 
 * @param new_config the address of the new config to set
 * @param old_config the address to store the old config in, `NULL` if not needed
 */
void errlogs_set_config(ERRLOGS_CONFIG *new_config, ERRLOGS_CONFIG *old_config);

// REPORT FUNCTIONS

/**
 * displays any logged messages that are stored
 */
void errlogs_display();

// LOG FUNCTIONS

/**
 * reports a warning to the logger
 * 
 * @param regex the regular expression
 * @param loc the location in the regex causing the warning
 * @param warn_msg the warning message for diagnostic
 */
void errlogs_report_warning(const char *regex, LOC loc, const char *warn_msg);

/**
 * reports a error to the logger
 * 
 * @param regex the regular expression
 * @param loc the location in the regex causing the error
 * @param err_msg the error message for diagnostic
 */
void errlogs_report_error(const char *regex, LOC loc, const char *err_msg);

#endif