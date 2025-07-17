const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe_mod = b.createModule(.{ .target = target, .optimize = optimize });
    const lex_mod = b.createModule(.{ .target = target, .optimize = optimize });
    const parse_mod = b.createModule(.{ .target = target, .optimize = optimize });

    const re2c_step = b.addSystemCommand(&.{ "re2c", "-c", "-o", "src/lexer/lexer.c" });
    re2c_step.addFileArg(b.path("src/lexer/lexer.re"));
    re2c_step.addFileInput(b.path("src/lexer/tokens.def"));

    const exe = b.addExecutable(.{ .name = "DreamCompiler", .root_module = exe_mod });
    exe.step.dependOn(&re2c_step.step);
    exe.addCSourceFiles(.{ .files = &.{
        "src/driver/main.c",
        "src/lexer/lexer.c",
        "src/parser/ast.c",
        "src/parser/parser.c",
        "src/parser/error.c",
        "src/parser/diagnostic.c",
        "src/sem/scope.c",
        "src/sem/symbol.c",
        "src/sem/type.c",
        "src/sem/infer.c",
        "ir/ir.c",
        "cfg/cfg.c",
        "ssa/ssa.c",
        "opt/pipeline.c",
        "opt/sccp.c",
        "opt/dce.c",
        "opt/value_numbering.c",
        "opt/licm.c",
        "codegen/c_emit.c",
        "src/codegen/codegen.c",
    }, .flags = &.{
        "-std=c11",
        "-Wall",
        "-Wextra",
        "-D_GNU_SOURCE",
    } });

    const lexexe = b.addExecutable(.{ .name = "lexdump", .root_module = lex_mod });
    lexexe.addCSourceFiles(.{ .files = &.{ "src/driver/lex_main.c", "src/lexer/lexer.c" },
        .flags = &.{ "-std=c11", "-Wall", "-Wextra", "-D_GNU_SOURCE" } });
    lexexe.linkLibC();
    lexexe.step.dependOn(&re2c_step.step);
    b.installArtifact(lexexe);

    const parseexe = b.addExecutable(.{ .name = "parse", .root_module = parse_mod });
    parseexe.addCSourceFiles(.{ .files = &.{
        "src/driver/parse_main.c",
        "src/lexer/lexer.c",
        "src/parser/ast.c",
        "src/parser/parser.c",
        "src/parser/error.c",
        "src/parser/diagnostic.c",
        "src/sem/type.c",
        "src/sem/infer.c",
        }, .flags = &.{ "-std=c11", "-Wall", "-Wextra", "-D_GNU_SOURCE" } });
    parseexe.linkLibC();
    parseexe.step.dependOn(&re2c_step.step);
    b.installArtifact(parseexe);

    exe.linkLibC();
    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.setEnvironmentVariable("CC", "zig cc");
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Build and run DreamCompiler");
    run_step.dependOn(&run_cmd.step);

    const lex_tests = b.addSystemCommand(&.{ "./run.sh" });
    lex_tests.setCwd(b.path("tests/lexer"));
    lex_tests.step.dependOn(&lexexe.step);

    const parse_tests = b.addSystemCommand(&.{ "./run.sh" });
    parse_tests.setCwd(b.path("tests/parser"));
    parse_tests.step.dependOn(&parseexe.step);

    const test_step = b.step("test", "Run lexer and parser tests");
    test_step.dependOn(&lex_tests.step);
    test_step.dependOn(&parse_tests.step);
}
