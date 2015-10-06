#include <xcb/xcb.h>
#include <stdio.h>

xcb_connection_t *c;
xcb_screen_t *screen;

unsigned int window;
unsigned int blackContext;
unsigned int whiteContext;

void VID_SetPixel( int x, int y, char color ) {
	xcb_point_t p[] = { {x, y} };
	int context;
	if ( color == 0 ) {
		context = blackContext;
	} else {
		context = whiteContext;
	}

	xcb_poly_point( c, XCB_COORD_MODE_ORIGIN, window, context, 1, p );
}

int VID_Init() {
	unsigned int v[2] = { 	0, 
							XCB_EVENT_MASK_EXPOSURE };

	c = xcb_connect( NULL, NULL );
	if ( xcb_connection_has_error( c ) ) {
			printf( "Cannot find display\n" );
			return -1;
	}
	screen = xcb_setup_roots_iterator( xcb_get_setup( c ) ).data;

	whiteContext = xcb_generate_id( c );
	v[0] = screen->white_pixel;
	xcb_create_gc( c, whiteContext, screen->root, XCB_GC_FOREGROUND, v );

	blackContext = xcb_generate_id( c );
	v[0] = screen->black_pixel;
	xcb_create_gc( c, blackContext, screen->root, XCB_GC_FOREGROUND, v );

	window = xcb_generate_id( c );
	xcb_create_window (		c, XCB_COPY_FROM_PARENT, window, screen->root, 
							0, 0, 512, 384, 
							0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, 
							XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, v);

	xcb_map_window ( c, window );
	return 0;
}

void VID_Cleanup() {
	xcb_disconnect( c );
}