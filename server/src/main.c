/*! \file main.c
 */

#include <asm-generic/socket.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT "2137"

int main(int argc, char *argv[]) {
    int status = 0;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *servinfo;

    status = getaddrinfo(NULL, PORT, &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "Error: %s\n", gai_strerror(status));
        exit(1);
    }

    int main_socket = socket(PF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    status = bind(main_socket, servinfo->ai_addr, servinfo->ai_addrlen);
    if (status == -1) {
        fprintf(stderr, "Bind error: %d\n", errno);
        exit(1);
    }

    status = listen(main_socket, 20);
    if (status == -1) {
        fprintf(stderr, "Bind error: %d\n", errno);
        exit(1);
    }

    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof their_addr;
    int their_file_descriptor;

    their_file_descriptor =
        accept(main_socket, (struct sockaddr *)&their_addr, &addr_size);
    if (status == -1) {
        fprintf(stderr, "Bind error: %d\n", errno);
        exit(1);
    }

    printf("accepted\n");

    char buf[1000];
    status = recv(their_file_descriptor, buf, 1000, 0);
    if (status == -1) {
        fprintf(stderr, "Bind error: %d\n", errno);
        exit(1);
    }

    printf("%s\n", buf);

    char *msg = "GET /24 HTTP/1.1\n"
                "Host: 192.168.1.249:2137\n"
                "User-Agent: curl/8.5.0\n"
                "Accept: */*\n"
                "Content-Length: 6\n"
                "Content-Type: application/x-www-form-urlencoded\n\n"
                "odpoweidz";
    printf("%s", msg);
    status = send(their_file_descriptor, msg, strlen(msg), 0);
    if (status == -1) {
        fprintf(stderr, "Bind error: %d\n", errno);
        exit(1);
    }

    freeaddrinfo(servinfo);
    return 0;
}
