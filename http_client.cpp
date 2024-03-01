// author : Andrew Bare aob1
// Date: 2/23/2024
// Version: 1.0
// SysNet2: Http Client
// This program is a simple http client that takes a url and sends a get request to the server

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

string get_url(string url) {
    string host;
    string path;

    // Parse the user arg for url
    size_t begin = url.find("://");
    if (begin == string::npos)
    {
        begin = 0;
    }
    else
    {
        begin += 3;
    }
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        cerr << "Usage: " << argv[0] << " <url>" << endl;
        return 1;
    }

    string url = argv[1];
    string host;
    string path;
    string port = "80";

    // Parse the url
    size_t hostStart = url.find("://");
    if (hostStart == string::npos) 
    {
        hostStart = 0;
    } 
    else 
    {
        hostStart += 3;
    }
    size_t hostEnd = url.find("/", hostStart);
    if (hostEnd == string::npos) 
    {
        hostEnd = url.length();
        path = "/";
    } 
    else 
    {
        path = url.substr(hostEnd);
    }
    host = url.substr(hostStart, hostEnd - hostStart);

    // Get the server's address
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo *res;
    int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (status != 0) 
    {
        cerr << "Error getting address info: " << gai_strerror(status) << endl;
        return 1;
    }

    // Create a socket
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == -1) 
    {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    // Connect to the server
    status = connect(sock, res->ai_addr, res->ai_addrlen);
    if (status == -1) 
    {
        cerr << "Error connecting to server" << endl;
        return 1;
    }

    // Send the request
    string request = "GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
    status = send(sock, request.c_str(), request.length(), 0);
    if (status == -1) 
    {
        cerr << "Error sending request" << endl;
        return 1;
    }

    // Receive the response
    char buffer[1024];
    int bytesReceived;
    while ((bytesReceived = recv(sock, buffer, 1024, 0)) > 0) 
    {
        cout.write(buffer, bytesReceived);
    }

    // Close the socket
    close(sock);
    return 0;
}