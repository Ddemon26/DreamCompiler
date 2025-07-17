const std = @import("std");

const AllCSources = [_][]const u8{
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
    "src/ir/ir.c",
    "src/cfg/cfg.c",
    "src/ssa/ssa.c",
    "src/opt/pipeline.c",
    "src/opt/sccp.c",
    "src/opt/dce.c",
    "src/opt/value_numbering.c",
    "src/opt/licm.c",
    "src/codegen/c_emit.c",
    "src/codegen/codegen.c",
};

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe_mod = b.createModule(.{ .target = target, .optimize = optimize });
    const bootstrap_mod = b.createModule(.{ .target = target, .optimize = optimize });
    const lex_mod = b.createModule(.{ .target = target, .optimize = optimize });
    const parse_mod = b.createModule(.{ .target = target, .optimize = optimize });

    const re2c_step = b.addSystemCommand(&.{ "re2c", "-c", "-o", "src/lexer/lexer.c" });
    re2c_step.addFileArg(b.path("src/lexer/lexer.re"));
    re2c_step.addFileInput(b.path("src/lexer/tokens.def"));

    const bootstrap = b.addExecutable(.{ .name = "dreamc-bootstrap", .root_module = bootstrap_mod });
    bootstrap.step.dependOn(&re2c_step.step);
    bootstrap.addCSourceFiles(.{ .files = &AllCSources, .flags = &.{
        "-std=c11",
        "-Wall",
        "-Wextra",
        "-D_GNU_SOURCE",
    } });

    const lexexe = b.addExecutable(.{ .name = "lexdump", .root_module = lex_mod });
    lexexe.addCSourceFiles(.{ .files = &.{ "src/driver/lex_main.c", "src/lexer/lexer.c" }, .flags = &.{ "-std=c11", "-Wall", "-Wextra", "-D_GNU_SOURCE" } });
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

    const dr_files = collectDrSources(b);
    const compile_step = addCompileDrStep(b, bootstrap, target, optimize, dr_files);

    const exe = b.addExecutable(.{ .name = "DreamCompiler", .root_module = exe_mod });
    exe.step.dependOn(compile_step.step);
    exe.addCSourceFiles(.{ .files = &AllCSources, .flags = &.{
        "-std=c11",
        "-Wall",
        "-Wextra",
        "-D_GNU_SOURCE",
    } });
    for (compile_step.files) |gen| {
        exe.addCSourceFile(.{ .file = gen, .flags = &.{ "-std=c11", "-Wall", "-Wextra", "-D_GNU_SOURCE" } });
    }
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

    const test_step = b.step("test", "Run compiler tests");
    test_step.dependOn(&lexexe.step);
    test_step.dependOn(&parseexe.step);

    const fmt_zig = b.addFmt(.{ .paths = &.{"build.zig"} });
    const fmt_c = b.addSystemCommand(&.{ "clang-format", "-i" });
    for (AllCSources) |cfile| fmt_c.addFileArg(b.path(cfile));
    fmt_c.addFileArg(b.path("src/driver/lex_main.c"));
    fmt_c.addFileArg(b.path("src/driver/parse_main.c"));
    const fmt_step = b.step("format", "Format source files");
    fmt_step.dependOn(&fmt_zig.step);
    fmt_step.dependOn(&fmt_c.step);
}

fn collectDrSources(b: *std.Build) []const []const u8 {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    var list = std.ArrayList([]const u8).init(arena.allocator());
    var dir = std.fs.cwd().openDir("src", .{ .iterate = true }) catch return &.{};
    var walker = dir.walk(arena.allocator()) catch return &.{};
    while (walker.next() catch null) |entry| {
        if (entry.kind == .file and std.mem.endsWith(u8, entry.basename, ".dr")) {
            const p = b.pathJoin(&.{ "src", entry.path });
            list.append(p) catch {};
        }
    }
    return b.dupeStrings(list.items);
}

fn addCompileDrStep(
    b: *std.Build,
    bootstrap: *std.Build.Step.Compile,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    sources: []const []const u8,
) struct { step: *std.Build.Step, files: []const std.Build.LazyPath } {
    const step = b.step("compile-dr", "Compile .dr sources");
    var generated = std.ArrayList(std.Build.LazyPath).init(b.allocator);

    const triple = target.query.zigTriple(b.allocator) catch @panic("OOM");
    const mode_str = switch (optimize) {
        .Debug => "debug",
        .ReleaseSafe => "release-safe",
        .ReleaseFast => "release-fast",
        .ReleaseSmall => "release-small",
    };

    for (sources) |src| {
        const run = b.addRunArtifact(bootstrap);
        run.addArgs(&.{ "--emit-c", "--target", triple, "--build-mode", mode_str });
        run.addFileArg(b.path(src));
        run.addFileInput(b.path("src/lexer/tokens.def"));
        const out = run.addOutputFileArg(b.fmt("{s}.c", .{std.fs.path.stem(src)}));
        generated.append(out) catch @panic("OOM");
        step.dependOn(&run.step);
    }

    return .{ .step = step, .files = generated.toOwnedSlice() catch @panic("OOM") };
}
