#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define VERSION "0.0.31"  // Incremented patch number

int is_little_endian = 1;  // Default to little-endian
const char *default_encoding = ".dec";  // Default numeric encoding style

// Function prototypes
void write_byte(FILE *out, uint8_t value);
void write_word(FILE *out, uint16_t value);
void write_dword(FILE *out, uint32_t value);
void write_ascii(FILE *out, const char *data);
void write_asciz(FILE *out, const char *data);
int is_valid_data(const char *data, const char *current_encoding);
uint32_t parse_value(const char *data, const char *current_encoding);
void process_file(const char *filename, int debug);

// List of valid directives
const char *valid_directives[] = {
    ".name", ".byte", ".word", ".dword", ".ascii", ".asciz",
    ".little-endian", ".big-endian", ".dec", ".hex", ".octal"
};

// Helper function to check if a directive is valid
int is_valid_directive(const char *directive) {
    size_t num_directives = sizeof(valid_directives) / sizeof(valid_directives[0]);
    for (size_t i = 0; i < num_directives; i++) {
        if (strncmp(directive, valid_directives[i], strlen(valid_directives[i])) == 0) {
            return 1;
        }
    }
    return 0;
}

// Validate data based on the current encoding
int is_valid_data(const char *data, const char *current_encoding) {
    if (strcmp(current_encoding, ".octal") == 0) {
        for (const char *p = data; *p; p++) {
            if (!isdigit((unsigned char)*p) || *p > '7') {
                return 0;  // Invalid octal digit
            }
        }
    } else if (strcmp(current_encoding, ".hex") == 0) {
        for (const char *p = data; *p; p++) {
            if (!isxdigit((unsigned char)*p)) {
                return 0;  // Invalid hexadecimal digit
            }
        }
    } else if (strcmp(current_encoding, ".dec") == 0) {
        for (const char *p = data; *p; p++) {
            if (!isdigit((unsigned char)*p)) {
                return 0;  // Invalid decimal digit
            }
        }
    } else {
        fprintf(stderr, "Error: Unknown encoding '%s'.\n", current_encoding);
        exit(EXIT_FAILURE);
    }
    return 1;  // Valid data
}

// Parse numeric values with validation
uint32_t parse_value(const char *data, const char *current_encoding) {
    if (!is_valid_data(data, current_encoding)) {
        fprintf(stderr, "Error: Value '%s' is invalid for encoding '%s'.\n", data, current_encoding);
        exit(EXIT_FAILURE);
    }

    if (strncmp(data, "0x", 2) == 0 || data[0] == '$') {
        return strtol(data + (data[0] == '$' ? 1 : 2), NULL, 16);  // Hexadecimal
    } else if (data[0] == '0' && strlen(data) > 1) {
        return strtol(data, NULL, 8);  // Octal
    } else if (strcmp(current_encoding, ".dec") == 0) {
        return strtol(data, NULL, 10);  // Decimal
    } else if (strcmp(current_encoding, ".hex") == 0) {
        return strtol(data, NULL, 16);  // Default Hexadecimal
    } else if (strcmp(current_encoding, ".octal") == 0) {
        return strtol(data, NULL, 8);  // Default Octal
    } else {
        fprintf(stderr, "Error: Unable to parse value '%s' with encoding '%s'.\n", data, current_encoding);
        exit(EXIT_FAILURE);
    }
}

void write_byte(FILE *out, uint8_t value) {
    fwrite(&value, 1, 1, out);
}

void write_word(FILE *out, uint16_t value) {
    if (!is_little_endian) {
        value = (value >> 8) | (value << 8);  // Convert to big-endian
    }
    fwrite(&value, 2, 1, out);
}

void write_dword(FILE *out, uint32_t value) {
    if (!is_little_endian) {
        value = __builtin_bswap32(value);  // Convert to big-endian
    }
    fwrite(&value, 4, 1, out);
}

void write_ascii(FILE *out, const char *data) {
    fwrite(data, 1, strlen(data), out);
}

void write_asciz(FILE *out, const char *data) {
    fwrite(data, 1, strlen(data) + 1, out);  // Include null terminator
}

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

        trimmed[strcspn(trimmed, "\r\n")] = '\0';  // Remove trailing newline

        if (!is_valid_directive(trimmed)) {
            fprintf(stderr, "Error: '%s' is not a valid directive. Check 'directives.md' for a list of valid directives.\n", trimmed);
            continue;  // Skip invalid directives
        }

        if (strncmp(trimmed, ".little-endian", 14) == 0) {
            is_little_endian = 1;
            if (debug) printf("Switched to little-endian mode\n");
        } else if (strncmp(trimmed, ".big-endian", 11) == 0) {
            is_little_endian = 0;
            if (debug) printf("Switched to big-endian mode\n");
        } else if (strncmp(trimmed, ".name", 5) == 0) {
            char *out_filename = trimmed + 5;
            while (isspace((unsigned char)*out_filename)) out_filename++;
            out_filename[strcspn(out_filename, "\r\n")] = '\0';  // Remove trailing newline

            if (output) fclose(output);
            output = fopen(out_filename, "wb");
            if (!output) {
                perror("Error opening output file");
                exit(EXIT_FAILURE);
            }
            if (debug) printf("Opened output file: %s\n", out_filename);
        } else if (strncmp(trimmed, ".dec", 4) == 0) {
            default_encoding = ".dec";
            if (debug) printf("Switched to decimal encoding\n");
        } else if (strncmp(trimmed, ".hex", 4) == 0) {
            default_encoding = ".hex";
            if (debug) printf("Switched to hexadecimal encoding\n");
        } else if (strncmp(trimmed, ".octal", 6) == 0) {
            default_encoding = ".octal";
            if (debug) printf("Switched to octal encoding\n");
        } else if (strncmp(trimmed, ".byte", 5) == 0) {
            char *data = trimmed + 5;
            while (isspace((unsigned char)*data)) data++;

            // Handle comma-separated values
            char *token = strtok(data, " ,");
            while (token) {
                uint32_t value = parse_value(token, default_encoding);
                if (value > 0xFF) {
                    fprintf(stderr, "Error: Value '%s' exceeds 8-bit limit for .byte directive.\n", token);
                    exit(EXIT_FAILURE);
                }
                if (output) write_byte(output, (uint8_t)value);
                if (debug) printf("Wrote byte: 0x%02X\n", (uint8_t)value);
                token = strtok(NULL, " ,");
            }
        }
    }

    if (output) fclose(output);
    fclose(file);
}

int main(int argc, char *argv[]) {
    int debug = 0;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <filename> [--debug] or %s --version\n", argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--version") == 0) {
        printf("rezc Version: %s\n", VERSION);
        return 0;
    }

    if (argc == 3 && strcmp(argv[2], "--debug") == 0) {
        debug = 1;
    }

    process_file(argv[1], debug);

    return 0;
}