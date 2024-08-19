#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>

// Function to apply thresholding to an image
void apply_threshold(unsigned char *image, int width, int height, int channels, int threshold) {
    int pixel_count = width * height * channels;
    for (int i = 0; i < pixel_count; i += channels) {
        // Convert RGB to grayscale using luminance formula
        unsigned char gray = (0.299 * image[i] + 0.587 * image[i+1] + 0.114 * image[i+2]);
        
        // Apply threshold
        unsigned char binary = (gray > threshold) ? 255 : 0;
        
        // Set all color channels to binary value
        image[i] = binary;      // Red channel
        image[i+1] = binary;    // Green channel
        image[i+2] = binary;    // Blue channel
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input.jpg> <threshold> <output.jpg>\n", argv[0]);
        return 1;
    }

    const char *input_filename = argv[1];
    int threshold = atoi(argv[2]);
    const char *output_filename = argv[3];

    int width, height, channels;
    unsigned char *image = stbi_load(input_filename, &width, &height, &channels, 0);
    if (!image) {
        fprintf(stderr, "Error loading image.\n");
        return 1;
    }

    // Apply threshold
    apply_threshold(image, width, height, channels, threshold);

    // Save as JPEG
    if (!stbi_write_jpg(output_filename, width, height, channels, image, 100)) {
        fprintf(stderr, "Error saving image.\n");
        stbi_image_free(image);
        return 1;
    }

    stbi_image_free(image);
    return 0;
}
