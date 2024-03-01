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

tuple<string,string, string> get_url(string url) 
{
    string host;
    string path;
    string port = "80";
    cout << "url: " << url << endl;
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
    size_t end = url.find("/", begin);
    if ( end == string::npos )
    {
        path = "/";
    }
    else
    {
        path = url.substr(end, url.length()); // pull uri
    }
    host = url.substr(begin, end);
    size_t portStart = url.find(":", begin);
    // if we find a port number
    if ( portStart != string::npos )
    {
        port = url.substr(begin, path.front());
        cout << "2 ret" << endl;
        return make_tuple(host,path, port);
    }
    cout << "2 ret" << endl;
    return make_tuple(host, path, port);

}

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        cerr << "Usage: " << argv[0] << " <url>" << endl;
        return 1;
    }

    auto [url, path, port] = get_url(argv[1]);
    // get server ip
    addrinfo* server = new addrinfo();
    // memset(server, 0, sizeof(server));
    getaddrinfo(url.c_str(), port.c_str(), 0, &server);
    cout << "url: " << url << ":" << port << endl;
    cout << "ip : " << inet_ntoa( ((sockaddr_in*)&server)->sin_addr) <<":"<< ntohs( ((sockaddr_in*)&server)->sin_port) << endl;
    int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int status = connect( server_fd, (sockaddr*)&server->ai_addr, server->ai_addrlen );
    cout << "connecting to " << url << endl;
    if (server == nullptr)
    {
        cout << "failed to connect to " << url << endl;
    }
    string msg = "Hello world\n\n";
    char* buff[1024] = {0};
    write(server_fd, msg.c_str(), msg.length());
    read(server_fd, buff, 1024);
    cout << buff << endl;


    return 0;
}