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

int main(int args, char *argv[]) {
	
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct sockaddr_in addr;

	// Get socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int status;
	status = getaddrinfo(INADDR_ANY, "0", &hints, &servinfo);
	//if (status == -1) {
	//	cerr << "Error getting address info" << endl;
	//	exit(1);
	//}
	int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	//if (sockfd == -1) {
	//	cerr << "Error getting socket file description" << endl;
	//	exit(1);
	//}
	bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
	
	// Print out the server address and port that it is binding on
        if (servinfo->ai_family == AF_INET) { // IPv4
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)servinfo->ai_addr;
                char str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(ipv4->sin_addr), str, INET_ADDRSTRLEN);
                cout << "SERVER_ADDRESS " << str << endl;
                cout << "SERVER_PORT " << ipv4->sin_port << endl;
        } else { // IPv6
                struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)servinfo->ai_addr;
                char str[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &(ipv6->sin6_addr), str, INET6_ADDRSTRLEN);
                cout << "SERVER_ADDRESS " << str << endl;
                cout << "SERVER_PORT " << ipv6->sin6_port << endl;
        }

	// Listen to all the socket	
	listen(sockfd, 5);

	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
    	int fdmax;        // maximum file descriptor number
	int newfd;        // newly accept()ed socket descriptor
    	struct sockaddr_storage remoteaddr; // client address
    	socklen_t addrlen;
	char buf[256];    // buffer for client data
    	int nbytes;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	fdmax = sockfd;

	FD_SET(sockfd, &master);

	for(;;) {
        	read_fds = master; // copy it
		select(fdmax+1, &read_fds, NULL, NULL, NULL);
       		//if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            	//	cerr << "Failed at select" << endl;
            	//	exit(1);
        	//}

        	// run through the existing connections looking for data to read
        	for(int i = 0; i <= fdmax; i++) {
            		if (FD_ISSET(i, &read_fds)) { // we got one!!
                		if (i == sockfd) {
                    			// handle new connections
                    			addrlen = sizeof remoteaddr;
                    			newfd = accept(sockfd,(struct sockaddr *)&remoteaddr,&addrlen);

                    			if (newfd == -1) {
                        			//cerr << "Can't accept new connect request" << endl;
						//exit(1);
                    			} else {
                        			FD_SET(newfd, &master); // add to master set
                        			if (newfd > fdmax) {    // keep track of the max
                            				fdmax = newfd;
                        			}
                        	
						// Do nothing ??????????????????????????????

                    			}
                		} else {
                    			// handle data from a client
					nbytes = recv(i, buf, sizeof buf, 0);
                    			if (nbytes <= 0) {
                        			// got error or connection closed by client
                        			//if (nbytes == 0) {
                            			//	cerr << "The connection is closed" << endl;
                        			//} else {
                            			//	cerr << "recv is not working properly" << endl;
                        			//}
                        			close(i); // bye!
                        			FD_CLR(i, &master); // remove from master set
                    			} else {
						if (i != sockfd) {
							// Send the respond
							for (int j = 0; j < nbytes; j++) {
								if (j == 0) {
									if ((buf[j] >= 'a') && (buf[j] <= 'z')) {
										buf[j] = buf[j] - ('a' - 'A');
									}
								} else if (buf[j-1] == ' ') {
									if ((buf[j] >= 'a') && (buf[j] <= 'z')) {
                                                                                buf[j] = buf[j] - ('a' - 'A');
                                                                        }
								} else {
									if ((buf[j] >= 'A') && (buf[j] <= 'Z')) {
                                                                                buf[j] = buf[j] + ('a' - 'A');
                                                                        }
								}
							}	
						}
						send(i, buf, nbytes, 0);
					}
				}
			}
		}
	}
}
