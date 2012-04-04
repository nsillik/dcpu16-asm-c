#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOURCE_PATH "fixtures/sample.s"

// ----------------------------------------
// Types.

enum lexeme_types {
  LX_COMMENT,
  LX_INSTRUCTION
};

typedef struct {
  enum lexeme_types type;
  int size;
  char * value;
} lexeme_t;

// ----------------------------------------
// Function headers.

void crash(char * message);
void debug(char * message);

char * load_file(char * path);

void print_lexeme(lexeme_t *);
void read_lexeme(lexeme_t *, char **);
void free_lexeme(lexeme_t *);

// ----------------------------------------
// Main.

int main(int argc, const char ** argv)
{
  debug("main");

  char * source = load_file(SOURCE_PATH);
  char * lexable = source;

  for (int i = 0; i < 10; i ++)
  {
    lexeme_t * lx = (lexeme_t *)malloc(sizeof(lexeme_t));
    read_lexeme(lx, &lexable);
    print_lexeme(lx);
    free_lexeme(lx);
  }

  free(source);
  exit(0);
}

// ----------------------------------------
// Functions.

void crash(char * message)
{
  printf("!! Failed: %s !!\n", message);
  exit(EXIT_FAILURE);
}

void debug(char * message)
{
  printf("== %s ==\n", message);
}

char * load_file(char * path)
{
  debug("load_file");

  long size;
  FILE * fd;
  char * buffer;

  // open file
  if (!(fd = fopen(path, "r"))) crash("open file");

  // read size
  if (fseek(fd, 0, SEEK_END) == -1) crash("seek to end");
  size = ftell(fd);
  if (fseek(fd, 0, SEEK_SET) == -1) crash("seek to start");

  // allocate memory
  if (!(buffer = (char *)malloc(size))) crash("malloc");

  // read info buffer
  if (fread(buffer, 1, (size_t)size, fd) != size) crash("fread");

  if (fclose(fd) == -1) crash("fclose");
  return(buffer);
}

void print_lexeme(lexeme_t * l)
{
  debug("print_lexeme");
  printf("Lexeme:\n");
  printf("  type: %u\n", l->type);
  printf("  size: %u\n", l->size);
  printf("  value: '%s'\n", l->value);
}

void read_lexeme(lexeme_t * l, char ** source)
{
  debug("read_lexeme");

  int value_size;
  char * base = *source;
  char * ptr;
  char c;

  // Ignore leading whitespace.
  while ((c = *base) == ' ') base++;
  ptr = base;

  if (c == ';')
  {
    debug("lexing comment");
    for (ptr++; c != '\n'; c = *(ptr++));
    value_size = ptr - base - 1;
    l->type = LX_COMMENT;
    l->size = value_size;
    l->value = (char *)malloc(value_size + 1);
    strlcpy(l->value, base, value_size + 1);
    *source = ptr;
  }
  else if (c >= 'A' && c <= 'Z')
  {
    debug("lexing name");
    for (; c != ' ' && c != '\t'; c = (*(ptr++)));
    value_size = ptr - base - 1;
    l->type = LX_INSTRUCTION;
    l->size = value_size;
    l->value = (char *)malloc(value_size + 1);
    strlcpy(l->value, base, value_size + 1);
    *source = ptr;
  }
  else
  {
    crash("unhandled lexeme");
  }
}

void free_lexeme(lexeme_t * l)
{
  free(l->value);
  free(l);
}
