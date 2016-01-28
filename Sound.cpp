#include "Sound.h"
#include "SoundSystem.h"

Sound::Sound( SoundSystem *s) : sound(0), parent(s), ch(0), default_volume(1), external_id(0) { }
void Sound::play(){
	play(default_volume);
}

void Sound::play(float vol){
	if(vol==0)return;
	FMOD_RESULT r;
	if( !this->ch ){
		r = FMOD_System_PlaySound( parent->sys, FMOD_CHANNEL_FREE, sound, 0, & this->ch );
	} else {
		r = FMOD_System_PlaySound( parent->sys, FMOD_CHANNEL_REUSE, sound, 0, & this->ch );            
	}
	FMOD_ERRCHECK(r);
	FMOD_Channel_SetVolume(ch, default_volume * vol );
}

void Sound::playDistance(float mindist, float maxdist, float dist, float relvol) {
	if(dist<mindist) {
		play(1 * relvol);
	} else if( dist > maxdist ) {
		return;
	} else {
		float width = maxdist - mindist;
		float r = 1 - (dist - mindist) / width;
		play(r * relvol);
	}
}


void Sound::stop() {
    //    print("Sound::stop! %p debugid:%d ch:%p",this, this->debug_id ,this->ch);
	FMOD_Channel_Stop( this->ch );
    this->ch = NULL;
}
void Sound::pause( bool to_pause ) {
    FMOD_Channel_SetPaused( this->ch, to_pause );
}

bool Sound::isPlaying() {
	if(!this->ch)return false;
	FMOD_BOOL val;
	FMOD_RESULT r;
	r = FMOD_Channel_IsPlaying( this->ch, &val );
	if( r != FMOD_OK ) return false;
	return val;
}
void Sound::setVolume( float v ) {
	FMOD_Channel_SetVolume(this->ch, v );
}
float Sound::getVolume() {
    float v;
    FMOD_Channel_GetVolume(this->ch,&v);
    return v;
}
void Sound::setLoop( bool flag ) {
	if( flag ) {
		FMOD_Sound_SetMode( sound, FMOD_LOOP_NORMAL );            
	} else {
		FMOD_Sound_SetMode( sound, FMOD_LOOP_OFF );    
	}
}