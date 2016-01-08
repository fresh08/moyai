#ifdef USE_OPENGL
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#endif

#include "../common/Viewport.h"
#include "Prop2D_OGL.h"
#include "../common/FragmentShader.h"
#include "../common/AnimCurve.h"

bool Prop2D_OGL::propPoll(double dt) {
	if( prop2DPoll(dt) == false ) return false;

	// animation of index
	if(anim_curve){
		int previndex = index;
		bool finished = false;
		index = anim_curve->getIndex( accum_time - anim_start_at, &finished );
		if( index != previndex ){
			onIndexChanged(previndex);
		}
		if(finished) {
			onAnimFinished();
		}
	}
	// animation of scale
	if( seek_scl_time != 0 ){
		double elt = accum_time - seek_scl_started_at;
		if( elt > seek_scl_time ){
			scl = seek_scl_target;
			seek_scl_time = 0;
		} else {
			double rate = elt / seek_scl_time;
			scl.x = seek_scl_orig.x + ( seek_scl_target.x - seek_scl_orig.x ) * rate;
			scl.y = seek_scl_orig.y + ( seek_scl_target.y - seek_scl_orig.y ) * rate;
		}
	}
	// animation of rotation
	if( seek_rot_time != 0 ){
		double elt = accum_time - seek_rot_started_at;
		if( elt > seek_rot_time ){
			rot = seek_rot_target;
			seek_rot_time = 0;
		} else {
			double rate = elt / seek_rot_time;
			rot = seek_rot_orig + ( seek_rot_target - seek_rot_orig ) * rate;
		}
	}
	// animation of color
	if( seek_color_time != 0 ){
		double elt = accum_time - seek_color_started_at;
		if( elt > seek_color_time ){
			color = seek_color_target;
			seek_color_time = 0;
		} else {
			double rate = elt / seek_color_time;
			color = Color( seek_color_orig.r + ( seek_color_target.r - seek_color_orig.r ) * rate,
				seek_color_orig.g + ( seek_color_target.g - seek_color_orig.g ) * rate,
				seek_color_orig.b + ( seek_color_target.b - seek_color_orig.b ) * rate,
				seek_color_orig.a + ( seek_color_target.a - seek_color_orig.a ) * rate );
		}
	}

	// children
	for(int i=0;i<children_num;i++){
		Prop2D_OGL *p = children[i];
		p->basePoll(dt);
	}

	return true;
}

void Prop2D_OGL::render(Camera *cam, DrawBatchList *bl ) {
	assertmsg(deck || grid_used_num > 0 || children_num > 0 || prim_drawer , "no deck/grid/prim_drawer is set. deck:%p grid:%d child:%d prim:%p", deck, grid_used_num, children_num, prim_drawer );
    
	float camx=0.0f;
	float camy=0.0f;
	if(cam){
		camx = cam->loc.x;
		camy = cam->loc.y;
	}

	if( children_num > 0 && render_children_first ){
		for(int i=0;i<children_num;i++){
			Prop2D_OGL *p = (Prop2D_OGL*) children[i];
            if( p->visible ) p->render( cam, bl );
		}
	}

	if( grid_used_num > 0 ){
		glEnable(GL_TEXTURE_2D);
		glColor4f(color.r,color.g,color.b,color.a);        
		for(int i=0;i<grid_used_num;i++){
			Grid *grid = grids[i];
			if(!grid)break;
			if(!grid->visible)continue;
            if(!grid->index_table)continue;

			TileDeck *draw_deck = deck;
			if( grid->deck ) draw_deck = grid->deck;
			if( grid->fragment_shader ){
				glUseProgram(grid->fragment_shader->program );
				grid->fragment_shader->updateUniforms();
			}

            if(!grid->mesh) {
                print("new grid mesh! wh:%d,%d", grid->width, grid->height );
                grid->mesh = new Mesh();
                VertexFormat *vf = DrawBatch::getVertexFormat( VFTYPE_COORD_COLOR_UV );
                IndexBuffer *ib = new IndexBuffer();
                VertexBuffer *vb = new VertexBuffer();
                vb->setFormat(vf);
                /*
                  3+--+--+--+--+
                   |  |  |  |  |
                  2+--+--+--+--+
                   |  |  |  |  |
                  1+--+--+--+--+
                   |  |  |  |  |
                  0+--+--+--+--+
                   0  1  2  3  4
                 */
                int quad_num = grid->width * grid->height;
                int triangle_num = quad_num * 2;
                int vert_num = quad_num * 4; // Can't share vertices because each vert has different UVs 
                vb->reserve( vert_num);
                ib->reserve( triangle_num*3 );
                grid->mesh->setVertexBuffer(vb);
                grid->mesh->setIndexBuffer(ib);
                grid->mesh->setPrimType( GL_TRIANGLES );

                grid->uv_changed = true;
                grid->color_changed = true;
            }

            if( grid->uv_changed || grid->color_changed ) {
                if(grid->debug) {
                    print("debug:%d Grid changed: uv:%d col:%d", grid->debug, grid->uv_changed, grid->color_changed );
                }
                grid->uv_changed = false;
                grid->color_changed = false;
                
                VertexBuffer *vb = grid->mesh->vb;
                IndexBuffer *ib = grid->mesh->ib;
                vb->unbless();
                ib->unbless();

                int quad_cnt=0;
                for(int y=0;y<grid->height;y++) {
                    for(int x=0;x<grid->width;x++) {
                        int ind = x+y*grid->width;
                        if(grid->debug) prt("%3d ", grid->index_table[ind] );
                        if( grid->index_table[ind] == Grid::GRID_NOT_USED ) continue;
                        
                        
                        Vec2 left_bottom, right_top;
                        float u0,v0,u1,v1;
                        draw_deck->getUVFromIndex( grid->index_table[ind], &u0,&v0,&u1,&v1,0,0,0);
                        if(grid->texofs_table) {
                            u0 += grid->texofs_table[ind].x;
                            v0 += grid->texofs_table[ind].y;
                            u1 += grid->texofs_table[ind].x;
                            v1 += grid->texofs_table[ind].y;
                        }

                        //
                        // Q (u0,v0) - R (u1,v0)      top-bottom upside down.
                        //      |           |
                        //      |           |                        
                        // P (u0,v1) - S (u1,v1)
                        //
                        if(grid->xflip_table && grid->xflip_table[ind]) {
                            swapf( &u0, &u1 );
                        }
                        if(grid->yflip_table && grid->yflip_table[ind]) {
                            swapf( &v0, &v1 );
                        }
                        Vec2 uv_p(u0,v1), uv_q(u0,v0), uv_r(u1,v0), uv_s(u1,v1);

                        
                        // left bottom
                        const float d = 1;
                        int vi = quad_cnt * 4;
                        vb->setCoord(vi,Vec3(d*x,d*y,0));
                        if(grid->rot_table && grid->rot_table[ind]) vb->setUV(vi,uv_s); else vb->setUV(vi,uv_p);
                        if(grid->color_table) vb->setColor(vi, grid->color_table[ind]); else vb->setColor(vi, Color(1,1,1,1));
                        // right bottom
                        vb->setCoord(vi+1,Vec3(d*(x+1),d*y,0));
                        if(grid->rot_table && grid->rot_table[ind]) vb->setUV(vi+1,uv_r); else vb->setUV(vi+1,uv_s);                        
                        if(grid->color_table) vb->setColor(vi+1,grid->color_table[ind]); else vb->setColor(vi+1, Color(1,1,1,1));
                        // left top
                        vb->setCoord(vi+2,Vec3(d*x,d*(y+1),0));
                        if(grid->rot_table && grid->rot_table[ind]) vb->setUV(vi+2,uv_p); else vb->setUV(vi+2,uv_q);
                        if(grid->color_table) vb->setColor(vi+2,grid->color_table[ind]); else vb->setColor(vi+2, Color(1,1,1,1));
                        // right top
                        vb->setCoord(vi+3,Vec3(d*(x+1),d*(y+1),0));
                        if(grid->rot_table && grid->rot_table[ind]) vb->setUV(vi+3,uv_q); else vb->setUV(vi+3,uv_r);
                        if(grid->color_table) vb->setColor(vi+3,grid->color_table[ind]); else vb->setColor(vi+3, Color(1,1,1,1));

                        // TODO: no need to update index every time it changes.
                       
                        int indi = quad_cnt * 6; // 2 triangles = 6 verts per quad
                        ib->setIndex(indi++, quad_cnt*4+0 );
                        ib->setIndex(indi++, quad_cnt*4+2 );
                        ib->setIndex(indi++, quad_cnt*4+1 );
                        ib->setIndex(indi++, quad_cnt*4+1 );
                        ib->setIndex(indi++, quad_cnt*4+2 );
                        ib->setIndex(indi++, quad_cnt*4+3 );

                        quad_cnt++; // next quad!
                    }
                    if(grid->debug) print("");
                }
                ib->setRenderLen(quad_cnt*6);
            } 

            
            // draw
            if(!draw_deck) {
                print("no tex? (grid)");
                continue;
            }
            //            print("ggg:%p %p %f,%f", grid->mesh, draw_deck, loc.x, loc.y );
            //drawMesh( grid->mesh, draw_deck->tex->tex, Vec2(camx,camy) );
            FragmentShader *fs = fragment_shader;
            if( grid->fragment_shader ) fs = grid->fragment_shader;
            bl->appendMesh( getViewport(), fs, getBlendType(), draw_deck->tex->tex, loc - Vec2(camx,camy), scl, rot, grid->mesh );
		}
	}


            
	if(deck && index >= 0 ){
        float u0,v0,u1,v1;
        deck->getUVFromIndex( index, &u0,&v0,&u1,&v1,0,0,0);
        bl->appendSprite1( getViewport(),
                           fragment_shader,
                           getBlendType(),
                           deck->tex->tex,
                           color,
                           loc - Vec2(camx,camy),
                           scl,
                           rot,
                           Vec2(u0,v1),
                           Vec2(u1,v0)
                           );
        //        drawMesh( mesh, deck->tex->tex, Vec2(camx,camy) );
	}

	if( children_num > 0 && (render_children_first == false) ){
		for(int i=0;i<children_num;i++){
			Prop2D_OGL *p = (Prop2D_OGL*) children[i];
			if(p->visible) p->render( cam, bl );
		}
	}

	// primitives should go over image sprites
	if( prim_drawer ){
        glDisable( GL_TEXTURE_2D );
        glLoadIdentity();
        glTranslatef( loc.x - camx, loc.y - camy, 0 );
        glRotatef( rot * (180.0f/M_PI), 0,0,1);
        glScalef( scl.x, scl.y, 1 );
		prim_drawer->drawAll( bl, getViewport(), loc - Vec2(camx,camy),scl,rot);
	}
}
void GLBINDTEXTURE( GLuint tex ) {
    static GLuint last_tex;
    if(tex!=last_tex) {
        glBindTexture( GL_TEXTURE_2D, tex );
        last_tex = tex;
    }
}




Prop *Prop2D_OGL::getNearestProp(){
	Prop *cur = parent_group->prop_top;
	float minlen = 999999999999999.0f;
	Prop *ans=0;
	while(cur){
		Prop2D_OGL *cur2d = (Prop2D_OGL*)cur;        
		if( cur2d->dimension == DIMENSION_2D ) {
			float l = cur2d->loc.len(loc);
			if(l<minlen && cur != this ){
				ans = cur;
				minlen = l;
			}
		}
		cur = cur->next;
	}
	return ans;
}

void Prop2D_OGL::updateMinMaxSizeCache(){
	max_rt_cache.x=0;
	max_rt_cache.y=0;
	min_lb_cache.x=0;
	min_lb_cache.y=0;

	float grid_max_x=0, grid_max_y=0;
	if(grid_used_num>0){
		for(int i=0;i<grid_used_num;i++){
			Grid *g = grids[i];
			float maxx = g->width * scl.x;
			if(maxx>grid_max_x )grid_max_x = maxx;
			float maxy = g->height * scl.y;
			if(maxy>grid_max_y )grid_max_y = maxy;
		}
	}
	if( grid_max_x > max_rt_cache.x ) max_rt_cache.x = grid_max_x;
	if( grid_max_y > max_rt_cache.y ) max_rt_cache.y = grid_max_y;

	//

	float child_max_x=0, child_max_y = 0;
	if( children_num > 0){
		for(int i=0;i<children_num;i++){
			Prop *p = children[i];
			float maxx = ((Prop2D_OGL*)p)->scl.x/2;
			float maxy = ((Prop2D_OGL*)p)->scl.y/2;
			if( maxx > child_max_x ) child_max_x = maxx;
			if( maxy > child_max_y ) child_max_y = maxy;
		}
	}
	if( child_max_x > max_rt_cache.x ) max_rt_cache.x = child_max_x;
	if( child_max_y > max_rt_cache.y ) max_rt_cache.y = child_max_y;

	if( prim_drawer ){
		Vec2 minv, maxv;
		prim_drawer->getMinMax( &minv, &maxv );
		if( minv.x < min_lb_cache.x ) min_lb_cache.x = minv.x;
		if( minv.y < min_lb_cache.y ) min_lb_cache.y = minv.y;
		if( maxv.x > max_rt_cache.x ) max_rt_cache.x = maxv.x;
		if( maxv.y > max_rt_cache.y ) max_rt_cache.y = maxv.y;
	}
}


#endif
