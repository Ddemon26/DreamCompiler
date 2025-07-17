#ifndef INFER_H
#define INFER_H
#include "type.h"
#include <stdbool.h>

/**
 * @brief Unifies two types, ensuring they are compatible.
 *
 * @param a Pointer to the first type.
 * @param b Pointer to the second type.
 * @return True if the types can be unified, false otherwise.
 */
bool unify(Type *a, Type *b);
#endif
