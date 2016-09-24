#include <stdio.h>
#include "ep_cevent.h"

char *str = "hello epoll";
int i = 0;
void fun(ep_event_handle* handle, int ev, void *p){
	puts((char*)p);
	i++;
	if (i == 10){
		ep_cevent_stop(handle);
	}
}
int main()
{
	ep_event_handle* hand = ep_cevent_create(2);
	ep_cevent_add(hand, 1, EPOUT, fun, str);
	ep_cevent_loop(hand);
	ep_cevent_destroy(hand);
	return 0;
}
