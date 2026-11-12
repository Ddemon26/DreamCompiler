#include <assert.h>
#include "../../src/runtime/memory/memory.c"
#include "../../src/runtime/extensions/custom.h"

/* Validate operands before the real allocator reads a DrRef header. This makes
 * invalid literal releases and repeated releases deterministic on every host. */
static size_t releases;
static void checked_release(void *ptr) {
    if (!ptr) return;
    DrRef *ref = dr_head;
    while (ref && (void *)(ref + 1) != ptr) ref = ref->next;
    assert(ref && "cleanup must release a live managed allocation");
    releases++;
    dr_release(ptr);
}

#define dr_release checked_release
#define main dream_entry_main
#include DREAM_STRING_PROGRAM
#undef main
#undef dr_release

int main(void) {
    // Check independent ownership for literal, empty, interior and managed
    // backing storage, including use after the backing allocation is released.
    assert(dream_string_copy(NULL) == NULL);
    const char *literal = "literal";
    char *copy = dream_string_copy(literal);
    assert(copy && copy != literal && strcmp(copy, literal) == 0);
    assert(dr_head && dr_head->refcount == 1 && dr_head->next == NULL);
    dr_release(copy);
    assert(dr_head == NULL);
    copy = dream_string_copy("");
    assert(copy && copy[0] == '\0');
    dr_release(copy);
    char *backing = dream_concat("dynamic", " value");
    DrRef *owner = dr_head;
    copy = dream_string_copy(backing + 8);
    assert(copy && strcmp(copy, "value") == 0);
    assert(owner->refcount == 1 && dr_head->next == owner);
    dr_release(backing);
    assert(dr_head && dr_head->next == NULL);
    assert(strcmp(copy, "value") == 0);
    dr_release(copy);
    assert(dr_head == NULL);

    int result = dream_entry_main();
    assert(result == 0);
    assert(releases == 4 && "each indexed scalar local must be released once");
    assert(dr_head == NULL && "program shutdown must reclaim remaining storage");
    return result;
}
