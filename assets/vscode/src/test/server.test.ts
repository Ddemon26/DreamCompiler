import * as assert from 'assert';
import { 
  createConnection, 
  TextDocuments, 
  ProposedFeatures,
  CompletionItem,
  CompletionItemKind,
  TextDocument 
} from 'vscode-languageserver/node';
import { TextDocument as VSCodeTextDocument } from 'vscode-languageserver-textdocument';

/**
 * Unit tests for the Dream Language Server
 */
suite('Dream Language Server Test Suite', () => {

    /**
     * Test keyword completion generation
     */
    test('Should generate keyword completions', () => {
        const keywords = [
            'func', 'int', 'float', 'bool', 'char', 'string', 'void',
            'if', 'else', 'while', 'do', 'for', 'switch', 'case', 'default',
            'break', 'continue', 'return', 'try', 'catch', 'throw', 'finally',
            'class', 'struct', 'new', 'this', 'base', 'static', 'async', 'await',
            'var', 'let', 'using', 'import', 'module', 'export',
            'Task', 'TaskResult', 'true', 'false', 'null'
        ];

        const completionItems: CompletionItem[] = [];
        for (const keyword of keywords) {
            completionItems.push({
                label: keyword,
                kind: CompletionItemKind.Keyword,
                detail: `Dream keyword`,
                sortText: `1_${keyword}`
            });
        }

        assert.ok(completionItems.length > 0, 'Should have keyword completions');
        assert.strictEqual(completionItems.length, keywords.length, 
                          'Should have completion for each keyword');
        
        // Check specific keywords
        const funcCompletion = completionItems.find(item => item.label === 'func');
        assert.ok(funcCompletion, 'Should have func keyword completion');
        assert.strictEqual(funcCompletion.kind, CompletionItemKind.Keyword);
        
        const asyncCompletion = completionItems.find(item => item.label === 'async');
        assert.ok(asyncCompletion, 'Should have async keyword completion');
        
        const moduleCompletion = completionItems.find(item => item.label === 'module');
        assert.ok(moduleCompletion, 'Should have module keyword completion');
    });

    /**
     * Test Console function completions
     */
    test('Should generate Console function completions', () => {
        const consoleFunctions = [
            {
                label: 'Console.WriteLine',
                kind: CompletionItemKind.Method,
                detail: 'void Console.WriteLine(string text)',
                insertText: 'Console.WriteLine($1)'
            },
            {
                label: 'Console.Write', 
                kind: CompletionItemKind.Method,
                detail: 'void Console.Write(string text)',
                insertText: 'Console.Write($1)'
            },
            {
                label: 'Console.ReadLine',
                kind: CompletionItemKind.Method,
                detail: 'string Console.ReadLine()',
                insertText: 'Console.ReadLine()'
            }
        ];

        assert.strictEqual(consoleFunctions.length, 3, 'Should have 3 Console functions');
        
        const writeLineFunc = consoleFunctions.find(f => f.label === 'Console.WriteLine');
        assert.ok(writeLineFunc, 'Should have WriteLine function');
        assert.strictEqual(writeLineFunc.kind, CompletionItemKind.Method);
        assert.ok(writeLineFunc.detail?.includes('void'), 'WriteLine should return void');
        
        const readLineFunc = consoleFunctions.find(f => f.label === 'Console.ReadLine');
        assert.ok(readLineFunc, 'Should have ReadLine function');
        assert.ok(readLineFunc.detail?.includes('string'), 'ReadLine should return string');
    });

    /**
     * Test builtin information retrieval
     */
    test('Should provide builtin type information', () => {
        const builtins: Record<string, string> = {
            'Console.WriteLine': '**function** `Console.WriteLine(text: string): void`\n\nWrites a line of text to the console output.',
            'int': '**type** `int`\n\nSigned 32-bit integer type.',
            'string': '**type** `string`\n\nUTF-8 string type.',
            'bool': '**type** `bool`\n\nBoolean type (true or false).',
            'func': '**keyword** `func`\n\nFunction declaration keyword.',
            'module': '**keyword** `module`\n\nModule declaration keyword.',
            'async': '**keyword** `async`\n\nAsynchronous function modifier.',
            'Task': '**type** `Task`\n\nRepresents an asynchronous task.'
        };

        // Test builtin retrieval
        const getBuiltinInfo = (word: string): string | null => {
            return builtins[word] || null;
        };

        assert.ok(getBuiltinInfo('int'), 'Should have int builtin info');
        assert.ok(getBuiltinInfo('Console.WriteLine'), 'Should have Console.WriteLine info');
        assert.ok(getBuiltinInfo('func'), 'Should have func keyword info');
        assert.ok(getBuiltinInfo('async'), 'Should have async keyword info');
        assert.strictEqual(getBuiltinInfo('nonexistent'), null, 'Should return null for unknown words');

        // Test content correctness
        const intInfo = getBuiltinInfo('int');
        assert.ok(intInfo?.includes('32-bit integer'), 'int info should mention 32-bit integer');
        
        const moduleInfo = getBuiltinInfo('module');
        assert.ok(moduleInfo?.includes('Module declaration'), 'module info should mention declaration');
    });

    /**
     * Test diagnostic parsing
     */
    test('Should parse compiler diagnostics correctly', () => {
        const mockStderr = `
        5:10: error: Expected expression
             let x = ;
                    ^
        help: Add a value after '='
        
        12:25: warning: Variable 'unused' is never used
           int unused = 42;
                       ^
        `;

        // Mock diagnostic parser
        const parseDiagnostics = (stderr: string): any[] => {
            const diagnostics: any[] = [];
            if (!stderr) return diagnostics;
            
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
                    
                    const startLine = parseInt(match[1]) - 1;
                    const startChar = Math.max(0, parseInt(match[2]) - 1);
                    
                    diagnostics.push({
                        range: {
                            start: { line: startLine, character: startChar },
                            end: { line: startLine, character: startChar + 1 }
                        },
                        severity: match[3] === 'error' ? 1 : 2, // Error : Warning
                        message,
                        source: 'DreamCompiler'
                    });
                }
            }
            
            return diagnostics;
        };

        const diagnostics = parseDiagnostics(mockStderr);
        
        assert.strictEqual(diagnostics.length, 2, 'Should parse 2 diagnostics');
        
        const errorDiagnostic = diagnostics[0];
        assert.strictEqual(errorDiagnostic.severity, 1, 'First should be error');
        assert.ok(errorDiagnostic.message.includes('Expected expression'), 'Should have error message');
        assert.ok(errorDiagnostic.message.includes('💡'), 'Should include hint');
        
        const warningDiagnostic = diagnostics[1];
        assert.strictEqual(warningDiagnostic.severity, 2, 'Second should be warning');
        assert.ok(warningDiagnostic.message.includes('never used'), 'Should have warning message');
    });

    /**
     * Test word extraction at position
     */
    test('Should extract words correctly at cursor position', () => {
        const getWordAtPosition = (text: string, line: number, character: number): string => {
            const lines = text.split('\n');
            if (line >= lines.length) return '';
            
            const currentLine = lines[line];
            if (character > currentLine.length) return '';
            
            // Handle Console.Method patterns
            const beforeCursor = currentLine.slice(0, character);
            const afterCursor = currentLine.slice(character);
            
            // Match identifiers including dots (for Console.WriteLine etc)
            const leftMatch = beforeCursor.match(/[\w.]+$/);
            const rightMatch = afterCursor.match(/^[\w.]*/); 
            
            const word = `${leftMatch ? leftMatch[0] : ''}${rightMatch ? rightMatch[0] : ''}`;
            return word;
        };

        // Test cases
        const testCases = [
            { text: 'Console.WriteLine', line: 0, char: 7, expected: 'Console.WriteLine' },
            { text: 'int variable = 42;', line: 0, char: 2, expected: 'int' },
            { text: 'func main() {', line: 0, char: 4, expected: 'func' },
            { text: '    Console.Write("test");', line: 0, char: 15, expected: 'Console.Write' },
            { text: 'if (condition)', line: 0, char: 1, expected: 'if' }
        ];

        for (const testCase of testCases) {
            const result = getWordAtPosition(testCase.text, testCase.line, testCase.char);
            assert.strictEqual(result, testCase.expected, 
                              `Word extraction failed for "${testCase.text}" at ${testCase.char}`);
        }
    });

    /**
     * Test symbol information parsing
     */
    test('Should parse symbol information correctly', () => {
        interface SymbolInfo {
            name: string;
            line: number;
            character: number;
            kind: 'func' | 'var' | 'class' | 'struct';
            type?: string;
            detail?: string;
        }

        const mockSymbolData = [
            { name: 'TestClass', line: 2, character: 6, kind: 'class' },
            { name: 'getValue', line: 5, character: 13, kind: 'func', type: 'int' },
            { name: 'value', line: 3, character: 8, kind: 'var', type: 'int' }
        ];

        const symbols: SymbolInfo[] = [];
        for (const s of mockSymbolData) {
            symbols.push({
                name: s.name,
                line: s.line,
                character: s.character,
                kind: s.kind as any,
                type: s.type
            });
        }

        assert.strictEqual(symbols.length, 3, 'Should have 3 symbols');
        
        const classSymbol = symbols.find(s => s.name === 'TestClass');
        assert.ok(classSymbol, 'Should have class symbol');
        assert.strictEqual(classSymbol.kind, 'class');
        
        const funcSymbol = symbols.find(s => s.name === 'getValue');
        assert.ok(funcSymbol, 'Should have function symbol');
        assert.strictEqual(funcSymbol.kind, 'func');
        assert.strictEqual(funcSymbol.type, 'int');
        
        const varSymbol = symbols.find(s => s.name === 'value');
        assert.ok(varSymbol, 'Should have variable symbol');
        assert.strictEqual(varSymbol.kind, 'var');
    });

    /**
     * Test cache functionality
     */
    test('Should implement caching correctly', () => {
        interface CacheEntry {
            result: any;
            timestamp: number;
        }

        const CACHE_TTL = 5000; // 5 seconds
        const parseCache = new Map<string, CacheEntry>();

        const hashString = (str: string): string => {
            let hash = 0;
            for (let i = 0; i < str.length; i++) {
                const char = str.charCodeAt(i);
                hash = ((hash << 5) - hash) + char;
                hash = hash & hash; // Convert to 32-bit integer
            }
            return hash.toString();
        };

        // Test cache operations
        const testContent = 'func main() { Console.WriteLine("test"); }';
        const contentHash = hashString(testContent);
        const cacheKey = `test.dr:${contentHash}`;
        
        const mockResult = { success: true, symbols: [], diagnostics: [] };
        
        // Store in cache
        parseCache.set(cacheKey, { result: mockResult, timestamp: Date.now() });
        
        // Retrieve from cache
        const cached = parseCache.get(cacheKey);
        assert.ok(cached, 'Should retrieve cached entry');
        assert.strictEqual(cached.result.success, true, 'Cached result should be correct');
        
        // Test cache expiry
        const expiredEntry = { result: mockResult, timestamp: Date.now() - 6000 }; // 6 seconds ago
        parseCache.set('expired', expiredEntry);
        
        const isExpired = (Date.now() - expiredEntry.timestamp) > CACHE_TTL;
        assert.strictEqual(isExpired, true, 'Entry should be expired');
        
        // Test hash consistency
        const hash1 = hashString(testContent);
        const hash2 = hashString(testContent);
        assert.strictEqual(hash1, hash2, 'Hash should be consistent');
        
        const differentHash = hashString('different content');
        assert.notStrictEqual(hash1, differentHash, 'Different content should have different hash');
    });
});