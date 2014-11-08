/*
	* Editor. Serving your editor needs since today!
*/

/* Include POSIX sockets and friends */
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

/* POSIX standard I/O */
#include <stdio.h>

/* and of course, the POSIX standard library. Used for malloc(). */
#include <stdlib.h>

/* Needed for string manipulation and accessing errors */
#include <errno.h>
#include <string.h>

/* Rename enumerated variables for ease of understanding. */
enum {
	CONNECTION_LOCAL = AF_UNIX,
	CONNECTION_REMOTE = AF_INET6
};

struct ServerState {
	struct SocketState *
};

struct SocketState {
	int handle;
	int connection_type;
};

/*!
A user may create a socket that listens for local or remote connections.
@param[in] connection_type - Either CONNECTION_LOCAL or CONNECTION_REMOTE.
*/
struct SocketState * create_socket(int connection_type) {
	/* Since we'll be talking to the client and the client to us, we need to use SOCK_STREAM. */
	int socket_handle = socket(connection_type, SOCK_STREAM, 0);
	if(socket_handle == -1) {
		fprintf(stderr, "Socket error: %s\n", strerror(errno));
		return NULL;
	}
	
	struct SocketState *socket_state = malloc(sizeof(struct SocketState));
	socket_state->handle = socket_handle;
	socket_state->connection_type = connection_type;
	return socket_state;
}

/*!
Initialize the server.
*/
void initialize(int connection_type, unsigned int port) {
	struct SocketState *socket_state = create_socket(connection_type);
}

int main(int argc, char *argv[]) {
	struct ServerState *server_state = initialize(CONNECTION_LOCAL, 3017);
	event_loop(server_state);	
	return 0;
}
