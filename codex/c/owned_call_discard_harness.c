#include <assert.h>
#include "../../src/runtime/memory/memory.c"
#include "../../src/runtime/extensions/custom.h"

static size_t copies, releases;
static int checking_identity;
static void *original;

static char *checked_copy(const char *source) {
    char *ptr = dream_string_copy(source);
    if (ptr) {
        copies++;
        if (checking_identity) {
            assert(original == NULL && dr_head && dr_head->next == NULL);
            assert((void *)(dr_head + 1) == ptr);
            original = ptr;
        }
    }
    return ptr;
}

static void checked_release(void *ptr) {
    if (!ptr) return;
    DrRef *ref = dr_head;
    while (ref && (void *)(ref + 1) != ptr) ref = ref->next;
    assert(ref && ref->refcount == 1);
    if (checking_identity) {
        assert(ptr == original);
        original = NULL;
    }
    releases++;
    dr_release(ptr);
}

static void checked_shutdown(void) {
    assert(dr_head == NULL);
    dr_release_all();
}

#define dream_string_copy checked_copy
#define dr_release checked_release
#define dr_release_all checked_shutdown
#define main dream_entry_main
#include DREAM_STRING_PROGRAM
#undef main
#undef dr_release_all
#undef dr_release
#undef dream_string_copy

int main(void) {
    checking_identity = 1;
    for (size_t i = 0; i < 10000; i++) {
        DREAM_IDENTITY_PROBE();
        assert(copies == i + 1 && releases == i + 1);
        assert(original == NULL && dr_head == NULL);
    }
    checking_identity = 0;
    copies = releases = 0;
    assert(dream_entry_main() == 0);
    assert(copies == 16 && releases == 16 && dr_head == NULL);
    return 0;
}
