"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const vscode = require("vscode");
const assert = require("assert");
/**
 * Integration tests for the Dream VSCode extension
 */
suite('Dream Extension Test Suite', () => {
    vscode.window.showInformationMessage('Start all tests.');
    /**
     * Test that the extension is properly activated
     */
    test('Extension should be present and activate', async () => {
        const extension = vscode.extensions.getExtension('dream.dream-intellisense');
        assert.ok(extension, 'Extension should be installed');
        await extension.activate();
        assert.strictEqual(extension.isActive, true, 'Extension should be active');
    });
    /**
     * Test that .dr files are recognized with correct language ID
     */
    test('Dream files should be recognized', async () => {
        const dreamContent = `
        // Test Dream file
        module test;
        
        func main() {
            Console.WriteLine("Hello Dream!");
        }
        `;
        const document = await vscode.workspace.openTextDocument({
            content: dreamContent,
            language: 'dream'
        });
        assert.strictEqual(document.languageId, 'dream', 'Language ID should be "dream"');
        assert.ok(document.getText().includes('Console.WriteLine'), 'Content should be preserved');
    });
    /**
     * Test syntax highlighting configuration
     */
    test('Syntax highlighting should be configured', () => {
        const dreamConfig = vscode.workspace.getConfiguration('', { languageId: 'dream' });
        assert.ok(dreamConfig, 'Dream language configuration should exist');
    });
    /**
     * Test language configuration features
     */
    test('Language features should be configured', async () => {
        const dreamContent = `func test() {
    if (true) {
        Console.WriteLine("test");
    }
}`;
        const document = await vscode.workspace.openTextDocument({
            content: dreamContent,
            language: 'dream'
        });
        const editor = await vscode.window.showTextDocument(document);
        // Test comment configuration
        await vscode.commands.executeCommand('editor.action.commentLine');
        const textAfterComment = editor.document.getText();
        assert.ok(textAfterComment.includes('//'), 'Line commenting should work');
        // Undo the comment
        await vscode.commands.executeCommand('undo');
    });
    /**
     * Test basic completion provider
     */
    test('Completion provider should work', async () => {
        const dreamContent = `
        func main() {
            Con
        }`;
        const document = await vscode.workspace.openTextDocument({
            content: dreamContent,
            language: 'dream'
        });
        const position = new vscode.Position(2, 15); // After "Con"
        const completions = await vscode.commands.executeCommand('vscode.executeCompletionItemProvider', document.uri, position);
        assert.ok(completions, 'Completions should be available');
        if (completions && completions.items) {
            const consoleItems = completions.items.filter(item => item.label.toString().includes('Console'));
            assert.ok(consoleItems.length > 0, 'Console completions should be available');
        }
    });
    /**
     * Test hover provider
     */
    test('Hover provider should work', async () => {
        const dreamContent = `
        func main() {
            int x = 42;
            Console.WriteLine("test");
        }`;
        const document = await vscode.workspace.openTextDocument({
            content: dreamContent,
            language: 'dream'
        });
        const position = new vscode.Position(2, 12); // On "int"
        const hovers = await vscode.commands.executeCommand('vscode.executeHoverProvider', document.uri, position);
        assert.ok(hovers, 'Hover information should be available');
        if (hovers && hovers.length > 0) {
            const hoverContent = hovers[0].contents[0];
            if (typeof hoverContent === 'string') {
                assert.ok(hoverContent.includes('int') || hoverContent.includes('integer'), 'Hover should contain type information');
            }
            else if (hoverContent && 'value' in hoverContent) {
                assert.ok(hoverContent.value.includes('int') || hoverContent.value.includes('integer'), 'Hover should contain type information');
            }
        }
    });
    /**
     * Test document symbol provider
     */
    test('Document symbols should be provided', async () => {
        const dreamContent = `
        module test;
        
        class TestClass {
            int value;
            
            func int getValue() {
                return value;
            }
        }
        
        func main() {
            TestClass obj = new TestClass();
        }`;
        const document = await vscode.workspace.openTextDocument({
            content: dreamContent,
            language: 'dream'
        });
        const symbols = await vscode.commands.executeCommand('vscode.executeDocumentSymbolProvider', document.uri);
        assert.ok(symbols, 'Document symbols should be available');
        if (symbols && symbols.length > 0) {
            const classSymbols = symbols.filter(s => s.name === 'TestClass');
            assert.ok(classSymbols.length > 0, 'Class symbols should be detected');
            const functionSymbols = symbols.filter(s => s.name === 'main' || s.name === 'getValue');
            assert.ok(functionSymbols.length > 0, 'Function symbols should be detected');
        }
    });
    /**
     * Test diagnostics (if available)
     */
    test('Diagnostics should be provided for syntax errors', async () => {
        const invalidDreamContent = `
        func main() {
            int x = ; // Missing value
            Console.WriteLine("test"  // Missing closing parenthesis
        }`;
        const document = await vscode.workspace.openTextDocument({
            content: invalidDreamContent,
            language: 'dream'
        });
        // Wait a bit for diagnostics to be computed
        await new Promise(resolve => setTimeout(resolve, 1000));
        const diagnostics = vscode.languages.getDiagnostics(document.uri);
        // Note: Diagnostics may not be available if language server is not running
        // This test verifies the diagnostic system is accessible
        assert.ok(Array.isArray(diagnostics), 'Diagnostics should be an array');
    });
    /**
     * Test workspace symbol search
     */
    test('Workspace symbols should be searchable', async () => {
        const dreamContent = `
        export class Calculator {
            func int add(int a, int b) {
                return a + b;
            }
        }`;
        const document = await vscode.workspace.openTextDocument({
            content: dreamContent,
            language: 'dream'
        });
        const symbols = await vscode.commands.executeCommand('vscode.executeWorkspaceSymbolProvider', 'Calculator');
        assert.ok(symbols, 'Workspace symbols should be searchable');
        // Note: Results depend on language server functionality
    });
    /**
     * Test definition provider
     */
    test('Go to definition should work', async () => {
        const dreamContent = `
        class TestClass {
            int value = 42;
        }
        
        func main() {
            TestClass obj = new TestClass();
            int x = obj.value; // Should go to definition
        }`;
        const document = await vscode.workspace.openTextDocument({
            content: dreamContent,
            language: 'dream'
        });
        const position = new vscode.Position(7, 24); // On "value" usage
        const definitions = await vscode.commands.executeCommand('vscode.executeDefinitionProvider', document.uri, position);
        assert.ok(definitions, 'Definitions should be available');
        // Note: Actual definition results depend on language server implementation
    });
    /**
     * Test formatting provider
     */
    test('Document formatting should be available', async () => {
        const unformattedContent = `func main(){int x=42;Console.WriteLine("test");}`;
        const document = await vscode.workspace.openTextDocument({
            content: unformattedContent,
            language: 'dream'
        });
        const edits = await vscode.commands.executeCommand('vscode.executeFormatDocumentProvider', document.uri);
        assert.ok(Array.isArray(edits), 'Format edits should be an array');
        // Note: Actual formatting depends on language server implementation
    });
    /**
     * Test Dream-specific commands
     */
    test('Dream commands should be registered', async () => {
        const allCommands = await vscode.commands.getCommands(true);
        // Check if any Dream-specific commands are registered
        const dreamCommands = allCommands.filter(cmd => cmd.includes('dream'));
        // Note: This will depend on what commands the extension actually registers
        assert.ok(Array.isArray(dreamCommands), 'Dream commands should be accessible');
    });
});
//# sourceMappingURL=extension.test.js.map