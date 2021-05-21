#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget.h"


/*
 * button_press --
 *
 *	Callback called when a user clicks on the button.
 */
void button_press(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
	printf("Click !\n");
}

/*
 * process_key --
 *
 *	Callback called when any key is pressed by the user.
 *	Simply looks for the "Escape" key to request the application to quit.
 */
ei_bool_t process_key(ei_event_t* event)
{
	if (event->type == ei_ev_keydown)
		if (event->param.key.key_code == SDLK_ESCAPE) {
			ei_app_quit_request();
			return EI_TRUE;
		}

	return EI_FALSE;
}

/*
 * ei_main --
 *
 *	Main function of the application.
 */
int main(int argc, char** argv)
{
	ei_size_t	screen_size		= {800, 600};
	ei_color_t	root_bgcol		= {0x52, 0x7f, 0xb4, 0xff};

	ei_widget_t*	button;
	ei_anchor_t	button_anchor		= ei_anc_southeast;
	int		button_x		= -20;
	int		button_y		= -20;
	float		button_rel_x		= 1.0;
	float		button_rel_y		= 1.0;
	float		button_rel_width	= 0.5;
	ei_color_t	button_color		= {0x88, 0x88, 0x88, 0xff};
	char*		button_title		= "click";
	ei_color_t	button_text_color	= {0x00, 0x00, 0x00, 0xff};
	ei_relief_t	button_relief		= ei_relief_raised;
	int		button_border_width	= 2;
	ei_callback_t	button_callback 	= button_press;

	ei_widget_t*	window;
	ei_size_t	window_size		= {320,240};
	char*		window_title		= "Hello World";
	ei_color_t	window_color		= {0xA0,0xA0,0xA0, 0xff};
	int		window_border_width	= 2;
	ei_bool_t	window_closable		= EI_TRUE;
	ei_axis_set_t	window_resizable	= ei_axis_both;
	ei_point_t	window_position		= {30, 10};

	
	/* Create the application and change the color of the background. */
	ei_app_create(screen_size, EI_FALSE); 
	ei_frame_configure(ei_app_root_widget(), NULL, &root_bgcol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	ei_event_set_default_handle_func(process_key);

	/* Create, configure and place a toplevel window on screen. */
	window = ei_widget_create("toplevel", ei_app_root_widget(), NULL, NULL);
	ei_toplevel_configure(window, &window_size, &window_color, &window_border_width,
				&window_title, &window_closable, &window_resizable, NULL);
	ei_place(window, NULL, &(window_position.x), &(window_position.y), NULL, NULL, NULL, NULL, NULL, NULL);

	ei_widget_t*	window2;
	char*		window_title2		= "(re)Hello World";
	ei_point_t	window_position2	= {300, 100};

	/* Create, configure and place a toplevel window on screen. */
	window2 = ei_widget_create("toplevel", ei_app_root_widget(), NULL, NULL);
	ei_toplevel_configure(window2, &window_size, &window_color, &window_border_width,
				&window_title2, &window_closable, &window_resizable, NULL);
	ei_place(window2, NULL, &(window_position2.x), &(window_position2.y), NULL, NULL, NULL, NULL, NULL, NULL);

	ei_widget_t*	window3;
	char*		window_title3		= "((re)re)Hello World";
        ei_color_t	window3_color		= {0xA0,0xA0,0xAF, 0xff};
        float		widows3_rel_x		= 0.0;
        float		widows3_rel_y		= 0.0;
        float		widows3_rel_width	= 0.5;
        float		widows3_rel_height	= 0.5;

	/* Create, configure and place a toplevel window on screen. */
	window3 = ei_widget_create("toplevel", window2, NULL, NULL);
	ei_toplevel_configure(window3, &window_size, &window3_color, &window_border_width,
				&window_title3, &window_closable, &window_resizable, NULL);
	ei_place(window3, NULL, NULL, NULL, NULL, NULL, &widows3_rel_x, &widows3_rel_y, &widows3_rel_width, &widows3_rel_height);

        ei_widget_t*	frame;
        float		frame_rel_x	        = 0.0;
        float		frame_rel_y	        = 0.0;
        float		frame_rel_width	        = 1.0;
        float		frame_rel_height        = 0.8;
        char*		frame_title		= "Troucheda & Vilminoa & Arvyp";
        ei_anchor_t     frame_text_anchor       = ei_anc_west;
        ei_relief_t     frame_relief            = ei_relief_raised;
        ei_color_t	frame_color		= {0xA0,0xAF,0xA0, 0xff};
        int		frame_border_width	= 5;

	frame = ei_widget_create("frame", window, NULL, NULL);
	ei_frame_configure(frame, NULL, &frame_color, &frame_border_width, &frame_relief, &frame_title,
	        ei_default_font, &root_bgcol, &frame_text_anchor, NULL, NULL, NULL);
	ei_place(frame, NULL, NULL, NULL, NULL, NULL, &frame_rel_x, &frame_rel_y, &frame_rel_width, &frame_rel_height);

        /* Create, configure and place a button as a descendant of the toplevel window. */
	button = ei_widget_create("button", frame, NULL, NULL);
	ei_button_configure	(button, NULL, &button_color,
				 &button_border_width, NULL, &button_relief, &button_title, NULL, &button_text_color, NULL,
				NULL, NULL, NULL, &button_callback, NULL);
	ei_place(button, &button_anchor, &button_x, &button_y, NULL, NULL, &button_rel_x, &button_rel_y, &button_rel_width, NULL);

	/* Run the application's main loop. */
	ei_app_run();

	ei_app_free();

	return (EXIT_SUCCESS);
}
