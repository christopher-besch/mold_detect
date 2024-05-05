#include "terminal.h"
#include "error.h"
#include "interrupts.h"
#include "uart.h"

#include <string.h>

void parse_error_subcmd(char* arguments)
{
    if(!arguments) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_PARSE_ERROR_SUBCMD);
    }

    null_terminate_after_first_word(&arguments);
    const char* sub_cmd = arguments;

    if(!strcmp(sub_cmd, "reset")) {
        reset_errors();
        return;
    }
    if(!strcmp(sub_cmd, "list")) {
        list_errors();
        return;
    }
    if(!strcmp(sub_cmd, "test")) {
        raise_error(MOLD_ERROR_TEST);
        return;
    }
    if(!strcmp(sub_cmd, "help")) {
        uart_println("The possible subcommands are: reset, list, test, help");
        return;
    }
    uart_print("Unknown error subcommand: '");
    uart_print(sub_cmd);
    uart_println("'");
    uart_println("Try error help for a list of all commands.");
}

void parse_cmd(char* input_line)
{
    if(!input_line) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_PARSE_CMD);
    }

    char*       arguments = null_terminate_after_first_word(&input_line);
    const char* cmd       = input_line;

    if(!strcmp(cmd, "")) {
        return;
    }
    if(!strcmp(cmd, "reset")) {
        reset();
        return;
    }
    if(!strcmp(cmd, "error")) {
        parse_error_subcmd(arguments);
        return;
    }
    if(!strcmp(cmd, "help")) {
        uart_println("This is the help page. The possible commands are: reset, help, error");
        return;
    }
    // TODO: measure, version
    uart_print("Unknown command: '");
    uart_print(cmd);
    uart_println("'");
    uart_println("Try help for a list of all commands.");
}

// This function never returns.
// The only way of leaving is by resetting the device.
void enter_terminal()
{
    // this code is not re-intrant
    disable_measurements();
    while(1) {
        uart_println("");
        uart_print("(mold_detect)$ ");
        char* line = uart_rec_line();
        parse_cmd(line);
    }
}

char* null_terminate_after_first_word(char** line)
{
    if(!line || !*line) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_NULL_TERMINATE_AFTER_FIRST_WORD);
        return "";
    }

    // ignore leading spaces
    while(**line == ' ')
        ++*line;

    // now find the first space
    char* my_line = *line;
    // return a pointer to NULL when there is no second word
    while(*my_line) {
        if(*my_line == ' ') {
            // replace first space with null terminator
            *my_line = 0;
            // skip all following spaces
            ++my_line;
            while(*my_line == ' ')
                ++my_line;
            // return pointer to first char after spaces
            // this might be the null terminator of the input string
            return my_line;
        }
        ++my_line;
    }
    return my_line;
}
