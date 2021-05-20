#ifndef PROJETC_IG_SINGLE_LINKED_LIST_H
#define PROJETC_IG_SINGLE_LINKED_LIST_H

#include <stdint.h>
#include "ei_types.h"

/**
 * @brief       Side structure used to represent all caracteristic of a side in a polygon. It is a single linked list.
 **/
typedef struct side{
        uint32_t ymax;
        uint32_t xymin;
        double_t inv_slope;

        int32_t error;          ///< Error used for Bresenham Algorithm
        int32_t dx, dy;

        struct side* next;
} side;

/*
 * Insertion of new_side on tail of single linked list ll
*/
extern void insert(struct side **ll, struct side *new_side);

/*
 * Deletion of each side where ymax == ymax
 */
extern void delete(struct side **ll, int ymax);

/*
 * Move each component of linked list ll1 on tail of linked list ll2
 */
extern void move(struct side **ll1, struct side **ll2);

/*
 * Test if the tab tc is empty
 */
extern int is_empty (struct side **tc, int size_tab);

/*
 * Sort linked list by abscisse with an insertion sort
 */
extern void insertion_sort(struct side **tc);

/*
 * Return xymin elem of the node placed in the position index of linked list
 */
extern int32_t get_xymin(struct side *tc, uint32_t index);

/*
 * Update xymin (intersection point) of linked list with Bresenham algorithm
 */
extern void set_xymin(struct side **ll);

#endif //PROJETC_IG_SINGLE_LINKED_LIST_H
