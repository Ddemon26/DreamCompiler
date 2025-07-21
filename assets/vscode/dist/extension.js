"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.deactivate = exports.activate = void 0;
const path = require("path");
const node_1 = require("vscode-languageclient/node");
let client;
function activate(context) {
    const serverModule = context.asAbsolutePath(path.join('dist', 'server.js'));
    const serverOptions = {
        run: { module: serverModule, transport: node_1.TransportKind.ipc },
        debug: {
            module: serverModule,
            transport: node_1.TransportKind.ipc,
            options: { execArgv: ['--nolazy', '--inspect=6009'] }
        }
    };
    const clientOptions = {
        documentSelector: [{ language: 'dream' }],
        outputChannelName: 'Dream Language Server'
    };
    client = new node_1.LanguageClient('dream', 'Dream Language Server', serverOptions, clientOptions);
    context.subscriptions.push({ dispose: () => client === null || client === void 0 ? void 0 : client.stop() });
    client.start();
}
exports.activate = activate;
function deactivate() {
    return client === null || client === void 0 ? void 0 : client.stop();
}
exports.deactivate = deactivate;
//# sourceMappingURL=extension.js.map