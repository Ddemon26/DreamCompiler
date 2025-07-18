import {
  createConnection,
  TextDocuments,
  ProposedFeatures,
  InitializeParams,
  CompletionItem,
  CompletionItemKind,
  Hover,
  TextDocumentPositionParams
} from 'vscode-languageserver/node';
import { TextDocument } from 'vscode-languageserver-textdocument';

const connection = createConnection(ProposedFeatures.all);
const documents = new TextDocuments(TextDocument);

connection.onInitialize((_params: InitializeParams) => {
  return {
    capabilities: {
      textDocumentSync: documents.syncKind,
      completionProvider: { resolveProvider: false },
      hoverProvider: true
    }
  };
});

connection.onCompletion((_params: TextDocumentPositionParams): CompletionItem[] => {
  return [
    { label: 'Console.WriteLine', kind: CompletionItemKind.Function },
    { label: 'Console.Write', kind: CompletionItemKind.Function }
  ];
});

connection.onHover((_params: TextDocumentPositionParams): Hover => {
  return { contents: { kind: 'markdown', value: 'Dream symbol' } };
});

documents.listen(connection);
connection.listen();
