#include "source_text.h"
#include <stdio.h>
#include <string.h>

void dream_normalize_newlines_seed(char *buffer);
int dogfood_caller(int value);

static int check(const unsigned char *input, size_t length) {
  unsigned char actual[48], seed[48], expected[48];
  memset(actual, 0xa5, sizeof(actual));
  memcpy(actual + 1, input, length);
  actual[length + 1] = 0;
  memcpy(seed, actual, sizeof(seed));
  size_t used = 0;
  for (size_t i = 0; i < length && input[i]; ++i) {
    if (input[i] == '\r' && i + 1 < length && input[i + 1] == '\n') ++i;
    expected[used++] = input[i];
  }
  expected[used] = 0;
  dream_normalize_newlines((char *)actual + 1);
  dream_normalize_newlines_seed((char *)seed + 1);
  if (memcmp(actual, seed, sizeof(actual)) ||
      memcmp(actual + 1, expected, used + 1) || actual[0] != 0xa5 ||
      actual[length + 2] != 0xa5) {
    fprintf(stderr, "normalization mismatch for length %zu\n", length);
    return 1;
  }
  /* Preserve the seed's single-pass behavior on repeated calls, too. In
     particular, CR CR LF becomes CR LF, then LF on the second invocation. */
  dream_normalize_newlines((char *)actual + 1);
  dream_normalize_newlines_seed((char *)seed + 1);
  if (memcmp(actual, seed, sizeof(actual))) return 2;
  return 0;
}

int main(void) {
  if (dogfood_caller(21) != 42) return 3;
  const unsigned char alphabet[] = {'\r', '\n', 'a', 0x80, 0xff};
  unsigned char input[32];
  unsigned long cases = 0;
  size_t combinations = 1;
  for (size_t length = 0; length <= 7; ++length) {
    for (size_t value = 0; value < combinations; ++value) {
      size_t remaining = value;
      for (size_t i = 0; i < length; ++i) {
        input[i] = alphabet[remaining % sizeof(alphabet)];
        remaining /= sizeof(alphabet);
      }
      if (check(input, length)) return 1;
      ++cases;
    }
    combinations *= sizeof(alphabet);
  }
  for (unsigned int byte = 1; byte <= 255; ++byte) {
    input[0] = (unsigned char)byte;
    if (check(input, 1)) return 1;
    ++cases;
  }
  const unsigned char embedded[] = {'a', '\r', '\n', 0, '\r', '\n', 'b'};
  if (check(embedded, sizeof(embedded))) return 1;
  ++cases;
  printf("%lu normalization cases passed\n", cases);
  return 0;
}
