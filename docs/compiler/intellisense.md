# Dream IntelliSense Setup

This guide shows how to enable IntelliSense for `*.dr` files in VS Code and JetBrains IDEs using the DreamCompiler language server. It assumes a clean Linux or Windows system with internet access and covers steps valid for 2025 versions of the tools.

## 1. Prerequisites

* **Node.js 20+** – required for the extension scaffolding and language server.
* **Java 17+** – required for JetBrains plugin builds.
* **DreamCompiler source** – clone the repository and build once:
  ```bash
  git clone https://github.com/Ddemon26/DreamCompiler.git
  cd DreamCompiler
  zig build
  ```
* Optionally install **Yeoman** (`npm install -g yo generator-code`) or **JetBrains Toolbox** for easier setup.

## 2. Project Scaffold

### VS Code
1. Create a new extension with `yo code` choosing the **TypeScript** language server template. Alternatively start with a minimal `package.json` and `tsconfig.json`.
2. The generated project contains a `client` and `server` folder. Copy the prebuilt `DreamCompiler` language server binary or include the `dream-language-server` npm package.

### JetBrains
1. Run `gradle init` inside a new directory or start from the skeleton under `assets/jetbrains` in this repo.
2. Ensure the Gradle `plugins` block includes `org.jetbrains.intellij` and `org.jetbrains.grammarkit` for LSP and lexer generation.
3. Define source modules under `src/main/java` and resources under `src/main/resources`.

## 3. Language Server Configuration

### VS Code
* `package.json` must declare activation events and contributions:
  ```json
  {
    "activationEvents": ["onLanguage:dream"],
    "contributes": {
      "languages": [{ "id": "dream", "extensions": [".dr"], "aliases": ["Dream"] }]
    },
    "main": "./dist/extension.js"
  }
  ```
* In `extension.ts` initialise a `LanguageClient` pointing at the DreamCompiler server executable. Place server code in `server/`.

### JetBrains
* `plugin.xml` registers a `lspServerDefinition` that launches `dream-language-server`:
  ```xml
  <extensions defaultExtensionNs="com.intellij">
      <lspServerDefinition implementation="com.dream.DreamLspServerDefinition"/>
  </extensions>
  ```
* `DreamLspServerDefinition` starts the Node-based server bundled with the plugin.

## 4. Implementing Completion & Features

Both platforms rely on the language server’s `textDocument/completion`, `hover`, and `definition` handlers.
The server obtains symbol information by invoking the `parse` driver with the
`--symbols` flag which prints a JSON array of declarations. This allows
completions and go-to-definition to reuse the compiler's AST.

### VS Code
* Inside `server/src/server.ts` implement the handlers using the `vscode-languageserver` module. Example:
```ts
connection.onCompletion(params => {
    // compute completion items
});
```

### JetBrains
The bundled plugin delegates completion, hover and definitions to the same
`dream-language-server` executable.

## 5. Building & Running Locally

### VS Code
  ```bash
  npm install
  npm run build
  # Press F5 inside VS Code to launch the Extension Host
  ```

### JetBrains
  ```bash
  ./gradlew runIde
  ```
This runs a sandbox IntelliJ instance with the plugin installed.

## 6. Packaging & Distribution

### VS Code
  ```bash
  npm install -g vsce
  vsce package
  # result: dream-intellisense-<version>.vsix
  ```
Publish the `.vsix` to the Visual Studio Marketplace or install locally via `Extensions: Install from VSIX`.

### JetBrains
  ```bash
  ./gradlew buildPlugin
  # result under build/distributions
  ```
Upload the zip to the JetBrains Plugin Repository or install locally via *Settings › Plugins › Install plugin from disk*.

## 7. Usage Example

1. Install the packaged extension or plugin.
2. Open a `.dr` file:
   ```dr
   Console.WriteLine("Hello");
   ```
3. Typing `Console.` should show method suggestions. Hovering an identifier reveals documentation from the language server.

## 8. Troubleshooting

* If the server fails to start, check the console for port conflicts or missing executables.
* Use `--verbose` for the Node server or inspect IDE logs under `~/.config/Code/logs` or `~/.cache/JetBrains/<IDE>/log`.
* For token updates run:
  ```bash
  node assets/jetbrains/scripts/genFromTokens.js
  ```
  followed by `gradle build`.
