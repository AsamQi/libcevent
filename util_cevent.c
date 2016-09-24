#include <stdlib.h>
#include <assert.h>

int util_mem_realloc(void* src, unsigned int size)
{
	void *tmp = 0;
	assert(src);
	
	//free be better
	if (size == 0){
		return -1;		
	}

	if ((tmp = realloc(src, size)) == 0){
		return -1;
	}
		
	if (src != tmp){
		src = tmp;
	}
	return 0;
}
