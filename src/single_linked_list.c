#include "single_linked_list.h"

/**
 * @brief       Insert a new side on tail of single linked list.
 *
 *
 * @param       ll          The linked list
 * @param       new_side    The new side
*/
void insert(struct side **ll, struct side *new_side) {
        // Backup the head of linked list
        struct side *first = *ll;

        if (*ll != NULL) {
                while ((*ll)->next != NULL) {
                        *ll = (*ll)->next;
                }
                // Update the tail
                (*ll)->next = new_side;
                *ll = first;
        } else {
                // If linked list is empty, fill it with the created side
                *ll = new_side;
        }
}

/**
 * @brief       Delete each side where the attribute ymax is equal to ymax param.
 *
 * @param       ll          The linked list
 * @param       ymax        An integer
 */
void delete(struct side **ll, int ymax) {
        // Backup the head of linked list
        struct side *first = *ll;

        if (*ll != NULL) {
                // Deletion of every node on head of linked list if they must be deleted
                while ((*ll)->ymax == ymax) {
                        struct side *to_suppr = *ll;
                        *ll = (*ll)->next;
                        free(to_suppr);
                        if (*ll == NULL) return;
                }

                // Browse the linked list to delete every node if they must be deleted
                while ((*ll)->next != NULL) {
                        first = *ll;
                        if (((*ll)->next)->ymax == ymax) {
                                struct side *to_suppr = (*ll)->next;
                                (*ll)->next = ((*ll)->next)->next;
                                free(to_suppr);
                        }
                        *ll = (*ll)->next;
                }
                *ll = first;
        }
}

/**
 * @@brief      Move each component of a source linked list on tail of destination linked list.
 *              At the end, the source linked list is empty.
 *
 * @param       ll1         The source linked list
 * @param       ll2         The destination linked list
 */
void move(struct side **ll1, struct side **ll2) {
        if (*ll1 != NULL) {
                if (*ll2 != NULL) {
                        // Get the tail of the destination linked list
                        struct side *first_ll2 = *ll2;
                        while ((*ll2)->next != NULL) {
                                *ll2 = (*ll2)->next;
                        }

                        // Insert the source linked list on tail of the destination linked list
                        (*ll2)->next = *ll1;
                        *ll2 = first_ll2;
                } else {
                        // If the destination linked list is empty, fill it with the source linked list
                        *ll2 = *ll1;
                }

                // Empty source linked list
                *ll1 = NULL;
        }
}

/**
 * @brief       Test if the array given in parameter is empty.
 *
 * @param       tc          The array
 * @param       size_tab    The size of tc array
 *
 * @return	A boolean: \ref EI_TRUE means that the array is empty,
 *		\ref EI_FALSE means it does not.
 */
int is_empty (struct side **tc, int size_tab) {
        for (uint32_t i = 0 ; i < size_tab ; ++i) {
                if (tc[i] != NULL) {
                        return EI_FALSE;
                }
        }
        return EI_TRUE;
}


/**
 * @brief       Sort linked list by attribut xymin with an insertion sort.
 *
 * @param       tc      The linked list
 */
void insertion_sort(struct side **tc) {
        // Node current will point to head
        struct side *current = *tc;

        // Temporary variables to swap
        struct side *index = NULL, *prec_current = NULL, *prec_index = NULL, *temp = NULL;

        if (*tc != NULL) {
                while (current != NULL) {
                        // Backup previous node of index
                        prec_index = current;
                        //Node index will point to node next to current
                        index = current->next;

                        while (index != NULL) {
                                //If current node's xymin is greater than index's node xymin, swap data between them
                                if (current->xymin > index->xymin) {
                                        // If we have to change the head of linked list
                                        if (prec_current == NULL) {
                                                temp = current->next;
                                                current->next = index->next;
                                                index->next = *tc;
                                                *tc = index;
                                        } else {
                                                temp = index->next;
                                                prec_current->next = index;
                                                index->next = current->next;
                                                prec_index->next = current;
                                                current->next = temp;
                                        }
                                }
                                // Update nodes
                                prec_index = index;
                                index = index->next;
                        }
                        // Update nodes
                        prec_current = current;
                        current = current->next;
                }
        }
}

/**
 * @brief       Return xymin attribute of the node placed in the position index of linked list.
 *
 * @param       tc      The linked list
 * @param       index   The place of concerned node in the linked list
 *
 * @return      An signed integer : the xymin attribute if the node placed in the position index of linked list exists,
 *              -1 if does not exist.
 */
int32_t get_xymin(struct side *tc, uint32_t index) {
        // Init the counter
        uint32_t counter = 0;

        while (tc != NULL) {
                if (counter == index) {
                        return tc->xymin;
                }
                ++counter;
                tc = tc->next;
        }
        return -1;
}

/**
 * @brief       Update xymin (intersection point) of the linked list with Bresenham algorithm.
 *
 * @param       ll      The linked list
 */
void set_xymin(struct side **ll) {
        // Backup the head of linked list
        struct side *first = *ll;

        while (*ll != NULL) {
                (*ll)->error += abs((*ll)->dx);
                uint32_t dx_signe = (*ll)->dx > 0 ? 1 : -1;

                if (2*(*ll)->error > abs((*ll)->dy)) {
                        if (abs((*ll)->dx) > (*ll)->dy) {
                                // Case near of horizontal
                                (*ll)->xymin += (uint32_t) (*ll)->inv_slope;
                        } else {
                                (*ll)->xymin += dx_signe;
                        }
                        (*ll)->error -= (*ll)->dy;
                }
                *ll = (*ll)->next;
        }
        *ll = first;
}
