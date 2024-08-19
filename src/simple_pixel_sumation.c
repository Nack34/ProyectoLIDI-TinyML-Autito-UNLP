#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>

// Function to count white pixels in a specific region
int count_white_pixels(unsigned char *image, int width, int height, int channels, int start_x, int end_x) {
    int count = 0;
    for (int y = 0; y < height; y++) {
        for (int x = start_x; x < end_x; x++) {
            int index = (y * width + x) * channels;
            if (image[index] == 255) { // White pixel
                count++;
            }
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input.jpg>\n", argv[0]);
        return 1;
    }

    const char *input_filename = argv[1];

    int width, height, channels;
    unsigned char *image = stbi_load(input_filename, &width, &height, &channels, 0);
    if (!image) {
        fprintf(stderr, "Error loading image.\n");
        return 1;
    }

    int mid_x = width / 2;
    int left_white_count = count_white_pixels(image, width, height, channels, 0, mid_x);
    int right_white_count = count_white_pixels(image, width, height, channels, mid_x, width);

    printf("White pixels on the left side: %d\n", left_white_count);
    printf("White pixels on the right side: %d\n", right_white_count);
    if (left_white_count > right_white_count) {
      printf("Girar a la izquierda");
    } else {
      printf("Girar a la derecha");
    }

    stbi_image_free(image);
    return 0;
}
