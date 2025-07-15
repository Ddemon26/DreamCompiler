plugins {
    id("java")
    id("org.jetbrains.kotlin.jvm") version "1.9.23"
    id("org.jetbrains.intellij") version "1.17.2"
    id("org.jetbrains.grammarkit") version "2022.3.2"
}

repositories {
    mavenCentral()
}

java {
    toolchain.languageVersion.set(JavaLanguageVersion.of(21))
}

tasks.withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile> {
    kotlinOptions.jvmTarget = "21"
}

tasks.withType<JavaCompile> {
    options.compilerArgs.add("-Xlint:deprecation")
}

intellij {
    version.set("2024.2")
    type.set("IC")
}

tasks {
    buildSearchableOptions {
        enabled = false
    }

    patchPluginXml {
        sinceBuild.set("242")
        untilBuild.set("242.*")
    }

    withType<org.jetbrains.grammarkit.tasks.GenerateLexerTask> {
        sourceFile.set(file("src/main/java/com/dream/DreamLexer.flex"))
        targetDir.set("build/generated-src/flex/com/dream")
        targetClass.set("DreamLexer")
        purgeOldFiles.set(true)
    }
}

sourceSets["main"].java.srcDir("build/generated-src/flex")

tasks.withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile> {
    dependsOn("generateLexer")
}