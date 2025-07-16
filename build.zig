const std = @import("std");

fn execFromStep(b: *std.Build, argv: struct { path: []const u8 }) *std.Build.Step {
    return &b.addSystemCommand(&.{ argv.path }).step;
}

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const dev = b.option(bool, "dev", "generate DFA tables") orelse false;
    if (dev) {
        _ = execFromStep(b, .{ .path = "tools/lexgen.zig" });
    }

    const exe = b.addExecutable(.{
        .name = "DreamCompiler",
        .root_module = module,
    });

    exe.addIncludePath(b.path("src/include"));
    exe.addCSourceFiles(.{
        .files = &.{
            "src/driver/main.c",
            "src/lib/lexer.c",
        },
        .flags = &.{
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-D_GNU_SOURCE",
        },
    });

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

    const test_mod = b.createModule(.{ .target = target, .optimize = optimize });
    const test_exe = b.addExecutable(.{
        .name = "lexer_test",
        .root_module = test_mod,
    });
    test_exe.addCSourceFile(.{ .file = b.path("tests/lexer.c"), .flags = &.{"-std=c11"} });
    test_exe.addIncludePath(b.path("src/include"));
    test_exe.addCSourceFile(.{ .file = b.path("src/lib/lexer.c"), .flags = &.{"-std=c11"} });
    test_exe.linkLibC();

    const test_step = b.step("test", "Run unit tests");
    const run_tests = b.addRunArtifact(test_exe);
    run_tests.step.dependOn(&test_exe.step);
    test_step.dependOn(&run_tests.step);
}
