#include <ei_event.h>

/**
 * @brief	Describes an event.
 */
typedef struct ei_event_t {
        ei_eventtype_t	type;				///< The type of the event.
        union {
                ei_key_event_t		key;		///< Event parameters for keyboard-related events (see \ref ei_key_event_t).
                ei_mouse_event_t	mouse;		///< Event parameters for mouse-related	events (see \ref ei_mouse_event_t).
                ei_app_event_t		application;	///< Event parameters for application-related events (see \ref ei_app_event_t).
        } param;
} ei_event_t;

/**
 * @brief	Tests is a modifier key was pressed at the time of an event.
 *
 * @param	modifier_mask	The modifier_mask field of a \ref ei_key_event_t or a \ref ei_mouse_event_t.
 * @param	modifier	The modifier key.
 *
 * @return			EI_TRUE if this modifier key was pressed, EI_FALSE otherwise.
 */
static
inline ei_bool_t	ei_has_modifier	(ei_modifier_mask_t	modifier_mask,
                                                ei_modifier_key_t	modifier)
{ return (ei_bool_t)(modifier_mask & (1 << modifier)); }

/**
 * Sets the widget which is currently being manipulated by the user.
 *
 * @param	widget		The widget to declare as active, or NULL to declare
 *				that a widget is no more being manipulated.
 */
void ei_event_set_active_widget(ei_widget_t* widget){
        if (widget){

        }
}

/**
 * Returns the widget currently being manipulated by the user.
 *
 * @return			The widget currenlty being manipulated, or NULL.
 */
ei_widget_t* ei_event_get_active_widget(void);



/**
 * @brief	A function that is called in response to an event that has not been processed
 *		by any widget.
 *
 * @param	event		The event containing all its parameters (type, etc.)
 *
 * @return			EI_TRUE if the function handled the event,
 *				EI_FALSE otherwise, in this case the event is dismissed.
 */
typedef ei_bool_t		(*ei_default_handle_func_t)(struct ei_event_t* event);

/**
 * Sets the function that must be called when an event has been received but no processed
 *	by any widget.
 *
 * @param	func		The event handling function.
 */
void ei_event_set_default_handle_func(ei_default_handle_func_t func);

/**
 * Returns the function that must be called when an event has been received but no processed
 *	by any widget.
 *
 * @return			The address of the event handling function.
 */
ei_default_handle_func_t ei_event_get_default_handle_func(void);