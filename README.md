# ESP32-WebFS

[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Language](https://img.shields.io/badge/Language-Arduino-teal.svg)](https://www.arduino.cc/)
[![AvantMaker](https://img.shields.io/badge/By-AvantMaker-red.svg)](https://www.avantmaker.com)

This project is proudly brought to you by the team at **AvantMaker.com**.

Visit us at [AvantMaker.com](https://www.avantmaker.com) where we've crafted a comprehensive collection of Reference and Tutorial materials for the ESP32, a mighty microcontroller that powers countless IoT creations.

---
![ESP32-WebFS](https://avantmaker.com/wp-content/uploads/2025/03/Simplifying-SPIFFS-File-Management-for-Your-ESP32-Projects.jpg)

**Manage your ESP32's SPIFFS files via a web browser!**

This project provides a web-based interface to manage the SPIFFS (SPI Flash File System) on your ESP32. It enables you to upload, download, delete, and view files stored on your ESP32, making it easy to interact with your ESP32's file system wirelessly.

---
**In essence, this project sets up an ESP32 web server that allows you to:**

* **Upload files** from your computer to the ESP32's SPIFFS storage.
* **Download files** from the ESP32's SPIFFS to your computer.
* **Delete files** stored in the ESP32's SPIFFS.
* **List files** currently stored in the ESP32's SPIFFS.
* **View the contents** of text files directly in your browser.

This is particularly useful for IoT projects where you need to configure the ESP32 remotely, store data, or manage resources without a direct connection.

## Key Features

* **Web-based File Management:** Access and manage your ESP32's files through a user-friendly web interface.
* **File Upload:** Upload new files to the ESP32's SPIFFS using a web form.
* **File Download:** Download existing files from the ESP32 to your computer via direct links.
* **File Deletion:** Delete files from the ESP32's SPIFFS by selecting them in a list and submitting a form.
* **File Listing:** View a list of files stored in the SPIFFS, including their names and sizes.
* **File Viewing:** Display the content of text files directly in the browser.
* **Simple Setup:** Designed for easy configuration and deployment on your ESP32 within the Arduino IDE.

## Getting Started

1.  **Prerequisites:**

    * ESP32 development board.
    * Arduino IDE installed with ESP32 core.
    * Basic understanding of Arduino and ESP32 programming.
    * A Wi-Fi network for the ESP32 to connect to.
2.  **Installation:**

    * Clone this repository to your local machine:

        ```bash
        git clone https://github.com/AvantMaker/ESP32-WebFS.git
        ```

    * Open the file `ESP32-WebFS.ino`  in your Arduino IDE. You can find this file in the `ESP32-WebFS` folder of the repository.
    * **Configure Wi-Fi:** In the  `ESP32-WebFS.ino`, modify the `ssid` and `password` variables with your Wi-Fi network credentials.

## Usage

1.  **Connect to the ESP32:** Once the code is uploaded, the ESP32 will connect to your Wi-Fi network. Open the Arduino IDE's Serial Monitor to see the IP address assigned to your ESP32. The ESP32 will print its IP address to the serial monitor.
2.  **Access the Web Interface:** Open a web browser on your computer or phone and navigate to the IP address you found in the Serial Monitor.
3.  **Manage Files:**

    * The main page (`/`) provides links to list, delete, and download files, as well as an upload form.
    * Use the "View Files" link (`/list`) to see a list of files, their sizes, and links to view their contents.
    * Use the "Delete Files" link (`/delete`) to see a list of files with checkboxes, allowing you to select files for deletion.
    * Use the "Download Files" link (`/download`) to see a list of files with download links.
    * The file download route (`/download-file`) handles the downloading of individual files.
    * You can view file contents using the `/file?name=filename` route, where `filename` is the name of the file you want to view.

## Security Note

This project provides basic file management functionality and doesn't include advanced security features like authentication beyond connecting to your Wi-Fi network. Be mindful of the security implications when using this in a production environment or on a publicly accessible network.

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

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT). See the `LICENSE` file for more information.
