#include "terminal.h"
#include "error.h"
#include "flash.h"
#include "flash_blocks.h"
#include "interrupts.h"
#include "measure.h"
#include "uart.h"

#include <string.h>

void parse_error_subcmd(char* arguments)
{
    MD_ASSERT(arguments, MOLD_ERROR_INVALID_PARAMS_PARSE_ERROR_SUBCMD);

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

void parse_flash_subcmd(char* arguments)
{
    MD_ASSERT(arguments, MOLD_ERROR_INVALID_PARAMS_PARSE_FLASH_SUBCMD);

    null_terminate_after_first_word(&arguments);
    const char* sub_cmd = arguments;

    if(!strcmp(sub_cmd, "erase")) {
        uart_print("Are you sure you want to erase the entire flash chip? [y/N] ");
        char resp = uart_rec();
        uart_println("");
        if(resp == 'y' || resp == 'Y')
            flash_chip_erase();
        else
            uart_println("Don't erase.");
        return;
    }
    if(!strcmp(sub_cmd, "usage")) {
        flash_print_usage();
        return;
    }
    if(!strcmp(sub_cmd, "print")) {
        flash_print_all_blocks();
        return;
    }
    if(!strcmp(sub_cmd, "help")) {
        uart_println("The possible subcommands are: erase, usage, print, help");
        return;
    }
    uart_print("Unknown flash subcommand: '");
    uart_print(sub_cmd);
    uart_println("'");
    uart_println("Try flash help for a list of all commands.");
}

void parse_cmd(char* input_line)
{
    MD_ASSERT(input_line, MOLD_ERROR_INVALID_PARAMS_PARSE_CMD);

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
    if(!strcmp(cmd, "flash")) {
        parse_flash_subcmd(arguments);
        return;
    }
    if(!strcmp(cmd, "measure")) {
        perform_measurement();
        return;
    }
    if(!strcmp(cmd, "help")) {
        uart_println("This is the help page. The possible commands are: reset, measure, help, error, flash");
        return;
    }
    // TODO: version, time
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
    MD_ASSERT(line && *line, MOLD_ERROR_INVALID_PARAMS_NULL_TERMINATE_AFTER_FIRST_WORD);

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
