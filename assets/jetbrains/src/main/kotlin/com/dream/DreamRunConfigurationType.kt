package com.dream

import com.intellij.execution.configurations.*
import com.intellij.execution.runners.ExecutionEnvironment
import com.intellij.execution.Executor
import com.intellij.execution.process.OSProcessHandler
import com.intellij.execution.configurations.GeneralCommandLine
import com.intellij.openapi.project.Project
import com.intellij.openapi.util.IconLoader

class DreamRunConfigurationType : ConfigurationTypeBase("DreamRun", "Dream Run", "Run Dream programs", IconLoader.getIcon("/icons/DreamScriptLogo.svg", DreamRunConfigurationType::class.java)) {
    init {
        addFactory(object : ConfigurationFactory(this) {
            override fun createTemplateConfiguration(project: Project): RunConfiguration {
                return DreamRunConfiguration(project, this, "Dream")
            }
        })
    }
}

class DreamRunConfiguration(project: Project, factory: ConfigurationFactory, name: String) : RunConfigurationBase<Any>(project, factory, name) {
    var file: String = ""

    override fun getConfigurationEditor() = DreamRunSettingsEditor(project)

    override fun checkConfiguration() {}

    override fun getState(executor: Executor, environment: ExecutionEnvironment): RunProfileState? {
        return object : CommandLineState(environment) {
            override fun startProcess(): OSProcessHandler {
                val cmd = GeneralCommandLine("zig", "build", "run", "--", file)
                cmd.workDirectory = project.basePath?.let { java.io.File(it) }
                return OSProcessHandler(cmd)
            }
        }
    }
}
