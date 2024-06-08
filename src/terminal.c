#include "terminal.h"
#include "error.h"
#include "flash.h"
#include "flash_blocks.h"
#include "interrupts.h"
#include "measure.h"
#include "print.h"
#include "uart.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// finds the first space and replaces it with a null terminator
// (excluding the ones before the first word)
// the char* will be moved forward to skip all leading spaces
// returns pointer to part after space
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
    uart_println("Try 'error help' for a list of all commands.");
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
    uart_println("Try 'flash help' for a list of all commands.");
}

void set_timestamp(const char* raw_timestamp)
{
    MD_ASSERT(raw_timestamp, MD_ERROR_SET_TIMESTAMP_NULL);
    uart_print("input: '");
    uart_print(raw_timestamp);
    uart_println("'");
    if(strlen(raw_timestamp) == 0) {
        uart_println("Error: No timestamp was provided.");
        return;
    }

    char*    end_ptr;
    uint64_t timestamp = strtoul(raw_timestamp, &end_ptr, 0);
    uint8_t  len_read  = end_ptr - raw_timestamp;
    if(len_read != strlen(raw_timestamp)) {
        uart_print("Invalid char at pos: ");
        uart_print_uint8_t_hex(len_read);
        uart_println("");
        return;
    }

    static FlashTimestamp timestamp_block;
    flash_create_timestamp_block(&timestamp_block, timestamp);
    // we could also use flash_print_cur_timestamp() but that might take a lot longer
    uart_print("current unix sec time: ");
    uart_print_uint64_t_hex(timestamp_block.unix_second_timestamp);
    uart_println("");
    // just try storing it; when it fails, it fails hard
    flash_write_next_block((GenericFlashBlock*)&timestamp_block);
}
void parse_time_subcmd(char* arguments)
{
    MD_ASSERT(arguments, MOLD_ERROR_INVALID_PARAMS_PARSE_TIME_SUBCMD);

    char*       sub_cmd_arguments = null_terminate_after_first_word(&arguments);
    const char* sub_cmd           = arguments;

    if(!strcmp(sub_cmd, "set")) {
        null_terminate_after_first_word(&sub_cmd_arguments);
        const char* new_time = sub_cmd_arguments;
        set_timestamp(new_time);
        return;
    }
    if(!strcmp(sub_cmd, "get")) {
        flash_print_cur_timestamp();
        return;
    }
    if(!strcmp(sub_cmd, "help")) {
        uart_println("The possible subcommands are: set, get, help");
        return;
    }
    uart_print("Unknown time subcommand: '");
    uart_print(sub_cmd);
    uart_println("'");
    uart_println("Try 'time help' for a list of all commands.");
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
        measure_perform_measurement();
        return;
    }
    if(!strcmp(cmd, "version")) {
        print_version();
        return;
    }
    if(!strcmp(cmd, "time")) {
        parse_time_subcmd(arguments);
        return;
    }
    if(!strcmp(cmd, "help")) {
        uart_println("This is the help page. The possible commands are: reset, measure, help, error, flash, version, time");
        return;
    }
    uart_print("Unknown command: '");
    uart_print(cmd);
    uart_println("'");
    uart_println("Try 'help' for a list of all commands.");
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

void print_version()
{
    uart_print("mold_detect (github.com/christopher-besch/mold_detect)\r\nversion: ");
    uart_print(MOLD_DETECT_VERSION);
    uart_print(" commit: ");
    uart_println(COMMIT_HASH);
    uart_println("Developed by Christopher Besch\r\nat the Chair for Embedded System at the Karlsruher Institut für Technologie");
    uart_print_kit_logo();
}
