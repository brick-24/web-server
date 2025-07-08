#include <sys/socket.h>      // For socket(), bind(), listen(), accept()
#include <netinet/in.h>      // For sockaddr_in
#include <string.h>          // For memset(), strchr()
#include <fcntl.h>           // For open()
#include <sys/sendfile.h>    // For sendfile()
#include <unistd.h>          // For read(), write(), close()

void main() {
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

    // 5. Accept one incoming client connection (blocks until a connection is made)
    int client_fd = accept(s, 0, 0);

    // 6. Read the HTTP request into a buffer
    char buffer[256] = {0};
    recv(client_fd, buffer, 256, 0); // store the HTTP request into buffer

    // Example buffer now likely contains: "GET /filename HTTP/1.1\r\n..."
    char* ok_header =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"
    "\r\n";

    send(client_fd, ok_header, strlen(ok_header), 0);


    // 7. Extract the requested file path from the GET request
    char* f = buffer + 5;            // skip "GET /" (5 bytes)
    *strchr(f, ' ') = 0;             // terminate the string at the first space

    // f now contains the filename like "index.html" (no leading '/')

    // 8. Open the file requested
    int opened_fd = open(f, O_RDONLY); // read-only file descriptor

    // 9. Send file contents to client (does NOT include HTTP headers!)
    sendfile(client_fd, opened_fd, 0, 256); // just sends raw file bytes

    // 10. Close all file descriptors and sockets
    close(opened_fd);
    close(client_fd);
    close(s);
}
