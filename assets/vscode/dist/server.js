"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const node_1 = require("vscode-languageserver/node");
const vscode_languageserver_textdocument_1 = require("vscode-languageserver-textdocument");
const child_process_1 = require("child_process");
const os = require("os");
const path = require("path");
const fs = require("fs");
const documentsInfo = new Map();
const workspaceFolders = [];
let globalSettings = {};
// Enhanced caching and performance
const CACHE_TTL = 5000; // 5 seconds
const MAX_CACHE_SIZE = 100;
const parseCache = new Map();
const compilerCache = { isRunning: false, lastActivity: 0 };
const connection = (0, node_1.createConnection)(node_1.ProposedFeatures.all);
const documents = new node_1.TextDocuments(vscode_languageserver_textdocument_1.TextDocument);
connection.onInitialize((params) => {
    if (params.workspaceFolders) {
        workspaceFolders.push(...params.workspaceFolders);
    }
    return {
        capabilities: {
            textDocumentSync: {
                openClose: true,
                change: 2, // Incremental
                willSaveWaitUntil: false,
                save: { includeText: false }
            },
            completionProvider: {
                resolveProvider: true,
                triggerCharacters: ['.', ':', '(', '<']
            },
            hoverProvider: true,
            definitionProvider: true,
            documentSymbolProvider: true,
            workspaceSymbolProvider: true,
            signatureHelpProvider: {
                triggerCharacters: ['(', ',']
            },
            documentFormattingProvider: true,
            documentRangeFormattingProvider: true,
            renameProvider: true,
            foldingRangeProvider: true,
            semanticTokensProvider: {
                legend: {
                    tokenTypes: ['keyword', 'string', 'number', 'operator', 'variable', 'function', 'class', 'comment'],
                    tokenModifiers: ['declaration', 'definition', 'static', 'async']
                },
                full: true,
                range: true
            }
        }
    };
});
function getParseExecutable() {
    const exe = os.platform() === 'win32' ? 'parse.exe' : 'parse';
    // Try multiple potential locations
    const candidates = [
        path.join(__dirname, '..', '..', 'zig-out', 'bin', exe),
        path.join(process.cwd(), 'zig-out', 'bin', exe),
        exe // Fallback to PATH
    ];
    for (const candidate of candidates) {
        if (fs.existsSync(candidate) || candidate === exe) {
            return candidate;
        }
    }
    return candidates[0]; // Default fallback
}
function getDreamCompilerExecutable() {
    const exe = os.platform() === 'win32' ? 'DreamCompiler.exe' : 'DreamCompiler';
    const candidates = [
        path.join(__dirname, '..', '..', 'zig-out', 'bin', exe),
        path.join(process.cwd(), 'zig-out', 'bin', exe),
        exe
    ];
    for (const candidate of candidates) {
        if (fs.existsSync(candidate) || candidate === exe) {
            return candidate;
        }
    }
    return candidates[0];
}
// Enhanced parsing with caching and better error handling
function parseDocumentEnhanced(document) {
    const content = document.getText();
    const contentHash = hashString(content);
    const cacheKey = `${document.uri}:${contentHash}`;
    // Check cache first
    const cached = parseCache.get(cacheKey);
    if (cached && (Date.now() - cached.timestamp) < CACHE_TTL) {
        return cached.result;
    }
    const parseExe = getParseExecutable();
    const tempFile = path.join(os.tmpdir(), `dream_lsp_${Date.now()}.dr`);
    try {
        // Write content to temp file
        fs.writeFileSync(tempFile, content);
        // Parse with symbols
        const symbolsResult = (0, child_process_1.spawnSync)(parseExe, ['--symbols', tempFile], {
            timeout: 5000,
            encoding: 'utf8'
        });
        // Parse for diagnostics
        const diagResult = (0, child_process_1.spawnSync)(parseExe, [tempFile], {
            timeout: 5000,
            encoding: 'utf8'
        });
        const symbols = [];
        if (symbolsResult.stdout) {
            try {
                const symbolData = JSON.parse(symbolsResult.stdout);
                for (const s of symbolData) {
                    symbols.push({
                        name: s.name,
                        line: s.line,
                        character: s.character,
                        kind: s.kind,
                        range: {
                            start: { line: s.line, character: s.character },
                            end: { line: s.line, character: s.character + s.name.length }
                        }
                    });
                }
            }
            catch (e) {
                // Ignore JSON parse errors
            }
        }
        const diagnostics = parseDiagnostics(diagResult.stderr || '', document);
        const result = {
            success: diagResult.status === 0,
            diagnostics,
            symbols,
            errors: diagResult.stderr ? diagResult.stderr.split('\n').filter(l => l.trim()) : []
        };
        // Cache the result
        parseCache.set(cacheKey, { result, timestamp: Date.now() });
        // Cleanup old cache entries
        if (parseCache.size > MAX_CACHE_SIZE) {
            const oldestKey = Array.from(parseCache.keys())[0];
            parseCache.delete(oldestKey);
        }
        return result;
    }
    catch (error) {
        return {
            success: false,
            diagnostics: [{
                    range: { start: { line: 0, character: 0 }, end: { line: 0, character: 0 } },
                    severity: node_1.DiagnosticSeverity.Error,
                    message: `LSP Error: ${error}`,
                    source: 'DreamLSP'
                }],
            symbols: [],
            errors: [String(error)]
        };
    }
    finally {
        // Cleanup temp file
        if (fs.existsSync(tempFile)) {
            fs.unlinkSync(tempFile);
        }
    }
}
function hashString(str) {
    let hash = 0;
    for (let i = 0; i < str.length; i++) {
        const char = str.charCodeAt(i);
        hash = ((hash << 5) - hash) + char;
        hash = hash & hash; // Convert to 32-bit integer
    }
    return hash.toString();
}
function parseDiagnostics(stderr, document) {
    const diagnostics = [];
    if (!stderr)
        return diagnostics;
    const lines = stderr.split(/\r?\n/);
    const pattern = /(\d+):(\d+): (error|warning): (.*)/;
    const hintPattern = /help: (.*)/;
    for (let i = 0; i < lines.length; i++) {
        const line = lines[i].trim();
        const match = pattern.exec(line);
        if (match) {
            let message = match[4];
            // Look for hints in subsequent lines
            if (i + 3 < lines.length) {
                const hintMatch = hintPattern.exec(lines[i + 3]);
                if (hintMatch) {
                    message += `\n💡 ${hintMatch[1]}`;
                }
            }
            // Enhanced range detection
            const startLine = parseInt(match[1]) - 1;
            const startChar = Math.max(0, parseInt(match[2]) - 1);
            let endLine = startLine;
            let endChar = startChar + 1;
            // Look for token span indicators
            if (i + 2 < lines.length && lines[i + 2].includes('~')) {
                const spanLine = lines[i + 2];
                const spanStart = spanLine.indexOf('~');
                const spanEnd = spanLine.lastIndexOf('~');
                if (spanStart >= 0 && spanEnd > spanStart) {
                    endChar = startChar + (spanEnd - spanStart) + 1;
                }
            }
            // Ensure range is within document bounds
            const docText = document.getText();
            const docLines = docText.split(/\r?\n/);
            if (startLine < docLines.length) {
                endChar = Math.min(endChar, docLines[startLine].length);
            }
            diagnostics.push({
                range: {
                    start: { line: startLine, character: startChar },
                    end: { line: endLine, character: endChar }
                },
                severity: match[3] === 'error' ? node_1.DiagnosticSeverity.Error : node_1.DiagnosticSeverity.Warning,
                message,
                source: 'DreamCompiler'
            });
        }
    }
    return diagnostics;
}
function analyzeDocument(document) {
    const parseResult = parseDocumentEnhanced(document);
    const info = {
        symbols: new Map(),
        diagnostics: parseResult.diagnostics,
        lastModified: Date.now(),
        version: document.version
    };
    // Build symbol map
    for (const symbol of parseResult.symbols) {
        info.symbols.set(symbol.name, symbol);
    }
    documentsInfo.set(document.uri, info);
    connection.sendDiagnostics({ uri: document.uri, diagnostics: parseResult.diagnostics });
}
documents.onDidOpen(e => analyzeDocument(e.document));
documents.onDidChangeContent(e => analyzeDocument(e.document));
// Enhanced completion with context awareness
connection.onCompletion((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return [];
    const items = [];
    const line = document.getText({
        start: { line: params.position.line, character: 0 },
        end: { line: params.position.line + 1, character: 0 }
    });
    const linePrefix = line.substring(0, params.position.character);
    // Built-in completions
    const builtins = [
        {
            label: 'Console.WriteLine',
            kind: node_1.CompletionItemKind.Method,
            detail: 'void Console.WriteLine(string text)',
            documentation: {
                kind: node_1.MarkupKind.Markdown,
                value: 'Writes a line of text to the console output'
            },
            insertText: 'Console.WriteLine($1)',
            insertTextFormat: node_1.InsertTextFormat.Snippet
        },
        {
            label: 'Console.Write',
            kind: node_1.CompletionItemKind.Method,
            detail: 'void Console.Write(string text)',
            documentation: {
                kind: node_1.MarkupKind.Markdown,
                value: 'Writes text to the console output without newline'
            },
            insertText: 'Console.Write($1)',
            insertTextFormat: node_1.InsertTextFormat.Snippet
        },
        {
            label: 'Console.ReadLine',
            kind: node_1.CompletionItemKind.Method,
            detail: 'string Console.ReadLine()',
            documentation: {
                kind: node_1.MarkupKind.Markdown,
                value: 'Reads a line of input from the console'
            },
            insertText: 'Console.ReadLine()',
            insertTextFormat: node_1.InsertTextFormat.PlainText
        }
    ];
    // Language keywords and constructs based on current token set
    const keywords = [
        'func', 'int', 'float', 'bool', 'char', 'string', 'void',
        'if', 'else', 'while', 'do', 'for', 'switch', 'case', 'default',
        'break', 'continue', 'return', 'try', 'catch', 'throw', 'finally',
        'class', 'struct', 'new', 'this', 'base', 'static', 'async', 'await',
        'var', 'let', 'using', 'import', 'module', 'export',
        'Task', 'TaskResult', 'true', 'false', 'null'
    ];
    // Add keyword completions based on context
    if (!linePrefix.match(/\w+\s*\./)) { // Not after a dot
        for (const keyword of keywords) {
            if (keyword.startsWith(getWordAtPosition(document, params.position))) {
                items.push({
                    label: keyword,
                    kind: node_1.CompletionItemKind.Keyword,
                    detail: `Dream keyword`,
                    sortText: `1_${keyword}` // Sort keywords higher
                });
            }
        }
    }
    // Add built-in completions
    items.push(...builtins);
    // Document symbols
    const info = documentsInfo.get(document.uri);
    if (info) {
        for (const [name, symbol] of info.symbols) {
            const kind = symbol.kind === 'func' ? node_1.CompletionItemKind.Function :
                symbol.kind === 'class' ? node_1.CompletionItemKind.Class :
                    symbol.kind === 'struct' ? node_1.CompletionItemKind.Struct :
                        node_1.CompletionItemKind.Variable;
            items.push({
                label: name,
                kind,
                detail: symbol.detail || `${symbol.kind} ${name}`,
                sortText: `2_${name}` // Sort after keywords
            });
        }
    }
    // Workspace symbols (from other files)
    for (const [uri, docInfo] of documentsInfo) {
        if (uri !== document.uri) {
            for (const [name, symbol] of docInfo.symbols) {
                if (symbol.kind === 'func' || symbol.kind === 'class') {
                    const kind = symbol.kind === 'func' ? node_1.CompletionItemKind.Function : node_1.CompletionItemKind.Class;
                    items.push({
                        label: name,
                        kind,
                        detail: `${symbol.kind} ${name} (from ${path.basename(uri)})`,
                        sortText: `3_${name}` // Sort workspace symbols last
                    });
                }
            }
        }
    }
    return items;
});
// Enhanced go-to-definition with cross-file support
connection.onDefinition((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return [];
    const word = getWordAtPosition(document, params.position);
    if (!word)
        return [];
    const locations = [];
    // Check current document first
    const info = documentsInfo.get(params.textDocument.uri);
    if (info) {
        const symbol = info.symbols.get(word);
        if (symbol && symbol.range) {
            locations.push({
                uri: params.textDocument.uri,
                range: symbol.range
            });
        }
    }
    // Check other documents in workspace
    if (locations.length === 0) {
        for (const [uri, docInfo] of documentsInfo) {
            if (uri !== params.textDocument.uri) {
                const symbol = docInfo.symbols.get(word);
                if (symbol && symbol.range) {
                    locations.push({
                        uri,
                        range: symbol.range
                    });
                }
            }
        }
    }
    return locations;
});
// Enhanced hover with rich information
connection.onHover((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return null;
    const word = getWordAtPosition(document, params.position);
    if (!word)
        return null;
    // Check built-in functions first
    const builtinInfo = getBuiltinInfo(word);
    if (builtinInfo) {
        return {
            contents: {
                kind: node_1.MarkupKind.Markdown,
                value: builtinInfo
            }
        };
    }
    // Check document symbols
    const info = documentsInfo.get(params.textDocument.uri);
    if (info) {
        const symbol = info.symbols.get(word);
        if (symbol) {
            const markdown = `**${symbol.kind}** \`${word}\`${symbol.type ? `: ${symbol.type}` : ''}`;
            return {
                contents: {
                    kind: node_1.MarkupKind.Markdown,
                    value: markdown + (symbol.detail ? `\n\n${symbol.detail}` : '')
                }
            };
        }
    }
    // Check workspace symbols
    for (const [uri, docInfo] of documentsInfo) {
        if (uri !== params.textDocument.uri) {
            const symbol = docInfo.symbols.get(word);
            if (symbol) {
                const filename = path.basename(uri);
                return {
                    contents: {
                        kind: node_1.MarkupKind.Markdown,
                        value: `**${symbol.kind}** \`${word}\` *(from ${filename})*${symbol.type ? `\n\nType: \`${symbol.type}\`` : ''}`
                    }
                };
            }
        }
    }
    return null;
});
function getBuiltinInfo(word) {
    const builtins = {
        'Console.WriteLine': '**function** `Console.WriteLine(text: string): void`\n\nWrites a line of text to the console output.',
        'Console.Write': '**function** `Console.Write(text: string): void`\n\nWrites text to the console output without a newline.',
        'Console.ReadLine': '**function** `Console.ReadLine(): string`\n\nReads a line of input from the console.',
        'int': '**type** `int`\n\nSigned 32-bit integer type.',
        'float': '**type** `float`\n\nSingle-precision floating-point type.',
        'bool': '**type** `bool`\n\nBoolean type (true or false).',
        'char': '**type** `char`\n\nSingle character type.',
        'string': '**type** `string`\n\nUTF-8 string type.',
        'void': '**type** `void`\n\nRepresents no return value.',
        'true': '**literal** `true`\n\nBoolean true value.',
        'false': '**literal** `false`\n\nBoolean false value.',
        'null': '**literal** `null`\n\nNull reference value.',
        'func': '**keyword** `func`\n\nFunction declaration keyword.',
        'class': '**keyword** `class`\n\nClass declaration keyword.',
        'struct': '**keyword** `struct`\n\nStruct declaration keyword.',
        'module': '**keyword** `module`\n\nModule declaration keyword.',
        'import': '**keyword** `import`\n\nImport statement for modules.',
        'export': '**keyword** `export`\n\nExport declaration for multi-file compilation.',
        'var': '**keyword** `var`\n\nVariable declaration with type inference.',
        'let': '**keyword** `let`\n\nImmutable variable declaration.',
        'async': '**keyword** `async`\n\nAsynchronous function modifier.',
        'await': '**keyword** `await`\n\nAwaits an asynchronous operation.',
        'Task': '**type** `Task`\n\nRepresents an asynchronous task.',
        'TaskResult': '**type** `TaskResult`\n\nRepresents a task with a result value.'
    };
    return builtins[word] || null;
}
function getWordAtPosition(doc, pos) {
    const line = doc.getText({
        start: { line: pos.line, character: 0 },
        end: { line: pos.line + 1, character: 0 }
    });
    // Handle Console.Method patterns
    const beforeCursor = line.slice(0, pos.character);
    const afterCursor = line.slice(pos.character);
    // Match identifiers including dots (for Console.WriteLine etc)
    const leftMatch = beforeCursor.match(/[\w.]+$/);
    const rightMatch = afterCursor.match(/^[\w.]*/);
    const word = `${leftMatch ? leftMatch[0] : ''}${rightMatch ? rightMatch[0] : ''}`;
    return word;
}
// Document symbols (outline view)
connection.onDocumentSymbol((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return [];
    const info = documentsInfo.get(params.textDocument.uri);
    if (!info)
        return [];
    const symbols = [];
    for (const [name, symbol] of info.symbols) {
        if (symbol.range) {
            const kind = symbol.kind === 'func' ? node_1.SymbolKind.Function :
                symbol.kind === 'class' ? node_1.SymbolKind.Class :
                    symbol.kind === 'struct' ? node_1.SymbolKind.Struct :
                        node_1.SymbolKind.Variable;
            symbols.push({
                name,
                kind,
                range: symbol.range,
                selectionRange: symbol.range,
                detail: symbol.detail || symbol.type
            });
        }
    }
    return symbols;
});
// Workspace symbols (global search)
connection.onWorkspaceSymbol((params) => {
    const query = params.query.toLowerCase();
    const symbols = [];
    for (const [uri, info] of documentsInfo) {
        for (const [name, symbol] of info.symbols) {
            if (name.toLowerCase().includes(query) && symbol.range) {
                const kind = symbol.kind === 'func' ? node_1.SymbolKind.Function :
                    symbol.kind === 'class' ? node_1.SymbolKind.Class :
                        symbol.kind === 'struct' ? node_1.SymbolKind.Struct :
                            node_1.SymbolKind.Variable;
                symbols.push({
                    name,
                    kind,
                    location: {
                        uri,
                        range: symbol.range
                    },
                    containerName: path.basename(uri)
                });
            }
        }
    }
    return symbols;
});
// Signature help for function calls
connection.onSignatureHelp((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return null;
    const line = document.getText({
        start: { line: params.position.line, character: 0 },
        end: { line: params.position.line + 1, character: 0 }
    });
    const beforeCursor = line.substring(0, params.position.character);
    // Check for Console methods
    if (beforeCursor.includes('Console.WriteLine(')) {
        return {
            signatures: [{
                    label: 'Console.WriteLine(text: string): void',
                    documentation: {
                        kind: node_1.MarkupKind.Markdown,
                        value: 'Writes a line of text to the console output'
                    },
                    parameters: [{
                            label: 'text: string',
                            documentation: 'The text to write'
                        }]
                }],
            activeSignature: 0,
            activeParameter: 0
        };
    }
    if (beforeCursor.includes('Console.Write(')) {
        return {
            signatures: [{
                    label: 'Console.Write(text: string): void',
                    documentation: {
                        kind: node_1.MarkupKind.Markdown,
                        value: 'Writes text to the console output'
                    },
                    parameters: [{
                            label: 'text: string',
                            documentation: 'The text to write'
                        }]
                }],
            activeSignature: 0,
            activeParameter: 0
        };
    }
    return null;
});
// Document formatting
connection.onDocumentFormatting((params) => {
    // Basic formatting - could be enhanced with actual formatter
    return [];
});
// Range formatting
connection.onDocumentRangeFormatting((params) => {
    return [];
});
// Rename support
connection.onRenameRequest((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return null;
    const word = getWordAtPosition(document, params.position);
    if (!word)
        return null;
    // For now, just rename in current document
    // Could be enhanced to support workspace-wide renaming
    const changes = {};
    changes[params.textDocument.uri] = [];
    const text = document.getText();
    const regex = new RegExp(`\\b${word}\\b`, 'g');
    let match;
    while ((match = regex.exec(text)) !== null) {
        const start = document.positionAt(match.index);
        const end = document.positionAt(match.index + word.length);
        changes[params.textDocument.uri].push({
            range: { start, end },
            newText: params.newName
        });
    }
    return { changes };
});
// Settings change handling
connection.onDidChangeConfiguration((change) => {
    globalSettings = change.settings;
});
// Completion resolve for additional details
connection.onCompletionResolve((item) => {
    // Add additional documentation or details if needed
    return item;
});
documents.listen(connection);
connection.listen();
//# sourceMappingURL=server.js.map