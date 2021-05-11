#include "ei_create_button.h"
#include "single_linked_list.h"

// TODO : commentaires
ei_linked_point_t*       arc                     (ei_point_t             center,
                                                 uint32_t               radius,
                                                 double_t               begin_angle,
                                                 double_t               end_angle) {
        // Init linked list
        struct ei_linked_point_t *ll = malloc(sizeof(struct ei_linked_point_t *));
        struct ei_linked_point_t *head_list = ll;

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
                struct ei_linked_point_t *next_point = malloc(sizeof(struct ei_linked_point_t *));

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

// TODO : commentaires
ei_linked_point_t*       rounded_frame           (ei_rect_t              rectangle,
                                                 uint32_t                  rounded_radius,
                                                  ei_part_frame           part) {
        // Init linked list
        struct ei_linked_point_t *ll = malloc(sizeof(struct ei_linked_point_t *));
        struct ei_linked_point_t *head_list = ll;

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
                struct ei_linked_point_t *ll_arc_bottom_right = arc(center, rounded_radius, (3*M_PI)/2, 0);
                struct ei_linked_point_t *end_point_bottom_right = get_last_node(ll_arc_bottom_right);

                if (part == FULL) {
                        // Top left corner
                        center.x = x0 + rounded_radius;
                        center.y = y0 + rounded_radius;
                        struct ei_linked_point_t *ll_arc_top_left = arc(center, rounded_radius, M_PI/2, M_PI);
                        struct ei_linked_point_t *end_point_top_left = get_last_node(ll_arc_top_left);

                        // Bottom left corner
                        center.x = x0 + rounded_radius;
                        center.y = y0 + rectangle.size.height - rounded_radius;
                        struct ei_linked_point_t *ll_arc_bottom_left = arc(center, rounded_radius, M_PI, (3*M_PI)/2);
                        struct ei_linked_point_t *end_point_bottom_left = get_last_node(ll_arc_bottom_left);

                        // Top right corner
                        center.x = x0 + rectangle.size.width - rounded_radius;
                        center.y = y0 + rounded_radius;
                        struct ei_linked_point_t *ll_arc_top_right = arc(center, rounded_radius, 0, M_PI/2);
                        struct ei_linked_point_t *end_point_top_right = get_last_node(ll_arc_top_right);

                        // Link each corner linked list
                        *ll = *ll_arc_bottom_left;
                        end_point_bottom_left->next = ll_arc_bottom_right;
                        end_point_bottom_right->next = ll_arc_top_right;
                        end_point_top_right->next = ll_arc_top_left;

                        // Link the last point to the first one
                        struct ei_linked_point_t *last_point = malloc(sizeof(struct ei_linked_point_t *));
                        *last_point = *ll_arc_bottom_left;
                        last_point->next = NULL;
                        end_point_top_left->next = last_point;
                } else {
                        // Case bottom

                        // Mid bottom left corner
                        center.x = x0 + rounded_radius;
                        center.y = y0 + rectangle.size.height - rounded_radius;
                        struct ei_linked_point_t *ll_arc_mid_bottom_left = arc(center, rounded_radius, 5*M_PI/4, (3*M_PI)/2);
                        struct ei_linked_point_t *end_point_mid_bottom_left = get_last_node(ll_arc_mid_bottom_left);

                        // Mid top right corner
                        center.x = x0 + rectangle.size.width - rounded_radius;
                        center.y = y0 + rounded_radius;
                        struct ei_linked_point_t *ll_arc_mid_top_right = arc(center, rounded_radius, 0, M_PI/4);
                        struct ei_linked_point_t *end_point_mid_top_right = get_last_node(ll_arc_mid_top_right);

                        // Points in center
                        struct ei_linked_point_t *p_right = malloc(sizeof(struct ei_linked_point_t *));
                        p_right->point.x = x0 + rectangle.size.width - h;
                        p_right->point.y = y0 + h;

                        struct ei_linked_point_t *p_left = malloc(sizeof(struct ei_linked_point_t *));
                        p_left->point.x = x0 + h;
                        p_left->point.y = y0 + rectangle.size.height - h;

                        // Link each corner linked list
                        *ll = *ll_arc_mid_bottom_left;
                        end_point_mid_bottom_left->next = ll_arc_bottom_right;
                        end_point_bottom_right->next = ll_arc_mid_top_right;
                        end_point_mid_top_right->next = p_right;
                        p_right->next = p_left;

                        // Link the last point to the first one
                        struct ei_linked_point_t *last_point = malloc(sizeof(struct ei_linked_point_t *));
                        *last_point = *ll_arc_mid_bottom_left;
                        last_point->next = NULL;
                        p_left->next = last_point;
                }

        } else {
                // Case top

                // Top left corner
                center.x = x0 + rounded_radius;
                center.y = y0 + rounded_radius;
                struct ei_linked_point_t *ll_arc_top_left = arc(center, rounded_radius, M_PI/2, M_PI);
                struct ei_linked_point_t *end_point_top_left = get_last_node(ll_arc_top_left);

                // Mid bottom left corner
                center.x = x0 + rounded_radius;
                center.y = y0 + rectangle.size.height - rounded_radius;
                struct ei_linked_point_t *ll_arc_mid_bottom_left = arc(center, rounded_radius, M_PI, (5*M_PI)/4);
                struct ei_linked_point_t *end_point_mid_bottom_left = get_last_node(ll_arc_mid_bottom_left);

                // Points in center
                struct ei_linked_point_t *p_left = malloc(sizeof(struct ei_linked_point_t *));
                p_left->point.x = x0 + h;
                p_left->point.y = y0 + rectangle.size.height - h;

                struct ei_linked_point_t *p_right = malloc(sizeof(struct ei_linked_point_t *));
                p_right->point.x = x0 + rectangle.size.width - h;
                p_right->point.y = y0 + h;

                // Mid top right corner
                center.x = x0 + rectangle.size.width - rounded_radius;
                center.y = y0 + rounded_radius;
                struct ei_linked_point_t *ll_arc_mid_top_right = arc(center, rounded_radius, M_PI/4, M_PI/2);
                struct ei_linked_point_t *end_point_mid_top_right = get_last_node(ll_arc_mid_top_right);

                // Link each corner linked list
                *ll = *ll_arc_top_left;
                end_point_top_left->next = ll_arc_mid_bottom_left;
                end_point_mid_bottom_left->next = p_left;
                p_left->next = p_right;
                p_right->next = ll_arc_mid_top_right;

                // Link the last point to the first one
                struct ei_linked_point_t *last_point = malloc(sizeof(struct ei_linked_point_t *));
                *last_point = *ll_arc_top_left;
                last_point->next = NULL;
                end_point_mid_top_right->next = last_point;
        }
        return head_list;
}

/*
 * Return the last point of a single linked list
 */
struct ei_linked_point_t * get_last_node(struct ei_linked_point_t *ll) {
        if (ll != NULL) {
                while (ll->next != NULL) {
                        ll = ll->next;
                }
        }
        return ll;
}