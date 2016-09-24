/*
 * ep_cevent.c
 *
 *  Created on: 2016Äê9ÔÂ24ÈÕ
 *      Author: cool
 */
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "ep_cevent.h"
#include "util_cevent.h"

ep_event_handle* ep_cevent_create(unsigned int fdnum)
{
	ep_event_handle* handle = 0;
	if (fdnum <= 0) {
		goto errout;
	}
	UTILS_ALIGN_NUM(fdnum);
	if ((handle = (ep_event_handle*)malloc(sizeof(ep_event_handle))) == 0){
		goto errout;
	}

	if ((handle->epfd = epoll_create(fdnum)) < 0){
		goto free_handle;
	}

	handle->event_data_size = fdnum;
	if ((handle->event_data_map = (ep_event_data*)malloc(sizeof(ep_event_data) *\
		handle->event_data_size)) == 0) {
		goto free_epfd;
	}

	handle->loop = 0;
	return handle;

free_eventmap:
	free(handle->event_data_map);
free_epfd:
	close(handle->epfd);
free_handle:
	free(handle);
errout:
	return 0;
}

void ep_cevent_destroy(ep_event_handle* handle)
{
	assert(handle);
	free(handle->event_data_map);
	close(handle->epfd);
	free(handle);
}

int ep_cevent_add(ep_event_handle* handle, int fd, unsigned int evt, CBFUN fun, void *data)
{
	assert(handle);
	struct epoll_event event;
	int newsize = 0;
	if (fd > (handle->event_data_size - 1)){
		newsize = fd+1;
		UTILS_ALIGN_NUM(newsize);	
		if ((util_mem_realloc((void*)handle->event_data_map, newsize*sizeof(ep_event_data))) < 0){
			return -2; //realloc mem failed
		}
		handle->event_data_size = newsize;
	}
	event.events = evt;
	event.data.fd = fd;
	handle->event_data_map[fd].cb = fun;
	handle->event_data_map[fd].data = (void*)data;
	return epoll_ctl(handle->epfd, EPOLL_CTL_ADD, fd, &event);
}

int ep_cevent_mod(ep_event_handle* handle, int fd, unsigned int evt)
{
	assert(handle);
	struct epoll_event event;
	event.events = evt;
	return epoll_ctl(handle->epfd, EPOLL_CTL_MOD, fd, &event);
}

int ep_cevent_del(ep_event_handle* handle, int fd)
{
	assert(handle);
	struct epoll_event event;  //BUG in kernel versions before 2.6.9
	return epoll_ctl(handle->epfd, EPOLL_CTL_DEL, fd, &event); 
}

int ep_cevent_loop(ep_event_handle* handle)
{
	int nfds = 0;
	int i = 0;
	handle->loop = 1;
	while(handle->loop){
		nfds = epoll_wait(handle->epfd, handle->events, EP_MAX_EVENTS, -1);

		if (nfds == -1){
			if (errno == EINTR){
				continue;
			}
			return -1;
		}
		for (i = 0; i < nfds; i++){
			handle->event_data_map[handle->events[i].data.fd].cb(handle, \
			 handle->events[i].events,handle->event_data_map[handle->events[i].data.fd].data);
		}
	}
	return 0;
}

inline void ep_cevent_stop(ep_event_handle* handle){
	assert(handle);
	handle->loop = 0;
}



