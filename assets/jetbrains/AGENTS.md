# JetBrains Plugin Instructions

This directory contains the Gradle project for the Dream language IDE plugin. Token definitions for the lexer and syntax highlighter are generated from the compiler's `src/lexer/tokens.def` file.

## Regenerating Tokens

Whenever `tokens.def` changes (new keywords, operators, etc.) run the generator script from the repository root:

```bash
node assets/jetbrains/scripts/genFromTokens.js
```

This script updates:

- `assets/vscode/syntaxes/dream.tmLanguage.json`
- `assets/jetbrains/src/main/java/com/dream/DreamLexer.flex`
- `assets/jetbrains/src/main/resources/tokens.json`
- `assets/jetbrains/tokens.json`

After running the script, build the plugin to regenerate the JFlex lexer and run tests:

```bash
cd assets/jetbrains
gradle generateLexer build test
```

The Gradle task `generateLexer` depends on the Node script, so running `gradle build` will also keep tokens in sync. Ensure JDK 17 is installed.

## Adding or Updating Tokens

1. Edit `src/lexer/tokens.def` with the new token definitions.
2. Run the steps above to regenerate all editor grammar files.
3. Commit the updated sources and regenerated files together so the plugin remains consistent.

