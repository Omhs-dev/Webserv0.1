#include <poll.h>
#include <stdio.h>

int main(void)
{
	int sock1 = 0;
	int sock2 = 1;
	int sock3 = 2;

	struct pollfd fds[3];
	
	fds[0].fd = sock1;
	fds[0].events = POLLOUT;
	fds[1].fd = sock2;
	fds[1].events = POLLOUT;
	fds[2].fd = sock3;
	fds[2].events = POLLOUT;
	
	int result = poll(fds, 3, 5000);
	
	if (result > 0)
	{
		for (int i = 0; i < 3; i++)
		{
			if (fds[i].revents & POLLIN)
			{
				printf("an event occurs here\n");
				return (0);
			}
			else if (fds[i].revents & POLLOUT)
				printf("there is a pollout here : %d\n", i);
		}
	}
	return (0);
}