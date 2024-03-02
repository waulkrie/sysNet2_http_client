// author : Andrew Bare aob1
// Date: 2/23/2024
// Version: 1.0
// SysNet2: Http Client
// This program is a simple http client that takes a url and sends a get request to the server
// printing the response to the console

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <tuple>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

string get_headers(string str, string host)
{
    string ret;
    ret = "GET " + str + " HTTP/1.1\r\n";
    ret += "Host: " + host + "\r\n";
    ret += "Accept: text/html\r\n";
    ret += "User-Agent: SnailHtml\r\n";
    ret += "Connection: close\r\n";
    ret += "\r\n";
    return ret;
}

tuple<string, string, string> get_url(string url) 
{
    string host;
    string path;
    string port = "80";

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

    size_t port_start = url.find(":", begin);
    size_t path_start = url.find("/", begin);
    if ( path_start == string::npos )
    {
        path = "/";
    }
    else
    {
        path = url.substr(path_start); // pull uri
        host = url.substr(begin, path_start - begin); // pull host
    }

    // if we find a port number
    if ( port_start != string::npos && port_start < path_start )
    {
        port = url.substr(port_start + 1, path_start - port_start - 1);
        host = url.substr(begin, port_start - begin);
    }
    else
    {
        host = url.substr(begin, path_start - begin);
    }

    return make_tuple(host, path, port);

}

string get_ip4_or_ip6(string url)
{
    addrinfo* server = new addrinfo();
    getaddrinfo(url.c_str(), "80", 0, &server);
    char host[NI_MAXHOST];
    getnameinfo(server->ai_addr, server->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
    return string(host);
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        cerr << "Usage: " << argv[0] << " <url>" << endl;
        return 1;
    }

    auto [url, path, port] = get_url(argv[1]);
    cout << "url : " << url << endl;
    cout << "path: " << path << endl;
    cout << "port: " << port << endl;


    // get server ip
    addrinfo* server = new addrinfo();
    addrinfo hints = addrinfo();
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // memset(server, 0, sizeof(server));
    getaddrinfo(url.c_str(), port.c_str(), &hints, &server);
    // Correcting the IP address printing
    // After calling getaddrinfo and ensuring server is not nullptr:
    char host[NI_MAXHOST];
    getnameinfo(server->ai_addr, server->ai_addrlen, host, sizeof(host),
     NULL, 0, NI_NUMERICHOST);

    cout << "IP Address: " << host << endl;
    cout << "Address Family: " << server->ai_family << endl;
    cout << "url: " << url << ":" << port << endl;


    int server_fd = socket( server->ai_family, SOCK_STREAM, IPPROTO_TCP);
    cout << "server_fd: " << server_fd << endl;
    cout << "server->ai_family: " << server->ai_family << endl;
    cout << "server->ai_protocol: " << server->ai_protocol << endl;
    cout << "server->ai_socktype: " << server->ai_socktype << endl;

    int status = connect( server_fd, server->ai_addr, server->ai_addrlen );
    cout << "connecting to " << host << endl;
    if ( status == -1 )
    {
        cout << "failed to connect to " << url << endl;
        perror("connect");
    }
    freeaddrinfo(server);
    string msg = "Hello world\n\n";
    char* buff[1024] = {0};
    string headers = get_headers(path, url);

    // copy payload after headers
    memcpy(buff, headers.c_str(), headers.length());
    memcpy(buff + headers.length(), msg.c_str(), msg.length()); 
    printf("%s\n", buff);
    status = write(server_fd, buff, msg.length() +  headers.length());
    if (status < 0)
    {
        cout << "failed to write to server" << endl;
    }
    status = read(server_fd, buff, 1024);
    if (status < 0)
    {
        cout << "failed to read from server" << endl;
    }

    printf("%s\n", buff);
    cout << "closing connection" << endl;
    close(server_fd);

    return 0;
}