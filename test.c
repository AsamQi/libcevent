#include <stdio.h>
#include "cevent.h"

char *str = "hello epoll";
int i = 0;
void fun(ep_event_handle* handle, uint32_t ev, void *p){
	puts((char*)p);
	i++;
	if (i == 10){
		ep_cevent_stop(handle);
	}
}
void tmfun(ep_event_handle* handle,uint32_t fd,  void *p){
    printf("fd:%lu\n", fd);
    i++;
    if (i == 15){
        ctimer_del(handle, fd);
		ep_cevent_stop(handle);
    }
}
int main()
{
	ep_event_handle* hand = ep_cevent_create(2);
	ep_cevent_add(hand, 1, EPOUT, fun, str);
	ep_cevent_loop(hand);
    ctimer_add(hand, 5000,tmfun, 0);
    ep_cevent_del(hand,1);
	ep_cevent_loop(hand);
    
	ep_cevent_destroy(hand);
	return 0;
}
