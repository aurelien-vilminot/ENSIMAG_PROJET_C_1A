#ifndef PROJETC_IG_WIDGET_MANAGER_H
#define PROJETC_IG_WIDGET_MANAGER_H

#include <string.h>

#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "application.h"

/*
 * Default values of widget parameters
 */

// BUTTON
static ei_size_t default_button_size = {50, 30};
static ei_color_t default_button_color = {0x7C, 0x87, 0x8E, 0xff};
static ei_anchor_t default_text_button_anchor = ei_anc_center;

// FRAME
static ei_size_t default_frame_size = {400, 400};
static ei_color_t default_frame_color = {0x89, 0xAb, 0xE3, 0xff};
static int default_frame_border_width = 0;
static ei_anchor_t default_text_frame_anchor = ei_anc_center;

// TOP LEVEL
static ei_size_t default_top_level_size = {400, 50};
static ei_color_t default_top_level_color = {0xD0, 0xD3, 0xD4, 0xff};
static int default_top_level_border_width = 0;
static ei_bool_t default_top_level_closable = EI_TRUE;
static ei_size_t default_top_level_min_size = {150, 150};
static uint32_t default_top_level_rect_resize = 10;
typedef enum {
        event_none      = 0,
        event_resize,
        event_move
} current_event;

/*
 * Structures which used to represent specific widget with privates fields
 * All of those must contained an attribute ei_widget_t in first.
 */
typedef struct ei_button_t {
        ei_widget_t		widget;
        ei_color_t	        color;
        int			border_width;
        int			corner_radius;
        ei_relief_t		relief;
        char*			text;
        ei_font_t		text_font;
        ei_color_t		text_color;
        ei_anchor_t		text_anchor;
        ei_surface_t		img;
        ei_rect_t*		img_rect;
        ei_anchor_t		img_anchor;
        ei_callback_t		callback;
        void*			user_param;
} ei_button_t;

typedef struct ei_top_level_t {
        ei_widget_t		widget;
        ei_color_t		color;
        int			border_width;
        char*			title;
        ei_bool_t		closable;
        ei_axis_set_t		resizable;
        ei_size_t*		min_size;
        ei_button_t*            close_button;
        ei_rect_t*              resize_rect;
        ei_rect_t*              top_bar;
        current_event           current_event;
} ei_top_level_t;

typedef struct ei_frame_t {
        ei_widget_t		widget;
        ei_color_t	        color;
        int			border_width;
        ei_relief_t		relief;
        char*			text;
        ei_font_t		text_font;
        ei_color_t		text_color;
        ei_anchor_t		text_anchor;
        ei_surface_t		img;
        ei_rect_t*		img_rect;
        ei_anchor_t		img_anchor;
} ei_frame_t;

/*
 * Allocation functions
 */

/**
 * @brief       Allocate memory used by a button widget.
 * @return      The corresponding widget.
 */
ei_widget_t* button_alloc_func();

/**
 * @brief       Allocate memory used by a top-level widget.
 * @return      The corresponding widget.
 */
ei_widget_t* top_level_alloc_func();

/**
 * @brief       Allocate memory used by a frame widget.
 * @return      The corresponding widget.
 */
ei_widget_t* frame_alloc_func();

/*
 * Release functions
 */

/**
 * @brief       Release memory for pointers attributes which were allocated in alloc function.
 *
 * @param       widget      The widget which resources are to be freed.
 */
void button_release(ei_widget_t* widget);

/**
 * @brief       Release memory for pointers attributes which were allocated in alloc function.
 *
 * @param       widget      The widget which resources are to be freed.
 */
void top_level_release(ei_widget_t* widget);

/**
 * @brief       Release memory for pointers attributes which were allocated in alloc function.
 *
 * @param       widget      The widget which resources are to be freed.
 */
void frame_release(ei_widget_t* widget);

/*
 * Draw functions
 */

/**
 * \brief	A function that draws widgets of button class.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		Where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 */
void ei_draw_button (ei_widget_t* widget,
                 ei_surface_t		surface,
                 ei_surface_t		pick_surface,
                 ei_rect_t*		clipper);

/**
 * \brief	A function that draws widgets of frame class.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		Where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 */
void ei_draw_frame (ei_widget_t* widget,
                    ei_surface_t		surface,
                    ei_surface_t		pick_surface,
                    ei_rect_t*		clipper);

/**
 * \brief	A function that draws widgets of top-level class.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		Where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 */
void ei_draw_top_level (ei_widget_t* widget,
                    ei_surface_t		surface,
                    ei_surface_t		pick_surface,
                    ei_rect_t*		clipper);

/*
 * Default set functions
 */

/**
 * \brief	A function that sets the default values for a widget button.
 *
 * @param	widget		A pointer to the widget instance to intialize.
 */
void set_default_button (ei_widget_t *widget);

/**
 * \brief	A function that sets the default values for a widget frame.
 *
 * @param	widget		A pointer to the widget instance to intialize.
 */
void set_default_frame (ei_widget_t *widget);

/**
 * \brief	A function that sets the default values for a widget top-level.
 *
 * @param	widget		A pointer to the widget instance to intialize.
 */
void set_default_top_level (ei_widget_t *widget);

/*
 * Geomnotify functions
 */

/**
 * \brief 	This function is called to notify the widget that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		Calculate or recalculated the content_rect attribute.
 *
 * @param	widget		The widget instance to notify of a geometry change.
 * @param	rect		The new rectangular screen location of the widget
 *				(i.e. = widget->screen_location).
 */
void button_geomnotifyfunc (struct ei_widget_t* widget, ei_rect_t rect);

/**
 * \brief 	This function is called to notify the widget that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		Calculate or recalculated the content_rect attribute.
 *
 * @param	widget		The widget instance to notify of a geometry change.
 * @param	rect		The new rectangular screen location of the widget
 *				(i.e. = widget->screen_location).
 */
void frame_geomnotifyfunc (struct ei_widget_t* widget, ei_rect_t rect);

/**
 * \brief 	This function is called to notify the widget that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		Calculate or recalculated the content_rect attribute.
 *		Configure or reconfigure the close button.
 *
 * @param	widget		The widget instance to notify of a geometry change.
 * @param	rect		The new rectangular screen location of the widget
 *				(i.e. = widget->screen_location).
 */
void top_level_geomnotifyfunc (struct ei_widget_t* widget, ei_rect_t rect);

/*
 * Intermediate functions
 */

/**
 * @brief       Give coordinates of top-left point where a text must be display depending on the anchor
 *
 * @param       text_anchor         The text anchor
 * @param       text_size           The text size
 * @param       widget_place        The place where the widget parent is
 * @param       widget_size         The size of the widget parent
 *
 * @return      A point which represents coordinates of top-left text place
 */
ei_point_t* text_place(ei_anchor_t *text_anchor, ei_size_t *text_size, ei_point_t *widget_place, ei_size_t *widget_size);

/**
 * @brief       All is in the title
 *
 * @param       surface             Used to know rgba position in the corresponding surface
 * @param       color_to_convert    Element which is convert into ei_color_t
 *
 * @return      A color corresponding to the 32 bits give as argument
 */
ei_color_t *inverse_map_rgba(ei_surface_t surface, uint32_t color_to_convert);

/**
 * @brief       Update child field of parent. It is a course of linked list.
 *
 * @param       widget      The widget child
 * @param       parent      The child's parent which the children field needs to be updated
 */
static void insert_child(ei_widget_t *widget, ei_widget_t *parent);

static uint32_t last_id(void);

/**
 * @brief       Return widget class wich correspond to class name give in parameter.
 *
 * @param       ll              The linked list which contained all classes (button, top-level and frame).
 * @param       class_name      The class name.
 *
 * @return      The class corresponding of class_name if exists. If not, return NULL.
 */
ei_widgetclass_t *get_class(ei_widgetclass_t *ll, ei_widgetclass_name_t class_name);

#endif //PROJETC_IG_WIDGET_MANAGER_H
