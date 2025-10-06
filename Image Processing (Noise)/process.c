/* This coursework specification, and the example code provided during the
 * course, is Copyright 2025 Heriot-Watt University.
 * Distributing this coursework specification or your solution to it outside
 * the university is academic misconduct and a violation of copyright law. */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* The RGB values of a pixel. */
/* As the program works on HPDEC files, each pixel is represented as three 8-bit unsigned integers for each colour. Therefore I chose the colour's types to be
 * of 'unsigned char'. This takes only the necessary storage (8-bits) rather than 16-bits which is used for an integer so that it doesn't take extra unused storage
 * therefore saving memory. This also ensures no value below 0 and above 255 is written into the file accidentally therefore keeping its HPDEC form */
struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

/* An image loaded from a file. */
/* I have included two extra fields, 'out_file' which stores the name of the output file. I did this so that it is more efficient and easy
 * to obtain the image's output file name from 'main' especially when the user inputs multiple images. I have also added a 'next' field for Image as I
 * have made Image possible to be a linked list so when user inputs multiple files, the program can easily navigate through each of them
 * (by going to the next one) to be processed in a loop efficiently */
struct Image {
    int height;
    int width;
    struct Pixel *pixels;
    char *out_file;
    struct Image *next;
};

/* Free a struct Image */
/* first frees all the pixels inside the Image and finally frees the Image itself*/
void free_image(struct Image *img)
{
    free(img->pixels);
    free(img);
}

/* In the cases that an error occurs with the first image in main and program needs to stop entirely, all the images in the linked list need to also 
 * be freed so I made a separate method that frees the whole list */
void free_image_list(struct Image *img)
{
    while(img != NULL){
        struct Image *next = img->next;
        free_image(img);
        img = next;
    }
}

/* Opens and reads an image file, returning a pointer to a new struct Image.
 * On error, prints an error message and returns NULL. */
struct Image *load_image(const char *filename)
{
    
    /* Open the file for reading */
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "File %s could not be opened.\n", filename);
        return NULL;
    }

    /* Allocate the Image object, and read the image from the file */
    struct Image *img = malloc(sizeof(struct Image));

    /* Error if image fails to be made */
    if (img == NULL) {
        fprintf(stderr, "Could not allocate memory for Image.\n");
        return NULL;
    }

    /* Variable for the header of the file "HPDEC" */
    char header[6];

    /* Scans the first three entities of the file: HPDEC, height and width and stores the height and width within the image's height and width fields */
    if(fscanf(f, "%5s %d %d", header, &img->height, &img->width) != 3){
        fprintf(stderr, "Incorrect header: %s\n", filename);
        return NULL;
    };

    /* Ensures the file is HPDEC by checking the first value in the header states "HPDEC" otherwise error is caused */
    if (strcmp(header, "HPDEC")){
        fprintf(stderr, "Invalid file format: %s, must be HPDEC\n", filename);
        return NULL;
    }

    /* Allocates memory for a dynamic array of pixels in an Image that will act as a two-dimensional array of size height * width as this is how many pixels there will be */
    img->pixels = calloc(img->height * img->width, sizeof(struct Pixel));

    /* Checks that memory is allocated to the pixels */
    if (img->pixels== NULL){
        fprintf(stderr, "Could not allocate memory for pixels\n");
        return NULL;
    }


    /* Array of integers that will hold red green and blue values respectively. I have made this integers as some values in the file could be outside the 8-bit range therefore I need
     * to ensure all the data is in the valid format so I scan it first as decimals */
    int colour[3];

    /* For each row, goes through every three numbers and stores them in order into the pixel's red, green and blue variables as the file holds them in that order. It then
     * moves on to the next pixel of the array and performs the same procedure until all the values have been loaded */
    for (int r = 0; r < img->height; r++){
        for (int c = 0; c < img->width; c++){
            /* I have utilised the 2D array and since it's a 2D array that doesn't use pointers to pointers I implemented the formula 'r * (img->width) + c'
             * to find the correct index for the current pixel in the 2D array and assigning it to 'pixel' for ease of use and readability*/
            struct Pixel *pixel = &img->pixels[r * (img->width) + c];
            /* After getting the index of the pixel, I then allocate the red, green and blue values of the file into the colour array to be checked later.
             * I have utilised 'd' instead of 'hhu' as the numbers that are expected to be 8 bits could be outside the range and scanning it as hhu will cause
             * wraparound resulting in inaccurate results. The values outside the range will be checked and cause an error. */
            if (fscanf(f, "%d %d %d", &colour[0], &colour[1], &colour[2]) != 3){
                fprintf(stderr, "Data length of file is incorrect or data is not in valid format: %s\n", filename); // If length of the file was incorrect (less than expected) or one of the values isn't a decimal, causes an error
                return NULL;
            };

            /* Loops through the colour values of the pixel and checks it falls in the 8-bit range otherwise error */
            for (int i = 0; i < 3; i++){
                if (colour[i] > 255 || colour[i] < 0){
                    fprintf(stderr, "Data is not in correct format: %s\n", filename);
                    return NULL;
                }
            }

            /* Assigns the valid colour values to the current pixel respectively */
            pixel->red = colour[0];
            pixel->green = colour[1];
            pixel->blue = colour[2];

        }
    }

    /* Variable for finding extra characters inside the file */
    unsigned char extra;

    /* If any extra characters are found, error is caused for data being incorrect length */
    if (fscanf(f, "%hhu", &extra) > 0){
        fprintf(stderr, "Data length of file: %s is incorrect\n", filename);
        return NULL;
    }


    /* Close the file */
    fclose(f);

    /* Returns the image made from the input file */
    return img;
}

/* Write the Image to the file that will be the output file. Return true on success, false on error. */
bool save_image(const struct Image *img, const char *filename)
{

    /* Opens the file for writing */
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "File %s could not be opened.\n", filename);
        return false;
    }

    /* Prints the header into the file using the input image's height and width and the header "HPDEC"*/
    fprintf(f, "HPDEC %d %d ", img->height, img->width);

    /* Loops through each row and prints the red, green and blue values of each pixel into the file until all pixel values are added.
     * This is similar to the load_image however uses fprintf rather than fscanf */
   for (int r = 0; r < img->height; r++) {
        for (int c = 0; c < img->width; c++){
            struct Pixel *pixel = &img->pixels[r * (img->width) + c];
            fprintf(f, "%hhu %hhu %hhu ", pixel->red, pixel->green, pixel->blue);
        }
    }

    /* Closes the file */
    fclose(f);
    return true;
}

/* Allocate a new struct Image and copy an existing struct Image's contents
 * into it. On error, returns NULL. */
struct Image *copy_image(const struct Image *source)
{

    /* If no source has been provided, an error is raised */
    if (source == NULL){
        fprintf(stderr, "No source provided");
        return NULL;
    }

    /* Allocates memory for the new image which will be the copy of the source */
    struct Image *copy = malloc(sizeof(struct Image));

    /* Error if the image has not successfully been allocated memory */
    if (copy == NULL){
        fprintf(stderr, "Could not allocate memory for copy image");
        return NULL;
    }

    /* Copies source image's height and width into copy image */
    copy->height = source->height;
    copy->width = source->width;

    /* Allocates memory for the pixels of the copy image */
    copy->pixels = calloc(copy->height * copy->width, sizeof(struct Pixel));

    /* Checks if memory has been allocated for the pixels */
    if (copy->pixels == NULL){
        fprintf(stderr, "Could not allocate memory for pixels\n");
        return NULL;
    }

    /* Loops through each pixel in 'source' and copies the values of it into the pixels in 'copy' */
    for (int r = 0; r < copy->height; r++) {
        for (int c = 0; c < copy->width; c++) {
            copy->pixels[r * copy->width + c] = source->pixels[r * source->width + c];
        }
    }

    /* Returns the copy image */
    return copy;
    
}

/* Adds random noise to each pixel's red, green and blue values. User provides value that will be the noise strength
 * and the function uses this to add a random value within the noise strength range between both its negative and positive values
 * to the pixel's red, green and blue values. Returns a new struct Image containing the result, or NULL on error. */
struct Image *apply_NOISE(const struct Image *source, int noise_strength)
{
    /* If no source is provided, error is raised */
    if (source == NULL){
        fprintf(stderr, "No source provided");
        return NULL;
    }

    // Source file mustn't change so makes a copy of the file using the copy_image function
    struct Image *noise_img = copy_image(source);

    /* Checks that the copy image successfully returned an image */
    if (noise_img == NULL){
        fprintf(stderr, "Could not make noise image");
        return NULL;
    }

    /* As red, green and blue values are of unsigned char, when adding random noise to it, it might pass the limit of 8 bits and will cause integer wraparound
     * therefore I decided that when I add the noise I first assign them into integer values in the array colour to avoid this. Then I would ensure no values go
     * outside 0 - 255 using if statements and I finally reassign the new values into their respective unsigned chars: pixel->(red|green|blue) */
    int colour[3];

    /* Loops through all pixels and adds noise */
    for (int r = 0; r < noise_img->height; r++) {
        for (int c = 0; c < noise_img->width; c++) {

            struct Pixel *pixel = &noise_img->pixels[r * (noise_img->width) + c];

            /* Adds noise to the values and assigns them to integers to avoid wraparounds */
            colour[0] = pixel->red + ((rand() % ((noise_strength*2)+1))-noise_strength);
            colour[1] = pixel->green + ((rand() % ((noise_strength*2)+1))-noise_strength);
            colour[2] = pixel->blue + ((rand() % ((noise_strength*2)+1))-noise_strength);

            /* If any of the values are over 255, they will be limited back to 255 and if any values are under 0, they will be limited back to 0 */
            for (int i = 0; i < 3; i++){
                if (colour[i] > 255)
                    colour[i] = 255;
            
                else if (colour[i] < 0)
                    colour[i] = 0;
            }

            /* Finally once the values fit within the 8 bit limit, they are reassigned back to their respective pixel colour */
            pixel->red = colour[0];
            pixel->green = colour[1];
            pixel->blue = colour[2];

        }
    }

    /* Returns the altered image that is a copy of the source image but with noise added to it */
    return noise_img;
}

/* The function finds the differences between each adjacent pixel's red, green and blue values of each row and prints the overall maximum
 * and minimum difference of each row for all values of all the pixels in that row. Returns true on success, or false on error. */
bool apply_EDGE(const struct Image *source)
{
    /* If no source is provided, error is raised and returns false */
    if (source == NULL){
        fprintf(stderr, "No source provided");
        return false;
    }

    int min; // minimum value for a row
    int max; // maximum value for a row
    int ovr_min = 256; // overall minimum value of all the rows (the whole image)
    int ovr_max = -1; // overall minimum value of all the rows (the whole image)
    int red_diff; // difference of red value
    int green_diff; // difference of green value
    int blue_diff; // difference of blue value


    /* Loops through all the pixels in the image, finds all the differences and prints the maximum and minimum differences out of all the colours in the full row */    
    for (int r = 0; r < source->height; r++){

        /* Repeatedly reassigns the min and max values to find the next min and max differences for the next row */
        min = 256;
        max = -1;

        /* Loops through all the columns from the first one upto the second last as the program checks the next pixel in the next adjacent column for each iteration */
        for (int c = 0; c < source->width - 1; c++){
            struct Pixel *curr_pix = &source->pixels[r * (source->width) + c]; // Assigns current pixel
            struct Pixel *next_pix = &source->pixels[r * (source->width) + c + 1]; // Assigns next adjacent pixel

            /* Finds the differences between all the colour values */
            red_diff = abs(curr_pix->red - next_pix->red);
            green_diff = abs(curr_pix->green - next_pix->green);
            blue_diff = abs(curr_pix->blue - next_pix->blue);

            /* Finds if these differences can be the new maximum and/or minimum differences and checks these for each colour and assigns if true */
            if (red_diff < min)
                min = red_diff;
            else if (red_diff > max)
                max = red_diff;

            if (green_diff < min)
                min = green_diff;
            else if (green_diff > max)
                max = green_diff;

            if (blue_diff < min)
                min = blue_diff;
            else if (blue_diff > max)
                max = blue_diff;

        }

        /* After looping through the whole row, prints out the maximum and minimum differences */
        printf("Row %d: minimum %d, maximum %d\n", r, min, max);

        /* Then checks if the maximum or minimum values of the row can be the new overall maximum and minimum values for the whole image */
        if (min < ovr_min)
            ovr_min = min;
        if (max > ovr_max)
            ovr_max = max;
    }

    /* After both loops are finished, prints the overall maximum and minimum values */
    printf("Overall: minimum %d, maximum %d\n\n", ovr_min, ovr_max);
    
    /* returns true as function has been successfull */
    return true;
}

int main(int argc, char *argv[])
{
    /* Initialise the random number generator, using the time as the seed */
    srand(time(NULL));

    /* Check command-line arguments ensuring minimum is 4 (for process input_file output_file noise_intensity) and is an even number so that both input and output images are entered*/
    if (argc < 4 || (argc % 2 != 0)) {
        fprintf(stderr, "Usage: process INPUTFILE1 OUTPUTFILE1 INPUTFILE2 OUTPUTFILE2 ... NOISE_INTENSITY\n");
        return 1;
    }

    /* Stores noise strength in pointer */
    char *noise_int = argv[argc-1];

    /* Ensures that what the user entered for noise intensity must be a positive decimal number and doesn't use other characters by looping through each character in the string.
     * Chars that are not a decimal can result in unexpected results in apply_NOISE */
    for (int i = 0; noise_int[i] != '\0'; i++){
        if(!(noise_int[i] >= '0' && noise_int[i] <= '9')){
            fprintf(stderr, "NOISE_INTENSITY must be a positive decimal number.\n");
            return 1;
        }
    }

    /* Calculates the number of images */
    int num_imgs = (argc-2)/2;

    /* As I am using a linked list to navigate through the images I have current image to mark the start of the list initialised as NULL */
    struct Image *curr_img = NULL;

    /* Load the input image */
     /* Iterates through the images in reverse order from the last file argument the user entered to the first so that the linked list ends up in order as I am adding to the head.  
      * Initially, I added images to the list from start to end, but this approach required extra variables or a while loop, in order for the linked list to hold the images in order.
      * Furthermore the while loop would add an additional O(N) time complexity. By iterating backwards, I can add each node to the head of the linked list which doesn't require
      * extra work, which will allow me to keep the order correct without needing to rearrange the list later and curr_img will hold the first image by the end to be used later in the program. */
    for (int i = 1; i<=num_imgs; i++){
       /* Index will start from the end of the user file input arguments and move backwards for each iteration */
        int idx = (argc-1) - (2*i);
        /* Loads the image and stores it in in_img */
        struct Image *in_img = load_image(argv[idx]);
        /* checks that in_img has been loaded, if not returns 1 (fail) */
        if (in_img == NULL) {
            free_image_list(curr_img); // frees other images in the list if there are any
            return 1;
        }
        /* idx + 1 allows me retrieve the output filename the user entered so I assign this in the image's out_file field */
        in_img->out_file = argv[idx + 1];
        /* in_img's next will be the previous image list (curr_img) effectively adding in_img to the head of the list */
        in_img->next = curr_img;
        /* The new current image will be the image list we have just made pointing to the head of the list (the most recent image added) */
        curr_img = in_img;
    }

    /* Loops through all the images in the list and applies the processes (NOISE, EDGE and saves) starting from the head (curr_img) which is the first image entered
     * by the user until curr_img becomes NULL implying end of the list is reached */
    while (curr_img != NULL){

        /* Apply the first process */
        /* Applies noise to the image using the apply_noise function and sends the last argument the user entered which is expected to be the noise intensity.
         * atoi is used to make the argument an integer so that the function can use it properly. If no output is retrieved, program raises error and returns 1 */
        struct Image *out_img = apply_NOISE(curr_img, atoi(noise_int));
        if (out_img == NULL) {
            fprintf(stderr, "Applying noise process failed.\n");
            free_image_list(curr_img); // Frees the entire image list as program stops entirely so must free up all memory
            return 1;
        }

        /* Apply the second process */
        /* The new image (out_img) is sent as the parameter for apply_EDGE function. Will return 1 if fails */
        printf("\n%s:\n", curr_img->out_file); // Prints the output file name so user knows what is being printed out is for which image
        if (!apply_EDGE(out_img)) {
            fprintf(stderr, "Applying edge process failed.\n");
            free_image_list(curr_img);
            free_image(out_img); // As out_img is not a linked list of images, utilises free_image method instead
            return 1;
        }

        /* Save the output image */
        /* After applying the two processes, out_img is saved and written to a new file which is the output file the user entered
         * and this is retrieved from the out_file field of the current image. Returns 1 if failure */
        if (!save_image(out_img, curr_img->out_file)) {
            fprintf(stderr, "Saving image to %s failed.\n", curr_img->out_file);
            free_image_list(curr_img);
            free_image(out_img);
            return 1;
        }
        /* Assigns the next image to "next" that is of Image. This is because I would be freeing curr_img and its next image will not be available to use
         * after freeing it */
        struct Image *next = curr_img->next;

        /* Frees only both the current image and the output image */
        free_image(curr_img);
        free_image(out_img);

        /* Assigns current to the next image of the list and loop repeats unless next is NULL */
        curr_img = next;
    }
    /* Returns 0 after a successful run */
    return 0;
}
