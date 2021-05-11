#include "ei_application.h"
#include "ei_widget.h"
#include "widget_manager.h"

// All classes of widget
static ei_widgetclass_t FRAME;
static ei_widgetclass_t TOP_LEVEL;
static ei_widgetclass_t BUTTON;

// Root elements
ei_surface_t *ROOT_WINDOW = NULL;
ei_widget_t ROOT_FRAME;

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

        // FRAME
        FRAME.allocfunc = &frame_alloc_func;
        FRAME.releasefunc = &frame_release;
        // FRAME->draw_func = &frame_drawfunc;

        FRAME.setdefaultsfunc = NULL; FRAME.geomnotifyfunc = NULL; FRAME.handlefunc = NULL;
        FRAME.next = NULL;

        // TOP_LEVEL
        TOP_LEVEL.allocfunc = &top_level_alloc_func;
        TOP_LEVEL.releasefunc = &top_level_release;
        // TOP_LEVEL->draw_func = &top_level_drawfunc;
        TOP_LEVEL.setdefaultsfunc = NULL; TOP_LEVEL.geomnotifyfunc = NULL; TOP_LEVEL.handlefunc = NULL;
        TOP_LEVEL.next = &FRAME;

        // BUTTON
        BUTTON.allocfunc = &button_alloc_func;
        BUTTON.releasefunc = &button_release;
        BUTTON.drawfunc = &ei_draw_button;
        BUTTON.setdefaultsfunc = NULL; BUTTON.geomnotifyfunc = NULL; BUTTON.handlefunc = NULL;
        BUTTON.next = &TOP_LEVEL;

        // Creates the root window. It is released by calling hw_quit later.
        ROOT_WINDOW = hw_create_window(main_window_size, fullscreen);

        // Initialize root frame (root widget).
        ROOT_FRAME.wclass = &FRAME;
        ROOT_FRAME.parent = NULL;
        ROOT_FRAME.next_sibling = NULL;

        //TODO:Geometry management is not done for ROOT_FRAME. It has to be done.
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
        getchar();
}

/**
 * \brief	Returns the "root widget" of the application: a "frame" widget that span the entire
 *		root window.
 *
 * @return 			The root widget.
 */
ei_widget_t* ei_app_root_widget(void){
        return &ROOT_FRAME;
}

/**
 * \brief	Returns the surface of the root window. Can be usesd to create surfaces with similar
 * 		r, g, b channels.
 *
 * @return 			The surface of the root window.
 */
ei_surface_t ei_app_root_surface(void){
        return *ROOT_WINDOW;
}
