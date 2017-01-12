/*
 * cevent.h
 *
 *  Created on: 2016.9.24
 *      Author: cool
 */

#ifndef CEVENT_H_
#define CEVENT_H_
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

typedef enum{
    TYPE_NU,
    TYPE_FD,
    TYPE_TM    
}EVENT_TYPE;

struct _ep_event_handle;
typedef struct {
    EVENT_TYPE fdtype;
	void (*cb) (struct _ep_event_handle*, uint32_t, void*);
	void *data;
}ep_event_data;

typedef struct _ep_event_handle {
	int32_t epfd;
	uint32_t loop;
	uint32_t event_data_size;
	ep_event_data *event_data_map;
	struct epoll_event events[EP_MAX_EVENTS];
}ep_event_handle;

//callback fun
typedef void (*FD_CBFUN) (ep_event_handle*, uint32_t, void*);
typedef void (*TM_CBFUN) (ep_event_handle*, uint32_t, void*);

//handle
ep_event_handle* ep_cevent_create(uint32_t fdnum);
void ep_cevent_destroy(ep_event_handle* handle);

//event 
int ep_cevent_add(ep_event_handle* handle, uint32_t fd, uint32_t evt, FD_CBFUN fun, void *data);
int ep_cevent_mod(ep_event_handle* handle, uint32_t fd, uint32_t evt);
int ep_cevent_del(ep_event_handle* handle, uint32_t fd);

//loop
int ep_cevent_loop(ep_event_handle* handle);
inline void ep_cevent_stop(ep_event_handle* handle);

//timer
int ctimer_add(ep_event_handle* handle, uint32_t timeout, TM_CBFUN cb, void *data);
void ctimer_del(ep_event_handle* handle, uint32_t fd);

# ifdef __cplusplus
}
# endif
#endif /* CEVENT_H_ */
