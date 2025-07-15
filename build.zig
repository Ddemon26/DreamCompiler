const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const exe = b.addExecutable(.{
        .name = "DreamCompiler",
        .root_module = module,
    });

    exe.addCSourceFiles(.{
        .files = &.{
            "src/main.c",
            "src/lexer.c",
            "src/parser.c",
            "src/codegen.c",
        },
        .flags = &.{
            "-std=c11",
            "-Wall",
            "-Wextra",
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
}
