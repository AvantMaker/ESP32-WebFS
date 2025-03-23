/*
 * Author: Avant Maker
 * Website: www.AvantMaker.com
 * Date: March 23, 2025
 * Version: 0.2
 * License: MIT 
 * 
 * Description: 
 * ESP32-WebFS transforms your ESP32 into a web server, enabling
 * wireless management of the SPIFFS (SPI Flash File System)
 * through a web browser. This allows you to easily upload,
 * download, delete, and view files stored on the ESP32, making
 * it ideal for remote configuration, data storage, and resource
 * management in IoT projects without requiring a direct physical
 * connection.
 *
 * Code Source:
 * This example code is sourced from the Comprehensive Guide
 * to the ESP32 Arduino Core Library, accessible on AvantMaker.com.
 * For additional code examples and in-depth documentation related to
 * the ESP32 Arduino Core Library, please visit:
 *
 * https://avantmaker.com/home/all-about-esp32-arduino-core-library/
 *
 * AvantMaker.com, your premier destination for all things
 * DIY, AI, IoT, Smart Home, and STEM projects. We are dedicated
 * to empowering makers, learners, and enthusiasts with
 * the resources they need to bring their nnovative ideas to life.
 */
 

#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

const char* ssid = "your-SSID";          // Replace with your Wi-Fi SSID
const char* password = "your-PASSWORD";  // Replace with your Wi-Fi password

WebServer server(80);

File uploadFile;

void setup() {
  Serial.begin(115200);
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Route for the root page
  server.on("/", HTTP_GET, []() {
    String html = "<h1>ESP32 WebFS</h1>";
    html += "<p>ESP32 WebFS provides a web-based interface to manage the SPIFFS on your ESP32. </p>";
    html += "<p><a href='/list'>View Files</a></p>";
    html += "<p><a href='/delete'>Delete Files</a></p>";
    html += "<p><a href='/download'>Download Files</a></p>";
    html += "<h2>Upload New File</h2>";
    html += "<form method='post' action='/upload' enctype='multipart/form-data'>";
    html += "<input type='file' name='upload'>";
    html += "<input type='submit' value='Upload'>";
    html += "</form>";
    html += "<p>This project is proudly brought to you by the team at AvantMaker.com.</p>";
    html += "<p>Visit us at <a href='AvantMaker.com'>AvantMaker.com</a> where we've crafted a comprehensive collection <br> of Reference and Tutorial materials for the ESP32</p>";
    server.send(200, "text/html", html);
  });

  // Route to handle file uploads
  server.on("/upload", HTTP_POST, 
    // This is the completion handler (called after upload finishes)
    []() {
      server.sendHeader("Location", "/list");
      server.send(303);
    },
    // This is the upload handler (called during upload)
    handleFileUpload
  );
  
  // Route to list all files in SPIFFS
  server.on("/list", HTTP_GET, handleFileList);
  
  // Route to view the delete files page
  server.on("/delete", HTTP_GET, handleDeletePage);
  
  // Route to handle file deletion
  server.on("/delete-file", HTTP_POST, handleDeleteFile);
  
  // Route to view the download files page
  server.on("/download", HTTP_GET, handleDownloadPage);
  
  // Route to handle file download
  server.on("/download-file", HTTP_GET, handleFileDownload);
  
  // Add handler for viewing file contents
  server.on("/file", HTTP_GET, [](){
    if(server.hasArg("name")){
      String fileName = server.arg("name");
      
      // Debug output
      Serial.print("Attempting to view file: ");
      Serial.println(fileName);
      
      // Ensure filename has a leading slash for SPIFFS
      if (!fileName.startsWith("/")) {
        fileName = "/" + fileName;
      }
      
      Serial.print("Normalized filename: ");
      Serial.println(fileName);
      
      if(SPIFFS.exists(fileName)){
        File file = SPIFFS.open(fileName, "r");
        if(file){
          String content = file.readString();
          file.close();
          
          server.send(200, "text/plain", content);
          Serial.println("File sent successfully");
        } else {
          server.send(500, "text/plain", "Failed to open file");
          Serial.println("Failed to open file");
        }
      } else {
        server.send(404, "text/plain", "File not found");
        Serial.println("File not found");
      }
    } else {
      server.send(400, "text/plain", "Missing name parameter");
      Serial.println("Missing name parameter");
    }
  });
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  
  if (upload.status == UPLOAD_FILE_START) {
    // Start a new upload
    String filename = "/" + upload.filename;
    Serial.printf("handleFileUpload Name: %s\n", filename.c_str());
    uploadFile = SPIFFS.open(filename, FILE_WRITE);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    // Write uploaded file data
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
    Serial.printf("handleFileUpload Data: %u\n", upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    // End of upload
    if (uploadFile) {
      uploadFile.close();
    }
    Serial.printf("handleFileUpload Size: %u\n", upload.totalSize);
  }
}

void handleFileList() {
  String path = "/";
  File root = SPIFFS.open(path);
  
  String output = "<html><head><title>ESP32 File List</title></head><body>";
  output += "<h1>SPIFFS File List</h1>";
  output += "<table border='1'><tr><th>Name</th><th>Size</th><th>Actions</th></tr>";
  
  if(root.isDirectory()){
    File file = root.openNextFile();
    while(file){
      if(!file.isDirectory()){
        // Get the filename with the leading slash
        String fileName = String(file.name());
        
        output += "<tr><td>" + fileName + "</td><td>" + String(file.size()) + " bytes</td>";
        // Pass the full filename including the slash in the URL
        output += "<td><a href='/file?name=" + fileName + "'>View</a></td></tr>";
      }
      file = root.openNextFile();
    }
  }
  
  output += "</table><br><a href='/'>Back to Home</a></body></html>";
  server.send(200, "text/html", output);
}

void handleDeletePage() {
  String path = "/";
  File root = SPIFFS.open(path);
  
  String output = "<html><head><title>Delete Files</title></head><body>";
  output += "<h1>Delete SPIFFS Files</h1>";
  output += "<form method='post' action='/delete-file'>";
  output += "<table border='1'><tr><th>Select</th><th>Name</th><th>Size</th></tr>";
  
  if(root.isDirectory()){
    File file = root.openNextFile();
    while(file){
      if(!file.isDirectory()){
        String fileName = String(file.name());
        // Make sure the value includes the leading slash in the filename
        output += "<tr><td><input type='checkbox' name='files' value='" + fileName + "'></td>";
        output += "<td>" + fileName + "</td><td>" + String(file.size()) + " bytes</td></tr>";
      }
      file = root.openNextFile();
    }
  }
  
  output += "</table><br>";
  output += "<input type='submit' value='Delete Selected Files'>";
  output += "</form>";
  output += "<br><a href='/'>Back to Home</a></body></html>";
  server.send(200, "text/html", output);
}

void handleDeleteFile() {
  int deletedCount = 0;
  
  // Check if there are files to delete
  if (server.args() > 0) {
    // Loop through all arguments in the form submission
    for (int i = 0; i < server.args(); i++) {
      // Check if the argument name is "files" (our checkbox name)
      if (server.argName(i) == "files") {
        String fileName = server.arg(i);
        
        // Ensure filename has a leading slash for SPIFFS
        if (!fileName.startsWith("/")) {
          fileName = "/" + fileName;
        }
        
        Serial.printf("Attempting to delete file: %s\n", fileName.c_str());
        
        // Make sure the file exists before attempting to delete
        if (SPIFFS.exists(fileName)) {
          if (SPIFFS.remove(fileName)) {
            deletedCount++;
            Serial.printf("Successfully deleted file: %s\n", fileName.c_str());
          } else {
            Serial.printf("Failed to delete file: %s\n", fileName.c_str());
          }
        } else {
          Serial.printf("File does not exist: %s\n", fileName.c_str());
        }
      }
    }
  }
  
  String output = "<html><head><title>Files Deleted</title>";
  output += "<meta http-equiv='refresh' content='2;url=/delete'></head><body>";
  output += "<h1>File Deletion Results</h1>";
  
  if (deletedCount > 0) {
    output += "<p>" + String(deletedCount) + " file(s) deleted successfully.</p>";
  } else {
    output += "<p>No files were deleted. Either none were selected or an error occurred.</p>";
    output += "<p>Check the Serial Monitor for details.</p>";
  }
  
  output += "<p>Redirecting back to file list...</p>";
  output += "<p><a href='/delete'>Back to Delete Files</a></p>";
  output += "</body></html>";
  
  server.send(200, "text/html", output);
}

// Handle download page
void handleDownloadPage() {
  String path = "/";
  File root = SPIFFS.open(path);
  
  String output = "<html><head><title>Download Files</title></head><body>";
  output += "<h1>Download SPIFFS Files</h1>";
  output += "<table border='1'><tr><th>Name</th><th>Size</th><th>Action</th></tr>";
  
  if(root.isDirectory()){
    File file = root.openNextFile();
    while(file){
      if(!file.isDirectory()){
        String fileName = String(file.name());
        output += "<tr><td>" + fileName + "</td>";
        output += "<td>" + String(file.size()) + " bytes</td>";
        output += "<td><a href='/download-file?name=" + fileName + "'>Download</a></td></tr>";
      }
      file = root.openNextFile();
    }
  }
  
  output += "</table><br>";
  output += "<a href='/'>Back to Home</a></body></html>";
  server.send(200, "text/html", output);
}

// Handle file download
void handleFileDownload() {
  if(server.hasArg("name")) {
    String fileName = server.arg("name");
    
    // Ensure filename has a leading slash for SPIFFS
    if (!fileName.startsWith("/")) {
      fileName = "/" + fileName;
    }
    
    Serial.print("Attempting to download file: ");
    Serial.println(fileName);
    
    if(SPIFFS.exists(fileName)) {
      File file = SPIFFS.open(fileName, "r");
      if(file) {
        // Get file size
        size_t fileSize = file.size();
        
        // Extract just the filename without the path
        String downloadName = fileName;
        if(fileName.startsWith("/")) {
          downloadName = fileName.substring(1);
        }
        
        Serial.print("File exists, size: ");
        Serial.println(fileSize);
        
        // Use the ESP-specific API for sending raw data
        WiFiClient client = server.client();
        
        // Send HTTP headers first
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/octet-stream");
        client.println("Content-Disposition: attachment; filename=\"" + downloadName + "\"");
        client.println("Content-Length: " + String(fileSize));
        client.println("Connection: close");
        client.println();
        
        // Send the file in chunks to avoid memory issues with large files
        uint8_t buffer[1024];
        size_t bytesRead;
        size_t totalSent = 0;
        
        while(file.available()) {
          bytesRead = file.read(buffer, sizeof(buffer));
          if(bytesRead > 0) {
            client.write(buffer, bytesRead);
            totalSent += bytesRead;
            yield(); // Allow background tasks to run
          }
        }
        
        file.close();
        Serial.printf("File %s (%u bytes) downloaded successfully\n", fileName.c_str(), totalSent);
        return; // Important: return here to skip the send() call below
      } else {
        Serial.println("Failed to open file");
      }
    } else {
      Serial.println("File does not exist");
    }
    
    // If we get here, something went wrong
    server.send(404, "text/plain", "File not found or could not be opened");
  } else {
    server.send(400, "text/plain", "Missing name parameter");
    Serial.println("Missing name parameter");
  }
}