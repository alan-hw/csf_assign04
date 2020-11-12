//
// The expose plugin changes all red/green/blue color component values by a specified factor.
//

#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
  //A floating point value to use as the factor. The factor must not be negative.
	float factor;
};

const char *get_plugin_name(void) {
	return "expose";
}

const char *get_plugin_desc(void) {
	return "adjust the intensity of all pixels";
}

void *parse_arguments(int num_args, char *args[]) {
	(void) args; // this is just to avoid a warning about an unused parameter

	if (num_args != 1||atof(args[0])<0) {
		return NULL;
	}

	struct Arguments *arg = calloc(1, sizeof(struct Arguments));
	  arg->factor = atof(args[0]);
	return arg;
}

// Helper function to swap the blue and green color component values.
static uint32_t change_expose(uint32_t pix, float factor) {
	uint8_t r, g, b, a;
	img_unpack_pixel(pix, &r, &g, &b, &a);
	uint32_t rnew = (uint32_t)r;
	uint32_t gnew = (uint32_t)g;
	uint32_t bnew = (uint32_t)b;
	rnew *= factor;
	gnew *= factor;
	bnew *= factor;
	if(rnew>255){
	  r=255;
	}
	else{
	  r = (uint8_t)rnew;
	}
	if(gnew>255){
	  g=255;
	}
	else{
	  g = (uint8_t)gnew;
	}
	if(bnew>255){
	  b=255;
	}
	else{
	  b = (uint8_t)bnew;
	}
	return img_pack_pixel(r, g, b, a);
}

struct Image *transform_image(struct Image *source, void *arg_data) {
  struct Arguments *args = arg_data;
	//load factor from arguments
	float factor = args->factor;
	
	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

	unsigned num_pixels = source->width * source->height;
	for (unsigned i = 0; i < num_pixels; i++) {
	  out->data[i] = change_expose(source->data[i], factor);
	}

	free(args);

	return out;
}
