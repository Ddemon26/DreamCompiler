#include <assert.h>
#include "../../src/runtime/memory/memory.c"
#include "../../src/runtime/extensions/custom.h"
#include "../../src/runtime/io/console.h"

static size_t copies, joins, releases, writes, lines;
static int checking_identity, observed;
static size_t expected_joins;
static char *original, *intermediate;
static const char *const join_text[] = {
    "startowned", "outer-startowned", "third-outer-startowned",
    "fourth-third-outer-startowned",
};

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
    if (expected_joins) {
        size_t index = joins % expected_joins;
        assert(ptr && !intermediate && !observed);
        assert(dr_head && (void *)(dr_head + 1) == ptr && dr_head->refcount == 1);
        assert(strcmp(ptr, join_text[index]) == 0);
        if (index == 0) {
            assert(!original);
        } else {
            assert(original && right == original);
            intermediate = original;
        }
        original = ptr;
        joins++;
    }
    return ptr;
}

static void observe_original(const char *ptr) {
    assert(ptr == original && !intermediate && !observed);
    assert(dr_head && !dr_head->next && dr_head->refcount == 1);
    assert(!expected_joins || joins % expected_joins == 0);
    const char *expected = expected_joins ? join_text[expected_joins - 1] : "owned";
    assert(strcmp(ptr, expected) == 0);
    observed = 1;
}

static void checked_write(const char *ptr) {
    if (!checking_identity && !expected_joins) {
        dr_console_write(ptr);
        return;
    }
    observe_original(ptr);
    writes++;
}

static void checked_writeln(const char *ptr) {
    if (!checking_identity && !expected_joins) {
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
    if (intermediate && ptr == intermediate) {
        assert(original && !observed);
        intermediate = NULL;
    }
    if (checking_identity || (expected_joins && ptr == original)) {
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
    expected_joins = 1;
    copies = releases = writes = lines = 0;
    for (size_t i = 1; i <= 10000; i++) {
        assert(DREAM_JOIN_OUTPUT_PROBE() == 4);
        assert(!original && !observed && !dr_head);
        assert(copies == 8 * i && joins == 4 * i && releases == 12 * i);
        assert(writes == 2 * i && lines == 2 * i);
    }
    expected_joins = 2;
    copies = joins = releases = writes = lines = 0;
    for (size_t i = 1; i <= 10000; i++) {
        assert(DREAM_NESTED_OUTPUT_PROBE() == 4);
        assert(!original && !intermediate && !observed && !dr_head);
        assert(copies == 12 * i && joins == 8 * i && releases == 20 * i);
        assert(writes == 2 * i && lines == 2 * i);
    }
    expected_joins = 3;
    copies = joins = releases = writes = lines = 0;
    for (size_t i = 1; i <= 10000; i++) {
        assert(DREAM_TRIPLE_OUTPUT_PROBE() == 4);
        assert(!original && !intermediate && !observed && !dr_head);
        assert(copies == 16 * i && joins == 12 * i && releases == 28 * i);
        assert(writes == 2 * i && lines == 2 * i);
    }
    expected_joins = 4;
    copies = joins = releases = writes = lines = 0;
    for (size_t i = 1; i <= 10000; i++) {
        assert(DREAM_FOURTH_OUTPUT_PROBE() == 4);
        assert(!original && !intermediate && !observed && !dr_head);
        assert(copies == 20 * i && joins == 16 * i && releases == 36 * i);
        assert(writes == 2 * i && lines == 2 * i);
    }
    expected_joins = 0;
    copies = releases = 0;
    assert(dream_entry_main() == 0);
    assert(copies == 63 && releases == 103 && !dr_head);
    return 0;
}
