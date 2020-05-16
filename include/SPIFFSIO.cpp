// Modified version of: https://github.com/espressif/arduino-esp32/blob/master/libraries/SPIFFS/examples/SPIFFS_Test/SPIFFS_Test.ino
// GNU LGPL

#include "FS.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED false

/** Initialize SPIFFS for use */
void initializeSPIFFS()
{
    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
}

/** List directory contents */
void listDir(const char *dirname, uint8_t levels, fs::FS &fs = SPIFFS)
{
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(file.name(), levels - 1, fs);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

/** Read file from SPIFFS */
void readFile(const char *path, fs::FS &fs = SPIFFS)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while (file.available())
    {
        Serial.write(file.read());
    }
}

/** Read file as String */
String readString(const char *path, fs::FS &fs = SPIFFS)
{
    File file = fs.open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return "-1";
    }
    return file.readStringUntil('\n');
}

/** Write file to SPIFFS */
void writeFile(const char *path, const char *message, fs::FS &fs = SPIFFS)
{
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- file written");
    }
    else
    {
        Serial.println("- frite failed");
    }
}

/** Append content to a file */
void appendFile(const char *path, const char *message, fs::FS &fs = SPIFFS)
{
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("- failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- message appended");
    }
    else
    {
        Serial.println("- append failed");
    }
}

void renameFile(const char *path1, const char *path2, fs::FS &fs = SPIFFS)
{
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2))
    {
        Serial.println("- file renamed");
    }
    else
    {
        Serial.println("- rename failed");
    }
}

void deleteFile(const char *path, fs::FS &fs = SPIFFS)
{
    Serial.printf("Deleting file: %s\r\n", path);
    if (fs.remove(path))
    {
        Serial.println("- file deleted");
    }
    else
    {
        Serial.println("- delete failed");
    }
}

/* For testing
void testFileIO(const char * path, fs::FS &fs = SPIFFS){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("- failed to open file for reading");
    }
}
*/

/* Example usage

  initializeSPIFFS();
  listDir("/", 0);
  writeFile("/hello.txt", "Hello ");
  appendFile("/hello.txt", "World!\r\n");
  readFile("/hello.txt");
  renameFile("/hello.txt", "/foo.txt");
  readFile("/foo.txt");
  deleteFile("/foo.txt");
  testFileIO("/test.txt");
  deleteFile("/test.txt");
*/