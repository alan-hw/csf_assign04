//
// The tile plugin generates an image containing an N x N arrangement of tiles, //each tile being a smaller version of the original image, and the overall resul//t image having the same dimensions as the original image. It takes one command// line parameter, an integer specifying the tiling factor N.
//

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
	(void) args; // this is just to avoid a warning about an unused parameter

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
	int row[source->width];
	int col[source->height];

	int tileWidth = source->width / factor;
	int tileHeight = source->height / factor;
	int widthRemainder = source->width % factor;
	int heightRemainder = source->height % factor;

	for (unsigned i = 0; i < source->width; i++) {
	  int groupLen = tileWidth;
	  if (widthRemainder >0){
	    groupLen++;
	    widthRemainder--;
	  }
	  for(unsigned j = i; j<i+groupLen; j++){
	    row[j] = j-i;
	  }
	  i+=(groupLen-1);
	}
	for (unsigned i = 0; i < source->height; i++) {
	  int groupLen = tileHeight;
	  if (heightRemainder >0){
	    groupLen++;
	    heightRemainder--;
	  }
	  for(unsigned j = i; j<i+groupLen; j++){
	    col[j] = j-i;
	  }
	  i+=(groupLen-1);
	}
	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

	for (unsigned i = 0; i < source->width; i++) {
	  for (unsigned j = 0; j< source->height; j++){
	    int targetRow = row[i]*factor;
	    int targetCol = col[j]*factor;
	    out->data[i*source->width+j] = source->data[targetRow*source->width+targetCol];
	  }
	}

	free(args);

	return out;
}
