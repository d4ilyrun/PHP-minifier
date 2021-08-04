#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define ERR(_msg) \
    fprintf(stderr, "Error: %s\n", _msg); \
    return 1;

/// Test if a strings starts with another and update its pointer if true
int starts_with(char **s, char *token, size_t token_len)
{
    int is_eq = 1;

    if (strlen(*s) < token_len)
        return 0;

    for (size_t i = 0; i < token_len && is_eq; ++i)
        is_eq = *s[i] == token[i];
    
    if (is_eq) *s += token_len;
    return is_eq;
}

int find_next_token(char *s, int i) {
    // POSSIBLE TOKENS ARE:
    // " ' : comments
    // space
    // \0
    
    while (s[i]) {
        if (s[i] == ' ' || s[i] == '"' || s[i] == '\'') {
            return i;
        } else {
            i++;
        }
    }

    return i;
}

#define FIND_TOKEN(_tok) (file_content[i-1] == (_tok) || file_content[i+1] == (_tok))
#define SKIP_INDEX() (i++)
#define ADD_INDEX() (minified[i_min++] = file_content[i++])

char *minifier(char *file_content, int file_size, int *out_size)
{
    int i = 0, i_min = 0;
    char *minified;

    minified = malloc(file_size);
    *out_size = 0;

    while (i < file_size) {
        int next_i = find_next_token(file_content, i);
        while (i < next_i)
            ADD_INDEX();
        if (file_content[i] == '\0') {
            // EOF
            *out_size = i_min;
            return minified;
        } else if (file_content[i] == '"') {
            // DOUBLE QUOTED STRING
            ADD_INDEX();
            while(i < file_size && file_content[i] != '"')
                ADD_INDEX();
            ADD_INDEX();
        } else if (file_content[i] == '\'') {
            // SINGLE QUOTED STRING
            ADD_INDEX();
            while(i < file_size && file_content[i] != '\'')
                ADD_INDEX();
            ADD_INDEX();
        } else if (file_content[i] == ' ') {
            // TOKENS
            if (file_content[i+1] == '\0') {
                *out_size = i_min;
                return minified;
            } else if (FIND_TOKEN(' ')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('{')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('}')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('(')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN(')')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN(';')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('*')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('+')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('-')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('=')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('>')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('<')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('!')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('?')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN('.')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN(',')) {
                SKIP_INDEX();
            } else if (FIND_TOKEN(':')) {
                SKIP_INDEX();
            } else {
                ADD_INDEX();
            }
        }
    }

    *out_size = i_min;
    return minified;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        ERR("Invalid args.\nShould be `parser 'file_name' file_size`");
    }

    char *file_content, *minified_content;
    int file_size, minified_size;
    int fd;
    FILE *file;

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        ERR("Couldn't open file");
    }

    // READ RAW CONTENT
    file_size = atoi(argv[2]);
    file_content = malloc(file_size);
    if ((file_size = read(fd, file_content, file_size)) == -1) {
        ERR("Couldn't read file");
    }
    close(fd);

    if ((fd = open(argv[1], O_WRONLY | O_TRUNC)) == -1) {
        ERR("Couldn't open file");
    }

    // REWRITE THE MODIFIED CONTENT
    minified_content = minifier(file_content, file_size, &minified_size);
    write(fd, minified_content, minified_size);
    close(fd);

    printf("%s: Saved %d bytes through parsing\n", argv[1], file_size - minified_size);

    return 0;
}
