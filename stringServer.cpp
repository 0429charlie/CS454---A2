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
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in addr;
		
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
              cerr << "Error getting socket file description" << endl;
              exit(1);
        }

	serv_addr.sin_family = AF_INET;;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = 0;

	int status;
	status = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (status == -1) {
              cerr << "Error binding" << endl;
              exit(1);
        }

	// Print out the server address and port that it is binding on
	int addr_len = sizeof(addr);
	getsockname(sockfd, (struct sockaddr *)&addr, (socklen_t *)&addr_len);
        char str[INET_ADDRSTRLEN];
	gethostname(str, INET_ADDRSTRLEN);
        cout << "SERVER_ADDRESS " << str << endl;
        cout << "SERVER_PORT " << ntohs(addr.sin_port) << endl;


	// Listen to all the socket
        listen(sockfd, 5);
	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
    	int fdmax;        // maximum file descriptor number
	int newfd;        // newly accept()ed socket descriptor
    	struct sockaddr_storage remoteaddr; // client address
    	socklen_t addrlen;
	char buf[8192];    // buffer for client data
    	int nbytes;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	fdmax = sockfd;

	FD_SET(sockfd, &master);

	for(;;) {
        	read_fds = master;
		select(fdmax+1, &read_fds, NULL, NULL, NULL);
       		//if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            	//	cerr << "Failed at select" << endl;
            	//	exit(1);
        	//}

        	// run through the existing connections looking for data to read
        	for(int i = 0; i <= fdmax; i++) {
            		if (FD_ISSET(i, &read_fds)) { 
                		if (i == sockfd) {
                    			// handle new connections
                    			addrlen = sizeof(remoteaddr);
                    			newfd = accept(sockfd,(struct sockaddr *)&remoteaddr,&addrlen);

                    			if (newfd == -1) {
                        			//cerr << "Can't accept new connect request" << endl;
						//exit(1);
                    			} else {
                        			FD_SET(newfd, &master); // add to master set
                        			if (newfd > fdmax) {    // keep track of the max
                            				fdmax = newfd;
                        			}
                    			}
                		} else {
                    			// handle data from a client
					nbytes = recv(i, buf, sizeof(buf), 0);
                    			if (nbytes <= 0) {
                        			// got error or connection closed by client
                        			//if (nbytes == 0) {
                            			//	cerr << "The connection is closed" << endl;
                        			//} else {
                            			//	cerr << "recv error" << endl;
						//}
                        			close(i); // bye!
                        			FD_CLR(i, &master); // remove from master set
                    			} else {
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
						send(i, buf, nbytes, 0);
					}
				}
			}
		}
	}
}
