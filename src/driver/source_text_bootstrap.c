#include "source_text.h"

/* Stage zero only: keep the C seed independent of generated Dream code. */
void dream_normalize_newlines(char *buffer) {
  char *read = buffer, *write = buffer;
  while (*read) {
    if (read[0] == '\r' && read[1] == '\n') {
      *write++ = '\n';
      read += 2;
    } else {
      *write++ = *read++;
    }
  }
  *write = 0;
}
