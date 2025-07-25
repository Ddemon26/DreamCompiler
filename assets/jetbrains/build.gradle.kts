import org.jetbrains.kotlin.gradle.dsl.JvmTarget

plugins {
    id("java")
    id("org.jetbrains.kotlin.jvm") version "1.9.24"
    id("org.jetbrains.intellij") version "1.17.2"
}

repositories {
    mavenCentral()
}

dependencies {
    testImplementation("junit:junit:4.13.2")
}

java {
    toolchain.languageVersion.set(JavaLanguageVersion.of(17))
}

tasks.withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile> {
    compilerOptions.jvmTarget.set(JvmTarget.JVM_17)
}

tasks.withType<JavaCompile> {
    options.compilerArgs.add("-Xlint:deprecation")
}

intellij {
    version.set("2023.3.7")
    type.set("IC")
    plugins.set(listOf("com.intellij.java"))
}

tasks {
    buildSearchableOptions {
        enabled = false
    }

    val generateTokens by registering(Exec::class) {
        workingDir = projectDir.parentFile
        commandLine("node", "jetbrains/scripts/genFromTokens.js")
    }

    patchPluginXml {
        sinceBuild.set("251")
        untilBuild.set("251.*")
    }

    // Note: Manual lexer is used, no generation needed for now
}


tasks.withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile> {
    dependsOn("generateTokens")
}

tasks.withType<JavaCompile> {
    dependsOn("generateTokens")
}