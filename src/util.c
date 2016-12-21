#include <util.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void *util_reserve_item_exp(void **reserve, int reserve_size, int item_array_size, int item_size)
{
    for(int i = 0; i < reserve_size; i++) {
        void *reserve_p = 0;
        memcpy(&reserve_p, &(reserve[i]), sizeof(void**));
        if(!reserve_p) {
            reserve_p = malloc(item_array_size * item_size);
            memset(reserve_p, 0, item_array_size * item_size);
            memcpy(&(reserve[i]), &reserve_p, sizeof(void**));
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

void *util_reserve_item_arr(void *reserve, int reserve_size, int item_size)
{
    char *resv = (char*)reserve;
    for(int i = 0; i < reserve_size; i++) {
        char *item = resv + i * item_size;
        char is_valid = *item;
        if(!is_valid) {
            *item = 1;
            return (void*)item;
        }
    }
}
