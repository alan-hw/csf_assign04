//
// The tile plugin generates an image containing an N x N arrangement of tiles, //each tile being a smaller version of the original image, and the overall resul//t image having the same dimensions as the original image. It takes one command// line parameter, an integer specifying the tiling factor N.
//
#include <stdio.h>
#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
  // the tiling factor N
	int factor;
};

const char *get_plugin_name(void) {
	return "tile";
}

const char *get_plugin_desc(void) {
	return "tile source image in an NxN arrangement";
}

void *parse_arguments(int num_args, char *args[]) {
	if (num_args != 1||atoi(args[0])<1) {
		return NULL;
	}
	struct Arguments *arg = calloc(1, sizeof(struct Arguments));
	  arg->factor = atoi(args[0]);
	return arg;
}


struct Image *transform_image(struct Image *source, void *arg_data) {
    struct Arguments *args = arg_data;
	//load factor from arguments
	int factor = args->factor;
    // Allocate a result Image
    struct Image *out = img_create(source->width, source->height);
    if (!out) {
        printf("Memory allocation fails.\n");
        free(args);
        return NULL;
    }
    out->width = source->width;
    out->height = source->height;
    
    int tileWidth = source->width / factor;
    int tileHeight = source->height / factor;
    // if x-coordinate (j) >= switchX, then it's in a smaller tile (with no excess)
    int switchX = (source->width - tileWidth * factor) * (tileWidth + 1);
    // if y-coordinate (i) >= switchY, then it's in a smaller tile (with no excess)
    int switchY = (source->height - tileHeight * factor) * (tileHeight + 1);
    
    // sourceJ, sourceI represent the coordinates that (j,i) on output image corresponds to
    int sourceI, sourceJ;
	for (int i = 0; i < (int)source->height; i++) {
	  for (int j = 0; j < (int)source->width; j++){
          if(j >= switchX) {
              sourceJ = ((j - switchX) % (tileWidth)) * factor;
          } else{
              sourceJ = (j % (tileWidth + 1)) * factor;
          }
          if(i >= switchY) {
              sourceI = ((i - switchY) % (tileHeight)) * factor;
          } else{
              sourceI = (i % (tileHeight + 1)) * factor;
          }
	    out->data[i*source->width+j] = source->data[sourceI*source->width+sourceJ];
	  }
	}
	free(args);
	return out;
}
