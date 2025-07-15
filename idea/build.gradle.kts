import org.jetbrains.kotlin.gradle.dsl.JvmTarget

plugins {
    id("java")
    id("org.jetbrains.kotlin.jvm") version "2.0.0"
    id("org.jetbrains.intellij") version "1.17.3"
    id("org.jetbrains.grammarkit") version "2022.3.2"
}

repositories {
    mavenCentral()
}

java {
    toolchain.languageVersion.set(JavaLanguageVersion.of(21))
    sourceCompatibility = JavaVersion.VERSION_17
    targetCompatibility = JavaVersion.VERSION_17
}

tasks.withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile> {
    compilerOptions.jvmTarget.set(JvmTarget.JVM_17)
}

tasks.withType<JavaCompile> {
    options.compilerArgs.add("-Xlint:deprecation")
}

intellij {
    version.set("2025.1")
    type.set("IC")
}

tasks {
    buildSearchableOptions {
        enabled = false
    }

    val generateTokens by registering(Exec::class) {
        workingDir = rootDir
        commandLine("node", "../scripts/genFromTokens.js")
    }

    patchPluginXml {
        sinceBuild.set("251")
        untilBuild.set("251.*")
    }

    withType<org.jetbrains.grammarkit.tasks.GenerateLexerTask> {
        sourceFile.set(file("src/main/java/com/dream/DreamLexer.flex"))
        targetDir.set("build/generated-src/flex/com/dream")
        targetClass.set("DreamLexer")
        purgeOldFiles.set(true)
        dependsOn(generateTokens)
    }
}

sourceSets["main"].java.srcDir("build/generated-src/flex")

tasks.withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile> {
    dependsOn("generateLexer")
}

dependencies {
    testImplementation(kotlin("test"))
}
