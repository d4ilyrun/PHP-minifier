#include <stdio.h>  // fprintf
#include <stdlib.h> // malloc
#include <unistd.h> // read / write
#include <fcntl.h>  // open

#define ERR(_msg) \
    fprintf(stderr, "%d - Error: %s\n", __LINE__, _msg); \
    return 1;

int starts_with(char *s, char *tok, int tok_len)
{
    int test = 1;
    for (int i = 0; i < tok_len && test; ++i)
        test = s[i] == tok[i];
    return test;
}

int find_next_token(char *s, int i) {
    // POSSIBLE TOKENS ARE:
    // " ' : strings
    // // /* < : comments
    // space
    // \n \t \r : line break
    // \0 : EOF
    
    while (s[i]) {
        if (s[i] == ' ' || s[i] == '"' || s[i] == '<' || s[i] == '\'' || s[i] == '/'
                || s[i] == '\n' || s[i] == '\t' || s[i] == '\r') {
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

const char classic_tokens[] = {' ','{','(',')',';','*','/','&','^','+','-','=','<','>','!','?','.',',',':','|','&'};
const int  classic_tokens_count = 21;

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
        switch (file_content[i]) {
            case '\0':
                // EOF
                *out_size = i_min;
                return minified;
            case '"':
                // DOUBLE QUOTED STRING
                ADD_INDEX();
                while(i < file_size && file_content[i] != '"')
                    ADD_INDEX();
                ADD_INDEX();
                break;
            case '\'':
                // SINGLE QUOTED STRING
                ADD_INDEX();
                while(i < file_size && file_content[i] != '\'')
                    ADD_INDEX();
                ADD_INDEX();
                break;
            case '/':
                if (file_content[i+1] == '/') {
                    while (file_content[i] != '\n')
                        SKIP_INDEX();
                } else if (file_content[i+1] == '*') {
                    SKIP_INDEX();
                    SKIP_INDEX();
                    while (i < file_size && (file_content[i] != '/' || file_content[i-1] != '*'))
                        SKIP_INDEX();
                    SKIP_INDEX();
                }
                break;
            case '<':
                if (starts_with(&file_content[i], "<!--", 4)) {
                    // HTML COMMENT
                    i += 4;
                    while (i < file_size && (file_content[i-1] != '>' || starts_with(&file_content[i-4], "-->", 3)))
                        SKIP_INDEX();
                } else {
                    ADD_INDEX();
                }
                break;
            case '\n':
            case '\t':
            case '\r':
                file_content[i] = ' ';
            case ' ':
                // TOKENS
                if (file_content[i+1] == '\0') {
                    *out_size = i_min;
                    return minified;
                } else if (FIND_TOKEN('}')) {
                    if (i > 3 && starts_with(&file_content[i-3], "php", 3)) {
                        ADD_INDEX();
                    } else {
                        SKIP_INDEX();
                    }
                } else {
                    int found = 0;
                    for (int j = 0; j < classic_tokens_count && !found; ++j) {
                        if ((found = FIND_TOKEN(classic_tokens[j])))
                            SKIP_INDEX();
                    }
                    if (!found)
                        ADD_INDEX();
                }
                break;
            default:
                fprintf(stderr, "%d - Error: unrecognised token '%c'", __LINE__, file_content[i]);
                *out_size = 0;
                return NULL;
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
    if ((minified_content = minifier(file_content, file_size, &minified_size)) == NULL)
        return 1;
    write(fd, minified_content, minified_size);

    close(fd);
    free(minified_content);
    free(file_content);

    //printf("%s: Saved %d bytes through parsing\n", argv[1], file_size - minified_size);

    return 0;
}
