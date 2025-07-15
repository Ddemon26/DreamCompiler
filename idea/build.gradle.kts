import org.jetbrains.changelog.Changelog
import org.jetbrains.changelog.markdownToHTML
import org.jetbrains.intellij.platform.gradle.IntelliJPlatformType

fun properties(key: String) = project.findProperty(key).toString()

plugins {
    // Java support
    id("java")
    // Kotlin support
    id("org.jetbrains.kotlin.jvm") version "1.9.24"
    // Gradle IntelliJ Plugin
    id("org.jetbrains.intellij") version "1.17.3"
    // Gradle Changelog Plugin
    id("org.jetbrains.changelog") version "2.2.0"
    // Gradle Qodana Plugin
    id("org.jetbrains.qodana") version "2024.1.4"
}

group = properties("pluginGroup")
version = properties("pluginVersion")

// Configure the IntelliJ Platform plugin.
// Read more: https://plugins.jetbrains.com/docs/intellij/tools-gradle-intellij-plugin.html
intellij {
    pluginName.set(properties("pluginName"))
    version.set(properties("platformVersion"))
    type.set(IntelliJPlatformType.valueOf(properties("platformType")))

    // Plugin Dependencies. Uses `platformPlugins` property from the gradle.properties file.
    plugins.set(properties("platformPlugins").split(',').map(String::trim).filter(String::isNotEmpty))
}

repositories {
    mavenCentral()
    maven { url = uri("https://www.jetbrains.com/intellij-repository/snapshots") }
}

// Configure the Changelog plugin.
// Read more: https://github.com/JetBrains/gradle-changelog-plugin
changelog {
    val changelog = project.file("CHANGELOG.md")
    if (changelog.exists() && changelog.readText().isNotBlank()) {
        version.set(properties("pluginVersion"))
        path.set(changelog.absolutePath)
    }

    groups.set(emptyList())
    combineVersionSections.set(true)
}

tasks {
    // Set the JVM compatibility versions
    withType<JavaCompile> {
        sourceCompatibility = "21"
        targetCompatibility = "21"
    }
    withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile> {
        kotlinOptions.jvmTarget = "21"
    }

    patchPluginXml {
        version.set(properties("pluginVersion"))
        sinceBuild.set(properties("pluginSinceBuild"))
        untilBuild.set(properties("pluginUntilBuild"))

        // Extract the description from the README file.
        pluginDescription.set(
            project.file("README.md").readText().lines().let { lines ->
                val start = lines.indexOfFirst { it.startsWith("<!-- Plugin description -->") }
                val end = lines.indexOfFirst { it.startsWith("<!-- Plugin description end -->") }
                if (start == -1 || end == -1) {
                    throw GradleException("Plugin description section not found in README.md")
                }
                lines.subList(start + 1, end).joinToString("\n").let(::markdownToHTML)
            }
        )

        // Get the latest available change notes from the changelog file.
        changeNotes.set(provider {
            changelog.get().run {
                if (isUnreleased.get()) {
                    getUnreleased()
                } else {
                    getLatest()
                }
            }.withHeader(false).toHTML()
        })
    }

    // Configure the Qodana plugin
    qodana {
        cachePath.set(project.layout.buildDirectory.dir("qodana-cache"))
        reportPath.set(project.layout.buildDirectory.dir("qodana-report"))
        saveReport.set(true)
        showReport.set(System.getenv("QODANA_SHOW_REPORT")?.toBoolean() ?: false)
    }

    // Configure the Verifications plugin
    runPluginVerifier {
        ideVersions.set(properties("pluginVerifierIdeVersions").split(',').map(String::trim).filter(String::isNotEmpty))
    }
}