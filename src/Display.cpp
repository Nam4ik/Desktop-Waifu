#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_error.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ctype.h>
#include <glib-2.0/gio/gio.h>
#include <pthread.h>

void LoadWMType(){
   
  const char * VMType = std::getenv("XDG_SESSION_TYPE");
  if (VMType != NULL){
    perror("Unable to load XDG_SESSION_TYPE");
  } 

}

int SetDBusConnection(){
    GError *error = NULL;
    GDBusConnection *connection;
    GVariant *result;
    const gchar *response; 
}
``
int main(){

}

