#pragma once

void enter_terminal();

// finds the first space and replaces it with a null terminator
// (excluding the ones before the first word)
// the char* will be moved forward to skip all leading spaces
// returns pointer to part after space
char* null_terminate_after_first_word(char** line);
