/*
 * ep_cevent.h
 *
 *  Created on: 2016.9.24
 *      Author: cool
 */

#ifndef EP_CEVENT_H_
#define EP_CEVENT_H_
#include <sys/epoll.h>


# ifdef __cplusplus
extern "C" {
# endif
#define EP_MAX_EVENTS 1024
typedef enum{
	EPIN = EPOLLIN,
	EPOUT =EPOLLOUT,
	EPRDHUP = EPOLLRDHUP,
	EPPRI = EPOLLPRI,
	EPERR = EPOLLERR,
	EPHUP = EPOLLHUP,
	EPET = EPOLLET,
	EPONCE = EPOLLONESHOT
}EVENT;

struct _ep_event_handle;
typedef struct {
	void (*cb) (struct _ep_event_handle*, int, void*);
	void *data;
}ep_event_data;

typedef struct _ep_event_handle {
	int epfd;
	int loop;
	ep_event_data *event_data_map;
	struct epoll_event events[EP_MAX_EVENTS];
	unsigned int event_data_size;
}ep_event_handle;

//callback fun
typedef void (*CBFUN) (ep_event_handle*, int, void*);

//handle
ep_event_handle* ep_cevent_create(unsigned int fdnum);
void ep_cevent_destroy(ep_event_handle* handle);

//event 
int ep_cevent_add(ep_event_handle* handle, int fd, unsigned int evt, CBFUN fun, void *data);
int ep_cevent_mod(ep_event_handle* handle, int fd, unsigned int evt);
int ep_cevent_del(ep_event_handle* handle, int fd);

//loop
int ep_cevent_loop(ep_event_handle* handle);
inline void ep_cevent_stop(ep_event_handle* handle);

# ifdef __cplusplus
}
# endif
#endif /* EP_CEVENT_H_ */
