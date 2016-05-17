#pragma once

#include "common.h"
#include "Pool.h"

class TrackerCamera;
class Client;
class Layer;

class Camera {
public:
    static int id_gen;
    int id;
	Vec3 loc;
	Vec3 look_at, look_up;
    TrackerCamera *tracker;
    Client *remote_client;
    ObjectPool<Layer> target_layers;
	Camera() : tracker(0), remote_client(0) { id = id_gen++; }
    Camera(Client *cl) : tracker(0), remote_client(cl) { id = id_gen++; }
	inline void setLoc(Vec2 lc) {
		loc.x = lc.x;
		loc.y = lc.y;
		loc.z = 0;
	}
	inline void setLoc(Vec3 lc) {
		loc = lc;
	}
	inline void setLoc(float x,float y){
		loc.x = x;
		loc.y = y;
	}
	inline void setLoc( float x, float y, float z ) {
		loc.x = x; loc.y = y; loc.z = z;
	}
	inline void setLookAt( Vec3 at, Vec3 up ) {
		look_at = at;
		look_up = up;
	}
	inline Vec3 getLookAt() { return look_at; }
	static void screenToGL( int scr_x, int scr_y, int scrw, int scrh, Vec2 *out );

	Vec2 screenToWorld( int scr_x, int scr_y, int scr_w, int scr_h );
	Vec3 getDirection() { return look_at - loc; }

	inline void adjustInsideDisplay( Vec2 scrsz, Vec2 area_min, Vec2 area_max, float zoom_rate ) {
		float xsz = scrsz.x / 2 / zoom_rate;
		float ysz = scrsz.y / 2 / zoom_rate;
		float left = area_min.x + xsz;
		if( loc.x < left ) {
			loc.x = left;
		}
		float right = area_max.x - xsz;
		if( loc.x > right ) {
			loc.x = right;
		}

		float bottom = area_min.y + ysz;
		if( loc.y < bottom ) {
			loc.y = bottom;
		}
		float top = area_max.y - ysz;
		if( loc.y > top ) {
			loc.y = top;
		}
	}
    void onTrack( RemoteHead *rh ); // Send changes to all clients
    void onTrackDynamic(); // Function for dynamic_cameras. send changes to only a client.
    void addTargetLayer(Layer *l);
    void delTargetLayer(Layer *l);
};
