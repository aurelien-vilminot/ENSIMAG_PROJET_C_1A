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
        if (widget->placer_params == NULL) {
                // At init, there is no placer, so it must be allocated
                widget->placer_params = calloc(1, sizeof(ei_placer_params_t));
                ei_placer_params_t *struct_placer = widget->placer_params;

                // Fill structure, data fields are filled with a default value if data concerned is NULL
                if (anchor) {
                        struct_placer->anchor = anchor;
                        struct_placer->anchor_data = *anchor;
                } else {
                        struct_placer->anchor_data = ei_anc_northwest;
                        struct_placer->anchor = &struct_placer->anchor_data;
                }

                if (x) {
                        struct_placer->x = x;
                        struct_placer->x_data = *x;
                } else {
                        struct_placer->x_data = 0;
                        struct_placer->x = &struct_placer->x_data;
                }

                if (y) {
                        struct_placer->y = y;
                        struct_placer->y_data = *y;
                } else {
                        struct_placer->y_data = 0;
                        struct_placer->y = &struct_placer->y_data;
                }

                if (width) {
                        struct_placer->w = width;
                        struct_placer->w_data = *width;
                } else {
                        if (widget->requested_size.width) {
                                struct_placer->w_data= widget->requested_size.width;
                                struct_placer->w = &widget->requested_size.width;
                        } else {
                                struct_placer->w_data = 0;
                                struct_placer->w = &struct_placer->w_data;
                        }
                }

                if (height) {
                        struct_placer->h = height;
                        struct_placer->h_data = *height;
                } else {
                        if (widget->requested_size.height) {
                                struct_placer->h_data = widget->requested_size.height;
                                struct_placer->h = &widget->requested_size.height;
                        } else {
                                struct_placer->h_data = 0;
                                struct_placer->h = &struct_placer->h_data;
                        }
                }

                if (rel_x) {
                        struct_placer->rx = rel_x;
                        struct_placer->rx_data = *rel_x;
                } else {
                        struct_placer->rx_data = 0.0;
                        struct_placer->rx = &struct_placer->rx_data;
                }

                if (rel_y) {
                        struct_placer->ry = rel_y;
                        struct_placer->ry_data = *rel_y;
                } else {
                        struct_placer->ry_data = 0.0;
                        struct_placer->ry = &struct_placer->ry_data;
                }

                if (rel_width) {
                        struct_placer->rw = rel_width;
                        struct_placer->rw_data = *rel_width;
                } else {
                        struct_placer->rw_data = 0.0;
                        struct_placer->rw = &struct_placer->rw_data;
                }

                if (rel_height) {
                        struct_placer->rh= rel_height;
                        struct_placer->rh_data = *rel_height;
                } else {
                        struct_placer->rh_data = 0.0;
                        struct_placer->rh = &struct_placer->rh_data;
                }
        } else {
                // Case if placer already managed
                ei_placer_params_t *struct_placer = widget->placer_params;

                // Fill structure, data fields are replaced only if data concerned is not NULL
                if (anchor) {
                        struct_placer->anchor = anchor;
                        struct_placer->anchor_data = *anchor;
                }

                if (x) {
                        struct_placer->x = x;
                        struct_placer->x_data = *x;
                }

                if (y) {
                        struct_placer->y = y;
                        struct_placer->y_data = *y;
                }

                if (width) {
                        struct_placer->w = width;
                        struct_placer->w_data = *width;
                }

                if (height) {
                        struct_placer->h = height;
                        struct_placer->h_data = *height;
                }

                if (rel_x) {
                        struct_placer->rx = rel_x;
                        struct_placer->rx_data = *rel_x;
                }

                if (rel_y) {
                        struct_placer->ry = rel_y;
                        struct_placer->ry_data = *rel_y;
                }

                if (rel_width) {
                        struct_placer->rw = rel_width;
                        struct_placer->rw_data = *rel_width;
                }

                if (rel_height) {
                        struct_placer->rh= rel_height;
                        struct_placer->rh_data = *rel_height;
                }
        }
}

/**
 * \brief	Tells the placer to remove a widget from the screen and forget about it.
 *		Note: the widget is not destroyed and still exists in memory.
 *
 * @param	widget		The widget to remove from screen.
 */
void ei_placer_forget(struct ei_widget_t* widget) {
        // Delete the concerned widget in its children field parent
        ei_widget_t *parent = widget->parent;
        ei_widget_t *current_child = parent->children_head;

        if (current_child == widget) {
                // Case if the first child is the concerned widget, then remove it of the head
                parent->children_head = current_child->next_sibling;
        } else {
                // Browse the children linked list to delete the concerned child
                while (current_child->next_sibling != NULL) {
                        if (current_child->next_sibling == widget) {
                                ei_widget_t *to_suppr = current_child->next_sibling;
                                current_child->next_sibling = current_child->next_sibling->next_sibling;
                                free(to_suppr);
                        }
                        current_child = current_child->next_sibling;
                }
        }

        // Remove parent of the concerned widget
        widget->parent = NULL;

        // Delete and free struct placer
        widget->placer_params = NULL;
        free(widget->placer_params);
}