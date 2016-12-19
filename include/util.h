/*
 * util.h
 *
 * Miscellaneous utilities
 * */

#ifndef util_h
#define util_h

/**
 * Reserve an item from a reserve. A reserve is an array of pointers pointing to item arrays.
 * Items should be structs with a char value as first parameter, which is 0 when the item is free,
 * or 1 if it is reserved.
 */
void *util_reserve_item(void *reserve, int reserve_size, int item_array_size, int item_size);

#endif // util_h
