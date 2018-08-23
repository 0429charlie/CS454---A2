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
#include <queue>
#include <list>
#include <pthread.h>

using namespace std;

// Global structure for both thread to put and get string from
queue<string> input_q;
bool getting;

void *getinput(void *arg) {
	string input;
	while (getline(cin, input)) {
		input_q.push(input);
	}
	getting = false;
	return NULL;
}


int main(int argc, char *argv[]) {


	struct hostent *server;
	struct sockaddr_in serv_addr;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
                cerr << "Error getting socket file description" << endl;
                exit(1);
        }

	char *server_addr = getenv("SERVER_ADDRESS");
        char *server_port = getenv("SERVER_PORT");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	server = gethostbyname(server_addr);
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(atoi(server_port));

	int c = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (c == -1) {
                cerr << "Can not connect to server" << endl;
                exit(1);
        }
	
	// Create Thread for reading input
	pthread_t tid;
	int rc;
	int arg;
	getting = true;
	rc = pthread_create(&tid, NULL, getinput, &arg);

	// sending, receiving input and printint output
	while ((getting == true) || (input_q.size() > 0)) {
		
		if (input_q.size() > 0) {
			string in;
			in = input_q.front();
			input_q.pop();
			// Send the input
              		int count = send(sockfd, in.c_str(), strlen(in.c_str())+1, 0);
              		if (count == -1) {
				cerr << "Error sending the string" << endl;
				exit(1);
              		}

			// Sleep for 2 seconds
			sleep(2);

              		// Get the output
              		int r;
              		char buf[count];
              		r = recv(sockfd, buf, count, 0);
              		if (r == -1) {
				cerr << "Error getting the returned string" << endl;
				exit(1);
			}

              		// print out the out put
			cout << in << endl;
              		cout << "Server: ";
              		cout << buf << endl;
		}
	}

	pthread_join(tid, NULL);

	close(sockfd);

	exit(0);
}
