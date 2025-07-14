const std = @import("std");

/// build.zig for DreamCompiler (tested on Zig 0.15‑dev)
/// ------------------------------------------------------------
/// * Builds `src/dream.c` with Zig’s built‑in clang wrapper
///   (`zig cc`), so the **build itself** needs no external GCC.
/// * At **runtime** (`zig build run`), we export `CC=zig cc`
///   for DreamCompiler’s back‑end C compilation step.
/// ------------------------------------------------------------

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // -------- Executable --------------------------------------------------
    const exe = b.addExecutable(.{
        .name     = "DreamCompiler",
        .target   = target,
        .optimize = optimize,
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

    // -------- Convenience: zig build run -- <args> ------------------------
    const run_cmd = b.addRunArtifact(exe);

    // Provide DreamCompiler with a known‑good C compiler at runtime.
    // NOTE: Step.Run uses `setEnvironmentVariable` in Zig 0.15.
    run_cmd.setEnvironmentVariable("CC", "zig cc");

    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Build and run DreamCompiler");
    run_step.dependOn(&run_cmd.step);
}