#include <assert.h>
#include "../../src/runtime/memory/memory.c"
#include "../../src/runtime/extensions/custom.h"

static size_t copies, releases, peak;
static void *original;
static int identity_probe;

static char *checked_copy(const char *source) {
    char *result = dream_string_copy(source);
    if (result) {
        copies++;
        original = result;
        size_t live = 0;
        for (DrRef *ref = dr_head; ref; ref = ref->next) {
            assert(ref->refcount == 1);
            live++;
        }
        if (live > peak) peak = live;
    }
    return result;
}

static void checked_release(void *ptr) {
    if (!ptr) return;
    DrRef *ref = dr_head;
    while (ref && (void *)(ref + 1) != ptr) ref = ref->next;
    assert(ref && "release must target a live allocation base");
    assert(ref->refcount == 1 && "adoption preserves the original reference");
    if (identity_probe)
        assert(ptr == original && "caller releases the original factory allocation");
    releases++;
    dr_release(ptr);
}

static void checked_shutdown(void) {
    // Top-level storage has program lifetime; all scoped results are gone.
    assert(dr_head && dr_head->next == NULL && dr_head->refcount == 1);
    assert((void *)(dr_head + 1) == original);
    assert(strcmp(original, "global") == 0);
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
    identity_probe = 1;
    for (size_t i = 0; i < 10000; i++) {
        DREAM_IDENTITY_PROBE();
        assert(dr_head == NULL);
        assert(copies == i + 1 && releases == i + 1);
        assert(peak == 1 && "repeated declarations need only one live result");
    }
    size_t before = copies;
    const char *result = DREAM_TRANSFER_PROBE();
    assert(copies == before + 1 && result == original);
    checked_release((void *)result);
    assert(dr_head == NULL);
    identity_probe = 0;
    copies = releases = peak = 0;
    assert(dream_entry_main() == 0);
    assert(copies == DREAM_EXPECTED_COPIES);
    assert(releases == DREAM_EXPECTED_COPIES - 1);
    assert(dr_head == NULL);
    return 0;
}
