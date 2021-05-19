#include "ei_event.h"
#include "event_manager.h"
#include "widget_manager.h"

/**
 * Sets the widget which is currently being manipulated by the user.
 *
 * @param	widget		The widget to declare as active, or NULL to declare
 *				that a widget is no more being manipulated.
 */
void ei_event_set_active_widget(ei_widget_t* widget){
        if (widget){
                *g_active_widget = *widget;
        } else {
                g_active_widget = NULL;
        }
}

/**
 * Returns the widget currently being manipulated by the user.
 *
 * @return			The widget currenlty being manipulated, or NULL.
 */
ei_widget_t* ei_event_get_active_widget(void){
        return g_active_widget;
}


/**
 * Callbacks
 */

/**
 * For now I set the active widget for the one which is under the click
 */
ei_bool_t mouse_callback(ei_event_t event){
        // Parameters of the offscreen
        hw_surface_lock(offscreen);
        uint32_t *pointed_pixel = (uint32_t *) hw_surface_get_buffer(offscreen);
        ei_size_t offscreen_size = hw_surface_get_size(offscreen);

        // What is the pointed pixel
        // May be I have to -1 the sum
        pointed_pixel += (offscreen_size.width * event.param.mouse.where.y) + event.param.mouse.where.x;
        uint32_t widget_id = *pointed_pixel;

        ei_widget_t *widget_to_treat = root_frame;

        // Depth course of each widgets
        do {
                if (widget_to_treat->children_head) {
                        widget_to_treat = widget_to_treat->children_head;
                        if (widget_to_treat->pick_id == widget_id){
                                widget_to_treat->wclass->handlefunc(widget_to_treat, &event);
                                return EI_TRUE;
                        }
                } else {
                        while (widget_to_treat != root_frame && widget_to_treat->next_sibling == NULL) {
                                widget_to_treat = widget_to_treat->parent;
                        }

                        if (widget_to_treat->next_sibling) {
                                widget_to_treat = widget_to_treat->next_sibling;
                                if (widget_to_treat->pick_id == widget_id){
                                        widget_to_treat->wclass->handlefunc(widget_to_treat, &event);
                                        return EI_TRUE;
                                }
                        }
                }
        } while (widget_to_treat != root_frame);

        if (widget_to_treat->pick_id == widget_id){
                widget_to_treat->wclass->handlefunc(widget_to_treat, &event);
                return EI_TRUE;
        }

        return EI_FALSE;
}


/**
 * Handle functions
 */

ei_bool_t handle_top_level_function(struct ei_widget_t* widget,
                                    struct ei_event_t* event){
        ei_top_level_t *toplevel_widget = (ei_top_level_t *) widget;
        printf("toplevel");
        // Verify if mouse pointer is in resize rect
        int xmax = toplevel_widget->resize_rect->top_left.x + toplevel_widget->resize_rect->size.width;
        int ymax = toplevel_widget->resize_rect->top_left.y + toplevel_widget->resize_rect->size.height;
        int xmin = toplevel_widget->resize_rect->top_left.x;
        int ymin = toplevel_widget->resize_rect->top_left.y;
        if (event->param.mouse.where.x >= xmin && event->param.mouse.where.x <= xmax && event->param.mouse.where.y <= ymax && event->param.mouse.where.y >= ymin){
                if (event->type == ei_ev_mouse_move && ei_event_get_active_widget() == widget) {
                                switch (toplevel_widget->resizable) {
                                        case ei_axis_none:
                                                break;
                                        case ei_axis_x:
                                                toplevel_widget->widget.screen_location.size.width =
                                                        event->param.mouse.where.x -
                                                        toplevel_widget->widget.screen_location.top_left.x;
                                                break;
                                        case ei_axis_y:
                                                toplevel_widget->widget.screen_location.size.height =
                                                        event->param.mouse.where.y -
                                                        toplevel_widget->widget.screen_location.top_left.y;
                                                break;
                                        case ei_axis_both:
                                                toplevel_widget->widget.screen_location.size.width =
                                                        event->param.mouse.where.x -
                                                        toplevel_widget->widget.screen_location.top_left.x;
                                                toplevel_widget->widget.screen_location.size.height =
                                                        event->param.mouse.where.y -
                                                        toplevel_widget->widget.screen_location.top_left.y;
                                                break;
                                }
                } else if (event->type == ei_ev_mouse_buttondown){
                        if (event->param.mouse.button == ei_mouse_button_left){
                                ei_event_set_active_widget(widget);
                        }
                } else if (event->type == ei_ev_mouse_buttonup && ei_event_get_active_widget() == widget) {
                        if (event->param.mouse.button == ei_mouse_button_left) {
                                ei_event_set_active_widget(NULL);
                        }
                }
        }

        return EI_TRUE;

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