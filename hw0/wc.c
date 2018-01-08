#include<stdio.h>
#include<stdlib.h>
#include<string.h>


extern FILE * stdin;
int page_size = 10; // static

typedef struct {
    char * data;
    size_t size;
} alloc_info;

typedef struct {
    int num_lines;
    int num_words;
    int num_characters;
} wc_info;

wc_info scan_buffer(char *buf, int length) {
    int num_lines = 0;
    int num_words = 0;
    int num_characters = 0;
    int i;
    short inside_space;
    short inside_characters;
    wc_info result;

    for (i = 0; i < length; i++) {
        if(buf[i] == '\n') {
            num_lines++;
            if (inside_characters) {
                num_characters++;
                inside_characters = 0;
                num_words++;
            }
        } else if (buf[i] == ' ') {
            num_characters++;
            inside_characters = 0;
            if (!inside_space) {
               num_words++;
               inside_space = 1;
            }
        } else if (buf[i] != ' ' || buf[i] != '\n') {
            inside_space = 0;
            inside_characters = 1;
            num_characters++;
        }
    }

    result.num_lines = num_lines;
    result.num_characters = num_characters;
    result.num_words = num_words;

    return result;
}

alloc_info* expand_buffer(alloc_info* buf) {
    int new_buf_length = buf->size + page_size;
    char * new_buffer = (char*) malloc(sizeof (char) * new_buf_length);
    alloc_info * new_buf_allocation = (alloc_info *) malloc(sizeof(alloc_info) * 1);

    int d = 0;
    int s = 0; 
    memset(new_buffer, 0, new_buf_length);
    memcpy(new_buffer, buf->data, buf->size);

    free(buf->data);
    free(buf);
    
    new_buf_allocation->data = new_buffer;
    new_buf_allocation->size = new_buf_length;

    return new_buf_allocation;
}

char * read_all_from_input_stream(FILE *stream) {
    char input;
    int buf_index;
    alloc_info * buf_alloc;

    buf_alloc = (alloc_info*) malloc(sizeof(alloc_info));
    buf_alloc->data = (char *) malloc(sizeof(char) * page_size);
    buf_alloc->size = page_size;

    buf_index = 0;

    while(!feof(stream)) {
        input = fgetc(stream);
        buf_alloc->data[buf_index++] = input;
        if (buf_index == (buf_alloc->size - 1)) {
            buf_alloc = expand_buffer(buf_alloc);
        }
    }
    return buf_alloc->data;
}

char * read_stdin() {
    return read_all_from_input_stream(stdin);
}

char * read_from_file(char * file_name) {
    FILE * file_ptr = fopen(file_name, "r");
    return read_all_from_input_stream(file_ptr);
}


int main(int argc, char *argv[]) {
    int i;
    char *buf;
    char * fname = "";
    wc_info scan_result;

    if (argc == 1) {
        // read from stdin
        buf = read_stdin();
    } else if (argc == 2) {
        // read from file
        buf = read_from_file(argv[1]);
        fname = argv[1];
    } else {
        fprintf(stderr, "Invalid arguments.");
        exit(-1);
    }

    scan_result = scan_buffer(buf, strlen(buf));

    printf("%d\t%d\t%d\t%s\n", scan_result.num_lines, scan_result.num_words, scan_result.num_characters, fname);
    return 0;
}
