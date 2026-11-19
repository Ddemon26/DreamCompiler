#include <assert.h>
#include "../../src/runtime/memory/memory.c"
#include "../../src/runtime/extensions/custom.h"
static size_t copies, joins, releases, peak;
static int identity_probe, join_assignment_probe;
static void *originals[128];
static size_t live_count(void) { size_t n=0; for(DrRef *r=dr_head;r;r=r->next) { assert(r->refcount==1); n++; } return n; }
static void record_allocation(char *ptr) {
    size_t index = copies + joins;
    assert(index < sizeof originals / sizeof originals[0]);
    originals[index] = ptr;
    size_t live = live_count();
    if (live > peak) peak = live;
}
static char *checked_copy(const char *s) {
    char *p=dream_string_copy(s);
    if (p) {
        record_allocation(p);
        copies++;
    }
    return p;
}
static char *checked_concat(const char *left, const char *right) {
    char *ptr = dream_concat(left, right);
    if (join_assignment_probe) {
        assert(joins < 2 && "execute each joined RHS once");
        assert(strcmp(ptr, joins == 0 ? "oldowned" : "newowned") == 0);
    }
    record_allocation(ptr);
    joins++;
    return ptr;
}
static void checked_release(void *p) {
    if(!p)return;
    DrRef *r=dr_head; while(r && (void*)(r+1)!=p)r=r->next;
    assert(r && r->refcount==1);
    // Each release consumes a recorded allocation. The probes also check order.
    size_t i=0; while(i<copies+joins && originals[i]!=p)i++;
    assert(i<copies+joins); originals[i]=NULL;
    if(identity_probe) assert(i==releases && "old/current slots release in allocation order");
    if (join_assignment_probe) {
        // First join frees its operands, then old. The nested assignment frees
        // that join, then the second join frees its operands before new.
        static const size_t order[] = {1, 2, 0, 3, 5, 6, 4, 7};
        assert(releases < sizeof order / sizeof order[0]);
        assert(i == order[releases] && "release the original join and post-RHS slot in order");
    }
    releases++; dr_release(p);
}
static void checked_shutdown(void) {
    assert(live_count()==1); assert(strcmp((char*)(dr_head+1),"global")==0);
    assert((void*)(dr_head+1)==originals[copies-1]);
    checked_release(dr_head+1); assert(dr_head==NULL);
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
    identity_probe=1;
    for(size_t i=0;i<10000;i++) {
        copies=releases=peak=0; DREAM_IDENTITY_PROBE();
        assert(copies==11 && releases==11 && peak==2 && dr_head==NULL);
        assert(originals[0]==NULL && originals[1]==NULL);
    }
    identity_probe=0;
    copies=releases=peak=0;
    const char *p=DREAM_TRANSFER_PROBE();
    assert(copies==2 && releases==1 && p==originals[1] && originals[0]==NULL);
    checked_release((void*)p); assert(dr_head==NULL);
    join_assignment_probe = 1;
    for (size_t i = 0; i < 10000; i++) {
        copies = joins = releases = peak = 0;
        DREAM_JOIN_ASSIGNMENT_PROBE();
        assert(copies == 6 && joins == 2 && releases == 8 && peak == 4);
        assert(dr_head == NULL);
    }
    join_assignment_probe = 0;
    joins = 0;
    copies=releases=peak=0;
    assert(dream_entry_main()==0); assert(dr_head==NULL && copies==19 && copies==releases);
    return 0;
}
