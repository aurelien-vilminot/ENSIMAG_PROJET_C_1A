#include <ei_widget.h>
#include "ei_placer.h"

/**
 * \brief	Configures the geometry of a widget using the "placer" geometry manager.
 *
 *		The placer computes a widget's geometry relative to its parent *content_rect*.
 *
 * 		If the widget was already managed by the "placer", then this calls simply updates
 *		the placer parameters: arguments that are not NULL replace previous values.
 *
 * 		When the arguments are passed as NULL, the placer uses default values (detailed in
 *		the argument descriptions below). If no size is provided (either absolute or
 *		relative), then either the requested size of the widget is used if one was provided,
 *		or the default size is used.
 *
 * @param	widget		The widget to place.
 * @param	anchor		How to anchor the widget to the position defined by the placer
 *				(defaults to ei_anc_northwest).
 * @param	x		The absolute x position of the widget (defaults to 0).
 * @param	y		The absolute y position of the widget (defaults to 0).
 * @param	width		The absolute width for the widget (defaults to the requested width or
 * 				the default width of the widget if rel_width is NULL, or 0 otherwise).
 * @param	height		The absolute height for the widget (defaults to the requested height or
 *				the default height of the widget if rel_height is NULL, or 0 otherwise).
 * @param	rel_x		The relative x position of the widget: 0.0 corresponds to the left
 *				side of the master, 1.0 to the right side (defaults to 0.0).
 * @param	rel_y		The relative y position of the widget: 0.0 corresponds to the top
 *				side of the master, 1.0 to the bottom side (defaults to 0.0).
 * @param	rel_width	The relative width of the widget: 0.0 corresponds to a width of 0,
 *				1.0 to the width of the master (defaults to 0.0).
 * @param	rel_height	The relative height of the widget: 0.0 corresponds to a height of 0,
 *				1.0 to the height of the master (defaults to 0.0).
 */
void		ei_place	(struct ei_widget_t*	widget,
                                     ei_anchor_t*		anchor,
                                     int*			x,
                                     int*			y,
                                     int*			width,
                                     int*			height,
                                     float*			rel_x,
                                     float*			rel_y,
                                     float*			rel_width,
                                     float*			rel_height){
        // Allocate placer structure
        // TODO : libérer placer param à la destruction
        widget->placer_params = calloc(1, sizeof(ei_placer_params_t));
        ei_placer_params_t * struct_placer = widget->placer_params;

        // Fill structure, data fields are filled only if the pointeur is not NULL
        struct_placer->anchor = anchor;
        if (anchor != NULL) struct_placer->anchor_data = *anchor;

        struct_placer->x = x;
        if (x != NULL) struct_placer->x_data = *x;

        struct_placer->y = y;
        if (y != NULL) struct_placer->y_data = *y;

        struct_placer->w = width;
        if (width != NULL) struct_placer->w_data = *width;

        struct_placer->h = height;
        if (height != NULL) struct_placer->h_data= *height;

        struct_placer->rx = rel_x;
        if (rel_x != NULL) struct_placer->rx_data= *rel_x;

        struct_placer->ry = rel_y;
        if (rel_y != NULL) struct_placer->ry_data = *rel_y;

        struct_placer->rw = rel_width;
        if (rel_width != NULL) struct_placer->rw_data = *rel_width;

        struct_placer->rh = rel_height;
        if (rel_height!= NULL) struct_placer->rh_data= *rel_height;
}