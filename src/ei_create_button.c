#include "ei_create_button.h"
#include "single_linked_list.h"

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
ei_linked_point_t*       arc                     (ei_point_t             center,
                                                 uint32_t               radius,
                                                 double_t               begin_angle,
                                                 double_t               end_angle) {
        // Init linked list
        ei_linked_point_t *ll = malloc(sizeof(ei_linked_point_t));
        ei_linked_point_t *head_list = ll;

        // Always the shortest path
        while (end_angle < begin_angle) {
                end_angle += 2*M_PI;
        }

        // Init first point of linked list
        ll->point.x = (int32_t) (center.x + radius * cos(begin_angle));
        ll->point.y = (int32_t) (center.y - radius * sin(begin_angle));
        ll->next = NULL;

        // Number points to calculate
        uint32_t nb_points = 20;

        for (uint32_t i = 1; i <= nb_points; ++i) {
                ei_linked_point_t *next_point = malloc(sizeof(ei_linked_point_t));

                // Calcul of the next point
                double_t angle = begin_angle + i * ((end_angle - begin_angle)/ nb_points);
                next_point->point.x = (int32_t) (center.x + radius * cos(angle));
                next_point->point.y = (int32_t) (center.y - radius * sin(angle));
                next_point->next = NULL;

                ll->next = next_point;
                ll = next_point;
        }
        return head_list;
}

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
                                                 uint32_t                  rounded_radius,
                                                  ei_part_frame           part) {
        int32_t x0 = rectangle.top_left.x;
        int32_t y0 = rectangle.top_left.y;

        uint32_t h;
        if (rectangle.size.height >= rectangle.size.width) {
                h = rectangle.size.width / 2;
        } else {
                h = rectangle.size.height / 2;
        }

        // Declare center var
        ei_point_t center;

        if (part != TOP) {
                // Case bottom and full

                // Bottom right corner
                center.x = x0 + rectangle.size.width - rounded_radius;
                center.y = y0 + rectangle.size.height - rounded_radius;
                ei_linked_point_t *ll_arc_bottom_right = arc(center, rounded_radius, (3*M_PI)/2, 0);
                ei_linked_point_t *end_point_bottom_right = get_last_node(ll_arc_bottom_right);

                if (part == FULL) {
                        // Top left corner
                        center.x = x0 + rounded_radius;
                        center.y = y0 + rounded_radius;
                        ei_linked_point_t *ll_arc_top_left = arc(center, rounded_radius, M_PI/2, M_PI);
                        ei_linked_point_t *end_point_top_left = get_last_node(ll_arc_top_left);

                        // Bottom left corner
                        center.x = x0 + rounded_radius;
                        center.y = y0 + rectangle.size.height - rounded_radius;
                        ei_linked_point_t *ll_arc_bottom_left = arc(center, rounded_radius, M_PI, (3*M_PI)/2);
                        ei_linked_point_t *end_point_bottom_left = get_last_node(ll_arc_bottom_left);

                        // Top right corner
                        center.x = x0 + rectangle.size.width - rounded_radius;
                        center.y = y0 + rounded_radius;
                        ei_linked_point_t *ll_arc_top_right = arc(center, rounded_radius, 0, M_PI/2);
                        ei_linked_point_t *end_point_top_right = get_last_node(ll_arc_top_right);

                        // Link each corner linked list
                        end_point_bottom_left->next = ll_arc_bottom_right;
                        end_point_bottom_right->next = ll_arc_top_right;
                        end_point_top_right->next = ll_arc_top_left;

                        // Link the last point to the first one
                        ei_linked_point_t *last_point = malloc(sizeof(ei_linked_point_t));
                        *last_point = *ll_arc_bottom_left;
                        last_point->next = NULL;
                        end_point_top_left->next = last_point;
                        return ll_arc_bottom_left;
                } else {
                        // Case bottom

                        // Mid bottom left corner
                        center.x = x0 + rounded_radius;
                        center.y = y0 + rectangle.size.height - rounded_radius;
                        ei_linked_point_t *ll_arc_mid_bottom_left = arc(center, rounded_radius, 5*M_PI/4, (3*M_PI)/2);
                        ei_linked_point_t *end_point_mid_bottom_left = get_last_node(ll_arc_mid_bottom_left);

                        // Mid top right corner
                        center.x = x0 + rectangle.size.width - rounded_radius;
                        center.y = y0 + rounded_radius;
                        ei_linked_point_t *ll_arc_mid_top_right = arc(center, rounded_radius, 0, M_PI/4);
                        ei_linked_point_t *end_point_mid_top_right = get_last_node(ll_arc_mid_top_right);

                        // Points in center
                        ei_linked_point_t *p_right = malloc(sizeof(ei_linked_point_t));
                        p_right->point.x = x0 + rectangle.size.width - h;
                        p_right->point.y = y0 + h;

                        ei_linked_point_t *p_left = malloc(sizeof(ei_linked_point_t));
                        p_left->point.x = x0 + h;
                        p_left->point.y = y0 + rectangle.size.height - h;

                        // Link each corner linked list
                        end_point_mid_bottom_left->next = ll_arc_bottom_right;
                        end_point_bottom_right->next = ll_arc_mid_top_right;
                        end_point_mid_top_right->next = p_right;
                        p_right->next = p_left;

                        // Link the last point to the first one
                        ei_linked_point_t *last_point = malloc(sizeof(ei_linked_point_t));
                        *last_point = *ll_arc_mid_bottom_left;
                        last_point->next = NULL;
                        p_left->next = last_point;
                        return ll_arc_mid_bottom_left;
                }

        } else {
                // Case top

                // Top left corner
                center.x = x0 + rounded_radius;
                center.y = y0 + rounded_radius;
                ei_linked_point_t *ll_arc_top_left = arc(center, rounded_radius, M_PI/2, M_PI);
                ei_linked_point_t *end_point_top_left = get_last_node(ll_arc_top_left);

                // Mid bottom left corner
                center.x = x0 + rounded_radius;
                center.y = y0 + rectangle.size.height - rounded_radius;
                ei_linked_point_t *ll_arc_mid_bottom_left = arc(center, rounded_radius, M_PI, (5*M_PI)/4);
                ei_linked_point_t *end_point_mid_bottom_left = get_last_node(ll_arc_mid_bottom_left);

                // Points in center
                ei_linked_point_t *p_left = malloc(sizeof(ei_linked_point_t));
                p_left->point.x = x0 + h;
                p_left->point.y = y0 + rectangle.size.height - h;

                ei_linked_point_t *p_right = malloc(sizeof(ei_linked_point_t));
                p_right->point.x = x0 + rectangle.size.width - h;
                p_right->point.y = y0 + h;

                // Mid top right corner
                center.x = x0 + rectangle.size.width - rounded_radius;
                center.y = y0 + rounded_radius;
                ei_linked_point_t *ll_arc_mid_top_right = arc(center, rounded_radius, M_PI/4, M_PI/2);
                ei_linked_point_t *end_point_mid_top_right = get_last_node(ll_arc_mid_top_right);

                // Link each corner linked list
                end_point_top_left->next = ll_arc_mid_bottom_left;
                end_point_mid_bottom_left->next = p_left;
                p_left->next = p_right;
                p_right->next = ll_arc_mid_top_right;

                // Link the last point to the first one
                ei_linked_point_t *last_point = malloc(sizeof(ei_linked_point_t));
                *last_point = *ll_arc_top_left;
                last_point->next = NULL;
                end_point_mid_top_right->next = last_point;
                return ll_arc_top_left;
        }
}

/**
 * @brief       Return the last point of a single linked list.
 *
 * @param       ll      The linked list.
 *
 * @return      The last node of the linked list ll.
 */
ei_linked_point_t * get_last_node(ei_linked_point_t *ll) {
        if (ll->next != NULL) {
                while (ll->next != NULL) {
                        ll = ll->next;
                }
        }
        return ll;
}

void free_list(ei_linked_point_t *lc) {
        ei_linked_point_t *next_node = lc;
        while (lc != NULL) {
                next_node = next_node->next;
                free(lc);
                lc = next_node;
        }
}

ei_linked_point_t *get_rectangle_list (ei_rect_t rectangle) {
        // Head of linked list which represent the top-left point
        ei_linked_point_t *ll = malloc((sizeof(ei_linked_point_t)));

        ei_linked_point_t *p2 = malloc(sizeof(ei_linked_point_t));
        ei_linked_point_t *p3 = malloc(sizeof(ei_linked_point_t));
        ei_linked_point_t *p4 = malloc(sizeof(ei_linked_point_t));
        ei_linked_point_t *last_point = malloc(sizeof(ei_linked_point_t));


        ll->point.x = rectangle.top_left.x;
        ll->point.y = rectangle.top_left.y;
        ll->next = p2;

        p2->point.x = rectangle.top_left.x + rectangle.size.width;
        p2->point.y = rectangle.top_left.y;
        p2->next = p3;

        p3->point.x = rectangle.top_left.x + rectangle.size.width;
        p3->point.y = rectangle.top_left.y + rectangle.size.height;
        p3->next = p4;

        p4->point.x = rectangle.top_left.x;
        p4->point.y = rectangle.top_left.y + rectangle.size.height;
        p4->next = last_point;

        last_point->point.x = ll->point.x;
        last_point->point.y = ll->point.y;
        last_point->next = NULL;

        return ll;
}

