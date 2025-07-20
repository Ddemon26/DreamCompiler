package com.dream

import com.intellij.openapi.options.SettingsEditor
import com.intellij.openapi.project.Project
import javax.swing.*
import java.awt.BorderLayout

class DreamRunSettingsEditor(private val project: Project) : SettingsEditor<DreamRunConfiguration>() {
    private val textField = JTextField()

    override fun resetEditorFrom(s: DreamRunConfiguration) {
        textField.text = s.file
    }

    override fun applyEditorTo(s: DreamRunConfiguration) {
        s.file = textField.text
    }

    override fun createEditor(): JComponent {
        val panel = JPanel(BorderLayout())
        panel.add(JLabel("Dream file:"), BorderLayout.WEST)
        panel.add(textField, BorderLayout.CENTER)
        return panel
    }
}
