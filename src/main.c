//
//  main.c
//  MNIST_ASCII_Explorer
//
//  Created by Sebastian Valencia on 12/29/16.
//

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* What's the minimum value from a and b */
#define MIN(a, b) ((a) < (b)) ? (a) : (b)

const char* FILE_NAME = "../data/data.csv";

const int MAX_BUFFER_SIZE = 1024 * 7 - 216; // By trial and error

/* Pixel count per file register */
const int PIXELS_COUNT = 784; 

/* The dimensions of the box */
const int MNIST_LINE_SIZE = 28;

/* Total registers in the file */
const int MNIST_DATA_COUNT = 42000;

/* Random coloring seed fot the ASCII CAPTCHA */
const char ASCII_colors[] = {'.', ',', ';', ':', '"', '\'', '`', '+', '*', '%', '#', '@'};

/* Predefined coloring constant */
const char DOT = '*';

/*
 * This block defines the global variables of the program
 * min_digits, and max_digits, forms a range that defines the number of digits
 * in the final CAPTCHA.
 *
 * white, if true, means that the digits shuld be filled with space character, while
 * the surroudings must have other non-blank charcters; that depends on the variable 
 * dots, if this variable is true, those non-blank characters are going to be DOT, 
 * otherwise, random characters taken from ASCII_colors.
*/

int min_digits, max_digits;
bool white;
bool dots;

/*
 * This block defines the basic data structures to handle MNIST data, that is, what is
 * taken from the file, and what's going to be processed through out the program.
 *
 * raw_MNIST_line, is a single line from the CSV file, it have the label (the real digit)
 * that number represents, and a list of integers holding the "brightness" of each of the 
 * 784 pixels, those are numbers between 0 and 255 (with higher numbers meaning darker).
 *
 * MNIST_array, is the colection of raw_MNIST_lines retrieved from the file.
*/

struct raw_MNIST_line {
    int label;
    int* pixels;    
};

struct raw_MNIST_line MNIST_array[MNIST_DATA_COUNT]; 

/*
 * Creates and returns a new element of type raw_MNIST_line, it creates an object whose fields 
 * are given as parameters
*/

struct raw_MNIST_line* alloc_raw_MNIST_line(int label, int size, int* pixels) {

    struct raw_MNIST_line* new_item = (struct raw_MNIST_line*)
        malloc(sizeof(struct raw_MNIST_line));

    /* Normalize the count of pixels */
    size = MIN(size, PIXELS_COUNT);

    new_item->label = label;
    new_item->pixels = (int*) malloc(sizeof(int) * size);

    for(int i = 0; i < size; i++)
        *((new_item->pixels) + i) = *(pixels + i);

    return new_item;
}

/*
 * Creates a raw_MNIST_line element from a line of the CSV file, where the first item
 * is the label, and the rest of values are the pixels of the element
*/

struct raw_MNIST_line* parse_raw_MNIST_line(char* line) {
    int label = 0, i = 0;
    int* pixels = (int*) malloc(sizeof(int) * PIXELS_COUNT);

    char* parse;
    parse = strtok(line, ",");

    /* string to int */
    label = (int) strtol(parse, (char **) NULL, 10);

    for(i = 0; i < PIXELS_COUNT && parse != NULL; i++) {
        parse = strtok (NULL, ",");
        *(pixels + i) = (int) strtol(parse, (char **) NULL, 10);
    }

    return alloc_raw_MNIST_line(label, i, pixels);
}

/*
 * Skips the header of the CSV file
*/

void skip_header(FILE* file, char* line) {
    fgets(line, MAX_BUFFER_SIZE, file);
    fgets(line, MAX_BUFFER_SIZE, file);
}

/*
 * Reads the file and populates MNIST_arrayy. If I/O processing failed, it throws an
 * error, and the program exits with erroneous termination
*/

void handle_file() {
    FILE * file;
    char line[MAX_BUFFER_SIZE];
    
    /* Error handling */
    file = fopen(FILE_NAME, "r");
    
    if(!file) {
        perror("fopen");
        exit(1);
    }
    
    skip_header(file, line);
   
    /* Create items and populate the array */
    int i = 0;
    while(fgets(line, MAX_BUFFER_SIZE, file) && i < MNIST_DATA_COUNT) {
        struct raw_MNIST_line* item = parse_raw_MNIST_line(line);
        MNIST_array[i++] = *item;
    }
    
    fclose(file);
    
}

/*
 * Accepts an input and compare it to the actual value displayed. It uses the banner
 * holding the number of displayed digits, and the actual digits in order
*/

bool receive_CAPTCHA(int banner_size, struct raw_MNIST_line banner[]) {
    bool accepted = true;
    int num;

    printf(">> ");

    for(int i = 0; i < banner_size; i++) {

        int actual = banner[i].label;

        /* scanf a single digit */
        scanf("%1d", &num);

        /* DON'T SHORT CIRCUIT THE LOOP */
        if(num != actual) 
            accepted = false;
    }

    return accepted;
}

/*
 * Telles the user whether or not, the given input corresponds to the displayed digits
*/

void report_CAPTCHA(bool status) {
    printf("%s\n", status ? "accepted" : "wrong");
}

/*
 * Returns a random integer between min_num and max_num + 1 inclusive both bounds
*/

int randr(int min_num, int max_num) {
    return (rand() % max_num) + min_num;
}

/*
 * Depending on the state of the global variables, and of the given value for brightness,
 * this function gives a value that corresponds (taking into account the semantic constraints
 * imposed by the global variables) to the character to display in the CAPTCHA banner
*/

char classify_brightness(int brightness) {
    char ans = ' ';
    char random_fill = ASCII_colors[randr(0, 10)];

    if(!white && !dots)
        ans = (brightness == 0) ? ' ' : random_fill;
    else if(!white && dots)
        ans = (brightness == 0) ? ' ' : DOT;
    else if(white && !dots)
        ans = (brightness == 0) ? random_fill : ' ';
    else /* white && dots */
        ans = (brightness == 0) ? DOT : ' ';

    return ans;
}

/*
 * Displays a single MNIST digit given the data structure that holds it. It display
 * a 24 X 24 ASCII image
*/

void display_MNIST_line(struct raw_MNIST_line* item) {
    printf("%d\n", item->label);

    for(int i = 0; i < PIXELS_COUNT; i++) {

        int brightness = item->pixels[i];

        char current = classify_brightness(brightness);

        if((i + MNIST_LINE_SIZE + 1) % MNIST_LINE_SIZE == 0)
            printf("%c\n", current);
        else
            printf("%c", current);

    }

    printf("\n\n");
}

/*
 * Displays the MNIST data set based CAPTCHA banner depending on the values of min_digits,
 * and max_digits to define the size of it, and receives the CAPTCHA as seen by the user,
 * depending and on the banner, it returns a boolean that tell if the user was right 
*/

bool display_MNIST_captcha() {

    /* Banner size, banner and actual answer for the CAPTCHA creation */
    int banner_size = randr(min_digits, max_digits - 1);
    struct raw_MNIST_line banner[banner_size];
    int answer[banner_size];

    /* Filling the banner and the actual answer */
    for(int i = 0; i < banner_size; i++) {
        int index = randr(0, MNIST_DATA_COUNT - 1);
        struct raw_MNIST_line item = MNIST_array[index];
        banner[i] = item;
        answer[i] = item.label;
    }

    /* Display the banner */
    for(int i = 0; i < MNIST_LINE_SIZE * MNIST_LINE_SIZE; i += MNIST_LINE_SIZE) {
        for(int j = 0; j < banner_size; j++) {
            for(int k = 0; k != MNIST_LINE_SIZE; k++) {
                int brightness = banner[j].pixels[k + i];
                char current = classify_brightness(brightness);
                printf("%c", current);
            }

        }

        printf("\n");
    }

    bool user_input = receive_CAPTCHA(banner_size, banner);
    return user_input;
}

/*
 * A function that checks if the given string is a positive integer
*/

bool isinteger(char const *str) {
    /* Handling empty string or just "-" */
    if (!*str)
        return false;

    /* Check for non-digit chars in the rest of the stirng */
    while (*str)
        if (!isdigit(*str))
            return false;
        else
            ++str;

    return true;
}

/*
 * A boring function to handle command line arguments. If something weird happens
 * the program will stop. The responsability of this procedure is to populate
 * the global variables depending on the arguments
*/

void handle_args(int argc, char const *argv[]) {

    /* args to temporary variables */
    char const* min_digits_arg = argv[1];
    char const* max_digits_arg = argv[2];
    char const* white_arg = argv[3];
    char const* dots_arg = argv[4];

    /* Is the range consistent? */
    if(!isinteger(min_digits_arg) || !isinteger(max_digits_arg)) {
        printf("Error: first two args must be digits [0-9]\n");
        exit(1);
    } else {
        min_digits = (int) strtol(min_digits_arg, (char **) NULL, 10);
        max_digits = (int) strtol(max_digits_arg, (char **) NULL, 10);

        if(max_digits < min_digits) {
            printf("Error: second arg must be bigger than the first one\n");
            exit(1);
        }
    }

    /* Is white_arg -b or -w? */
    if(strcmp(white_arg, "-b") != 0 && strcmp(white_arg, "-w") != 0) {
        printf("Error: third args must be -w for space filled digits, or -b for filled digits\n");
        exit(1);
    } else
        white = (strcmp(white_arg, "-w") == 0);

    /* Is dots_arg -d or -r? */
    if(strcmp(dots_arg, "-d") != 0 && strcmp(dots_arg, "-r") != 0) {
        printf("Error: thirs args must be -d to fill non-white space with dots, ");
        printf("or -r to fill it with random ASCII characters\n");
        exit(1);
    } else
        dots = (strcmp(dots_arg, "-d") == 0);
}

/*
 * The main function takes care of the arguments, global variable instantiation, handling the 
 * MNIST file and the logic behind the CAPTCHA (displaying the CAPTCHA banner), taking the user
 * input, and reporting the status of the input vs the banner, in a loop until the user input 
 * corresponds to the actual digits displayed.
 *
 * ./main m n [-b | -w] [-d | -r]
*/

int main(int argc, char const *argv[]) {

    handle_args(argc, argv);
    handle_file();
    srand(time(NULL));

    bool value = false;

    while(!value) {
        value = display_MNIST_captcha();
        report_CAPTCHA(value);
    }

    return 0;
}