/*
 * cevent.c
 *
 *  Created on: 2016.9.24
 *      Author: cool
 */
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "cevent.h"
#include "util_cevent.h"

ep_event_handle* ep_cevent_create(uint32_t fdnum)
{
	ep_event_handle* handle = 0;
	if (fdnum <= 0) {
		goto errout;
	}
	UTILS_ALIGN_NUM_32(fdnum);
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
    int i = 0;
	assert(handle);
    for (; i < handle->event_data_size; i++){
        if (handle->event_data_map[i].fdtype == TYPE_TM){
            close(i);
        }
    }
	free(handle->event_data_map);
	close(handle->epfd);
	free(handle);
}

int ep_cevent_add(ep_event_handle* handle, uint32_t fd, uint32_t evt, FD_CBFUN fun, void *data)
{
	assert(handle);
	struct epoll_event event;
	int newsize = 0;
	if (fd > (handle->event_data_size - 1)){
		newsize = fd+1;
		UTILS_ALIGN_NUM_32(newsize);	
		if ((util_mem_realloc((void*)handle->event_data_map, newsize*sizeof(ep_event_data))) < 0){
			return -2; //realloc mem failed
		}
		handle->event_data_size = newsize;
	}
	event.events = evt;
	event.data.fd = fd;
	handle->event_data_map[fd].cb = fun;
	handle->event_data_map[fd].data = (void*)data;
    handle->event_data_map[fd].fdtype = TYPE_FD;
	return epoll_ctl(handle->epfd, EPOLL_CTL_ADD, fd, &event);
}

int ep_cevent_mod(ep_event_handle* handle, uint32_t fd, uint32_t evt)
{
	assert(handle);
	struct epoll_event event;
	event.events = evt;
	return epoll_ctl(handle->epfd, EPOLL_CTL_MOD, fd, &event);
}

int ep_cevent_del(ep_event_handle* handle, uint32_t fd)
{
	assert(handle);
	struct epoll_event event;  //BUG in kernel versions before 2.6.9
	return epoll_ctl(handle->epfd, EPOLL_CTL_DEL, fd, &event); 
}

static void timerfd_cb_fun(ep_event_handle* handle, void* data, uint32_t fd)
{
    uint64_t count = 0;
    int n = 0;
    if ((n = read(fd, &count, 8)) < 8){
        return; //BUG
    }
    handle->event_data_map[fd].cb(handle, fd,  data);
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
            if (handle->event_data_map[handle->events[i].data.fd].fdtype == TYPE_FD){
                handle->event_data_map[handle->events[i].data.fd].cb(handle, \
                handle->events[i].events,handle->event_data_map[handle->events[i].data.fd].data);
            }
            else if(handle->event_data_map[handle->events[i].data.fd].fdtype == TYPE_TM){
                timerfd_cb_fun(handle, handle->event_data_map[handle->events[i].data.fd].data,\
                  handle->events[i].data.fd);
            }
            else{
    
            }
		}
	}
	return 0;
}

inline void ep_cevent_stop(ep_event_handle* handle){
	assert(handle);
	handle->loop = 0;
}



