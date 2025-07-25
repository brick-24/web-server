# minimalistic HTTP web-server in C

A minimalistic HTTP server written in C that handles GET requests, serves files and supports MIME-type detection and logging

## Features
- Serve raw contents of files over HTTP GET requests
- Simple 404 handling
- MIME type support
- Requests and Output logging
- Handling Bad requests

## Overview

<img width="1448" height="1913" alt="image" src="https://github.com/user-attachments/assets/d9c61934-4cf3-44fc-b761-ded772c69bf8" />

## Build and Run
### Compile
```bash
gcc server.c -o server
```
### Run
``` bash
./server
```
Then visit [http://localhost:8080/index.html](http://localhost:8080/index.html)


## Example requests
```
GET /index.html HTTP/1.1
Host: localhost:8080
```

<!--
so far i've built a C HTTP server that send raw contents of a file in response to a basic HTTP GET request
simple 404 handling created
header logic for different mime types
requests and output logging
handling bad requests
-->
