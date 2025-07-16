const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const re2c_step = b.addSystemCommand(&.{ "re2c", "-c", "-o", "src/lexer/lexer.c" });
    re2c_step.addFileArg(b.path("src/lexer/lexer.re"));
    re2c_step.addFileInput(b.path("src/lexer/tokens.def"));

    const exe = b.addExecutable(.{
        .name = "DreamCompiler",
        .root_module = module,
    });
    exe.step.dependOn(&re2c_step.step);

    exe.addCSourceFiles(.{
        .files = &.{
            "src/driver/main.c",
            "src/lexer/lexer.c",
            "src/parser/ast.c",
            "src/parser/parser.c",
            "src/parser/error.c",
        },
        .flags = &.{
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-D_GNU_SOURCE",
        },
    });

    const lexexe = b.addExecutable(.{
        .name = "lexdump",
        .root_module = module,
    });
    lexexe.addCSourceFiles(.{
        .files = &.{ "src/driver/lex_main.c", "src/lexer/lexer.c" },
        .flags = &.{ "-std=c11", "-Wall", "-Wextra", "-D_GNU_SOURCE" },
    });
    lexexe.linkLibC();
    lexexe.step.dependOn(&re2c_step.step);
    b.installArtifact(lexexe);

    const parseexe = b.addExecutable(.{
        .name = "parse",
        .root_module = module,
    });
    parseexe.addCSourceFiles(.{
        .files = &.{
            "src/driver/parse_main.c",
            "src/lexer/lexer.c",
            "src/parser/ast.c",
            "src/parser/parser.c",
            "src/parser/error.c",
        },
        .flags = &.{ "-std=c11", "-Wall", "-Wextra", "-D_GNU_SOURCE" },
    });
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

    const lex_tests = b.addSystemCommand(&.{"tests/lexer/run.sh"});
    lex_tests.step.dependOn(&lexexe.step);

    const parse_tests = b.addSystemCommand(&.{"tests/parser/run.sh"});
    parse_tests.step.dependOn(&parseexe.step);

    const test_step = b.step("test", "Run lexer and parser tests");
    test_step.dependOn(&lex_tests.step);
    test_step.dependOn(&parse_tests.step);
}
