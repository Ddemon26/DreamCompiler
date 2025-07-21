package com.dream

import com.intellij.testFramework.fixtures.BasePlatformTestCase
import com.intellij.openapi.fileTypes.FileTypeManager

class DreamFileTypeTest : BasePlatformTestCase() {
    
    /**
     * Test that .dr files are recognized as Dream files
     */
    fun testDreamFileExtensionRecognition() {
        val fileTypeManager = FileTypeManager.getInstance()
        val dreamFileType = fileTypeManager.getFileTypeByExtension("dr")
        
        assertNotNull("Dream file type should be registered", dreamFileType)
        assertEquals("File type should be DreamFileType", 
                    DreamFileType.INSTANCE, dreamFileType)
    }
    
    /**
     * Test Dream file type properties
     */
    fun testDreamFileTypeProperties() {
        val fileType = DreamFileType.INSTANCE
        
        assertEquals("Name should be 'Dream file'", "Dream file", fileType.name)
        assertEquals("Description should contain 'Dream'", true, 
                    fileType.description.contains("Dream"))
        assertEquals("Default extension should be 'dr'", "dr", 
                    fileType.defaultExtension)
        assertNotNull("Icon should not be null", fileType.icon)
    }
    
    /**
     * Test that Dream files can be created and recognized
     */
    fun testDreamFileCreation() {
        val dreamContent = """
        // Test Dream file
        func main() {
            Console.WriteLine("Hello Dream!");
        }
        """
        
        val file = myFixture.configureByText("test.dr", dreamContent)
        
        assertNotNull("File should be created", file)
        assertEquals("File type should be Dream", 
                    DreamFileType.INSTANCE, file.fileType)
        assertTrue("Virtual file should exist", file.virtualFile.exists())
        assertEquals("File extension should be .dr", "dr", 
                    file.virtualFile.extension)
    }
    
    /**
     * Test language association
     */
    fun testLanguageAssociation() {
        val fileType = DreamFileType.INSTANCE
        val language = fileType.language
        
        assertNotNull("Language should not be null", language)
        assertEquals("Language should be DreamLanguage", 
                    DreamLanguage, language)
        assertEquals("Language ID should be 'dream'", "dream", language.id)
        assertEquals("Language display name should be 'dream'", "dream", 
                    language.displayName)
    }
}