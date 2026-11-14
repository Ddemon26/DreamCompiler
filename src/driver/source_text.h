#ifndef DREAM_SOURCE_TEXT_H
#define DREAM_SOURCE_TEXT_H

/* Writable, NUL-terminated input. Collapse CRLF in place; preserve lone CR. */
void dream_normalize_newlines(char *buffer);

#endif
