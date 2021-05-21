#include <ei_utils.h>
#include <ei_event.h>
#include "ei_application.h"
#include "ei_widget.h"
#include "widget_manager.h"
#include "event_manager.h"

/**
 * \brief	Creates an application.
 *		<ul>
 *			<li> initializes the hardware (calls \ref hw_init), </li>
 *			<li> registers all classes of widget, </li>
 *			<li> creates the root window (either in a system window, or the entire screen), </li>
 *			<li> creates the root widget to access the root window. </li>
 *		</ul>
 *
 * @param	main_window_size	If "fullscreen is false, the size of the root window of the
 *					application.
 *					If "fullscreen" is true, the current monitor resolution is
 *					used as the size of the root window. \ref hw_surface_get_size
 *					can be used with \ref ei_app_root_surface to get the size.
 * @param	fullScreen		If true, the root window is the entire screen. Otherwise, it
 *					is a system window.
 */
void ei_app_create(ei_size_t main_window_size, ei_bool_t fullscreen){

        // Initialize the hardware. Must be closed at the end of the manipulation.
        hw_init();

        // Allocate memory for each class
        frame_class = malloc(sizeof(ei_widgetclass_t));
        button_class = malloc(sizeof(ei_widgetclass_t));
        top_level_class = malloc(sizeof(ei_widgetclass_t));

        // Register class name
        strcpy(frame_class->name, "frame");
        strcpy(top_level_class->name, "toplevel");
        strcpy(button_class->name, "button");

        // Register classes
        ei_widgetclass_register(frame_class);
        ei_widgetclass_register(top_level_class);
        ei_widgetclass_register(button_class);

        // Creates the root window. It is released by calling hw_quit later
        root_windows = hw_create_window(main_window_size, fullscreen);

        // Def offscreen
        offscreen = hw_surface_create(root_windows, hw_surface_get_size(root_windows), hw_surface_has_alpha(root_windows));

        // Init default font
        ei_default_font = hw_text_font_create(ei_default_font_filename, ei_style_normal, ei_font_default_size);

        // Initialize root frame (root widget)
        root_frame = ei_widget_create("frame", NULL, NULL, NULL);

        // Geometry management
        root_frame->screen_location.size.width = main_window_size.width;
        root_frame->screen_location.size.height = main_window_size.height;
        root_frame->content_rect = &root_frame->screen_location;

        // Event management
        root_frame->pick_id = 0;
        root_frame->pick_color = inverse_map_rgba(offscreen, root_frame->pick_id);
}

/**
 * @brief	Registers a class to the program so that widgets of this class can be created.
 *		This must be done only once per widged class in the application.
 *
 * @param	widgetclass	The structure describing the class.
 */
void			ei_widgetclass_register		(ei_widgetclass_t* widgetclass) {
        char * class_name = ei_widgetclass_stringname(widgetclass->name);

        if (strcmp(class_name, "button") == 0) {
                button_class->allocfunc = &button_alloc_func;
                button_class->releasefunc = &button_release;
                button_class->drawfunc = &ei_draw_button;
                button_class->setdefaultsfunc = &set_default_button;
                button_class->next = NULL;
                button_class->geomnotifyfunc = &button_geomnotifyfunc; button_class->handlefunc = &handle_button_function;
        } else if (strcmp(class_name, "toplevel") == 0) {
                top_level_class->allocfunc = &top_level_alloc_func;
                top_level_class->releasefunc = &top_level_release;
                top_level_class->drawfunc= &ei_draw_top_level;
                top_level_class->setdefaultsfunc = &set_default_top_level;
                top_level_class->next = button_class;
                top_level_class->geomnotifyfunc = &top_level_geomnotifyfunc; top_level_class->handlefunc = &handle_top_level_function;
        } else if (strcmp(class_name, "frame") == 0) {
                frame_class->allocfunc = &frame_alloc_func;
                frame_class->releasefunc = &frame_release;
                frame_class->drawfunc = &ei_draw_frame;
                frame_class->setdefaultsfunc = &set_default_frame;
                frame_class->next = top_level_class;
                frame_class->geomnotifyfunc = &frame_geomnotifyfunc; frame_class->handlefunc = &handle_frame_function;
        }
}

/**
 * \brief	Releases all the resources of the application, and releases the hardware
 *		(ie. calls \ref hw_quit).
 */
void ei_app_free(void){


        // Delete all existing widgets
        ei_widget_destroy(root_frame);

        // Delete linked list classes
        ei_widgetclass_t *linked_list_classes = frame_class;

        while (linked_list_classes->next != NULL) {
                ei_widgetclass_t *to_suppr = linked_list_classes;
                linked_list_classes = linked_list_classes->next;
                free(to_suppr);
        }

        free(linked_list_classes);

        // Release the hardware
        hw_quit();
}

/**
 * \brief	Runs the application: enters the main event loop. Exits when
 *		\ref ei_app_quit_request is called.
 */
void ei_app_run(void){

        // Must allocate the memory for use g_next_event in hw_event_wait_next
        g_next_event = malloc(sizeof(ei_event_t));

        // Fill g_next_event with the g_next_event
        hw_event_wait_next(g_next_event);

        while (g_not_the_end) {

                // Draw root
                root_frame->wclass->drawfunc(root_frame, root_windows, offscreen, NULL);

                ei_widget_t *widget_to_print = root_frame;

                // Depth course of each widgets
                do {
                        ei_widget_t * parent;
                        if (widget_to_print->children_head) {
                                parent = widget_to_print;
                                widget_to_print = widget_to_print->children_head;
                                widget_to_print->wclass->drawfunc(widget_to_print, root_windows, offscreen, parent->content_rect);
                        } else {
                                while (widget_to_print != root_frame && widget_to_print->next_sibling == NULL) {
                                        widget_to_print = widget_to_print->parent;
                                }

                                if (widget_to_print->next_sibling) {
                                        parent = widget_to_print->parent;
                                        widget_to_print = widget_to_print->next_sibling;
                                        widget_to_print->wclass->drawfunc(widget_to_print, root_windows, offscreen, parent->content_rect);
                                }
                        }
                } while (widget_to_print != root_frame);

                // Update screen and event

                hw_surface_update_rects(root_windows, NULL);
                hw_event_wait_next(g_next_event);

                // Not a panacea to break after change while condition
                if (g_next_event->type == ei_ev_keydown){
                        if (g_next_event->param.key.key_code == SDLK_ESCAPE){
                                ei_app_quit_request();
                                break;
                        }
                }

                // This is a situate event
                if (g_next_event->type <= 7 && g_next_event->type >= 5){
                        situate_event_callback(g_next_event);
                }

                if (g_next_event->type == 3){
                        keyword_event_callback(g_next_event);
                }


        }
        free(g_next_event);
}

/**
 * \brief	Returns the "root widget" of the application: a "frame" widget that span the entire
 *		root window.
 *
 * @return 			The root widget.
 */
ei_widget_t* ei_app_root_widget(void){
        return root_frame;
}

/**
 * \brief	Returns the surface of the root window. Can be usesd to create surfaces with similar
 * 		r, g, b channels.
 *
 * @return 			The surface of the root window.
 */
ei_surface_t ei_app_root_surface(void){
        return *root_windows;
}

/**
 * \brief	Tells the application to quite. Is usually called by an event handler (for example
 *		when pressing the "Escape" key).
 */
void ei_app_quit_request(void) {
        g_not_the_end = EI_FALSE;
}

/**
 * @brief       Return a linked list which represent all widget classes
 * @return      A pointeur on the linked list
 */
extern ei_widgetclass_t* get_linked_list_classes(void) {
        return frame_class;
}
