#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]) {
	
	struct addrinfo hints;
	struct addrinfo *servinfo;  // will point to the results

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	// Get the port and address
	char *server_addr = getenv("SERVER ADDRESS");
	char *server_port = getenv("SERVER PORT");

	// Get all the info needed
	int status;
	status = getaddrinfo(server_addr, server_port, &hints, &servinfo);
	if (status == -1) {
		cerr << "Error getting address info" << endl;
		exit(1);
	}

	// Setup and connect
	int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (sockfd == -1) {
		cerr << "Error getting socket file description" << endl;
		exit(1);
	}
	int c = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
	if (c == -1) {
		cerr << "Can not connect to server" << endl;
		exit(1);
	}
	
	// Get the string
	char *input;

	while (cin >> input) {
		// Pass the input
		int count = send(sockfd, input, strlen(input), 0);
		if (count == -1) {
			cerr << "Error sending the string" << endl;
			exit(1);
		}

		// Get the output
		int r;
		void *buf;
		r = recv(sockfd, buf, count, 0);
		if (r == -1) {
			cerr << "Error getting the returned string" << endl;
			exit(1);
		}

		// print out the out put
		cout << "Server: ";
		cout << buf << endl;

		// Sleep for 2 seconds
		sleep(2000);
	}
		
	close(sockfd);

	// Free teh servince info
	freeaddrinfo(servinfo);

	exit(0);
}
