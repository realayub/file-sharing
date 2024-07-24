# File-Sharing

In my latest project, I've developed a C socket programming solution that enables clients to upload and download files to and from a server. This application supports various file types, including images, videos, and text documents. One client can upload files to the server, making them accessible for other clients to download.

## Commands

```bash
gcc server.c -o server_name
./server

gcc client.c -o client_name
./client ip_address upload file_name
./client ip_address download file_name
