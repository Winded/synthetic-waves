#include <util.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void *util_reserve_item(void *reserve, int reserve_size, int item_array_size, int item_size)
{
    for(int i = 0; i < reserve_size; i++) {
        void *reserve_p = *(void*)((char*)reserve + i * sizeof(void*));
        if(!reserve_p) {
            reserve_p = malloc(item_array_size * item_size);
            memset(reserve_p, 0, item_array_size * item_size);
            *(void*)((char*)reserve + i * sizeof(void*)) = reserve_p;
        }
        for(int ii = 0; ii < item_array_size; ii++) {
            void *item_p = (char*)reserve_p + ii * item_size;
            char is_valid = *((char*)item_p);
            if(!is_valid) {
                *((char*)item_p) = 1;
                return item_p;
            }
        }
    }
    assert("Reserve full" && 0);
    return 0;
}
