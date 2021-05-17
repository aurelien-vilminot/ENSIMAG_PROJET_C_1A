#ifndef PROJETC_IG_WIDGET_MANAGER_H
#define PROJETC_IG_WIDGET_MANAGER_H

#include <string.h>

#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "application.h"

typedef struct ei_top_level_t {
        ei_widget_t		widget;
        ei_color_t*		color;
        int*			border_width;
        char**			title;
        ei_bool_t*		closable;
        ei_axis_set_t*		resizable;
        ei_size_t**		min_size;
} ei_top_level_t;

typedef struct ei_button_t {
        ei_widget_t		widget;
        const ei_color_t*	color;
        int*			border_width;
        int*			corner_radius;
        ei_relief_t*		relief;
        char**			text;
        ei_font_t*		text_font;
        ei_color_t*		text_color;
        ei_anchor_t*		text_anchor;
        ei_surface_t*		img;
        ei_rect_t**		img_rect;
        ei_anchor_t*		img_anchor;
        ei_callback_t*		callback;
        void**			user_param;
} ei_button_t;

typedef struct ei_frame_t {
        ei_widget_t		widget;
        const ei_color_t*	color;
        int*			border_width;
        ei_relief_t*		relief;
        char**			text;
        ei_font_t*		text_font;
        ei_color_t*		text_color;
        ei_anchor_t*		text_anchor;
        ei_surface_t*		img;
        ei_rect_t**		img_rect;
        ei_anchor_t*		img_anchor;
} ei_frame_t;

ei_widgetclass_t *get_class(ei_widgetclass_t *ll, ei_widgetclass_name_t class_name);


/**
 * Allocation memory
 * @return
 */
ei_widget_t* button_alloc_func();
ei_widget_t* top_level_alloc_func();
ei_widget_t* frame_alloc_func();

/**
 * Release memory
 * @return
 */
void button_release(ei_widget_t* widget);
void top_level_release(ei_widget_t* widget);
void frame_release(ei_widget_t* widget);

/**
 * Draw functions
 */

void ei_draw_button (ei_widget_t* widget,
                 ei_surface_t		surface,
                 ei_surface_t		pick_surface,
                 ei_rect_t*		clipper);

void ei_draw_frame (ei_widget_t* widget,
                    ei_surface_t		surface,
                    ei_surface_t		pick_surface,
                    ei_rect_t*		clipper);

void ei_draw_top_level (ei_widget_t* widget,
                    ei_surface_t		surface,
                    ei_surface_t		pick_surface,
                    ei_rect_t*		clipper);

void set_default_button (ei_widget_t *widget);
void set_default_frame (ei_widget_t *widget);
void set_default_top_level (ei_widget_t *widget);

/**
 * Default values of widget parameters
 */

static ei_size_t default_button_size = {50, 30};
static ei_color_t default_button_color = {0x7C, 0x87, 0x8E, 0xff};
static int default_button_border_width = 5;
static int default_button_corner_radius = 10;
static ei_anchor_t default_button_anchor = ei_anc_center;

#endif //PROJETC_IG_WIDGET_MANAGER_H
