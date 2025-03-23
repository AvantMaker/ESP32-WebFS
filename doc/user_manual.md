
# User Manual for ESP32-WebFS

## Introduction

ESP32-WebFS is a powerful tool designed to manage the SPIFFS (SPI Flash File System) on your ESP32 microcontroller via a web interface allowing you to upload, download, remove and view files stored in ESP32's SPI Flash Memory. This manual will guide you through setting up and using ESP32-WebFS for your IoT projects.

## Prerequisites

Before you begin, ensure you have:
- An ESP32 development board.
- The Arduino IDE installed with the ESP32 core.
- A basic understanding of Arduino and ESP32 programming.
- A Wi-Fi network for the ESP32 to connect to.

## Installation

1. **Clone the Repository:**
   Clone this repository to your local machine using Git:
   ```
   git clone https://github.com/AvantMaker/ESP32-WebFS.git
   ```

2. **Open in Arduino IDE:**
   Open the file `ESP32-WebFS.ino`  in your Arduino IDE. You can find this file in the `ESP32-WebFS` folder of the repository.

3. **Configure Wi-Fi:**
   Open the `ESP32-WebFS.ino` file and modify the `ssid` and `password` variables with your Wi-Fi network credentials.

4. **Upload the Code:**
After configuring your Wi-Fi credentials, select the correct board and port in the Arduino IDE and click the 'Upload' button.


## Usage

### Connecting to the ESP32

1. **Find the IP Address:**
Once the code is uploaded, the ESP32 will connect to your Wi-Fi network. Open the Arduino IDE's Serial Monitor (Tools > Serial Monitor) to see the IP address assigned to your ESP32.

2. **Access the Web Interface:**
Open a web browser on your computer or phone and navigate to the IP address you found in the Serial Monitor.

### Managing Files

#### File Operations

- **List Files:**
  Use the "View Files" link (`/list`) to see a list of files, their sizes, and links to view their contents.

- **Delete Files:**
  Use the "Delete Files" link (`/delete`) to see a list of files with checkboxes, allowing you to select files for deletion.

- **Download Files:**
  Use the "Download Files" link (`/download`) to see a list of files with download links.

- **Upload Files:**
  Use the upload form on the main page to upload new files to the ESP32's SPIFFS.

- **View File Contents:**
  You can view file contents using the `/file?name=filename` route, where `filename` is the name of the file you want to view.

## Troubleshooting

  * **Unable to connect to the IP address:**
      * Double-check your Wi-Fi credentials (`ssid` and `password`) in the `ESP32-WebFS.ino` file and ensure they are correct.
      * Verify that your ESP32 is successfully connected to your Wi-Fi network by checking the Serial Monitor for connection messages.
      * Ensure that your computer or phone is on the same Wi-Fi network as the ESP32.
      * Try restarting your ESP32.
  * **Issues with file upload:**
      * Make sure the file size does not exceed the available SPIFFS space on your ESP32.
      * Check the Serial Monitor for any error messages during the upload process.
      * Ensure your web browser supports file uploads.
  * **Issues with file download:**
      * Verify that the file you are trying to download exists in the SPIFFS.
      * Check your browser's download settings and ensure downloads are not being blocked.
      * Look at the Serial Monitor for any errors during the download attempt.
  * **Problems viewing file contents:**
      * This feature is primarily intended for text-based files. Binary files might not display correctly.
      * If the text file has a very large size, it might take some time to load or might cause the browser to become unresponsive.
      * Check the Serial Monitor for any errors when trying to read the file.
  * **Web interface not loading or behaving unexpectedly:**
      * Try refreshing the web page in your browser.
      * Clear your browser's cache and cookies and try again.
      * Check the Serial Monitor for any error messages from the ESP32 web server.
      * Ensure you are accessing the correct IP address of the ESP32.

---

This project is proudly brought to you by the team at **AvantMaker.com**.

Visit us at [AvantMaker.com](https://www.avantmaker.com) where we've crafted a comprehensive collection of Reference and Tutorial materials for the ESP32, a mighty microcontroller that powers countless IoT creations.