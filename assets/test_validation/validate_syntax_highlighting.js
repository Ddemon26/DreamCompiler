#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

/**
 * Comprehensive syntax highlighting validation for Dream language plugins
 */

// Load the VSCode grammar
const grammarPath = path.join(__dirname, '..', 'vscode', 'syntaxes', 'dream.tmLanguage.json');
let dreamGrammar;

try {
    dreamGrammar = JSON.parse(fs.readFileSync(grammarPath, 'utf8'));
} catch (error) {
    console.error('Failed to load Dream grammar:', error.message);
    process.exit(1);
}

// Test cases for different token types
const testCases = {
    keywords: {
        control: ['if', 'else', 'while', 'for', 'do', 'break', 'continue', 'return', 'switch', 'case', 'default', 'try', 'catch', 'finally', 'throw'],
        types: ['int', 'string', 'bool', 'float', 'char', 'void'],
        other: ['class', 'struct', 'static', 'new', 'func', 'var', 'let', 'base', 'using', 'import', 'module', 'export', 'async', 'await', 'Task', 'TaskResult'],
        literals: ['true', 'false']
    },
    operators: {
        arithmetic: ['+', '-', '*', '/', '%', '++', '--', '+=', '-=', '*=', '/=', '%='],
        bitwise: ['&', '|', '^', '<<', '>>', '~', '&=', '|=', '^=', '<<=', '>>='],
        comparison: ['<', '<=', '>', '>=', '==', '!='],
        logical: ['&&', '||', '!'],
        assignment: ['='],
        conditional: ['?', '??', '??='],
        arrow: ['->', '=>']
    },
    literals: {
        numbers: ['42', '3.14', '0', '1234567', '0.5f'],
        strings: ['"Hello World"', '"Escaped \\"quotes\\""', '"Multi\\nline"'],
        chars: ["'a'", "'\\n'", "'\\\\'",'"\'"']
    },
    comments: [
        '// Line comment',
        '/* Block comment */',
        '/** Documentation comment */',
        '/// Doc comment'
    ],
    console: ['Console.WriteLine', 'Console.Write', 'Console.ReadLine'],
    punctuation: [';', ',', '.', '(', ')', '{', '}', '[', ']']
};

/**
 * Test if a pattern matches expected tokens
 */
function testPattern(patternName, pattern, testTokens, shouldMatch = true) {
    console.log(`\n--- Testing ${patternName} ---`);
    let passCount = 0;
    let totalCount = 0;
    
    for (const token of testTokens) {
        totalCount++;
        const regex = new RegExp(pattern);
        const matches = regex.test(token);
        
        if (matches === shouldMatch) {
            console.log(`✓ ${token} - ${shouldMatch ? 'matched' : 'not matched'} as expected`);
            passCount++;
        } else {
            console.log(`✗ ${token} - ${shouldMatch ? 'should match' : 'should not match'} but ${matches ? 'matched' : 'did not match'}`);
        }
    }
    
    const passRate = (passCount / totalCount * 100).toFixed(1);
    console.log(`${patternName}: ${passCount}/${totalCount} passed (${passRate}%)`);
    
    return { passed: passCount, total: totalCount, rate: parseFloat(passRate) };
}

/**
 * Validate all grammar patterns
 */
function validateGrammar() {
    console.log('🔍 Validating Dream Language Grammar');
    console.log('=====================================');
    
    const results = [];
    const patterns = dreamGrammar.repository.tokens.patterns;
    
    // Find patterns by name
    const findPattern = (name) => patterns.find(p => p.name === name);
    
    // Test control keywords
    const controlKeywordPattern = findPattern('keyword.control');
    if (controlKeywordPattern) {
        results.push(testPattern('Control Keywords', controlKeywordPattern.match, testCases.keywords.control));
    }
    
    // Test type keywords
    const typeKeywordPattern = findPattern('storage.type');
    if (typeKeywordPattern) {
        results.push(testPattern('Type Keywords', typeKeywordPattern.match, testCases.keywords.types));
    }
    
    // Test other keywords
    const otherKeywordPattern = findPattern('keyword.other');
    if (otherKeywordPattern) {
        results.push(testPattern('Other Keywords', otherKeywordPattern.match, testCases.keywords.other));
    }
    
    // Test literal keywords
    const literalPattern = findPattern('constant.language');
    if (literalPattern) {
        results.push(testPattern('Literal Keywords', literalPattern.match, testCases.keywords.literals));
    }
    
    // Test numeric literals
    const numberPattern = findPattern('constant.numeric');
    if (numberPattern) {
        results.push(testPattern('Numeric Literals', numberPattern.match, testCases.literals.numbers));
    }
    
    // Test string literals
    const stringPattern = findPattern('string.quoted.double');
    if (stringPattern) {
        results.push(testPattern('String Literals', stringPattern.match, testCases.literals.strings));
    }
    
    // Test character literals
    const charPattern = findPattern('string.quoted.single');
    if (charPattern) {
        results.push(testPattern('Character Literals', charPattern.match, testCases.literals.chars));
    }
    
    // Test operators
    const operatorPatterns = patterns.filter(p => p.name && p.name.startsWith('keyword.operator'));
    for (const opPattern of operatorPatterns) {
        const opType = opPattern.name.split('.').pop();
        if (testCases.operators[opType]) {
            results.push(testPattern(`${opType} Operators`, opPattern.match, testCases.operators[opType]));
        }
    }
    
    // Test Console functions
    const consolePattern = findPattern('support.function');
    if (consolePattern) {
        results.push(testPattern('Console Functions', consolePattern.match, testCases.console));
    }
    
    // Test comments
    const commentPatterns = patterns.filter(p => p.name && p.name.includes('comment'));
    for (const commentPattern of commentPatterns) {
        // Test each comment pattern individually
        for (const comment of testCases.comments) {
            const matches = new RegExp(commentPattern.match).test(comment);
            if (matches) {
                console.log(`✓ Comment "${comment}" matched by ${commentPattern.name}`);
            }
        }
    }
    
    return results;
}

/**
 * Test against real Dream code files
 */
function testRealFiles() {
    console.log('\n🔍 Testing Against Real Dream Files');
    console.log('===================================');
    
    const testFilesDir = path.join(__dirname, '..', 'test_examples');
    const testFiles = [
        'basic_syntax.dr',
        'advanced_features.dr',
        'multifile_example/math_utils.dr',
        'multifile_example/string_utils.dr',
        'multifile_example/main.dr'
    ];
    
    for (const testFile of testFiles) {
        const filePath = path.join(testFilesDir, testFile);
        
        if (!fs.existsSync(filePath)) {
            console.log(`⚠️  File not found: ${testFile}`);
            continue;
        }
        
        console.log(`\n--- Analyzing ${testFile} ---`);
        
        const content = fs.readFileSync(filePath, 'utf8');
        const lines = content.split('\n');
        
        let tokenCounts = {
            keywords: 0,
            operators: 0,
            strings: 0,
            numbers: 0,
            comments: 0,
            console: 0
        };
        
        // Simple token counting
        const patterns = dreamGrammar.repository.tokens.patterns;
        
        for (const line of lines) {
            for (const pattern of patterns) {
                const regex = new RegExp(pattern.match, 'g');
                const matches = line.match(regex);
                
                if (matches) {
                    if (pattern.name.includes('keyword')) {
                        tokenCounts.keywords += matches.length;
                    } else if (pattern.name.includes('operator')) {
                        tokenCounts.operators += matches.length;
                    } else if (pattern.name.includes('string')) {
                        tokenCounts.strings += matches.length;
                    } else if (pattern.name.includes('numeric')) {
                        tokenCounts.numbers += matches.length;
                    } else if (pattern.name.includes('comment')) {
                        tokenCounts.comments += matches.length;
                    } else if (pattern.name.includes('support.function')) {
                        tokenCounts.console += matches.length;
                    }
                }
            }
        }
        
        console.log(`Keywords: ${tokenCounts.keywords}`);
        console.log(`Operators: ${tokenCounts.operators}`);
        console.log(`Strings: ${tokenCounts.strings}`);
        console.log(`Numbers: ${tokenCounts.numbers}`);
        console.log(`Comments: ${tokenCounts.comments}`);
        console.log(`Console calls: ${tokenCounts.console}`);
        console.log(`Lines: ${lines.length}`);
        
        if (tokenCounts.keywords > 0 && tokenCounts.operators > 0) {
            console.log('✅ File appears to be properly tokenized');
        } else {
            console.log('⚠️  File may have tokenization issues');
        }
    }
}

/**
 * Generate summary report
 */
function generateSummary(results) {
    console.log('\n📊 Validation Summary');
    console.log('=====================');
    
    let totalPassed = 0;
    let totalTests = 0;
    
    for (const result of results) {
        totalPassed += result.passed;
        totalTests += result.total;
    }
    
    const overallRate = totalTests > 0 ? (totalPassed / totalTests * 100).toFixed(1) : 0;
    
    console.log(`Total tests: ${totalTests}`);
    console.log(`Passed: ${totalPassed}`);
    console.log(`Failed: ${totalTests - totalPassed}`);
    console.log(`Overall success rate: ${overallRate}%`);
    
    if (parseFloat(overallRate) >= 90) {
        console.log('🎉 Excellent! Syntax highlighting is working very well.');
    } else if (parseFloat(overallRate) >= 75) {
        console.log('👍 Good! Most syntax highlighting is working correctly.');
    } else if (parseFloat(overallRate) >= 50) {
        console.log('⚠️  Moderate. Some syntax highlighting issues need attention.');
    } else {
        console.log('❌ Poor. Significant syntax highlighting issues detected.');
    }
    
    return parseFloat(overallRate);
}

/**
 * Check JetBrains token types
 */
function validateJetBrainsTokens() {
    console.log('\n🔍 Validating JetBrains Token Types');
    console.log('===================================');
    
    const jetbrainsTokenPath = path.join(__dirname, '..', 'jetbrains', 'src', 'main', 'java', 'com', 'dream', 'DreamTokenTypes.java');
    
    if (!fs.existsSync(jetbrainsTokenPath)) {
        console.log('⚠️  JetBrains token types file not found');
        return;
    }
    
    const tokenTypes = fs.readFileSync(jetbrainsTokenPath, 'utf8');
    const tokenNames = tokenTypes.match(/public static final IElementType (\w+)/g);
    
    if (tokenNames) {
        console.log(`Found ${tokenNames.length} token types:`);
        const expectedTokens = [
            'KEYWORD', 'KEYWORDTYPE', 'KEYWORDOTHER', 'KEYWORDLITERAL',
            'NUMBER', 'STRING', 'CHAR', 'COMMENT', 'COMMENTBLOCK', 'COMMENTDOC',
            'CONSOLEFUNCTION', 'IDENTIFIER',
            'OPERATORARITHMETIC', 'OPERATORBITWISE', 'OPERATORCOMPARISON',
            'OPERATORLOGICAL', 'OPERATORASSIGNMENT', 'OPERATORCONDITIONAL', 'OPERATORARROW',
            'SEMICOLON', 'COMMA', 'DOT', 'PAREN', 'BRACE', 'BRACKET'
        ];
        
        let foundTokens = 0;
        for (const expected of expectedTokens) {
            if (tokenTypes.includes(expected)) {
                console.log(`✓ ${expected}`);
                foundTokens++;
            } else {
                console.log(`✗ ${expected} - MISSING`);
            }
        }
        
        const completeness = (foundTokens / expectedTokens.length * 100).toFixed(1);
        console.log(`JetBrains token completeness: ${foundTokens}/${expectedTokens.length} (${completeness}%)`);
    } else {
        console.log('❌ Could not parse token types');
    }
}

/**
 * Main execution
 */
function main() {
    console.log('Dream Language Plugin Validation Tool');
    console.log('====================================');
    console.log(`Timestamp: ${new Date().toISOString()}\n`);
    
    try {
        // Validate grammar patterns
        const results = validateGrammar();
        
        // Test against real files
        testRealFiles();
        
        // Validate JetBrains tokens
        validateJetBrainsTokens();
        
        // Generate summary
        const successRate = generateSummary(results);
        
        console.log('\n🏁 Validation Complete');
        console.log('======================');
        
        if (successRate >= 85) {
            console.log('✅ Plugin validation PASSED');
            process.exit(0);
        } else {
            console.log('❌ Plugin validation FAILED');
            process.exit(1);
        }
        
    } catch (error) {
        console.error('❌ Validation failed with error:', error.message);
        console.error(error.stack);
        process.exit(1);
    }
}

// Run if called directly
if (require.main === module) {
    main();
}

module.exports = {
    validateGrammar,
    testRealFiles,
    generateSummary,
    validateJetBrainsTokens
};