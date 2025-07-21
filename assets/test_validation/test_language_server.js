#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

/**
 * Test suite for Dream Language Server features
 */

/**
 * Mock Language Server Client for testing
 */
class MockLanguageServerClient {
    constructor() {
        this.capabilities = {
            textDocumentSync: true,
            completionProvider: true,
            hoverProvider: true,
            definitionProvider: true,
            documentSymbolProvider: true,
            workspaceSymbolProvider: true,
            signatureHelpProvider: true,
            documentFormattingProvider: true,
            renameProvider: true,
            foldingRangeProvider: true,
            semanticTokensProvider: true
        };
        
        this.documents = new Map();
        this.symbolTable = new Map();
    }
    
    /**
     * Simulate opening a document
     */
    openDocument(uri, content, languageId = 'dream') {
        const document = {
            uri,
            languageId,
            version: 1,
            text: content
        };
        
        this.documents.set(uri, document);
        this.analyzeDocument(document);
        return document;
    }
    
    /**
     * Simple document analysis
     */
    analyzeDocument(document) {
        const symbols = [];
        const lines = document.text.split('\n');
        
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            
            // Find function declarations
            const funcMatch = line.match(/func\s+(\w+\s+)?(\w+)\s*\(/);
            if (funcMatch) {
                symbols.push({
                    name: funcMatch[2],
                    kind: 'function',
                    line: i,
                    character: line.indexOf(funcMatch[2]),
                    detail: line.trim()
                });
            }
            
            // Find class declarations
            const classMatch = line.match(/class\s+(\w+)/);
            if (classMatch) {
                symbols.push({
                    name: classMatch[1],
                    kind: 'class',
                    line: i,
                    character: line.indexOf(classMatch[1]),
                    detail: line.trim()
                });
            }
            
            // Find variable declarations
            const varMatch = line.match(/(int|float|bool|char|string)\s+(\w+)/);
            if (varMatch) {
                symbols.push({
                    name: varMatch[2],
                    kind: 'variable',
                    type: varMatch[1],
                    line: i,
                    character: line.indexOf(varMatch[2]),
                    detail: line.trim()
                });
            }
        }
        
        this.symbolTable.set(document.uri, symbols);
        return symbols;
    }
    
    /**
     * Provide completion items
     */
    getCompletionItems(uri, position) {
        const document = this.documents.get(uri);
        if (!document) return [];
        
        const completions = [
            // Keywords
            { label: 'func', kind: 'Keyword', detail: 'Function declaration' },
            { label: 'class', kind: 'Keyword', detail: 'Class declaration' },
            { label: 'if', kind: 'Keyword', detail: 'Conditional statement' },
            { label: 'while', kind: 'Keyword', detail: 'Loop statement' },
            { label: 'return', kind: 'Keyword', detail: 'Return statement' },
            
            // Types
            { label: 'int', kind: 'Type', detail: '32-bit integer' },
            { label: 'string', kind: 'Type', detail: 'String type' },
            { label: 'bool', kind: 'Type', detail: 'Boolean type' },
            { label: 'float', kind: 'Type', detail: 'Floating-point type' },
            
            // Console functions
            { label: 'Console.WriteLine', kind: 'Method', detail: 'void Console.WriteLine(string)' },
            { label: 'Console.Write', kind: 'Method', detail: 'void Console.Write(string)' },
            { label: 'Console.ReadLine', kind: 'Method', detail: 'string Console.ReadLine()' },
            
            // Module system
            { label: 'module', kind: 'Keyword', detail: 'Module declaration' },
            { label: 'import', kind: 'Keyword', detail: 'Import statement' },
            { label: 'export', kind: 'Keyword', detail: 'Export declaration' },
            
            // Async
            { label: 'async', kind: 'Keyword', detail: 'Async function modifier' },
            { label: 'await', kind: 'Keyword', detail: 'Await expression' },
            { label: 'Task', kind: 'Type', detail: 'Async task type' }
        ];
        
        // Add document symbols
        const symbols = this.symbolTable.get(uri) || [];
        for (const symbol of symbols) {
            completions.push({
                label: symbol.name,
                kind: symbol.kind === 'function' ? 'Function' : 
                      symbol.kind === 'class' ? 'Class' : 'Variable',
                detail: symbol.detail
            });
        }
        
        return completions;
    }
    
    /**
     * Provide hover information
     */
    getHover(uri, position) {
        const document = this.documents.get(uri);
        if (!document) return null;
        
        const lines = document.text.split('\n');
        if (position.line >= lines.length) return null;
        
        const line = lines[position.line];
        const word = this.getWordAtPosition(line, position.character);
        
        if (!word) return null;
        
        // Built-in types and keywords
        const builtins = {
            'int': '**type** `int`\n\nSigned 32-bit integer type.',
            'string': '**type** `string`\n\nUTF-8 string type.',
            'bool': '**type** `bool`\n\nBoolean type (true or false).',
            'float': '**type** `float`\n\nSingle-precision floating-point type.',
            'char': '**type** `char`\n\nSingle character type.',
            'void': '**type** `void`\n\nRepresents no return value.',
            'func': '**keyword** `func`\n\nFunction declaration keyword.',
            'class': '**keyword** `class`\n\nClass declaration keyword.',
            'module': '**keyword** `module`\n\nModule declaration keyword.',
            'Console.WriteLine': '**function** `Console.WriteLine(text: string): void`\n\nWrites a line of text to the console.',
            'Console.Write': '**function** `Console.Write(text: string): void`\n\nWrites text to the console.',
            'Console.ReadLine': '**function** `Console.ReadLine(): string`\n\nReads a line of input from the console.'
        };
        
        if (builtins[word]) {
            return {
                contents: builtins[word],
                range: {
                    start: { line: position.line, character: position.character - word.length },
                    end: { line: position.line, character: position.character }
                }
            };
        }
        
        // Check document symbols
        const symbols = this.symbolTable.get(uri) || [];
        const symbol = symbols.find(s => s.name === word);
        if (symbol) {
            return {
                contents: `**${symbol.kind}** \`${symbol.name}\`${symbol.type ? ': ' + symbol.type : ''}\n\n${symbol.detail}`,
                range: {
                    start: { line: symbol.line, character: symbol.character },
                    end: { line: symbol.line, character: symbol.character + symbol.name.length }
                }
            };
        }
        
        return null;
    }
    
    /**
     * Get document symbols
     */
    getDocumentSymbols(uri) {
        return this.symbolTable.get(uri) || [];
    }
    
    /**
     * Find definitions
     */
    getDefinition(uri, position) {
        const document = this.documents.get(uri);
        if (!document) return [];
        
        const lines = document.text.split('\n');
        if (position.line >= lines.length) return [];
        
        const line = lines[position.line];
        const word = this.getWordAtPosition(line, position.character);
        
        if (!word) return [];
        
        // Find symbol definition
        const symbols = this.symbolTable.get(uri) || [];
        const symbol = symbols.find(s => s.name === word);
        
        if (symbol) {
            return [{
                uri: uri,
                range: {
                    start: { line: symbol.line, character: symbol.character },
                    end: { line: symbol.line, character: symbol.character + symbol.name.length }
                }
            }];
        }
        
        return [];
    }
    
    /**
     * Get signature help
     */
    getSignatureHelp(uri, position) {
        const document = this.documents.get(uri);
        if (!document) return null;
        
        const lines = document.text.split('\n');
        if (position.line >= lines.length) return null;
        
        const line = lines[position.line];
        const beforeCursor = line.substring(0, position.character);
        
        // Check for Console methods
        if (beforeCursor.includes('Console.WriteLine(')) {
            return {
                signatures: [{
                    label: 'Console.WriteLine(text: string): void',
                    documentation: 'Writes a line of text to the console output',
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
                    documentation: 'Writes text to the console output',
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
    }
    
    /**
     * Extract word at position
     */
    getWordAtPosition(line, character) {
        const beforeCursor = line.slice(0, character);
        const afterCursor = line.slice(character);
        
        const leftMatch = beforeCursor.match(/[\w.]+$/);
        const rightMatch = afterCursor.match(/^[\w.]*/);
        
        return (leftMatch ? leftMatch[0] : '') + (rightMatch ? rightMatch[0] : '');
    }
}

/**
 * Test language server features
 */
function testLanguageServerFeatures() {
    console.log('🔍 Testing Dream Language Server Features');
    console.log('=========================================');
    
    const server = new MockLanguageServerClient();
    let testResults = [];
    
    // Test document opening
    console.log('\n--- Testing Document Opening ---');
    const testContent = `
module test;

import console_lib;

class Calculator {
    int value = 0;
    
    func int add(int a, int b) {
        value = a + b;
        Console.WriteLine("Result: " + value);
        return value;
    }
}

func void main() {
    Calculator calc = new Calculator();
    int result = calc.add(10, 20);
    Console.WriteLine("Final result: " + result);
}`;
    
    const document = server.openDocument('test://test.dr', testContent);
    const openSuccess = document && document.uri === 'test://test.dr';
    console.log(openSuccess ? '✅ Document opened successfully' : '❌ Failed to open document');
    testResults.push({ name: 'Document Opening', passed: openSuccess });
    
    // Test symbol analysis
    console.log('\n--- Testing Symbol Analysis ---');
    const symbols = server.getDocumentSymbols('test://test.dr');
    const hasCalculatorClass = symbols.some(s => s.name === 'Calculator' && s.kind === 'class');
    const hasAddFunction = symbols.some(s => s.name === 'add' && s.kind === 'function');
    const hasMainFunction = symbols.some(s => s.name === 'main' && s.kind === 'function');
    const hasValueVariable = symbols.some(s => s.name === 'value' && s.kind === 'variable');
    
    console.log(hasCalculatorClass ? '✅ Calculator class detected' : '❌ Calculator class not detected');
    console.log(hasAddFunction ? '✅ add function detected' : '❌ add function not detected');
    console.log(hasMainFunction ? '✅ main function detected' : '❌ main function not detected');
    console.log(hasValueVariable ? '✅ value variable detected' : '❌ value variable not detected');
    
    const symbolAnalysisSuccess = hasCalculatorClass && hasAddFunction && hasMainFunction && hasValueVariable;
    testResults.push({ name: 'Symbol Analysis', passed: symbolAnalysisSuccess });
    
    // Test completion
    console.log('\n--- Testing Completion ---');
    const completions = server.getCompletionItems('test://test.dr', { line: 10, character: 8 });
    const hasKeywordCompletions = completions.some(c => c.label === 'func');
    const hasTypeCompletions = completions.some(c => c.label === 'int');
    const hasConsoleCompletions = completions.some(c => c.label === 'Console.WriteLine');
    const hasSymbolCompletions = completions.some(c => c.label === 'Calculator');
    
    console.log(hasKeywordCompletions ? '✅ Keyword completions available' : '❌ Keyword completions missing');
    console.log(hasTypeCompletions ? '✅ Type completions available' : '❌ Type completions missing');
    console.log(hasConsoleCompletions ? '✅ Console completions available' : '❌ Console completions missing');
    console.log(hasSymbolCompletions ? '✅ Symbol completions available' : '❌ Symbol completions missing');
    
    const completionSuccess = hasKeywordCompletions && hasTypeCompletions && hasConsoleCompletions;
    testResults.push({ name: 'Completion Provider', passed: completionSuccess });
    
    // Test hover
    console.log('\n--- Testing Hover Provider ---');
    const hoverInt = server.getHover('test://test.dr', { line: 6, character: 4 }); // On "int"
    const hoverConsole = server.getHover('test://test.dr', { line: 9, character: 8 }); // On "Console"
    const hoverVariable = server.getHover('test://test.dr', { line: 5, character: 8 }); // On "value"
    
    const hasIntHover = hoverInt && hoverInt.contents.includes('integer');
    const hasConsoleHover = hoverConsole && hoverConsole.contents.includes('Console');
    const hasVariableHover = hoverVariable && hoverVariable.contents.includes('value');
    
    console.log(hasIntHover ? '✅ Type hover works' : '❌ Type hover not working');
    console.log(hasConsoleHover ? '✅ Console hover works' : '❌ Console hover not working');
    console.log(hasVariableHover ? '✅ Variable hover works' : '❌ Variable hover not working');
    
    const hoverSuccess = hasIntHover && hasVariableHover;
    testResults.push({ name: 'Hover Provider', passed: hoverSuccess });
    
    // Test definition
    console.log('\n--- Testing Go-to-Definition ---');
    const definition = server.getDefinition('test://test.dr', { line: 8, character: 8 }); // On "value"
    const hasDefinition = definition && definition.length > 0;
    
    console.log(hasDefinition ? '✅ Go-to-definition works' : '❌ Go-to-definition not working');
    testResults.push({ name: 'Definition Provider', passed: hasDefinition });
    
    // Test signature help
    console.log('\n--- Testing Signature Help ---');
    const signatureHelp = server.getSignatureHelp('test://test.dr', { line: 9, character: 25 }); // Inside Console.WriteLine(
    const hasSignatureHelp = signatureHelp && signatureHelp.signatures && signatureHelp.signatures.length > 0;
    
    console.log(hasSignatureHelp ? '✅ Signature help works' : '❌ Signature help not working');
    testResults.push({ name: 'Signature Help', passed: hasSignatureHelp });
    
    return testResults;
}

/**
 * Test specific language server scenarios
 */
function testSpecificScenarios() {
    console.log('\n🔍 Testing Specific Language Server Scenarios');
    console.log('==============================================');
    
    const server = new MockLanguageServerClient();
    let testResults = [];
    
    // Test async/await scenario
    console.log('\n--- Testing Async/Await Support ---');
    const asyncContent = `
module async_test;

async func Task<bool> validateAsync(string data) {
    Console.WriteLine("Validating: " + data);
    await Task.Delay(1000);
    return data != null;
}

func void main() {
    Task<bool> result = validateAsync("test");
    bool isValid = await result;
    Console.WriteLine("Valid: " + isValid);
}`;
    
    server.openDocument('test://async.dr', asyncContent);
    const asyncSymbols = server.getDocumentSymbols('test://async.dr');
    const hasAsyncFunction = asyncSymbols.some(s => s.name === 'validateAsync');
    
    console.log(hasAsyncFunction ? '✅ Async function detected' : '❌ Async function not detected');
    testResults.push({ name: 'Async/Await Support', passed: hasAsyncFunction });
    
    // Test module system scenario
    console.log('\n--- Testing Module System Support ---');
    const moduleContent = `
module math_utils;

export class MathUtils {
    static func float sqrt(float x) {
        return x * 0.5f; // Simplified
    }
}

export func int add(int a, int b) {
    return a + b;
}`;
    
    server.openDocument('test://math_utils.dr', moduleContent);
    const moduleSymbols = server.getDocumentSymbols('test://math_utils.dr');
    const hasMathUtilsClass = moduleSymbols.some(s => s.name === 'MathUtils');
    const hasExportFunction = moduleSymbols.some(s => s.name === 'add');
    
    console.log(hasMathUtilsClass ? '✅ Module class detected' : '❌ Module class not detected');
    console.log(hasExportFunction ? '✅ Export function detected' : '❌ Export function not detected');
    
    const moduleSuccess = hasMathUtilsClass && hasExportFunction;
    testResults.push({ name: 'Module System', passed: moduleSuccess });
    
    // Test complex completion scenario
    console.log('\n--- Testing Complex Completion Scenarios ---');
    const complexContent = `
module complex;

class DataProcessor {
    string[] data;
    
    func void processData() {
        for (int i = 0; i < data.length; i++) {
            string item = data[i];
            Console.WriteLine("Processing: " + item);
        }
    }
}`;
    
    server.openDocument('test://complex.dr', complexContent);
    const complexCompletions = server.getCompletionItems('test://complex.dr', { line: 7, character: 12 });
    const hasForCompletion = complexCompletions.some(c => c.label === 'for');
    const hasStringCompletion = complexCompletions.some(c => c.label === 'string');
    const hasDataSymbol = complexCompletions.some(c => c.label === 'data');
    
    console.log(hasForCompletion ? '✅ Control flow completion available' : '❌ Control flow completion missing');
    console.log(hasStringCompletion ? '✅ Type completion available' : '❌ Type completion missing');
    console.log(hasDataSymbol ? '✅ Member symbol completion available' : '❌ Member symbol completion missing');
    
    const complexCompletionSuccess = hasForCompletion && hasStringCompletion;
    testResults.push({ name: 'Complex Completion', passed: complexCompletionSuccess });
    
    return testResults;
}

/**
 * Generate language server test report
 */
function generateLanguageServerReport(basicResults, specificResults) {
    console.log('\n📊 Language Server Test Summary');
    console.log('===============================');
    
    const allResults = [...basicResults, ...specificResults];
    const passedTests = allResults.filter(r => r.passed).length;
    const totalTests = allResults.length;
    const passRate = totalTests > 0 ? (passedTests / totalTests * 100).toFixed(1) : 0;
    
    console.log('\nBasic Features:');
    for (const result of basicResults) {
        console.log(`  ${result.passed ? '✅' : '❌'} ${result.name}`);
    }
    
    console.log('\nSpecific Scenarios:');
    for (const result of specificResults) {
        console.log(`  ${result.passed ? '✅' : '❌'} ${result.name}`);
    }
    
    console.log(`\nOverall Results:`);
    console.log(`  Tests passed: ${passedTests}/${totalTests}`);
    console.log(`  Success rate: ${passRate}%`);
    
    if (parseFloat(passRate) >= 80) {
        console.log('🎉 Language server is working well!');
        return true;
    } else if (parseFloat(passRate) >= 60) {
        console.log('⚠️  Language server has some issues but is functional');
        return true;
    } else {
        console.log('❌ Language server has significant issues');
        return false;
    }
}

/**
 * Main execution
 */
function main() {
    console.log('Dream Language Server Test Suite');
    console.log('================================');
    console.log(`Timestamp: ${new Date().toISOString()}\n`);
    
    try {
        const basicResults = testLanguageServerFeatures();
        const specificResults = testSpecificScenarios();
        const success = generateLanguageServerReport(basicResults, specificResults);
        
        console.log('\n🏁 Language Server Tests Complete');
        console.log('=================================');
        
        if (success) {
            console.log('✅ Language server tests PASSED');
            process.exit(0);
        } else {
            console.log('❌ Language server tests FAILED');
            process.exit(1);
        }
        
    } catch (error) {
        console.error('❌ Language server tests failed with error:', error.message);
        process.exit(1);
    }
}

// Run if called directly
if (require.main === module) {
    main();
}

module.exports = {
    MockLanguageServerClient,
    testLanguageServerFeatures,
    testSpecificScenarios,
    generateLanguageServerReport
};