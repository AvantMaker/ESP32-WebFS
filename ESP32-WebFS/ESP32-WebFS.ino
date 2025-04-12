/*
* Author: Avant Maker
* Website: www.AvantMaker.com
* Date: April 11, 2025
* Version: 0.25
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

const char* ssid = "your_SSID";          // Replace with your Wi-Fi SSID
const char* password = "your_PASSWORD";  // Replace with your Wi-Fi password
WebServer server(80);
File uploadFile;

// Common technical information for all pages
const String technicalInfo = "<div class='info-card'>"
                             "<h2>Technical Information</h2>"
                             "<p>ESP32 SPIFFS is a flat file system that supports a limited number of files due to memory constraints.</p>"
                             "<p>Limitations:</p>"
                             "<ul>"
                             "<li>Maximum of 32 characters for filenames (including path)</li>"
                             "<li>No support for directories (although '/' can be used in filenames to simulate directories)</li>"
                             "<li>Limited space depending on your ESP32 flash partition scheme</li>"
                             "</ul>"
                             "</div>";

// Common CSS for all pages
const String commonCSS = "<style>"
                          "body { font-family: Arial, sans-serif; margin: 20px; }"
                          ".progress-bar { background-color: #f0f0f0; border-radius: 4px; height: 25px; margin: 10px 0; }"
                          ".progress-fill { background-color: #4CAF50; height: 100%; border-radius: 4px; text-align: center; line-height: 25px; color: white; }"
                          ".info-card { border: 1px solid #ddd; border-radius: 8px; padding: 20px; margin-bottom: 20px; background-color: #f9f9f9; }"
                          "table { border-collapse: collapse; width: 100%; }"
                          "th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }"
                          "th { background-color: #f2f2f2; }"
                          "</style>";

// Common footer for all pages
const String siteFooter = "<hr><p>This project is proudly brought to you by the team at <a href='https://www.AvantMaker.com'>AvantMaker.com</a>.</p>";

void setup() {
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
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
    String html = "<html><head><title>ESP32 WebFS</title>";
    html += commonCSS;
    html += "</head><body>";
    html += "<h1>ESP32 WebFS</h1>";
    html += "<p>ESP32 WebFS provides a web-based interface to manage the SPIFFS on your ESP32. </p>";
    html += "<p><a href='/list'>View Files</a></p>";
    html += "<p><a href='/delete'>Delete Files</a></p>";
    html += "<p><a href='/download'>Download Files</a></p>";
    html += "<p><a href='/fs-info'>View SPIFFS Information</a></p>";
    html += "<h2>Upload New File</h2>";
    html += "<strong>Note:</strong> Only files smaller than 300 KB can be uploaded. Attempting to upload larger files may cause the system to freeze.";
    html += "<form method='post' action='/upload' enctype='multipart/form-data' onsubmit='return validateForm()'>";
    html += "<input type='file' name='upload' id='fileInput' onchange='checkFileSelected()'>";
    html += "<input type='submit' value='Upload' id='uploadButton' disabled>";
    html += "</form>";
    html += "<script>";
    html += "function checkFileSelected() {";
    html += "  var fileInput = document.getElementById('fileInput');";
    html += "  var uploadButton = document.getElementById('uploadButton');";
    html += "  if(fileInput.files.length > 0) {";
    html += "    uploadButton.disabled = false;";
    html += "  } else {";
    html += "    uploadButton.disabled = true;";
    html += "  }";
    html += "}";
    html += "function validateForm() {";
    html += "  var fileInput = document.getElementById('fileInput');";
    html += "  if(fileInput.files.length === 0) {";
    html += "    alert('Please select a file to upload first.');";
    html += "    return false;";
    html += "  }";
    html += "  return true;";
    html += "}";
    html += "</script>";
    html += technicalInfo;
    html += siteFooter;
    html += "</body></html>";
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

  // Route to display SPIFFS file system information
  server.on("/fs-info", HTTP_GET, handleFSInfo);

  // Add handler for viewing file contents
  server.on("/file", HTTP_GET, []() {
    if (server.hasArg("name")) {
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

      if (SPIFFS.exists(fileName)) {
        // Determine content type based on file extension
        String contentType = "text/plain";
        if (fileName.endsWith(".htm") || fileName.endsWith(".html")) contentType = "text/html";
        else if (fileName.endsWith(".css")) contentType = "text/css";
        else if (fileName.endsWith(".js")) contentType = "application/javascript";
        else if (fileName.endsWith(".png")) contentType = "image/png";
        else if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) contentType = "image/jpeg";
        else if (fileName.endsWith(".gif")) contentType = "image/gif";
        else if (fileName.endsWith(".ico")) contentType = "image/x-icon";
        else if (fileName.endsWith(".xml")) contentType = "text/xml";
        else if (fileName.endsWith(".pdf")) contentType = "application/pdf";
        else if (fileName.endsWith(".zip")) contentType = "application/zip";
        else if (fileName.endsWith(".json")) contentType = "application/json";

        File file = SPIFFS.open(fileName, "r");
        if (file) {
          // For text files, you can use readString
          if (contentType.startsWith("text/") ||
              contentType == "application/javascript" ||
              contentType == "application/json") {
            String content = file.readString();
            file.close();
            server.send(200, contentType, content);
          }
          // For binary files, use the client directly
          else {
            WiFiClient client = server.client();
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: " + contentType);
            client.println("Connection: close");
            client.println();

            // Send file in chunks
            uint8_t buffer[1024];
            size_t bytesRead;
            while ((bytesRead = file.read(buffer, sizeof(buffer))) > 0) {
              client.write(buffer, bytesRead);
              yield(); // Allow background tasks
            }
            file.close();
            return; // Skip the send() as we've already sent the response
          }
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

// Function to handle displaying SPIFFS file system information
void handleFSInfo() {
  // Get SPIFFS information
  size_t totalBytes = SPIFFS.totalBytes();
  size_t usedBytes = SPIFFS.usedBytes();
  size_t freeBytes = totalBytes - usedBytes;

  // Calculate usage percentages
  float usagePercent = (float)usedBytes / (float)totalBytes * 100.0;
  float freePercent = 100.0 - usagePercent;

  // Count total files in SPIFFS
  int fileCount = 0;
  File root = SPIFFS.open("/");
  if (root.isDirectory()) {
    File file = root.openNextFile();
    while (file) {
      if (!file.isDirectory()) {
        fileCount++;
      }
      file = root.openNextFile();
    }
  }

  // Format the HTML output
  String html = "<html><head><title>SPIFFS Information</title>";
  html += commonCSS;
  html += "</head><body>";
  html += "<h1>SPIFFS File System Information</h1>";

  // Storage utilization card
  html += "<div class='info-card'>";
  html += "<h2>Storage Utilization</h2>";
  html += "<div class='progress-bar'>";
  html += "<div class='progress-fill' style='width: " + String(usagePercent) + "%;'>" + String(usagePercent, 1) + "%</div>";
  html += "</div>";

  // Storage information table
  html += "<table>";
  html += "<tr><th>Metric</th><th>Value</th></tr>";
  html += "<tr><td>Total Space</td><td>" + formatBytes(totalBytes) + "</td></tr>";
  html += "<tr><td>Used Space</td><td>" + formatBytes(usedBytes) + " (" + String(usagePercent, 1) + "%)</td></tr>";
  html += "<tr><td>Free Space</td><td>" + formatBytes(freeBytes) + " (" + String(freePercent, 1) + "%)</td></tr>";
  html += "<tr><td>Total Files</td><td>" + String(fileCount) + "</td></tr>";
  html += "</table>";
  html += "</div>";

  // Technical information card - now using the common variable
  html += technicalInfo;
  html += "<p><a href='/'>Back to Home</a></p>";
  html += siteFooter;
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Helper function to format bytes to KB, MB, etc.
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + " B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0, 2) + " KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0, 2) + " MB";
  }
  return String(bytes) + " B";
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
  String output = "<html><head><title>ESP32 File List</title>";
  output += commonCSS;
  output += "</head><body>";
  output += "<h1>SPIFFS File List</h1>";

  // Adding the note about viewable file types
  output += "<div class='info-card'>";
  output += "<strong>Note:</strong> The following file types have been tested and confirmed viewable in browser: ";
  output += "<ul>";
  output += "<li>Text files (.txt, .html, .css, .js, .json)</li>";
  output += "<li>Images (.jpg, .png, .jpeg, .gif)</li>";
  output += "<li>PDF (viewable if your browser supports displaying it)</li>";
  output += "</ul>";
  output += "Other file types may not display properly in the browser and might be downloaded instead.";
  output += "</div>";

  output += "<table border='1'><tr><th>Name</th><th>Size</th><th>Actions</th></tr>";
  if (root.isDirectory()) {
    File file = root.openNextFile();
    while (file) {
      if (!file.isDirectory()) {
        // Get the filename with the leading slash
        String fileName = String(file.name());
        output += "<tr><td>" + fileName + "</td><td>" + String(file.size()) + " bytes</td>";
        // Pass the full filename including the slash in the URL
        output += "<td><a href='/file?name=" + fileName + "'>View</a></td></tr>";
      }
      file = root.openNextFile();
    }
  }
  output += "</table><br><a href='/'>Back to Home</a>";

  // Add technical information before footer
  output += technicalInfo;
  output += siteFooter;
  output += "</body></html>";

  server.send(200, "text/html", output);
}

void handleDeletePage() {
  String path = "/";
  File root = SPIFFS.open(path);
  String output = "<html><head><title>Delete Files</title>";
  output += commonCSS;
  output += "</head><body>";
  output += "<h1>Delete SPIFFS Files</h1>";
  output += "<form method='post' action='/delete-file'>";
  output += "<table border='1'><tr><th>Select</th><th>Name</th><th>Size</th></tr>";
  if (root.isDirectory()) {
    File file = root.openNextFile();
    while (file) {
      if (!file.isDirectory()) {
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
  output += "<br><a href='/'>Back to Home</a>";

  // Add technical information before footer
  output += technicalInfo;
  output += siteFooter;
  output += "</body></html>";

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
  output += commonCSS;
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

  // Add technical information before footer
  output += technicalInfo;
  output += siteFooter;
  output += "</body></html>";

  server.send(200, "text/html", output);
}

// Handle download page
void handleDownloadPage() {
  String path = "/";
  File root = SPIFFS.open(path);
  String output = "<html><head><title>Download Files</title>";
  output += commonCSS;
  output += "</head><body>";
  output += "<h1>Download SPIFFS Files</h1>";
  output += "<table border='1'><tr><th>Name</th><th>Size</th><th>Action</th></tr>";
  if (root.isDirectory()) {
    File file = root.openNextFile();
    while (file) {
      if (!file.isDirectory()) {
        String fileName = String(file.name());
        output += "<tr><td>" + fileName + "</td>";
        output += "<td>" + String(file.size()) + " bytes</td>";
        output += "<td><a href='/download-file?name=" + fileName + "'>Download</a></td></tr>";
      }
      file = root.openNextFile();
    }
  }
  output += "</table><br>";
  output += "<a href='/'>Back to Home</a>";

  // Add technical information before footer
  output += technicalInfo;
  output += siteFooter;
  output += "</body></html>";

  server.send(200, "text/html", output);
}

// Handle file download
void handleFileDownload() {
  if (server.hasArg("name")) {
    String fileName = server.arg("name");

    // Ensure filename has a leading slash for SPIFFS
    if (!fileName.startsWith("/")) {
      fileName = "/" + fileName;
    }

    Serial.print("Attempting to download file: ");
    Serial.println(fileName);

    if (SPIFFS.exists(fileName)) {
      File file = SPIFFS.open(fileName, "r");
      if (file) {
        // Get file size
        size_t fileSize = file.size();

        // Extract just the filename without the path
        String downloadName = fileName;
        if (fileName.startsWith("/")) {
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
        while (file.available()) {
          bytesRead = file.read(buffer, sizeof(buffer));
          if (bytesRead > 0) {
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