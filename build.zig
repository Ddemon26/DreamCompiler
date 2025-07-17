const std = @import("std");

fn compileDrSources(b: *std.Build, exe: *std.Build.Step.Compile) []std.Build.LazyPath {
    const allocator = b.allocator;
    var c_files = std.ArrayList(std.Build.LazyPath).init(allocator);
    var src_dir = std.fs.cwd().openDir("src", .{ .iterate = true }) catch unreachable;
    var it = src_dir.walk(allocator) catch unreachable;
    while (it.next() catch unreachable) |entry| {
        if (entry.kind != .file) continue;
        if (!std.mem.endsWith(u8, entry.path, ".dr")) continue;
        const in_path = b.pathJoin(&.{ "src", entry.path });
        const run = b.addRunArtifact(exe);
        run.addArg(in_path);
        run.setEnvironmentVariable("SKIP_COMPILE", "1");
        const base = std.fs.path.basename(entry.path);
        const stem = std.fs.path.stem(base);
        const out_lp = run.addOutputFileArg(b.fmt("{s}.c", .{stem}));
        const out_path = out_lp.getPath2(b, &run.step);
        run.setEnvironmentVariable("DREAM_OUT", out_path);
        run.step.dependOn(b.getInstallStep());
        c_files.append(out_lp) catch unreachable;
    }
    return c_files.toOwnedSlice() catch unreachable;
}

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

    const generated = compileDrSources(b, exe);
    if (generated.len > 0) {
        const prog_mod = b.createModule(.{ .target = target, .optimize = optimize });
        const prog = b.addExecutable(.{ .name = "dream_program", .root_module = prog_mod });
        for (generated) |g| {
            prog.addCSourceFile(.{ .file = g, .flags = &.{"-std=c11"} });
        }
        prog.linkLibC();
        b.installArtifact(prog);

        const run_dream = b.addRunArtifact(prog);
        if (b.args) |args| run_dream.addArgs(args);
        const dr_step = b.step("rundr", "Build and run Dream program");
        dr_step.dependOn(&run_dream.step);
    }

    const test_step = b.step("test", "Run regression tests");
    var tdir = std.fs.cwd().openDir("tests", .{ .iterate = true }) catch unreachable;
    var tit = tdir.walk(b.allocator) catch unreachable;
    while (tit.next() catch unreachable) |e| {
        if (e.kind != .file) continue;
        if (!std.mem.endsWith(u8, e.path, ".dr")) continue;
        const path = b.pathJoin(&.{ "tests", e.path });
        const r = b.addRunArtifact(exe);
        r.addArg(path);
        r.step.dependOn(b.getInstallStep());
        test_step.dependOn(&r.step);
    }

    const fmt_step = b.step("format", "Format C sources");
    var sdir = std.fs.cwd().openDir("src", .{ .iterate = true }) catch unreachable;
    var sit = sdir.walk(b.allocator) catch unreachable;
    while (sit.next() catch unreachable) |f| {
        if (f.kind != .file) continue;
        const ext = std.fs.path.extension(f.path);
        if (!std.mem.eql(u8, ext, ".c") and !std.mem.eql(u8, ext, ".h")) continue;
        const fp = b.pathJoin(&.{ "src", f.path });
        const cmd = b.addSystemCommand(&.{ "clang-format", "-i", fp });
        fmt_step.dependOn(&cmd.step);
    }
}
