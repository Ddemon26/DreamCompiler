import * as path from 'path';
import * as vscode from 'vscode';
import { LanguageClient, LanguageClientOptions, ServerOptions, TransportKind } from 'vscode-languageclient/node';

let client: LanguageClient | undefined;

export function activate(context: vscode.ExtensionContext) {
  const serverModule = context.asAbsolutePath(path.join('dist', 'server.js'));

  const serverOptions: ServerOptions = {
    run: { module: serverModule, transport: TransportKind.ipc },
    debug: {
      module: serverModule,
      transport: TransportKind.ipc,
      options: { execArgv: ['--nolazy', '--inspect=6009'] }
    }
  };

  const clientOptions: LanguageClientOptions = {
    documentSelector: [{ language: 'dream' }],
    outputChannelName: 'Dream Language Server'
  };

  client = new LanguageClient('dream', 'Dream Language Server', serverOptions, clientOptions);
  context.subscriptions.push({ dispose: () => client?.stop() });
  client.start();
}

export function deactivate(): Thenable<void> | undefined {
  return client?.stop();
}
