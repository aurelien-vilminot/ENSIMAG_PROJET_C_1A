#ifndef PROJETC_IG_BUTTON_H
#define PROJETC_IG_BUTTON_H

#include <stdint.h>
#include "ei_types.h"
#include "hw_interface.h"
#include "ei_widget.h"

/**
 * @brief	//TODO : commenter
 */
typedef enum {
        TOP             = 0,
        BOTTOM		= 1,
        FULL            = 2
} ei_part_frame;

ei_linked_point_t*       arc                     (ei_point_t             center,
                                                 uint32_t               radius,
                                                 double_t               begin_angle,
                                                 double_t               end_angle);

ei_linked_point_t*       rounded_frame           (ei_rect_t              rectangle,
                                                 uint32_t                rounded_radius,
                                                 ei_part_frame           part);

struct ei_linked_point_t * get_last_node(struct ei_linked_point_t *ll);

void                    ei_draw_button          (ei_widget_t*	widget,
                                                 ei_surface_t		surface,
                                                 ei_surface_t		pick_surface,
                                                 ei_rect_t*		clipper);



#endif //PROJETC_IG_BUTTON_H
