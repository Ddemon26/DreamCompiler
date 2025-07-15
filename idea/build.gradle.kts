plugins {
    id("java")
    id("org.jetbrains.kotlin.jvm") version "1.9.23"
    id("org.jetbrains.intellij") version "1.17.2"
    id("org.jetbrains.grammarkit") version "2022.3.2"
}

repositories {
    mavenCentral()
}

intellij {
    version.set("2024.2")
    type.set("IC")
}

tasks.patchPluginXml {
    sinceBuild.set("241")
    untilBuild.set("*")
}

tasks.named<org.jetbrains.grammarkit.tasks.GenerateLexerTask>("generateLexer") {
    sourceFile.set(file("src/main/java/com/dream/DreamLexer.flex"))
    targetDir.set("build/generated-src/flex/com/dream")
    targetClass.set("DreamLexer")
    purgeOldFiles.set(true)
}

sourceSets["main"].java.srcDir("build/generated-src/flex")

tasks.named<JavaCompile>("compileJava") { dependsOn("generateLexer") }