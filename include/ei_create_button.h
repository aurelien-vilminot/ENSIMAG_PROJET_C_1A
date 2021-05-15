#ifndef PROJETC_IG_BUTTON_H
#define PROJETC_IG_BUTTON_H

#include <stdint.h>
#include "ei_types.h"
#include "hw_interface.h"
#include "ei_widget.h"
#include "ei_utils.h"

/**
 * @brief	Represents the part of a rectangle which must be generated
 */
typedef enum {
        TOP             = 0,
        BOTTOM		= 1,
        FULL            = 2
} ei_part_frame;

/**
 * @brief       Generate a linked list of points which represent a bow.
 *              The linked list is malloc so it must be free by the user of this function.
 *
 * @param       center          The bow center represented as an ei_point_t.
 * @param       radius          The bow angle.
 * @param       begin_angle     The angle where the bow start.
 * @param       end_angle       The angle where the bow end.
 *
 * @return      The linked list of points.
 */
ei_linked_point_t*       arc                     (ei_point_t            center,
                                                 uint32_t               radius,
                                                 double_t               begin_angle,
                                                 double_t               end_angle);

/**
 * @brief       Generate a linked list of points which represent a rectangle with rounded edges.
 *              This function is usually used to create buttons.
 *
 * @param       rectangle           The rectangle (size and top-left position) which must be rounded.
 * @param       rounded_radius      The radius of roundings.
 * @param       part                The part of rectangle which must be generated.
 *                                  It could be, top (TOP), bottom(BOTTOM) or the entire rectangle (FULL).
 *
 * @return      The linked list of points.
 */
ei_linked_point_t*       rounded_frame           (ei_rect_t              rectangle,
                                                 uint32_t                rounded_radius,
                                                 ei_part_frame           part);

/**
 * @brief       Return the last point of a single linked list.
 *
 * @param       ll      The linked list.
 *
 * @return      The last node of the linked list ll.
 */
struct ei_linked_point_t * get_last_node(struct ei_linked_point_t *ll);



#endif //PROJETC_IG_BUTTON_H
