//#include "prerender.h"
#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>




void prerender(const char* filename)
{
	
SDL_Surface * surf
 =	IMG_Load(filename);

float avgpixel[3] = {0.0f, 0.0f, 0.0f};

 

int height = surf->h;
int width = surf->w;

const SDL_PixelFormat * format = surf->format;
Uint32* pixels = surf->pixels;


for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Uint32 pixel = pixels[(i * width) + j];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, format, &r, &g, &b);

            avgpixel[0] += r;
            avgpixel[1] += g;
            avgpixel[2] += b;
        }
    }

    int total = height * width;
    avgpixel[0] /= total;
    avgpixel[1] /= total;
    avgpixel[2] /= total;




for (int i =0 ; i<height;i++)
{
	for(int j=0; j<width;j++)
	{
		Uint32 pixel = pixels[(i*width) + j];
			
		Uint8  r;
		Uint8  g;
		Uint8  b;
		SDL_GetRGB( pixel,format,&r,&g,&b);
		if(r<(int)avgpixel[0]|| (g)<(int)avgpixel[1]||(b)<(int)avgpixel[2])
		{
			pixels[(i * width) + j]=SDL_MapRGB(format,0, 0, 0);		
		}
		else
		{
			pixels[(i * width) + j]=SDL_MapRGB(format,255, 255, 255);		
		}
	}
}

SDL_SaveBMP(surf, "out__put.bmp");

SDL_FreeSurface(surf);
}
int main()
{
prerender("img.png");

return 0; 
}