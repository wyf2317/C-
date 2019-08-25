#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <event2/event.h>
#include <event2/bufferevent.h>

int main()
{
	struct event_base *base;
	base = event_base_new();
	const char *strMethod = event_base_get_method(base);
	printf("method:%s\n", strMethod);
	int y = event_base_dispatch(base);
	event_base_free(base);
	return 1;
}
