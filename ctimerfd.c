#include <sys/timerfd.h>
#include <fcntl.h>
#include "cevent.h"
#define MSEC_TO_SEC 1000
#define MSEC_TO_NSEC 1000000 
static int fcntl_non_blocking(uint32_t fd);
int ctimer_add(ep_event_handle* handle, uint32_t timeout, TM_CBFUN cb, void *data)
{
    int fd = 0;
    struct itimerspec alm;
    if ((fd = timerfd_create(CLOCK_MONOTONIC, 0)) < 0){
        goto errout;
    }
    if ((fcntl_non_blocking(fd))< 0){
        goto free_fd;
    }
    alm.it_value.tv_sec = timeout / MSEC_TO_SEC;
    alm.it_value.tv_nsec = (timeout % MSEC_TO_SEC) * MSEC_TO_NSEC;
    alm.it_interval.tv_sec = alm.it_value.tv_sec;
    alm.it_interval.tv_nsec = alm.it_value.tv_nsec;
    if ((timerfd_settime(fd, 0, &alm, NULL)) < 0){
        goto free_fd;
    }
    if ((ep_cevent_add(handle, fd, EPOLLIN, (FD_CBFUN)cb, data)) < 0){
        goto free_fd;
    }
    handle->event_data_map[fd].fdtype = TYPE_TM;
    return fd;
free_fd:
    close(fd);
errout:
    return -1;
}

void ctimer_del(ep_event_handle* handle, uint32_t fd){
    ep_cevent_del(handle, fd);
    handle->event_data_map[fd].fdtype = TYPE_NU;
    close(fd);
}

static int fcntl_non_blocking(uint32_t fd){
    uint32_t flag = 0;
    if ((flag = fcntl(fd, F_GETFL, 0)) < 0){
        return -1;
    }
    flag |= O_NONBLOCK;
    if ((fcntl(fd, F_SETFL, flag)) < 0){
        return -1;
    }
    return 0;
}
