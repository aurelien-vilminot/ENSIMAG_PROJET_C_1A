#include <ei_utils.h>
#include "ei_application.h"
#include "ei_widget.h"
#include "widget_manager.h"


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

        root_frame = ei_widget_create("frame", NULL, NULL, NULL);

        // Creates the root window. It is released by calling hw_quit later.
        root_windows = hw_create_window(main_window_size, fullscreen);

        // Initialize root frame (root widget).
        root_frame->wclass = frame_class;
        root_frame->parent = NULL;
        root_frame->next_sibling = NULL;

        //TODO:Geometry management is not done for root_frame. It has to be done.
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

}

/**
 * \brief	Runs the application: enters the main event loop. Exits when
 *		\ref ei_app_quit_request is called.
 */
void ei_app_run(void){
        if (root_frame->children_head != NULL) {
                ei_widget_t *current_children = root_frame->children_head;
                while (current_children != NULL) {
                        // Call draw function
                        // TODO : changer paramètres
                        current_children->wclass->drawfunc(root_frame, root_windows, NULL, NULL);
                        current_children = current_children->next_sibling;
                }
        }
        root_frame->wclass->drawfunc(root_frame, root_windows, NULL, NULL);
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
