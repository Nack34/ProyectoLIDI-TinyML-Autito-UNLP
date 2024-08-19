#include <stdio.h>
#include <stdlib.h>

// Function prototypes
int read_raw_image(const char *filename, unsigned char ***image, int rows, int cols);
int write_raw_image(const char *filename, unsigned char **image, int rows, int cols);
void free_image(unsigned char **image, int rows);
unsigned char **allocate_image(int rows, int cols);
int threshold_image(unsigned char **image, int rows, int cols, int threshold, unsigned char ***imageout);

int main(int argc, char *argv[]) {
    char *input_filename = NULL, *output_filename = NULL;
    unsigned char **image = NULL, **imageout = NULL;
    int threshold;
    int rows, cols;

    if (argc != 6) {
        fprintf(stderr, "\n\n");
        fprintf(stderr, "********************************************************************************\n");
        fprintf(stderr, "This program will threshold an image. Thresholding is a process of examining\n");
        fprintf(stderr, "the value of each pixel in an image and comparing it to a threshold value. If\n");
        fprintf(stderr, "the pixel value is less than the threshold, the pixel value is set to zero and\n");
        fprintf(stderr, "if it is above the threshold value it is set to 255.\n");
        fprintf(stderr, "********************************************************************************\n");
        fprintf(stderr, "\n<USAGE> %s inputimage.raw rows cols threshold outputimage.raw\n", argv[0]);
        fprintf(stderr, "   inputimage.raw   - The name of a RAW file to process.\n");
        fprintf(stderr, "   rows             - Number of rows in the image.\n");
        fprintf(stderr, "   cols             - Number of columns in the image.\n");
        fprintf(stderr, "   threshold        - An integer in the range 0->255.\n");
        fprintf(stderr, "   outputimage.raw  - The name of a file to store the thresholded image.\n\n");
        exit(1);
    }

    input_filename = argv[1];
    rows = atoi(argv[2]);
    cols = atoi(argv[3]);
    threshold = atoi(argv[4]);
    output_filename = argv[5];

    // Check to see that the threshold value is in the proper range.
    if ((threshold < 0) || (threshold > 255)) {
        fprintf(stderr, "\nError! The threshold value of %d is not in the range 0->255.\n\n", threshold);
        exit(1);
    }

    // Read in the image from the file.
    if (read_raw_image(input_filename, &image, rows, cols) == 0) exit(1);

    // Call the function to threshold the image.
    if (threshold_image(image, rows, cols, threshold, &imageout) == 0) exit(1);

    // Free the input image because it is no longer needed.
    free_image(image, rows);

    // Write the thresholded image out to a file.
    if (write_raw_image(output_filename, imageout, rows, cols) == 0) exit(1);

    // Free the thresholded image because it is no longer needed.
    free_image(imageout, rows);

    return 0;
}

// Function to read a RAW image file
int read_raw_image(const char *filename, unsigned char ***image, int rows, int cols) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    *image = allocate_image(rows, cols);
    if (!*image) {
        fclose(file);
        return 0;
    }

    for (int r = 0; r < rows; r++) {
        if (fread((*image)[r], sizeof(unsigned char), cols, file) != cols) {
            fprintf(stderr, "Error reading image data.\n");
            fclose(file);
            free_image(*image, rows);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

// Function to write a RAW image file
int write_raw_image(const char *filename, unsigned char **image, int rows, int cols) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    for (int r = 0; r < rows; r++) {
        if (fwrite(image[r], sizeof(unsigned char), cols, file) != cols) {
            fprintf(stderr, "Error writing image data.\n");
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

// Function to allocate memory for a 2D image
unsigned char **allocate_image(int rows, int cols) {
    unsigned char **image = (unsigned char **)malloc(rows * sizeof(unsigned char *));
    if (!image) return NULL;

    for (int i = 0; i < rows; i++) {
        image[i] = (unsigned char *)malloc(cols * sizeof(unsigned char));
        if (!image[i]) {
            for (int j = 0; j < i; j++) free(image[j]);
            free(image);
            return NULL;
        }
    }

    return image;
}

// Function to free memory for a 2D image
void free_image(unsigned char **image, int rows) {
    for (int i = 0; i < rows; i++) {
        free(image[i]);
    }
    free(image);
}

// Function to threshold an image
int threshold_image(unsigned char **image, int rows, int cols, int threshold, unsigned char ***imageout) {
    unsigned char **imagetmp = NULL;

    if ((imagetmp = allocate_image(rows, cols)) == NULL) return 0;
    *imageout = imagetmp;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            imagetmp[r][c] = (image[r][c] < threshold) ? 0 : 255;
        }
    }

    return 1;
}
