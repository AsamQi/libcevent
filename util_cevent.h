/*
 * util_cevent.h
 *
 *  Created on: 2016.9.24
 *      Author: cool
 */

#ifndef UTIL_CEVENT_H_
#define UTIL_CEVENT_H_

# ifdef __cplusplus
}
# endif

#define UTILS_ALIGN_NUM_32(n)  do{\
		n -= 1;\
		n = n | (n >> 16);\
		n = n | (n >> 8);\
		n = n | (n >> 4);\
		n = n | (n >> 2);\
		n = n | (n >> 1);\
		n += 1;\
}while(0)

# ifdef __cplusplus
}
# endif
#endif /* UTIL_CEVENT_H_ */
