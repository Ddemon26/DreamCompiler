#include <assert.h>
#include "../../src/runtime/memory/memory.c"
#include "../../src/runtime/extensions/custom.h"

static size_t copies, joins, releases;
static int checking_identity;
static size_t expected_joins;
static void *original;
static const char *const join_text[] = {
    "startowned", "outer-startowned", "third-outer-startowned", "fourth-third-outer-startowned",
};
static char *join[sizeof join_text / sizeof join_text[0]];

static char *checked_concat(const char *left, const char *right) {
    char *result = dream_concat(left, right);
    if (expected_joins) {
        assert(joins < expected_joins && join[joins] == NULL && result);
        assert(strcmp(result, join_text[joins]) == 0);
        assert(dr_head && (void *)(dr_head + 1) == result && dr_head->refcount == 1);
        join[joins++] = result;
    }
    return result;
}

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
    if (expected_joins && releases >= 3 && releases < 2 * expected_joins && releases % 2 == 1) {
        // Each outer append releases its slot, then the original inner join.
        size_t inner = (releases - 3) / 2;
        assert(ptr == join[inner]);
        join[inner] = NULL;
    }
    if (expected_joins && releases == 2 * expected_joins) {
        // Both operands must be gone when the original final join is released.
        assert(joins == expected_joins && ptr == join[joins - 1]);
        assert(ref == dr_head && ref->next == NULL);
        join[joins - 1] = NULL;
    }
    releases++;
    dr_release(ptr);
}

static void checked_shutdown(void) {
    assert(dr_head == NULL);
    dr_release_all();
}

#define dream_string_copy checked_copy
#define dream_concat checked_concat
#define dr_release checked_release
#define dr_release_all checked_shutdown
#define main dream_entry_main
#include DREAM_STRING_PROGRAM
#undef main
#undef dr_release_all
#undef dr_release
#undef dream_concat
#undef dream_string_copy

int main(void) {
    checking_identity = 1;
    for (size_t i = 0; i < 10000; i++) {
        DREAM_IDENTITY_PROBE();
        assert(copies == i + 1 && releases == i + 1);
        assert(original == NULL && dr_head == NULL);
    }
    checking_identity = 0;
    int (*const probes[])(void) = {
        DREAM_JOIN_DISCARD_PROBE, DREAM_NESTED_JOIN_DISCARD_PROBE, DREAM_TRIPLE_JOIN_DISCARD_PROBE,
        DREAM_FOURTH_JOIN_DISCARD_PROBE,
    };
    for (expected_joins = 1; expected_joins <= sizeof probes / sizeof probes[0]; expected_joins++) {
        for (size_t i = 0; i < 10000; i++) {
            copies = joins = releases = 0;
            int count = probes[expected_joins - 1]();
            assert(count == 1);
            assert(dr_head == NULL && "discard the original final join before returning");
            for (size_t j = 0; j < expected_joins; j++) {
                assert(join[j] == NULL);
            }
            assert(copies == expected_joins + 1 && joins == expected_joins &&
                   releases == 2 * expected_joins + 1);
        }
    }
    expected_joins = 0;
    copies = releases = 0;
    assert(dream_entry_main() == 0);
    assert(copies == 30 && releases == 40 && dr_head == NULL);
    return 0;
}
