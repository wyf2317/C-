#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>

#include <event2/event.h>
#include <event2/bufferevent.h>

// 发送心跳包
void do_timeout(evutil_socket_t fd, short event, void *arg)
{
	char buf[] = "tick tick!!!!!\n";
	printf("%s\n", buf);
}

// 添加定时器
void create_timer(struct timeval tv, struct event *pTimeEvent, struct event_base *pBase_ev)
{
	printf("have a new connect!\n");
	if (pTimeEvent == NULL)
	{
		pTimeEvent = event_new(pBase_ev, -1, EV_PERSIST, do_timeout, NULL);
		if (pTimeEvent == NULL)
		{
			perror("fun create_timer event_new failed!!!\n");
			return ;
		}
	}
	event_add(pTimeEvent, &tv);

}

void do_read(evutil_socket_t fd, short event, void *arg)
{
	char buf[1024];
	int len;
	if ((len = recv(fd, buf, 1024, 0)) > 0)
	{
		buf[len] = '\0';
		printf("recv:%s\n", buf);
		if (send(fd, buf, len, 0) < 0)
		{
			printf("send msg error\n");
		}
	}
}

void do_accept(evutil_socket_t fd, short event, void *arg)
{
	int client_sockerfd;
	struct sockaddr_in client_addr;
	socklen_t in_size = sizeof(struct sockaddr_in);
	client_sockerfd = accept(fd, (struct sockaddr *)&client_addr, &in_size);

	if (client_sockerfd < 0)
	{
		printf("accept error\n");
		return;
	}

	struct event_base *base_ev = (struct event_base *)arg;

	const char *msg = "hello,world\n";
	int size = send(client_sockerfd, msg, strlen(msg), 0);

	struct event *ev;
	// EV_TIMEOUT:定时事件
	// EV_READ/EV_WRITE:I/O事件
	// EV_PERSIST: 永久事件
	ev = event_new(base_ev, client_sockerfd, EV_TIMEOUT | EV_READ | EV_PERSIST, do_read, base_ev);
	event_add(ev, NULL);

	struct timeval tvTime;
	tvTime.tv_sec = 5;
	tvTime.tv_usec = 0;
	create_timer(tvTime, NULL, base_ev);
}

void quit(evutil_socket_t fd, short event, void *arg)
{	
	printf("recv sig:%d\n", SIGINT);
	exit(0);
}

int main()
{	
	int server_socketfd;
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8001);

	server_socketfd = socket(PF_INET, SOCK_STREAM, 0);
	if (server_socketfd < 0)
	{
		printf("create socket error");
		return -1;
	}

	evutil_make_listen_socket_reuseable(server_socketfd); // 设置端口重用
	evutil_make_socket_nonblocking(server_socketfd); // 设置无阻塞

	if (bind(server_socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("bind error\n");
		return -1;
	}
 
	listen(server_socketfd, 10);

	struct event_base *base_ev;
	base_ev = event_base_new();
	struct event *ev;

	// 最后一个参数将base_ev传入do_accept函数内
	ev = event_new(base_ev, server_socketfd, EV_TIMEOUT | EV_READ| EV_PERSIST, do_accept, base_ev);
	event_add(ev, NULL);

	// 设置关闭信号
	struct event *sigEv = evsignal_new(base_ev, SIGINT, quit, NULL);
	event_add(sigEv, NULL);

	event_base_dispatch(base_ev);

	event_base_free(base_ev);
	return 1;
}