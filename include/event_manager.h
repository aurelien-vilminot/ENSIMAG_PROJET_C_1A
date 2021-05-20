#ifndef PROJETC_IG_EVENT_MANAGER_H
#define PROJETC_IG_EVENT_MANAGER_H

#include <string.h>
#include "ei_event.h"
#include "event_manager.h"
#include "widget_manager.h"
#include "ei_widget.h"

/*
 * Global variables
 */

// Is used to know what is the widget currently used by the user
ei_widget_t *g_active_widget;
// Next event
ei_event_t *next_event;
// Boolean of the main program which quit the program or not
static ei_bool_t not_the_end = EI_TRUE;
// Previous position of the mouse
ei_event_t *g_previous_event;


/*
 * Intermediate functions
 */
void children_resizing(struct ei_widget_t* widget);
static ei_bool_t is_rectangle_in_rectangle(ei_rect_t rectangle, ei_point_t point, ei_size_t* rect);

/*
 * Callback functions
 */
ei_bool_t situate_event_callback(ei_event_t *event);
ei_bool_t keyword_event_callback(ei_event_t *event);



/*
 * Handle functions
 */



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
                                 struct ei_event_t* event);

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
                                    struct ei_event_t* event);

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
                                struct ei_event_t* event);



#endif //PROJETC_IG_EVENT_MANAGER_H
