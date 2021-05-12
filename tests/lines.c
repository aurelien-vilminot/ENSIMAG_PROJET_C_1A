#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_draw.h"
#include "ei_types.h"
#include "ei_event.h"
#include "../src/ei_create_button.h"


/* test_line --
 *
 *	Draws a simple line in the canonical octant, that is, x1>x0 and y1>y0, with
 *	dx > dy. This can be used to test a first implementation of Bresenham
 *	algorithm, for instance.
 */
void test_line(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_color_t		color		= { 255, 0, 255, 255 };
	ei_linked_point_t	pts[2];

	pts[0].point.x = 200; pts[0].point.y = 200; pts[0].next = &pts[1];
	pts[1].point.x = 600; pts[1].point.y = 400; pts[1].next = NULL;
	
	ei_draw_polyline(surface, pts, color, clipper);
}



/* test_octogone --
 *
 *	Draws an octogone in the middle of the screen. This is meant to test the
 *	algorithm that draws a polyline in each of the possible octants, that is,
 *	in each quadrant with dx>dy (canonical) and dy>dx (steep).
 */
void test_octogone(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_color_t		color		= { 0, 255, 0, 255 };
	ei_linked_point_t	pts[9];
	int			i, xdiff, ydiff;

	/* Initialisation */
	pts[0].point.x = 400;
	pts[0].point.y = 90;

	/* Draw the octogone */
	for(i = 1; i <= 8; i++) {
		 /*	Add or remove 70/140 pixels for next point
			The first term of this formula gives the sign + or - of the operation
			The second term is 2 or 1, according to which coordinate grows faster
			The third term is simply the amount of pixels to skip */
		xdiff = pow(-1, (i + 1) / 4) * pow(2, (i / 2) % 2 == 0) * 70;
		ydiff = pow(-1, (i - 1) / 4) * pow(2, (i / 2) % 2) * 70;

		pts[i].point.x = pts[i-1].point.x + xdiff;
		pts[i].point.y = pts[i-1].point.y + ydiff;
		pts[i-1].next = &(pts[i]);
	}

	/* End the linked list */
	pts[i-1].next = NULL;

	/* Draw the form with polylines */
	ei_draw_polygon(surface, pts, color, clipper);
}



/* test_square --
 *
 *	Draws a square in the middle of the screen. This is meant to test the
 *	algorithm for the special cases of horizontal and vertical lines, where
 *	dx or dy are zero
 */
void test_square(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_color_t		color		= { 255, 0, 0, 255 };
	ei_linked_point_t	pts[5];
	int			i, xdiff, ydiff;

	/* Initialisation */
	pts[0].point.x = 300;
	pts[0].point.y = 400;

	/* Draw the square */
	for(i = 1; i <= 4; i++) {
		/*	Add or remove 200 pixels or 0 for next point
			The first term of this formula gives the sign + or - of the operation
			The second term is 0 or 1, according to which coordinate grows
			The third term is simply the side of the square */
		xdiff = pow(-1, i / 2) * (i % 2) * 200;
		ydiff = pow(-1, i / 2) * (i % 2 == 0) * 200;

		pts[i].point.x = pts[i-1].point.x + xdiff;
		pts[i].point.y = pts[i-1].point.y + ydiff;
		pts[i-1].next = &(pts[i]);
	}

	/* End the linked list */
	pts[i-1].next = NULL;

	/* Draw the form with polylines */
	//ei_draw_polyline(surface, pts, color, clipper);
        ei_draw_polygon(surface, pts, color, clipper);
}



/* test_dot --
 *
 *	Draws a dot in the middle of the screen. This is meant to test the special 
 *	case when dx = dy = 0
 */
void test_dot(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_color_t		color		= { 0, 0, 0, 255 };
	ei_linked_point_t	pts[3];

	pts[0].point.x = 400; pts[0].point.y = 300; pts[0].next = &(pts[1]);
	pts[1].point.x = 400; pts[1].point.y = 300; pts[1].next = NULL;

	ei_draw_polyline(surface, pts, color, clipper);
}


/*
 * Test the ei_copy_surface function of the ei_draw.c module.
 * It creates a small yellow square, and copy it in the same surface
 * at different coordinates.
 */
void test_copy_surface(ei_surface_t surface, ei_rect_t* clipper){

        ei_color_t color1 = {0x00, 0x00, 0xff, 0xff};
        ei_color_t color2 = {0xff, 0x00, 0x00, 0xaa};
        ei_size_t src_and_dst_size = {70, 70};

        /////// Simple test ///////

        // Source Elements
        ei_point_t src_first_point = {30, 30};
        ei_rect_t src_rect = {src_first_point, src_and_dst_size};

        // Destination Elements
        ei_point_t dst_first_point = {200, 200};
        ei_rect_t dst_rect = {dst_first_point, src_and_dst_size};

        // Test copy_surface with a simple line
        ei_linked_point_t	pts[2];
        pts[0].point.x = 30; pts[0].point.y = 30; pts[0].next = &pts[1];
        pts[1].point.x = 100; pts[1].point.y = 100; pts[1].next = NULL;
        ei_draw_polyline(surface, pts, color1, clipper);

        // Testing the good execution of copy_surface
        assert(ei_copy_surface(surface, &dst_rect, surface, &src_rect, EI_TRUE)==0);

        /////// More Difficult ////////

        // Square polyline
        ei_linked_point_t pts_2[5];
        pts_2[0].point.x = 300; pts_2[0].point.y = 300; pts_2[0].next = &pts_2[1];
        pts_2[1].point.x = 350; pts_2[1].point.y = 300; pts_2[1].next = &pts_2[2];
        pts_2[2].point.x = 350; pts_2[2].point.y = 350; pts_2[2].next = &pts_2[3];
        pts_2[3].point.x = 300; pts_2[3].point.y = 350; pts_2[3].next = &pts_2[4];
        pts_2[4].point.x = 300; pts_2[4].point.y = 300; pts_2[4].next = NULL;
        ei_draw_polyline(surface, pts, color2, clipper);

        // Source and Destination Elements
        src_first_point.x = 300; src_first_point.y = 300;
        dst_first_point.x = 450; dst_first_point.y = 450;
        src_rect.top_left = src_first_point;
        src_rect.size = src_and_dst_size;
        dst_rect.top_left = dst_first_point;
        dst_rect.size = src_and_dst_size;

        // Test
        assert(ei_copy_surface(surface, &dst_rect, surface, &src_rect, EI_FALSE)==0);

        /////// Test Incoherent Sizes ///////
        dst_rect.size.width = 100;
        assert(ei_copy_surface(surface, &dst_rect, surface, &src_rect, EI_FALSE)==1);

}

// TODO : batterie de tests complets
void test_arc(ei_surface_t surface, ei_rect_t* clipper) {
        ei_point_t center = {200, 200};
        uint32_t radius = 110;
        double_t begin_angle = 0;
        double_t end_angle = M_PI;

        ei_color_t color = {0xff, 0x00, 0x00, 0xff};

        ei_linked_point_t *pts = arc(center, radius, begin_angle, end_angle);
        ei_draw_polyline(surface, pts, color, clipper);

        ///// More Difficult /////

        center.x = 300;
        center.y = 300;
        radius = 300;
        begin_angle = 0;
        end_angle = 3*M_PI/2;

        color.red = 0x00;



        pts = arc(center, radius, begin_angle, end_angle);
        ei_draw_polyline(surface, pts, color, clipper);

        begin_angle = 3*M_PI/2;
        end_angle = 2*M_PI;

        color.red = 0xf0;
        color.blue = 0xf0;

        pts = arc(center, radius, begin_angle, end_angle); //draw the complementary of 2nd circle
        ei_draw_polyline(surface, pts, color, clipper);

}

// TODO : batterie de tests complets
void test_rounded_frame(ei_surface_t surface, ei_rect_t* clipper) {
        // Display a button

        ei_rect_t rect_border = ei_rect(ei_point(20, 20), ei_size(300, 200));
        ei_rect_t rect_center = ei_rect(ei_point(25, 25), ei_size(290, 190));
        uint32_t radius = 30;

        ei_color_t top_color = {0xB2, 0xB2, 0xB2, 0xff};
        ei_color_t bottom_color = {0x4E, 0x4E, 0x4E, 0x4E};
        ei_color_t center_color = {0x7f, 0x7f, 0x7f, 0xff};

        ei_linked_point_t *top = rounded_frame(rect_border, radius, TOP);
        ei_linked_point_t *bottom = rounded_frame(rect_border, radius, BOTTOM);
        ei_linked_point_t *center = rounded_frame(rect_center, radius, FULL);

        ei_draw_polygon(surface, top, top_color, clipper);
        ei_draw_polygon(surface, bottom, bottom_color, clipper);
        ei_draw_polygon(surface, center, center_color, clipper);

        // Separate border and center rectangles

        rect_border = ei_rect(ei_point(300, 300), ei_size(300, 200));
        rect_center = ei_rect(ei_point(400, 400), ei_size(290, 190));
        radius = 90;

        top = rounded_frame(rect_border, radius, TOP);
        bottom = rounded_frame(rect_border, radius, BOTTOM);
        center = rounded_frame(rect_center, radius, FULL);

        ei_draw_polygon(surface, top, top_color, clipper);
        ei_draw_polygon(surface, bottom, bottom_color, clipper);
        ei_draw_polygon(surface, center, center_color, clipper);
}

void test_draw_text(ei_surface_t surface, ei_rect_t* clipper){

        // Parameters to write the text
        ei_color_t color = {0xff, 0x00, 0x00, 0xff};
        ei_font_t font = hw_text_font_create("misc/font.ttf", ei_style_bold, 100);
        ei_point_t where_write_text = { 100 , 100 };

        // Write the text and free the font
        ei_draw_text (surface, &where_write_text, "C en Y", font, color, NULL);
        hw_text_font_free(font);
}

/*
 * ei_main --
 *
 *	Main function of the application.
 */
int main(int argc, char** argv)
{
	ei_size_t		win_size	= ei_size(800, 600);
	ei_surface_t		main_window	= NULL;
	ei_color_t		white		= { 0xff, 0xff, 0xff, 0xff };
	ei_rect_t*		clipper_ptr	= NULL;
	ei_rect_t		clipper		= ei_rect(ei_point(0, 0), ei_size(5000, 5000));
	clipper_ptr		= &clipper;
	ei_event_t		event;

	hw_init();
		
	main_window = hw_create_window(win_size, EI_FALSE);
	
	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);
	ei_fill		(main_window, &white, clipper_ptr);

	/* Draw polylines. */
//	test_line	(main_window, clipper_ptr);
//	test_octogone	(main_window, clipper_ptr);
//	test_square	(main_window, clipper_ptr);
//	test_dot	(main_window, clipper_ptr);
//
        /* Test copy surface */
//        test_copy_surface(main_window, clipper_ptr);

        /* Test ei_text_draw */
//        test_draw_text(main_window, clipper_ptr);

        /* Test arc */
//        test_arc(main_window, clipper_ptr);

        /* Test rounded_frame */
        test_rounded_frame(main_window, clipper_ptr);

	/* Unlock and update the surface. */
	hw_surface_unlock(main_window);
	hw_surface_update_rects(main_window, NULL);
	
	/* Wait for a character on command line. */
	event.type = ei_ev_none;
	while (event.type != ei_ev_keydown)
		hw_event_wait_next(&event);

	hw_quit();
	return (EXIT_SUCCESS);
}
