#include "event_manager.h"

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

static ei_bool_t is_in_button(ei_button_t *button, ei_point_t point){
        int x_min = button->widget.screen_location.top_left.x;
        int y_min = button->widget.screen_location.top_left.y;
        int x_max = x_min + button->widget.screen_location.size.width;
        int y_max = y_min + button->widget.screen_location.size.height;
        return point.x <= x_max && point.x >= x_min && point.y <= y_max && point.y >= y_min;
}

/**
 * @brief compute the widget concerned and call its callback
 * @param a situate event
 * @return a boolean, true if the event is treated
 */
ei_bool_t situate_event_callback(ei_event_t *event){
//        printf("reach beginning of situate_event_callback\n");
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

//        printf("je vais atteindre la top_level\n");
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

//        printf("reach end of situate_event_callback\n");
        // If no widget handle_function is treated, so has to call the default function
        return EI_FALSE;
}

/**
 * Handle functions
 */

ei_bool_t handle_top_level_function(struct ei_widget_t* widget,
                                    struct ei_event_t* event){
        // Cast the widget to treat itself
        ei_top_level_t *toplevel_widget = (ei_top_level_t *) widget;
//        printf("reach handle_top_level_function");

        if (event->type == ei_ev_mouse_buttondown && event->param.mouse.button == ei_mouse_button_left){

                if (is_in_button(toplevel_widget->close_button, event->param.mouse.where)){
                        printf("Doit fermer normalement\n\n\n");
                        ei_widget_destroy(widget);
                        return EI_TRUE;
                }

        }





//        typedef struct ei_top_level_t {
//                ei_widget_t		widget;
//                ei_color_t		color;
//                int			border_width;
//                char*			title;
//                ei_bool_t		closable;
//                ei_axis_set_t		resizable;
//                ei_size_t*		min_size;
//                ei_button_t*            close_button;
//                ei_rect_t*              resize_rect;
//        } ei_top_level_t;
//
//        typedef struct ei_button_t {
//                ei_widget_t		widget;
//                ei_color_t	        color;
//                int			border_width;
//                int			corner_radius;
//                ei_relief_t		relief;
//                char*			text;
//                ei_font_t		text_font;
//                ei_color_t		text_color;
//                ei_anchor_t		text_anchor;
//                ei_surface_t		img;
//                ei_rect_t*		img_rect;
//                ei_anchor_t		img_anchor;
//                ei_callback_t		callback;
//                void*			user_param;
//        } ei_button_t;


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