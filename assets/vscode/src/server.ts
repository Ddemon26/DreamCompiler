import {
  createConnection,
  TextDocuments,
  ProposedFeatures,
  InitializeParams,
  CompletionItem,
  CompletionItemKind,
  Hover,
  Location,
  Diagnostic,
  DiagnosticSeverity,
  TextDocumentPositionParams
} from 'vscode-languageserver/node';
import { TextDocument } from 'vscode-languageserver-textdocument';
import { spawnSync } from 'child_process';
import * as os from 'os';
import * as path from 'path';

interface SymbolInfo {
  line: number;
  character: number;
  kind: 'func' | 'var' | 'class';
}

const documentsInfo: Map<string, { symbols: Map<string, SymbolInfo> }> = new Map();

const connection = createConnection(ProposedFeatures.all);
const documents = new TextDocuments(TextDocument);

connection.onInitialize((_params: InitializeParams) => {
  return {
    capabilities: {
      textDocumentSync: documents.syncKind,
      completionProvider: { resolveProvider: false },
      hoverProvider: true,
      definitionProvider: true
    }
  };
});

function getParseExecutable(): string {
  const exe = os.platform() === 'win32' ? 'parse.exe' : 'parse';
  return path.join(__dirname, '..', '..', 'zig-out', 'bin', exe);
}

function analyzeDocument(doc: TextDocument): void {
  const info = { symbols: new Map<string, SymbolInfo>() };
  const text = doc.getText();
  const lines = text.split(/\r?\n/);
  const symbolRegex = /^(func|var|class)\s+(\w+)/;
  lines.forEach((line, idx) => {
    const m = symbolRegex.exec(line);
    if (m) {
      info.symbols.set(m[2], { line: idx, character: line.indexOf(m[2]), kind: m[1] as SymbolInfo['kind'] });
    }
  });
  documentsInfo.set(doc.uri, info);

  const parseExe = getParseExecutable();
  const res = spawnSync(parseExe, [doc.uri.replace('file://', '')]);
  const diagnostics: Diagnostic[] = [];
  if (res.stderr) {
    const pattern = /(\d+):(\d+): (error|warning): (.*)/;
    res.stderr.toString().split(/\r?\n/).forEach(line => {
      const m = pattern.exec(line.trim());
      if (m) {
        diagnostics.push({
          range: {
            start: { line: parseInt(m[1]) - 1, character: parseInt(m[2]) - 1 },
            end: { line: parseInt(m[1]) - 1, character: parseInt(m[2]) }
          },
          severity: m[3] === 'error' ? DiagnosticSeverity.Error : DiagnosticSeverity.Warning,
          message: m[4]
        });
      }
    });
  }
  connection.sendDiagnostics({ uri: doc.uri, diagnostics });
}

documents.onDidOpen(e => analyzeDocument(e.document));
documents.onDidChangeContent(e => analyzeDocument(e.document));

connection.onCompletion((params: TextDocumentPositionParams): CompletionItem[] => {
  const doc = documents.get(params.textDocument.uri);
  const items: CompletionItem[] = [
    { label: 'Console.WriteLine', kind: CompletionItemKind.Function },
    { label: 'Console.Write', kind: CompletionItemKind.Function }
  ];
  if (doc) {
    const info = documentsInfo.get(doc.uri);
    if (info) {
      for (const [name, sym] of info.symbols) {
        items.push({ label: name, kind: sym.kind === 'func' ? CompletionItemKind.Function : CompletionItemKind.Variable });
      }
    }
  }
  return items;
});

connection.onDefinition((params: TextDocumentPositionParams): Location | null => {
  const info = documentsInfo.get(params.textDocument.uri);
  const doc = documents.get(params.textDocument.uri);
  if (!info || !doc) return null;
  const word = getWordAtPosition(doc, params.position);
  const sym = info.symbols.get(word);
  if (!sym) return null;
  return {
    uri: params.textDocument.uri,
    range: {
      start: { line: sym.line, character: sym.character },
      end: { line: sym.line, character: sym.character + word.length }
    }
  };
});

connection.onHover((params: TextDocumentPositionParams): Hover => {
  const doc = documents.get(params.textDocument.uri);
  const info = documentsInfo.get(params.textDocument.uri);
  if (!doc || !info) return { contents: { kind: 'markdown', value: '' } };
  const word = getWordAtPosition(doc, params.position);
  const sym = info.symbols.get(word);
  if (!sym) return { contents: { kind: 'markdown', value: '' } };
  return { contents: { kind: 'markdown', value: `**${sym.kind}** \`${word}\`` } };
});

function getWordAtPosition(doc: TextDocument, pos: { line: number; character: number }): string {
  const line = doc.getText({ start: { line: pos.line, character: 0 }, end: { line: pos.line + 1, character: 0 } });
  const left = line.slice(0, pos.character).match(/\w+$/);
  const right = line.slice(pos.character).match(/^\w+/);
  return `${left ? left[0] : ''}${right ? right[0] : ''}`;
}

documents.listen(connection);
connection.listen();
