#include <ei_utils.h>
#include "ei_application.h"
#include "ei_widget.h"
#include "widget_manager.h"
#include "ei_event.h"


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

        //TODO: Registers all classes of widget. For now some pointers are NULL, but after implementation of
        // functions they must be initialized.

        // Allocate memory for each class
        frame_class = malloc(sizeof(ei_widgetclass_t));
        button_class = malloc(sizeof(ei_widgetclass_t));
        top_level_class = malloc(sizeof(ei_widgetclass_t));

        // Register class name
        strcpy(frame_class->name, "frame");
        strcpy(top_level_class->name, "top_level");
        strcpy(button_class->name, "button");

        // Register classes
        ei_widgetclass_register(frame_class);
        ei_widgetclass_register(top_level_class);
        ei_widgetclass_register(button_class);

        // Creates the root window. It is released by calling hw_quit later
        root_windows = hw_create_window(main_window_size, fullscreen);

        // Initialize root frame (root widget)
        root_frame = ei_widget_create("frame", NULL, NULL, NULL);
        // Geometry management
        ei_place(root_frame, NULL, NULL, NULL, &main_window_size.width, &main_window_size.height, NULL, NULL, NULL, NULL);
        root_frame->wclass = frame_class;
        root_frame->parent = NULL;
        root_frame->next_sibling = NULL;
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
                button_class->geomnotifyfunc = NULL; button_class->handlefunc = NULL;
        } else if (strcmp(class_name, "top_level") == 0) {
                top_level_class->allocfunc = &top_level_alloc_func;
                top_level_class->releasefunc = &top_level_release;
                top_level_class->drawfunc= &ei_draw_top_level;
                top_level_class->setdefaultsfunc = &set_default_top_level;
                top_level_class->next = button_class;
                top_level_class->geomnotifyfunc = NULL; top_level_class->handlefunc = NULL;
        } else if (strcmp(class_name, "frame") == 0) {
                frame_class->allocfunc = &frame_alloc_func;
                frame_class->releasefunc = &frame_release;
                frame_class->drawfunc = &ei_draw_frame;
                frame_class->setdefaultsfunc = &set_default_frame;
                frame_class->next = top_level_class;
                frame_class->geomnotifyfunc = NULL; frame_class->handlefunc = NULL;
        }
}

/**
 * \brief	Releases all the resources of the application, and releases the hardware
 *		(ie. calls \ref hw_quit).
 */
void ei_app_free(void){
        // Delete linked list classes
        ei_widgetclass_t *linked_list_classes = frame_class;
        while (linked_list_classes->next != NULL) {
                ei_widgetclass_t *to_suppr = linked_list_classes;
                linked_list_classes->next = linked_list_classes->next;
                to_suppr = NULL;
                free(to_suppr);
                linked_list_classes = linked_list_classes->next;
        }
        linked_list_classes = NULL;
        free(linked_list_classes);

        // Delete all existing widgets
        ei_widget_destroy(root_frame);
        // Release the hardware
        hw_quit();
}

/**
 * \brief	Runs the application: enters the main event loop. Exits when
 *		\ref ei_app_quit_request is called.
 */
void ei_app_run(void){
        // Wait for a key press.
        // TODO : utiliser ei_app_quit_request pour la condition du while
        ei_event_t event;
        event.type = ei_ev_none;
        while (event.type != ei_ev_keydown) {
                // Draw root
                root_frame->wclass->drawfunc(root_frame, root_windows, NULL, NULL);

                // Depth course of each widgets
                if (root_frame->children_head != NULL) {

                        // Get the first child and the first child of its child (this could be NULL)
                        ei_widget_t *current_widget = root_frame->children_head;
                        ei_widget_t *first_next_child = current_widget->children_head;

                        do {
                                while (current_widget != NULL) {
                                        if (first_next_child == NULL) {
                                                // Put the first child, if it doesn't store
                                                first_next_child = current_widget->children_head;
                                        }

                                        // Call draw function
                                        // TODO : changer paramètre surface, pick surface, clipper
                                        current_widget->wclass->drawfunc(current_widget, root_windows, NULL, NULL);

                                        current_widget = current_widget->next_sibling;
                                }

                                // Change deep level
                                current_widget = first_next_child;
                                first_next_child = NULL;
                        } while (current_widget != NULL);
                }

                // Update screen and event
                hw_surface_update_rects(root_windows, NULL);
                hw_event_wait_next(&event);
        }
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
}

/**
 * @brief       Return a linked list which represent all widget classes
 * @return      A pointeur on the linked list
 */
extern ei_widgetclass_t* get_linked_list_classes() {
        return frame_class;
}
