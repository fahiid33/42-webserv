# Webserv - An Advanced HTTP Server in C++ 98

![GitHub](https://img.shields.io/badge/C%2B%2B-98-blue)
![Status](https://img.shields.io/badge/Status-Under%20Development-orange)
![License](https://img.shields.io/badge/License-MIT-green)

## Description

Webserv is a powerful HTTP server written in C++ 98, inspired by popular web servers like Nginx. It is designed to provide efficient, non-blocking, and scalable web hosting capabilities. This server can serve fully static websites, handle HTTP methods (GET, POST, DELETE), support CGI for dynamic content, and much more. Webserv aims to be stress-tested and remains available under heavy loads.

## Features

- **Configuration File Support**: Webserv accepts a configuration file as an argument or uses a default path if none is provided. The configuration file allows you to define various server settings, route configurations, default error pages, and more.

- **Non-blocking I/O**: Webserv employs the `select()` system call for all I/O operations, ensuring non-blocking communication between clients and the server, including listening.

- **Compatibility**: Webserv is designed to be compatible with popular web browsers and adheres to HTTP standards to ensure seamless communication.

- **Accurate HTTP Response Status Codes**: The server provides accurate HTTP response status codes to enhance the reliability and consistency of the web application.

- **Default Error Pages**: In case no custom error pages are provided, Webserv includes default error pages to enhance user experience.

- **Static Website Hosting**: Webserv can serve fully static websites, allowing you to host static content with ease.

- **File Upload Support**: Clients can upload files to the server, facilitating various interactive web applications.

- **Stress-tested and High Availability**: Webserv is designed to be stress-tested and remains available under heavy loads to ensure a reliable web hosting experience.

- **Multiple Listening Ports**: Webserv can listen on multiple ports, and these ports can be configured in the configuration file.

- **Customizable Server Settings**: You can choose the host and port for each server and set up server names as needed.

- **Route Configuration**: Webserv allows you to configure routes with various settings, including accepted HTTP methods, HTTP redirection, directory listing, default files for directories, and more.

- **CGI Support**: Webserv supports CGI for dynamic content generation. The server runs the CGI with the requested file as the first argument and handles the CGI output correctly.

- **Cookies and Session Management**: Webserv provides support for cookies and session management to enable interactive and personalized web applications.

- **Multiple CGI Handling**: The server can handle multiple CGI types, such as php-CGI, Python, etc., allowing you to integrate various scripting languages seamlessly.

## Usage

To run Webserv, follow these steps:

1. Compile the source code using a C++98 compatible compiler.

2. Execute the compiled binary, providing a configuration file as an argument or using the default configuration path.

3. The server will start listening on the specified ports and will be ready to accept incoming HTTP requests.

For detailed instructions and examples of the configuration file format, please refer to the [Wiki](https://github.com/yourusername/webserv/wiki).
