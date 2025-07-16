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

    const dev_refactor = b.option(bool, "dev-refactor", "generate DFA tables") orelse false;
    if (dev_refactor) {
        _ = execFromStep(b, .{ .path = "tools/lexgen.zig" });
    }

    const exe = b.addExecutable(.{
        .name = "DreamCompiler",
        .root_module = module,
    });

    exe.addCSourceFiles(.{
        .files = &.{
            "src/driver/main.c",
            "src/lexer/lexer.c",
            "src/parser/expression.c",
            "src/parser/statement.c",
            "src/parser/declaration.c",
            "src/parser/control_flow.c",
            "src/codegen/codegen.c",
            "src/ast/node.c",
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
}
