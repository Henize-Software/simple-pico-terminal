//create 2024 Joshua Henize
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#define COM(args) (sizeof(args) / sizeof((args)[0]))
 
struct CommandBuffer { 
    char buff[80];
    int elements;
};

struct Commands {
    const char* command;
    void (*handler)(char*);
};

void echo_command(char* args) {
        uart_puts(uart0, args);
        uart_tx_wait_blocking(uart0);
}

struct Commands c[] = { {"echo", echo_command} };

struct CommandBuffer com_buff;

bool stdin_chars_available = false;

void uart_input_callback(void *param) {
    stdin_chars_available = true;
}

void split_string(const char* input, char* before_whitespace, char* after_whitespace) {
    const char* whitespace = strchr(input, ' ');
    
    if (whitespace == NULL) {
        strcpy(before_whitespace, input);
        after_whitespace[0] = '\0';
    } else {
        size_t length = whitespace - input;
        strncpy(before_whitespace, input, length);
        before_whitespace[length] = '\0';  
        strcpy(after_whitespace, whitespace + 1);
    }
}

void process_command(struct CommandBuffer *cb) {
    cb->elements = 0;
    
    char com[80];
    char args[80];

    split_string(cb->buff, com, args);
    for (int i = 0; i < COM(c); i++) {
        if (strcmp(com, c[i].command) == 0) {
            c[i].handler(args);  
            return;
        }
    }
    printf("%s Invalid\n", cb->buff);
    
}

void process_stdin(struct CommandBuffer *cb) {
    if (stdin_chars_available)
    {
        int ch, buffch; //ch goes to stdout, buffch goes to the command buffer
        //non-blocking input from stdin
        while ((ch = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
            cb->elements++;
            if (ch == '\r') ch = '\n';
            buffch = ch;
            //if buffer is full or newline then terminate buffer string.
            if(cb->elements == sizeof(cb->buff) || ch == '\n') buffch = '\0';
            //write to command buffer
            cb->buff[cb->elements-1] = (char)buffch;
            //echo stdin to stdout
            putchar(ch);    
            //if buffer full or newline, process command
            if(buffch == '\0') {
                process_command(cb);
                return;
            }
        }
        stdin_chars_available = false;
    }
}

void init_terminal()
{
    memset(&com_buff, 0, sizeof(struct CommandBuffer));
    stdio_init_all();
    stdio_set_chars_available_callback(uart_input_callback, NULL);
}

int main() {
    init_terminal();

    //this is for the echo command demonstration.
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    while (true) {
        process_stdin(&com_buff);
    }

    return 0;
}
