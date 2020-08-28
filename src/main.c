#include "network.h"
#include "functions.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // close

#define BUFF_SIZE 1024*1024 // 1Mo
#define SEND_COUNT 1

char c = 0; // char to send, default is null byte

void* tcp_handler(void *);

int main(int argc, char* argv[]) {
  // args checks
  if(argc < 2) {
    printf("Missing port number. Usage : %s PORT [CHAR_TO_SEND]\n", argv[0]);
    return -1;
  }
  // get server port
  int port = get_int(argv[1], MIN_PORT-1);
  if(port < MIN_PORT || port > MAX_PORT) {
    printf("Invalid port number. Port must range from %d to %d.\n", MIN_PORT, MAX_PORT);
    return -2;
  }
  // get char to send
  if(argc > 2) {
    c = get_int(argv[2], c);
    printf("Char is set to %c.\n", c);
  }
  // open server
  int tcp_socket = tcp_serve(port);
  if(tcp_socket < 0) {
    printf("Error listening TCP socket.\n");
    return -3;
  }
  printf("Server listening on port %d.\n", port);
  // allocate variables
  int client_socket;
  struct sockaddr_in client_address = { 0 };
  int client_address_size = sizeof client_address;
  pthread_t client_thread;
  // server loop
  while(1) {
    // accept
    client_socket = accept(tcp_socket, (struct sockaddr *) &client_address, &client_address_size);
    if (client_socket == -1) {
       printf("Error accepting client.\nMake sure port %d is not in use.\n", port);
       return -4;
    }
    // check address size
    if (client_address_size != sizeof client_address) {
       printf("Wrong address size: %d.\n", client_address_size);
       continue;
    }
    // create thread
    printf("Connection accepted from ");
    print_addr(client_address.sin_addr.s_addr);
    printf(":%u.\n", client_address.sin_port);
    // TODO create structure with client_socket in case of high load?
    if(pthread_create(&client_thread, 0,  tcp_handler, (void*) &client_socket) != 0) {
        perror("could not create thread");
        continue;
    }
    // detach thrad, automatically free resources without join
    if(pthread_detach(client_thread) != 0) {
        perror("could not detach thread");
        continue;
    }
  }
  //close(tcp_socket);
  return 0;
}


void* tcp_handler(void *socket) {
  int client = *(int*)socket;
  char buffer[BUFF_SIZE];
  int i, r;

  // set buffer to send
  // can use memcpy
  memset(buffer, c, BUFF_SIZE);
  // send the buffer SEND_COUNT times
  for(i = 0; i < SEND_COUNT; i++) {
    r = send(client, buffer, BUFF_SIZE, 0);
    if(r != BUFF_SIZE) {
      printf("error sending buffer %d.", i);
      break;
    }
  }
  printf("Sent %d times the buffer, reading eventual input.\n", i);
  //* read eventual input
  do {
    r = recv(client, buffer, BUFF_SIZE, MSG_DONTWAIT);
  } while(r > 0);
  //*/
  printf("Read input, closing.\n");
  close(client);
  printf("Closed.\n");
}




