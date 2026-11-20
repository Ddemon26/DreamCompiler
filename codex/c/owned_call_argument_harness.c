#include <assert.h>
#include "../../src/runtime/memory/memory.c"
#include "../../src/runtime/extensions/custom.h"
#include "../../src/runtime/io/console.h"

static size_t copies, joins, releases, writes, lines;
static int checking_identity, checking_join, observed;
static char *original;

static char *checked_copy(const char *source) {
    char *ptr = dream_string_copy(source);
    assert(ptr);
    copies++;
    if (checking_identity) {
        assert(!original && !observed && dr_head && !dr_head->next);
        assert((void *)(dr_head + 1) == ptr);
        original = ptr;
    }
    return ptr;
}

static char *checked_concat(const char *left, const char *right) {
    char *ptr = dream_concat(left, right);
    if (checking_join) {
        assert(ptr && !original && !observed);
        assert(dr_head && (void *)(dr_head + 1) == ptr && dr_head->refcount == 1);
        original = ptr;
        joins++;
    }
    return ptr;
}

static void observe_original(const char *ptr) {
    assert(ptr == original && !observed);
    assert(dr_head && !dr_head->next && dr_head->refcount == 1);
    assert(strcmp(ptr, checking_join ? "startowned" : "owned") == 0);
    observed = 1;
}

static void checked_write(const char *ptr) {
    if (!checking_identity && !checking_join) {
        dr_console_write(ptr);
        return;
    }
    observe_original(ptr);
    writes++;
}

static void checked_writeln(const char *ptr) {
    if (!checking_identity && !checking_join) {
        dr_console_writeln(ptr);
        return;
    }
    observe_original(ptr);
    lines++;
}

static void checked_release(void *ptr) {
    assert(ptr);
    DrRef *ref = dr_head;
    while (ref && (void *)(ref + 1) != ptr) ref = ref->next;
    assert(ref && ref->refcount == 1);
    if (checking_identity || (checking_join && ptr == original)) {
        assert(ptr == original && observed);
        original = NULL;
        observed = 0;
    }
    releases++;
    dr_release(ptr);
}

static void checked_shutdown(void) {
    assert(!dr_head);
    dr_release_all();
}

#define dream_string_copy checked_copy
#define dream_concat checked_concat
#define dr_console_write checked_write
#define dr_console_writeln checked_writeln
#define dr_release checked_release
#define dr_release_all checked_shutdown
#define main dream_entry_main
#include DREAM_STRING_PROGRAM
#undef main
#undef dr_release_all
#undef dr_release
#undef dr_console_writeln
#undef dr_console_write
#undef dream_concat
#undef dream_string_copy

int main(void) {
    checking_identity = 1;
    for (size_t i = 1; i <= 10000; i++) {
        DREAM_IDENTITY_PROBE();
        assert(copies == 3 * i && releases == 3 * i);
        assert(writes == 2 * i && lines == i);
        assert(!original && !observed && !dr_head);
    }
    checking_identity = 0;
    checking_join = 1;
    copies = releases = writes = lines = 0;
    for (size_t i = 1; i <= 10000; i++) {
        assert(DREAM_JOIN_OUTPUT_PROBE() == 4);
        assert(!original && !observed && !dr_head);
        assert(copies == 8 * i && joins == 4 * i && releases == 12 * i);
        assert(writes == 2 * i && lines == 2 * i);
    }
    checking_join = 0;
    copies = releases = 0;
    assert(dream_entry_main() == 0);
    assert(copies == 15 && releases == 19 && !dr_head);
    return 0;
}
