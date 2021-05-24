#ifndef PROJETC_IG_EVENT_MANAGER_H
#define PROJETC_IG_EVENT_MANAGER_H

#include <string.h>

#include "ei_event.h"
#include "widget_manager.h"
#include "ei_widget.h"

/*
 * Global variables and associate function
 */

// Represents what is the widget currently used by the user. If no widget is used, equals NULL.
static ei_widget_t *g_active_widget = NULL;
// Represents the next user event. Is used in the main program.
static ei_event_t *g_next_event = NULL;
// Boolean of the main program which quit the program or not.
static ei_bool_t g_not_the_end = EI_TRUE;
// Previous position of the mouse. Used to compute movement.
static ei_event_t *g_previous_event = NULL;
// Global default handle function. Used when an event isn't treated.
static ei_default_handle_func_t *g_default_handle_func = NULL;


/*
 * Intermediate functions
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
static ei_bool_t is_rectangle_in_rectangle(ei_rect_t rectangle, ei_point_t t_point, ei_size_t* t_rect);

/**
 * @param widget, the widget of which each child will be updated by @ref ei_placer_run function
 *
 * @return Nothing.
 */
void children_resizing(struct ei_widget_t* widget);

/**
 * @brief Replace all the parents that are toplevel of the widget to the front, i.e. the top parent
 * before the root frame is put as children tail (the last widget subtree printed).
 * The function is used when the mouse clicks on a certain widget.
 *
 * @param widget, the widget concerned by the replacement
 */
static void replace_order(ei_widget_t* widget);

/*
 * Callback functions
 */

/**
 * @param event, the event which has to be treated. If is given as argument, so is a keyword event.
 *
 * @return EI_TRUE if the event is treated, EI_FALSE otherwise.
 */
ei_bool_t keyword_event_callback(ei_event_t *event);

/**
 * @brief compute the widget concerned by the mouse position and call its callback
 *
 * @param event, the event which has to be treated. If is given as argument, so is a situate event.
 *
 * @return I_TRUE if the event is treated, EI_FALSE otherwise.
 */
ei_bool_t situate_event_callback(ei_event_t *event);


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
