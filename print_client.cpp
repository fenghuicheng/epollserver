#include "include.h"
#include <string.h>

int main()
{
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	QUIT_IF_FAIL(sockfd);

	struct sockaddr_in clientaddr;
	bzero(&clientaddr, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(2000);
	inet_pton(AF_INET, "127.0.0.1", &clientaddr.sin_addr);

	int value = connect(sockfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
	QUIT_IF_FAIL(value);

	const char* essay = "Remeber Red, hope is a good thing, maybe the best of things, and no good thing ever dies.\n";
	value = write(sockfd, essay, strlen(essay));
	return 0;
}
