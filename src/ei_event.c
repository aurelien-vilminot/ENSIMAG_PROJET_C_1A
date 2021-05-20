#include "event_manager.h"

/**
 * Sets the widget which is currently being manipulated by the user.
 *
 * @param	widget		The widget to declare as active, or NULL to declare
 *				that a widget is no more being manipulated.
 */
void ei_event_set_active_widget(ei_widget_t* widget){
        g_active_widget = widget;
}

/**
 * Returns the widget currently being manipulated by the user.
 *
 * @return			The widget currenlty being manipulated, or NULL.
 */
ei_widget_t* ei_event_get_active_widget(void){
        return g_active_widget;
}

static ei_bool_t is_in_rectangle(ei_rect_t rectangle, ei_point_t point){
        int x_min = rectangle.top_left.x;
        int y_min = rectangle.top_left.y;
        int x_max = x_min + rectangle.size.width;
        int y_max = y_min + rectangle.size.height;
        return point.x <= x_max && point.x >= x_min && point.y <= y_max && point.y >= y_min;
}

/**
 * @brief compute the widget concerned and call its callback
 * @param a situate event
 * @return a boolean, true if the event is treated
 */
ei_bool_t situate_event_callback(ei_event_t *event){

        if (ei_event_get_active_widget()){
                ei_event_get_active_widget()->wclass->handlefunc(ei_event_get_active_widget(), event);
        }
        // Parameters of the offscreen
        hw_surface_lock(offscreen);
        uint32_t *clicked_pixel = (uint32_t *) hw_surface_get_buffer(offscreen);
        ei_size_t offscreen_size = hw_surface_get_size(offscreen);

        // Compute memory location of clicked_pixel and put the its value in widget_id
        clicked_pixel += (offscreen_size.width * event->param.mouse.where.y) + event->param.mouse.where.x;
        uint32_t widget_id = *clicked_pixel;

        printf("%u\n", *clicked_pixel);
        // Test if the clicked pixel is in the root_frame
        ei_widget_t *widget_to_treat = root_frame;
        if (widget_to_treat->pick_id == widget_id){
                return widget_to_treat->wclass->handlefunc(widget_to_treat, event);
        }

        // Depth course of each widgets
        do {
                if (widget_to_treat->children_head) {
                        widget_to_treat = widget_to_treat->children_head;
                        if (widget_to_treat->pick_id == widget_id){
                                return widget_to_treat->wclass->handlefunc(widget_to_treat, event);
                        }
                } else {
                        while (widget_to_treat != root_frame && widget_to_treat->next_sibling == NULL) {
                                widget_to_treat = widget_to_treat->parent;
                        }

                        if (widget_to_treat->next_sibling) {
                                widget_to_treat = widget_to_treat->next_sibling;
                                if (widget_to_treat->pick_id == widget_id){
                                        return widget_to_treat->wclass->handlefunc(widget_to_treat, event);
                                }
                        }
                }
        } while (widget_to_treat != root_frame);

        // If no widget handle_function is treated, so has to call the default function
        return EI_FALSE;
}

/**
 * Intermediate functions
 */

void children_resizing(struct ei_widget_t* widget){
        ei_widget_t * widget_to_destroy = widget;

        // Depth course of each widgets
        do {
                if (widget_to_destroy->children_head) {

                        widget_to_destroy = widget_to_destroy->children_head;
                        ei_placer_run(widget_to_destroy);
                } else {
                        while (widget_to_destroy != widget && widget_to_destroy->next_sibling == NULL) {
                                widget_to_destroy = widget_to_destroy->parent;
                        }

                        if (widget_to_destroy->next_sibling) {
                                widget_to_destroy = widget_to_destroy->next_sibling;
                                ei_placer_run(widget_to_destroy);
                        }
                }
        } while (widget_to_destroy != widget);
}


/**
 * Handle functions
 */

ei_bool_t handle_top_level_function(struct ei_widget_t* widget,
                                    struct ei_event_t* event){
        // Cast the widget to treat itself
        ei_top_level_t *toplevel_widget = (ei_top_level_t *) widget;

        if (event->param.mouse.button == ei_mouse_button_left){
                if (event->type == ei_ev_mouse_buttondown){
                        if (is_in_rectangle(toplevel_widget->close_button->widget.screen_location, event->param.mouse.where)){
                                ei_widget_destroy(widget);
                                return EI_TRUE;
                        }

                        if (is_in_rectangle(*(toplevel_widget->resize_rect), event->param.mouse.where)){
                                ei_event_set_active_widget(widget);
                        }
                } else if (event->type == ei_ev_mouse_buttonup) {
                        printf("j'arrive ici");
                        ei_event_set_active_widget(NULL);
                } else if (event->type == ei_ev_mouse_move && ei_event_get_active_widget() == widget){
                        // We consider that a widget has always a min_size
                        int new_width = event->param.mouse.where.x -
                                toplevel_widget->widget.screen_location.top_left.x;
                        if ((toplevel_widget->resizable == ei_axis_x || toplevel_widget->resizable == ei_axis_both) &&
                                new_width >= toplevel_widget->min_size->width)
                        ei_place(widget, NULL, NULL, NULL, &new_width, NULL, NULL, NULL, NULL, NULL);

                        int new_height = event->param.mouse.where.y -
                                toplevel_widget->widget.screen_location.top_left.y;
                        if ((toplevel_widget->resizable == ei_axis_y || toplevel_widget->resizable == ei_axis_both) &&
                            new_height >= toplevel_widget->min_size->height)
                        ei_place(widget, NULL, NULL, NULL, NULL, &new_height, NULL, NULL, NULL, NULL);

                        children_resizing(widget);

                        return EI_TRUE;
                }
        } else if (event->param.mouse.button == ei_mouse_button_middle){
                return EI_FALSE;
        } else if (event->param.mouse.button == ei_mouse_button_right){
                return EI_FALSE;
        }

//        // Verify if mouse pointer is in resize rect
//        int xmax = toplevel_widget->resize_rect->top_left.x + toplevel_widget->resize_rect->size.width;
//        int ymax = toplevel_widget->resize_rect->top_left.y + toplevel_widget->resize_rect->size.height;
//        int xmin = toplevel_widget->resize_rect->top_left.x;
//        int ymin = toplevel_widget->resize_rect->top_left.y;
//        if (event->param.mouse.where.x >= xmin && event->param.mouse.where.x <= xmax && event->param.mouse.where.y <= ymax && event->param.mouse.where.y >= ymin){
//                if (event->type == ei_ev_mouse_move && ei_event_get_active_widget() == widget) {
//                                switch (toplevel_widget->resizable) {
//                                        case ei_axis_none:
//                                                break;
//                                        case ei_axis_x:
//                                                toplevel_widget->widget.screen_location.size.width =
//                                                        event->param.mouse.where.x -
//                                                        toplevel_widget->widget.screen_location.top_left.x;
//                                                break;
//                                        case ei_axis_y:
//                                                toplevel_widget->widget.screen_location.size.height =
//                                                        event->param.mouse.where.y -
//                                                        toplevel_widget->widget.screen_location.top_left.y;
//                                                break;
//                                        case ei_axis_both:
//                                                toplevel_widget->widget.screen_location.size.width =
//                                                        event->param.mouse.where.x -
//                                                        toplevel_widget->widget.screen_location.top_left.x;
//                                                toplevel_widget->widget.screen_location.size.height =
//                                                        event->param.mouse.where.y -
//                                                        toplevel_widget->widget.screen_location.top_left.y;
//                                                break;
//                                }
//                } else if (event->type == ei_ev_mouse_buttondown){
//                        if (event->param.mouse.button == ei_mouse_button_left){
//                                ei_event_set_active_widget(widget);
//                        }
//                } else if (event->type == ei_ev_mouse_buttonup && ei_event_get_active_widget() == widget) {
//                        if (event->param.mouse.button == ei_mouse_button_left) {
//                                ei_event_set_active_widget(NULL);
//                        }
//                }
//        }
//
//        return EI_TRUE;

        // THe widget hasn't been treated, so we return EI_FALSE to call the default handle fucntion
        return EI_FALSE;

}

ei_bool_t handle_button_function(struct ei_widget_t* widget,
                                 struct ei_event_t* event){
//
//        ei_top_level_t *button_widget = (ei_button_t *) widget;
//        if (event->type == ei_ev_mouse_buttondown){
//                if (button_widget->closable)
//
//
//        }
//
//
//
//
//


        return EI_FALSE;
}

ei_bool_t handle_frame_function(struct ei_widget_t* widget,
                                 struct ei_event_t* event){
        return EI_FALSE;
}


///**
// * @brief	A function that is called in response to an event that has not been processed
// *		by any widget.
// *
// * @param	event		The event containing all its parameters (type, etc.)
// *
// * @return			EI_TRUE if the function handled the event,
// *				EI_FALSE otherwise, in this case the event is dismissed.
// */
//typedef ei_bool_t		(*ei_default_handle_func_t)(struct ei_event_t* event){
//
//}
//
//
//
///**
// * Sets the function that must be called when an event has been received but no processed
// *	by any widget.
// *
// * @param	func		The event handling function.
// */
//void ei_event_set_default_handle_func(ei_default_handle_func_t func) {
//
//}
//
//
///**
// * Returns the function that must be called when an event has been received but no processed
// *	by any widget.
// *
// * @return			The address of the event handling function.
// */
//ei_default_handle_func_t ei_event_get_default_handle_func(void){
//
//}