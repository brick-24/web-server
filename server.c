#include <sys/socket.h>      // For socket(), bind(), listen(), accept()
#include <netinet/in.h>      // For sockaddr_in
#include <string.h>          // For memset(), strchr()
#include <fcntl.h>           // For open()
#include <sys/sendfile.h>    // For sendfile()
#include <unistd.h>          // For read(), write(), close()
#include <sys/stat.h>
#include <stdio.h>           // For snfprintf()
#include <arpa/inet.h>       // For inet_ntoa()

const char* get_mime_type(const char* ext) {
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0)  return "text/css";
    if (strcmp(ext, ".js") == 0)   return "application/javascript";
    if (strcmp(ext, ".png") == 0)  return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0)  return "image/gif";
    if (strcmp(ext, ".txt") == 0)  return "text/plain";
    else return "404";
}

int main() {
    // 1. Create a TCP socket (IPv4, stream-oriented)
    int s = socket(AF_INET, SOCK_STREAM, 0); // returns a socket file descriptor

    // 2. Create and configure the server address
    struct sockaddr_in addr = {
        AF_INET,          // Address family: IPv4
        0x901f,           // Port number: 0x901f = 8080 (in hex, little endian)
        0                 // INADDR_ANY (0.0.0.0) — accept any incoming address
    };

    // 3. Bind the socket to the IP and port
    bind(s, (struct sockaddr*)&addr, sizeof(addr)); // cast is required in C

    // 4. Set the socket to listen for incoming connections
    listen(s, 10); // backlog = 10

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        // accept a client on socket s, gives client_addr with IP and port
        int client_fd = accept(s, (struct sockaddr*)&client_addr, &addr_len);

        char* client_ip = inet_ntoa(client_addr.sin_addr);
        printf("Client connected from IP: %s\n", client_ip);

        
        // 6. Read the HTTP request into a buffer
        char buffer[256] = {0};
        recv(client_fd, buffer, 256, 0); // store the HTTP request into buffer

        printf("Request: %s\n\n", buffer);
        //"GET /no.html HTTP/1.1"
        
        
        // 7. Extract the requested file path from the GET request
        if (strncmp(buffer, "GET /", 5) != 0) {
            printf("Bad Request");
            char* bad_req = "HTTP/1.1 400 Bad Request\r\n";
            send(client_fd, bad_req, strlen(bad_req), 0);
            continue;
        }
        
        char* f = buffer + 5;            // skip "GET /" (5 bytes)
        char* file_ext = strrchr(f, '.');
        *strchr(f, ' ') = 0;             // terminate the string at the first space
        printf("Requested file: %s\n\n", f);

        // f now contains the filename like "index.html" (no leading '/')
        
        // 8. Open the file requested
        int opened_fd = open(f, O_RDONLY); // read-only file descriptor

        // check if file actually exists for 404
        if (opened_fd == -1) {
            // if file does NOT exist
            printf("Response: 404 Not Found (0 bytes)\n\n");
            char* not_found_response =
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<html><body><h1>404 Not Found</h1></body></html>\n";

            send(client_fd, not_found_response, strlen(not_found_response), 0);
        } else {
            // if file DOES exist
            
            struct stat st;
            fstat(opened_fd, &st); // file stats
            
            printf("Response: 200 OK (%ld bytes)\n\n", st.st_size);
            // this is what is sent to the client as a HTTP header
            // char* ok_header =
            // "HTTP/1.1 200 OK\r\n"
            // "Content-Type: text/html\r\n"
            // "Connection: close\r\n"
            // "\r\n";
        
            char header[512];
            snprintf(header, sizeof(header),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: %s\r\n"
                "Content-Length: %ld\r\n"
                "Connection: close\r\n"
                "\r\n",
                get_mime_type(file_ext), st.st_size);


            send(client_fd, header, strlen(header), 0);
        
            // 9. Send file contents to client (does NOT include HTTP headers!)    
            sendfile(client_fd, opened_fd, 0, st.st_size); // just sends raw file bytes

        }

        if (opened_fd != -1){
            close(opened_fd);
        }
        close(client_fd); // clean up connection
    }
    // 10. Close all file descriptors and sockets
    close(s);
}


