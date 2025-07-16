#ifndef INFER_H
#define INFER_H
#include "type.h"
#include <stdbool.h>

bool unify(Type *a, Type *b);
#endif
