/* Compiled explicitly by TestCompiler_LiteralFailures, outside the Go/cgo build.
 * Include the real implementation to inspect invariants without production fault
 * flags or test-name paths in the compiler. */
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
static int fail_sccp;
static void *test_calloc(size_t n, size_t size) {
  return fail_sccp ? NULL : calloc(n, size);
}
#define calloc test_calloc
#include "../../src/opt/sccp.c"
#undef calloc
static int bad_ir, nonconverging;
static int test_sccp(CFG *cfg) {
  if (nonconverging) return 1;
  if (bad_ir) {
    cfg->entry->instrs[0]->op = IR_MOV;
    cfg->entry->instrs[0]->a = ir_const(41);
    return 0;
  }
  return sccp_checked(cfg);
}
#define sccp_checked test_sccp
#include "../../src/opt/literal_fold.c"
#undef sccp_checked

static int fail_arena;
static void *test_malloc(size_t size) { return fail_arena ? NULL : malloc(size); }
#define malloc test_malloc
#include "../../src/parser/ast.c"
#undef malloc

int main(int argc, char **argv) {
  (void)argc;
  Node lhs = {.kind = ND_INT, .as.lit = {"6",1}};
  Node rhs = {.kind = ND_INT, .as.lit = {"7",1}};
  Node n = {.kind = ND_BINOP, .pos = {12,4}, .source_file = "input.dr",
            .source_text = "source", .as.bin = {TK_STAR,&lhs,&rhs}};
  Arena arena;
  arena_init(&arena);
  if (argv[1] && strcmp(argv[1],"sccp") == 0) fail_sccp = 1;
  if (argv[1] && strcmp(argv[1],"arena") == 0) fail_arena = 1;
  if (argv[1] && strcmp(argv[1],"mismatch") == 0) bad_ir = 1;
  if (argv[1] && strcmp(argv[1],"convergence") == 0) nonconverging = 1;
  if (fail_sccp || fail_arena || bad_ir || nonconverging) {
    bool ok = fold_literal_arithmetic(&arena,&n);
    assert(!ok && n.kind == ND_BINOP && n.as.bin.lhs == &lhs);
    arena_free(&arena);
    return ok ? 0 : 1;
  }
  assert(fold_literal_arithmetic(&arena,&n));
  assert(n.kind == ND_INT && n.as.lit.len == 2 && !memcmp(n.as.lit.start,"42",2));
  assert(n.pos.line == 12 && n.pos.column == 4 && !strcmp(n.source_file,"input.dr") && !strcmp(n.source_text,"source"));
  assert(!arena_try_alloc(&arena,SIZE_MAX));
  ExpressionIR ir = {0};
  IRInstr root = {.op = IR_MOV, .dst = {0}, .a = {-43}};
  ir.count = 1; ir.pointers[0] = &root;
  int result;
  assert(transformed_value(&ir,(IRValue){0},42,&result) && result == 42);
  assert(!transformed_value(&ir,(IRValue){1},42,&result));
  assert(!transformed_value(&ir,(IRValue){0},41,&result));
  root.op = IR_ADD;
  assert(!transformed_value(&ir,(IRValue){0},42,&result));
  root.op = IR_MOV; root.a.id = INT_MIN;
  assert(!transformed_value(&ir,(IRValue){0},42,&result));
  arena_free(&arena);
  return 0;
}
