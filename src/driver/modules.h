#ifndef DREAM_MODULES_H
#define DREAM_MODULES_H

#include "../parser/parser.h"

/* Resolve sibling .dr imports and lower private/exported functions into one program. */
bool modules_expand_program(Arena *arena, const char *entry_path, Node *entry,
                            const char *source, WarningConfig warnings, Node **expanded);

#endif
