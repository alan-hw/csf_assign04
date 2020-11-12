//
// The mirrorv plugin generates a mirror image of the input image, with all pixe// ls being reflected vertically. It does not take any command line parameters. 
//

#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	// This plugin doesn't accept any command line arguments;
	// just define a single dummy field.
	int dummy;
};

const char *get_plugin_name(void) {
	return "mirrorv";
}

const char *get_plugin_desc(void) {
	return "mirror image vertically";
}

void *parse_arguments(int num_args, char *args[]) {
	(void) args; // this is just to avoid a warning about an unused parameter

	if (num_args != 0) {
		return NULL;
	}
	return calloc(1, sizeof(struct Arguments));
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = arg_data;

	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

	//for an horizontal flip, the pixel situated at coordinate (x, y) will b        //e situated at coordinate (x, height-1-y) in the new image.
	for (unsigned i = 0; i < source->width; i++) {
	  for (unsigned j = 0; j < source->height; j++){
		out->data[i*source->width+j] = source->data[i*source->width+source->height-1-j];
	  }
	}


	free(args);

	return out;
}
