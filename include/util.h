/*
 * util.h
 *
 * Miscellaneous utilities
 * */

#ifndef util_h
#define util_h

/**
 * Reserve an item from an exanding reserve. An expanding reserve is an array of pointers pointing to item arrays.
 * Items should be structs with a char value as first parameter, which is 0 when the item is free,
 * or 1 if it is reserved. Expanding reserves are divided into several arrays so that their size can be increased without
 * reallocating existing items.
 */
void *util_reserve_item_exp(void **reserve, int reserve_size, int item_array_size, int item_size);

/**
 * Reserve an item from an array reserve. It works similarly to reserving from an expanding reserve, but is used
 * for a single fixed array of items instead.
 */
void *util_reserve_item_arr(void *reserve, int reserve_size, int item_size);

#endif // util_h
