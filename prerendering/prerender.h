#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>


void prerender(const char* filename,char* file,int po );
void apply_median_filter (SDL_Surface* surf,int width,int height,int total,Uint32 * pixels);
void pixel_processing(int total,Uint32 * pixels,float avgpixel[3],const SDL_PixelFormat * format);
