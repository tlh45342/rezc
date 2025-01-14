#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VERSION "0.0.8"

// ======================================
// Section: Function Prototypes
// ======================================

void process_file(const char *filename, int debug);
void write_byte(FILE *out, const char *data, int debug);
void write_data(FILE *out, const char *data, int length, const char *format, int debug);

// ======================================
// Section: Functions
// ======================================

/*
 *  Useed to write a single byte of output.
 */

void write_byte(FILE *out, const char *data, int debug) {
    unsigned char value = (unsigned char)strtol(data, NULL, 16); // Assume hexadecimal
    fwrite(&value, 1, 1, out);
    if (debug) printf("Wrote byte: %s\n", data);
}

/*
 *  Used to switch assumptions about incoming data
 */
 
 void write_data(FILE *out, const char *data, int length, const char *format, int debug) {
    unsigned int value;

    if (strcmp(format, "dec") == 0) {
        value = (unsigned int)strtol(data, NULL, 10);
    } else if (strcmp(format, "oct") == 0) {
        value = (unsigned int)strtol(data, NULL, 8);
    } else if (strcmp(format, "hex") == 0) {
        value = (unsigned int)strtol(data, NULL, 16);
    } else {
        fprintf(stderr, "Unknown format: %s\n", format);
        exit(EXIT_FAILURE);
    }

    fwrite(&value, length, 1, out);
    if (debug) printf("Wrote word data: %s in %s format\n", data, format);
}

/*
 *   Main loop.  Used to iterate over the file and process data it encounters
 */
 
void process_file(const char *filename, int debug) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    FILE *output = NULL;
    char line[1024];

    while (fgets(line, sizeof(line), file)) {
        char *trimmed = line;

        // Ignore comments
        char *comment = strchr(trimmed, ';');
        if (comment) *comment = '\0';

        // Skip empty lines
        while (isspace((unsigned char)*trimmed)) trimmed++;
        if (*trimmed == '\0') continue;

        if (strncmp(trimmed, ".name", 5) == 0) {
            char *out_filename = trimmed + 5;
            while (isspace((unsigned char)*out_filename)) out_filename++;
            out_filename[strcspn(out_filename, "\r\n")] = '\0'; // Remove trailing newline or carriage return

            if (output) fclose(output);
            output = fopen(out_filename, "wb");
            if (!output) {
                perror("Error opening output file");
                exit(EXIT_FAILURE);
            }
            if (debug) printf("Opened output file: %s\n", out_filename);
        } else if (strncmp(trimmed, ".hex", 4) == 0) {
            char *data = trimmed + 4;
            while (isspace((unsigned char)*data)) data++; // Skip spaces
            if (*data == '\0') {
                if (debug) printf("Skipping empty .hex directive\n");
                continue; // Skip processing if no data
            }
            if (output) {
                write_byte(output, data, debug);
            }
        } else if (strncmp(trimmed, ".byte", 5) == 0) {
            char *data = trimmed + 5;
            while (isspace((unsigned char)*data)) data++;

            // Handle comma-separated values
            char *token = strtok(data, " ,");
            while (token) {
                if (output) {
                    write_byte(output, token, debug);
                }
                token = strtok(NULL, " ,");
            }
        }
    }

    if (output) fclose(output);
    fclose(file);
}

/*
 *   MAIN: Traditional
 */

int main(int argc, char *argv[]) {
    int debug = 0;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <filename> [--debug] or %s --version\n", argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--version") == 0) {
        printf("rezc Version:%s\n", VERSION);
        return 0;
    }

    if (argc == 3 && strcmp(argv[2], "--debug") == 0) {
        debug = 1;
    }

    process_file(argv[1], debug);

    return 0;
}
