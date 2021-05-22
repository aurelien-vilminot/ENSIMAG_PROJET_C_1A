#include "ei_event.h"
#include "event_manager.h"

/*
 * Intermediate functions, use by callback functions
 */

/**
 * @brief Verify if a rectangle is in another. Note that the rectangle tested can be a simple point.
 *
 * @param rectangle, the rectangle in which the tested rectangle is or not.
 * @param t_point, the top left point tested.
 * @param t_rect, the size of the rectangle tested.
 *
 * @return EI_TRUE if the tested rectangle is in rectangle, EI_FALSE otherwise.
 */
static ei_bool_t is_rectangle_in_rectangle(ei_rect_t rectangle, ei_point_t t_point, ei_size_t* t_rect){
        int x_min = rectangle.top_left.x;
        int y_min = rectangle.top_left.y;
        int x_max = x_min + rectangle.size.width;
        int y_max = y_min + rectangle.size.height;

        if (t_rect){
                return t_point.x >= x_min && (t_point.x + t_rect->width) <= x_max && t_point.y >= y_min && (t_point.y + t_rect->height) <= y_max;
        } else {
                return t_point.x <= x_max && t_point.x >= x_min && t_point.y <= y_max && t_point.y >= y_min;
        }
}

/**
 * @param widget, the widget of which each child will be updated by @ref ei_placer_run function
 *
 * @return Nothing.
 */
void children_resizing(struct ei_widget_t* widget){
        ei_widget_t * widget_to_change = widget;

        // Depth course of each widgets
        do {
                if (widget_to_change->children_head) {

                        widget_to_change = widget_to_change->children_head;
                        ei_placer_run(widget_to_change);
                } else {
                        while (widget_to_change != widget && widget_to_change->next_sibling == NULL) {
                                widget_to_change = widget_to_change->parent;
                        }

                        if (widget_to_change->next_sibling && widget_to_change != widget) {
                                widget_to_change = widget_to_change->next_sibling;
                                ei_placer_run(widget_to_change);
                        }
                }
        } while (widget_to_change != widget);
}

static void replace_order(ei_widget_t* widget){
        while (widget != g_root_frame) {
                if (strcmp(widget->wclass->name, "toplevel") == 0) {
                        if (widget != widget->parent->children_tail) {
                                ei_widget_t *widget_to_change = widget->parent->children_head;
                                if (widget_to_change == widget) {
                                        widget->parent->children_head = widget->next_sibling;

                                } else {
                                        while (widget_to_change->next_sibling != widget) {
                                                widget_to_change = widget_to_change->next_sibling;
                                        }
                                        widget_to_change->next_sibling = widget->next_sibling;
                                }

                                widget->parent->children_tail->next_sibling = widget;
                                widget->parent->children_tail = widget;
                                widget->parent->children_tail->next_sibling = NULL;
                        }
                }
                widget = widget->parent;
        }
}

/*
 * Callback functions
 */

/**
 * @param event, the event which has to be treated. If is given as argument, so is a keyword event.
 *
 * @return EI_TRUE if the event is treated, EI_FALSE otherwise.
 */
ei_bool_t keyword_event_callback(ei_event_t *event){
        // If a widget is already active, we treat this widget
        if (ei_event_get_active_widget()){
                return ei_event_get_active_widget()->wclass->handlefunc(ei_event_get_active_widget(), event);
        }
        // Otherwise, we search the event to treat
        if (event->type == ei_ev_keydown){
                // The last toplevel created (with the highest pick_id) is deleted by the following depth course
                if (event->param.key.key_code == SDLK_w && ei_has_modifier(event->param.key.modifier_mask, ei_mod_ctrl_left)){
                        ei_widget_t *widget_to_destroy = NULL;
                        ei_widget_t *widget_to_treat = g_root_frame;
                        // Depth course of each widgets
                        do {
                                if (widget_to_treat->children_head) {
                                        widget_to_treat = widget_to_treat->children_head;
                                        if (strcmp(widget_to_treat->wclass->name,"toplevel")==0){
                                                if (widget_to_destroy){
                                                        if (widget_to_treat->pick_id > widget_to_destroy->pick_id){
                                                                widget_to_destroy = widget_to_treat;
                                                        }
                                                } else {
                                                        widget_to_destroy = widget_to_treat;
                                                }
                                        }
                                } else {
                                        while (widget_to_treat != g_root_frame && widget_to_treat->next_sibling == NULL) {
                                                widget_to_treat = widget_to_treat->parent;
                                        }

                                        if (widget_to_treat->next_sibling) {
                                                widget_to_treat = widget_to_treat->next_sibling;
                                                if (widget_to_destroy){
                                                        if (widget_to_treat->pick_id > widget_to_destroy->pick_id){
                                                                widget_to_destroy = widget_to_treat;
                                                        }
                                                } else {
                                                        widget_to_destroy = widget_to_treat;
                                                }
                                        }
                                }
                        } while (widget_to_treat != g_root_frame);
                        if (widget_to_destroy){
                                ei_widget_destroy(widget_to_destroy);
                                return EI_TRUE;
                        }
                        return EI_FALSE;
                }
        }
        return EI_FALSE;
}

/**
 * @brief compute the widget concerned by the mouse position and call its callback
 *
 * @param event, the event which has to be treated. If is given as argument, so is a situate event.
 *
 * @return I_TRUE if the event is treated, EI_FALSE otherwise.
 */
ei_bool_t situate_event_callback(ei_event_t *event){
        // If a widget is already active, we treat this widget
        if (ei_event_get_active_widget()){
                ei_event_get_active_widget()->wclass->handlefunc(ei_event_get_active_widget(), event);
        }
        // Otherwise, we search the event to treat
        // Parameters of the offscreen
        hw_surface_lock(g_offscreen);
        uint32_t *clicked_pixel = (uint32_t *) hw_surface_get_buffer(g_offscreen);
        ei_size_t offscreen_size = hw_surface_get_size(g_offscreen);

        // Compute memory location of clicked_pixel and put the its value in widget_id
        clicked_pixel += (offscreen_size.width * event->param.mouse.where.y) + event->param.mouse.where.x;
        uint32_t widget_id = *clicked_pixel;

        // Test if the clicked pixel is in the g_root_frame
        ei_widget_t *widget_to_treat = g_root_frame;
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
                        while (widget_to_treat != g_root_frame && widget_to_treat->next_sibling == NULL) {
                                widget_to_treat = widget_to_treat->parent;
                        }

                        if (widget_to_treat->next_sibling) {
                                widget_to_treat = widget_to_treat->next_sibling;
                                if (widget_to_treat->pick_id == widget_id){
                                        return widget_to_treat->wclass->handlefunc(widget_to_treat, event);
                                }
                        }
                }
        } while (widget_to_treat != g_root_frame);

        // If no widget handle_function is treated, so has to call the default function
        return EI_FALSE;
}

/*
 * Handle functions
 */

/**
 *
 * @param widget    The active widget (a top level in this case) concerned by the event
 *
 * @param event     The event containing all parameters
 *
 * @return          EI_TRUE if  he function handled the event,
 *                  EI_FALSE otherwise, in this case the event is dismissed.
 */

ei_bool_t handle_top_level_function(struct ei_widget_t* widget,
                                    struct ei_event_t* event){
        // Cast the widget to treat itself
        ei_top_level_t *toplevel_widget = (ei_top_level_t *) widget;

        if (event->param.mouse.button == ei_mouse_button_left){
                if (event->type == ei_ev_mouse_buttondown){
                        // If the mouse is in the rectangle corresponding to the closing button
                        if (is_rectangle_in_rectangle(toplevel_widget->close_button->widget.screen_location, event->param.mouse.where, NULL)){
                                ei_event_set_active_widget(widget);
                                return EI_TRUE;
                        }

                        replace_order(widget);
                        // If the mouse is in the resizing rectangle
                        if (is_rectangle_in_rectangle(*(toplevel_widget->resize_rect), event->param.mouse.where, NULL)){
                                ei_event_set_active_widget(widget);
                                toplevel_widget->current_event = event_resize;
                                return EI_TRUE;
                        }
                        // If the mouse is in the top bar and tries to move the top level
                        if (is_rectangle_in_rectangle(*(toplevel_widget->top_bar), event->param.mouse.where, NULL)){
                                ei_event_set_active_widget(widget);
                                toplevel_widget->current_event = event_move;
                                g_previous_event = malloc(sizeof(ei_event_t));
                                *g_previous_event = *event;
                                return EI_TRUE;
                        }
                // If the mouse button is up, release the active widget
                } else if (event->type == ei_ev_mouse_buttonup) {
                        if (is_rectangle_in_rectangle(toplevel_widget->close_button->widget.screen_location, event->param.mouse.where, NULL)){
                                ei_widget_destroy(widget);
                                ei_event_set_active_widget(NULL);
                        } else {
                                // Put all elements to NONE or NULL
                                toplevel_widget->current_event = event_none;
                                ei_event_set_active_widget(NULL);
                                g_previous_event = NULL;
                        }
                        return EI_TRUE;
                // If the mouse move, move the top level corresponding to
                } else if (event->type == ei_ev_mouse_move && ei_event_get_active_widget() == widget){
                        // We consider that a widget has always a min_size
                        if (toplevel_widget->current_event == event_resize) {
                                int new_width, new_height;
                                int zero = 0;
                                if (strcmp(toplevel_widget->widget.parent->wclass->name, "toplevel")==0){
                                        new_width = event->param.mouse.where.x -
                                                        toplevel_widget->widget.screen_location.top_left.x;
                                        new_height = event->param.mouse.where.y -
                                                     toplevel_widget->widget.screen_location.top_left.y;
                                        float new_rel_width = (float) new_width / toplevel_widget->widget.parent->content_rect->size.width;
                                        float new_rel_height = ((float) new_height) / toplevel_widget->widget.parent->content_rect->size.height;
                                        if ((toplevel_widget->resizable == ei_axis_x ||
                                             toplevel_widget->resizable == ei_axis_both) &&
                                                (new_width >= toplevel_widget->min_size->width || new_width >= toplevel_widget->widget.screen_location.size.width)) {
                                                ei_place(widget, NULL, NULL, NULL, &zero, NULL, NULL, NULL, &new_rel_width,
                                                         NULL);
                                        }
                                        if ((toplevel_widget->resizable == ei_axis_y ||
                                             toplevel_widget->resizable == ei_axis_both) &&
                                                (new_height >= toplevel_widget->min_size->height || new_height >= toplevel_widget->widget.screen_location.size.height)) {
                                                ei_place(widget, NULL, NULL, NULL, NULL, &zero, NULL, NULL, NULL,
                                                         &new_rel_height);
                                        }

                                } else {
                                        new_width = event->param.mouse.where.x -
                                                        toplevel_widget->widget.screen_location.top_left.x;
                                        if ((toplevel_widget->resizable == ei_axis_x ||
                                             toplevel_widget->resizable == ei_axis_both) &&
                                            new_width >= toplevel_widget->min_size->width) {
                                                ei_place(widget, NULL, NULL, NULL, &new_width, NULL, NULL, NULL, NULL,
                                                         NULL);
                                        }

                                        new_height = event->param.mouse.where.y -
                                                         toplevel_widget->widget.screen_location.top_left.y;
                                        if ((toplevel_widget->resizable == ei_axis_y ||
                                             toplevel_widget->resizable == ei_axis_both) &&
                                            new_height >= toplevel_widget->min_size->height) {
                                                ei_place(widget, NULL, NULL, NULL, NULL, &new_height, NULL, NULL, NULL,
                                                         NULL);
                                        }
                                }

                                children_resizing(widget);
                        } else if (toplevel_widget->current_event == event_move){
                                // New location of coord x and y
                                int new_loc_x = toplevel_widget->widget.screen_location.top_left.x + event->param.mouse.where.x - g_previous_event->param.mouse.where.x;
                                int new_loc_y = toplevel_widget->widget.screen_location.top_left.y + event->param.mouse.where.y - g_previous_event->param.mouse.where.y;
                                ei_point_t new_loc = {new_loc_x, new_loc_y};
                                // If the mouse is still in the parent limit, so the top level is moved to the new coord
                                if (strcmp(toplevel_widget->widget.parent->wclass->name, "toplevel") == 0){
                                        if (is_rectangle_in_rectangle(*toplevel_widget->widget.parent->content_rect, new_loc, &toplevel_widget->widget.screen_location.size)){
                                                new_loc_x -= toplevel_widget->widget.parent->content_rect->top_left.x;
                                                new_loc_y -= toplevel_widget->widget.parent->content_rect->top_left.y;
                                                ei_place(&toplevel_widget->widget, NULL, &new_loc_x, &new_loc_y, NULL, NULL, NULL, NULL, NULL, NULL);
                                                children_resizing(widget);
                                        }
                                // If there isn't parent, so verifies that the top level is still in the root frame
                                } else {
                                        if (is_rectangle_in_rectangle(g_root_frame->screen_location, new_loc, &toplevel_widget->widget.screen_location.size)){
                                                ei_place(widget, NULL, &new_loc_x, &new_loc_y, NULL, NULL, NULL, NULL, NULL, NULL);
                                                children_resizing(widget);
                                        }
                                }
                                // The current event is saved as previous event for the next computation of new location
                                *g_previous_event = *event;
                        }
                        return EI_TRUE;
                }
        } else if (event->param.mouse.button == ei_mouse_button_middle){
                return EI_FALSE;
        } else if (event->param.mouse.button == ei_mouse_button_right){
                return EI_FALSE;
        }
        return EI_FALSE;

}

/**
 *
 * @param widget    The active widget (a button in this case) concerned by the event
 *
 * @param event     The event containing all parameters
 *
 * @return          EI_TRUE if  he function handled the event,
 *                  EI_FALSE otherwise, in this case the event is dismissed.
 */

ei_bool_t handle_button_function(struct ei_widget_t* widget,
                                 struct ei_event_t* event){

        if (event->type == ei_ev_mouse_buttondown){
                replace_order(widget);
        }

        ei_button_t *button_widget = (ei_button_t *) widget;
        if (event->param.mouse.button == ei_mouse_button_left) {
                // If the left button of the mouse is down
                if (event->type == ei_ev_mouse_buttondown) {
                        button_widget->relief = ei_relief_sunken;
                        ei_event_set_active_widget(widget);
                        return EI_TRUE;
                }

                // If the left button of the mouse is up
                else if (event->type == ei_ev_mouse_buttonup) {
                        button_widget->relief = ei_relief_raised;
                        ei_event_set_active_widget(NULL);
                        return EI_TRUE;
                }
        }
        return EI_FALSE;
}

/**
 *
 * @param widget    The active widget (a frame in this case) concerned by the event
 *
 * @param event     The event containing all parameters
 *
 * @return          EI_TRUE if  he function handled the event,
 *                  EI_FALSE otherwise, in this case the event is dismissed.
 */

ei_bool_t handle_frame_function(struct ei_widget_t* widget,
                                 struct ei_event_t* event){
        if (event->type == ei_ev_mouse_buttondown){
                replace_order(widget);
                return EI_TRUE;
        }
        return EI_FALSE;
}

/*
 * Other functions
 */

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
 * @return			The widget currently being manipulated, or NULL.
 */
ei_widget_t* ei_event_get_active_widget(void){
        return g_active_widget;
}

/**
 * Sets the function that must be called when an event has been received but no processed
 *	by any widget.
 *
 * @param	func		The event handling function.
 */
void ei_event_set_default_handle_func(ei_default_handle_func_t func){
        g_default_handle_func = malloc(sizeof(ei_default_handle_func_t));
        *g_default_handle_func = func;
}

/**
 * Returns the function that must be called when an event has been received but no processed
 *	by any widget.
 *
 * @return			The address of the event handling function.
 */
ei_default_handle_func_t ei_event_get_default_handle_func(void){
        return *g_default_handle_func;
}