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

int errlogs_install();

int errlogs_uninstall();

int errlogs_get_output_fd();

int errlogs_set_output_fd(int output_fd);

void errlogs_enable_color_diagnostics();

void errlogs_disable_color_diagnostics();

void errlogs_hide_warnings();

void errlogs_show_warnings();

void errlogs_enable_silent_success();

void errlogs_disable_silent_success();

int errlogs_get_context_frame();

int errlogs_set_context_frame(int frame);

void errlogs_get_config(ERRLOGS_CONFIG *config);

void errlogs_set_config(ERRLOGS_CONFIG *new_config, ERRLOGS_CONFIG *old_config);

// REPORT FUNCTIONS

int errlogs_report_warning(const char *regex, LOC loc, const char *warn_msg);

int errlogs_report_error(const char *regex, LOC loc, const char *warn_msg);

// LOG FUNCTIONS

void errlogs_push_error();

#endif