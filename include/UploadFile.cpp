#include <iostream>
#include <fstream>
#include <string>
#include "Request.h"    // Hypothetical Request class
#include "Response.h"   // Hypothetical Response class

// Helper function to extract file data from the Request object
std::string extractFileFromRequest(const Request& req, const std::string& boundary) {
    std::string fileContent;
    
    // Assuming the Request object allows you to access the raw body content.
    std::string body = req.getBody();
    
    // Find the boundary that separates file parts in the multipart form data.
    size_t boundaryPos = body.find(boundary);
    if (boundaryPos == std::string::npos) {
        throw std::runtime_error("Boundary not found in the request body.");
    }

    // Start processing the multipart content.
    size_t startOfFile = body.find("\r\n\r\n", boundaryPos) + 4;
    size_t endOfFile = body.find("--" + boundary + "--", startOfFile);
    
    if (endOfFile == std::string::npos) {
        throw std::runtime_error("File content not found.");
    }

    // Extract file content between start and end.
    fileContent = body.substr(startOfFile, endOfFile - startOfFile);
    
    return fileContent;
}

// Main file upload handler
void handleFileUpload(const Request& req, Response& res) {
    try {
        // Extract the boundary from the Content-Type header of the request.
        std::string contentType = req.getHeader("Content-Type");
        size_t boundaryPos = contentType.find("boundary=");
        if (boundaryPos == std::string::npos) {
            res.setStatus(400);  // Bad Request
            res.send("Invalid content type. Boundary not found.");
            return;
        }
        // Extract the boundary string from Content-Type header.
        std::string boundary = contentType.substr(boundaryPos + 9);
        // Extract the file data from the request.
        std::string fileData = extractFileFromRequest(req, boundary);
        // Write the extracted file data to a file (assuming it's a simple text file for this example).
        std::ofstream outFile("uploaded_file.txt", std::ios::binary);
        outFile.write(fileData.c_str(), fileData.size());
        outFile.close();

        // Respond with a success message.
        res.setStatus(200);  // OK
        res.send("File uploaded successfully.");
    } catch (const std::exception& e) {
        // In case of any errors, respond with an error message.
        res.setStatus(500);  // Internal Server Error
        res.send(std::string("File upload failed: ") + e.what());
    }
}
