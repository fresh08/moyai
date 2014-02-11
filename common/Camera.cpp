#include "Camera.h"

void Camera::screenToGL( int scr_x, int scr_y, int scrw, int scrh, Vec2 *out ) {    
	out->x = scr_x - scrw/2;
	out->y = scr_y - scrh/2;
	out->y *= -1;
}

Vec2 Camera::screenToWorld( int scr_x, int scr_y, int scr_w, int scr_h ) {
	Vec2 glpos;
	screenToGL( scr_x, scr_y, scr_w, scr_h, & glpos );
	return glpos + Vec2(loc.x,loc.y);
}