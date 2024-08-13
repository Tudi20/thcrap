/**
  * Touhou Community Reliant Automatic Patcher
  * Team Shanghai Alice support plugin
  *
  * ----
  *
  * libpng extensions.
  */

#include <thcrap.h>
#include <png.h>
#include "png_ex.h"

int png_image_new(
	png_image_ex &image,
	const png_uint_32 w,
	const png_uint_32 h,
	const png_uint_32 format
)
{
	size_t image_size;
	png_image_clear(image);

	image.img.version = PNG_IMAGE_VERSION;
	image.img.width = w;
	image.img.height = h;
	image.img.format = format;

	image_size = PNG_IMAGE_SIZE(image.img);
	image.buf = (png_bytep)malloc(image_size);
	if(image.buf) {
		ZeroMemory(image.buf, image_size);
	}
	return image.buf == NULL;
}

int png_image_resize(
	png_image_ex &image,
	const size_t new_w,
	const size_t new_h
)
{
	size_t prev_w;
	size_t prev_h;
	size_t prev_stride;
	size_t prev_size;
	size_t new_size;
	size_t new_stride;
	png_bytep new_buf;

	if(!image.buf) {
		return -1;
	}

	prev_w = image.img.width;
	prev_h = image.img.height;

	if(prev_w >= new_w && prev_h >= new_h) {
		return 0;
	}

	prev_stride = PNG_IMAGE_ROW_STRIDE(image.img);
	prev_size = PNG_IMAGE_SIZE(image.img);

	image.img.width = new_w;
	image.img.height = new_h;
	new_stride = PNG_IMAGE_ROW_STRIDE(image.img);
	new_size = PNG_IMAGE_SIZE(image.img);

	new_buf = (png_bytep)realloc(image.buf, new_size);
	if(new_buf) {
		image.buf = new_buf;
		if(new_size > prev_size)
			ZeroMemory(image.buf + prev_size, new_size - prev_size);

		// If the width has changed, we need to move all rows
		// in the image, going from bottom to top.
		// (Luckily, this is the only case we have to implement.)
		if(new_w > prev_w && new_h >= prev_h) {
			png_bytep in = image.buf + (prev_stride * (prev_h - 1));
			png_bytep out = image.buf + (new_stride * (prev_h - 1));
			size_t i;
			for(i = 0; i < prev_h; i++, out -= new_stride, in -= prev_stride) {
				memmove(out, in, prev_stride);
				ZeroMemory(out + prev_stride, new_stride - prev_stride);
			}
		}
		return 0;
	}
	// Memory allocation failure
	return 1;
}

int png_image_store(const char *fn, png_image_ex &image)
{
	int ret = -1;
	if(image.img.version == PNG_IMAGE_VERSION && fn) {
		FILE *fp = fopen_u(fn, "wb");
		if(fp != NULL) {
			ret = !png_image_write_to_stdio(
				&image.img, fp, 0, image.buf, 0, image.palette
			);
			if(ret) {
				log_func_printf("Error writing %s (%s).", fn, image.img.message);
			}
			fflush(fp);
			fclose(fp);
		} else {
			log_func_printf(
				"Couldn't open %s for writing (%s)\n",
				fn, strerror(errno)
			);
			ret = 2;
		}
	}
	return ret;
}

int png_image_clear(png_image_ex &image)
{
	SAFE_FREE(image.buf);
	SAFE_FREE(image.palette);
	png_image_free(&image.img);
	ZeroMemory(&image.img, sizeof(png_image));
	return 0;
}
