#include "prerender.h"
#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>
void apply_median_filter (SDL_Surface* surf,int width,int height,int total,Uint32 * pixels)
{Uint32* new_pixels = (Uint32*)malloc(total *sizeof(Uint32));

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            // Create an array of the neighboring pixels
            Uint32 neighbors[9];
            int idx = 0;
            for (int k = -1; k <= 1; k++) {
                for (int l = -1; l <= 1; l++) {
                    neighbors[idx++] = pixels[(i + k) * width + (j + l)];
                }
            }

            // Sort the array to find the median value
            for (int x = 0; x < 9; x++) {
                for (int y = x + 1; y < 9; y++) {
                    if (neighbors[x] > neighbors[y]) {
                        Uint32 temp = neighbors[x];
                        neighbors[x] = neighbors[y];
                        neighbors[y] = temp;
                    }
                }
            }

            // Replace the pixel with the median value
            new_pixels[i * width + j] = neighbors[4]; 
        }
    }

    // Replace the original pixels with the new ones
    memcpy(pixels, new_pixels, width * height *sizeof(Uint32));
    free(new_pixels);
}








void pixel_processing(int total,Uint32 * pixels,float avgpixel[3],const SDL_PixelFormat * format)
{
for (int i = 0; i < total; i++) {

     Uint32 pixel = pixels[i];
     Uint8 r, g, b;
     SDL_GetRGB(pixel, format, &r, &g, &b);
     avgpixel[0] += r;
     avgpixel[1] += g;
     avgpixel[2] += b;

}


avgpixel[0] /= total;
avgpixel[1] /= total;
avgpixel[2] /= total;



for (int i =0 ; i<total;i++)
{
        Uint32 pixel = pixels[i];

        Uint8  r;
        Uint8  g;
        Uint8  b;
        SDL_GetRGB( pixel,format,&r,&g,&b);
        if(r<(int)avgpixel[0]|| (g)<(int)avgpixel[1]||(b)<(int)avgpixel[2])
        {
                pixels[i]=SDL_MapRGB(format,0, 0, 0);
        }
        else
        {
                pixels[i]=SDL_MapRGB(format,255, 255, 255);
        }

}




}
void prerender(const char* filename,char* file,int po )
{
SDL_Init(SDL_INIT_VIDEO);
//printf("deb\n");	
SDL_Surface * surf
 =	IMG_Load(filename);

//printf("file\n");
float avgpixel[3] = {0.0f, 0.0f, 0.0f};
//printf("loaded\n");
 

int height = surf->h;
int width = surf->w;

const SDL_PixelFormat * format = surf->format;
Uint32* pixels = surf->pixels;

int total = height * width;


apply_median_filter(surf,width,height,total,pixels);
pixel_processing(total,pixels,avgpixel,format);
//apply_median_filter(surf,width,height,total,pixels);
//pixel_processing(total,pixels,avgpixel,format);




SDL_SaveBMP(surf,file);

SDL_FreeSurface(surf);
IMG_Quit();
SDL_Quit();
}

/*
int main()
{
prerender("l2.png","l.bmp",0);
prerender("lv2.png","lv.bmp",0);


prerender("img.png","img.bmp",0);
prerender("po.png","po.bmp",0);
prerender("pot.png","pot.bmp",0);



return 0; 
}*/
