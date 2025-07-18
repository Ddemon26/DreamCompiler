package com.dream

import com.intellij.lsp.LspServerDefinition
import com.intellij.lsp.LspServerSupportProvider
import com.intellij.openapi.project.Project
import com.intellij.openapi.vfs.VirtualFile
import java.nio.file.Paths

class DreamLspServerDefinition : LspServerSupportProvider {
    override fun isSupported(project: Project, file: VirtualFile): Boolean {
        return file.extension == "dr"
    }

    override fun createLspServer(project: Project, file: VirtualFile): LspServerDefinition? {
        val node = if (System.getProperty("os.name").startsWith("Windows")) "node.exe" else "node"
        val pluginPath = Paths.get(project.basePath ?: return null)
            .resolve("assets")
            .resolve("vscode")
            .resolve("dist")
            .resolve("server.js")
            .toString()
        return LspServerDefinition(project, listOf(node, pluginPath))
    }
}
