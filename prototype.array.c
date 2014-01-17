#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef FILE *Stream;
typedef char String[128];

/* Book */
typedef struct _Book {
  String name;
  String author;
  double price;
} *Book;

Book book_new(void) {
  return (Book)calloc(1, sizeof(struct _Book));
}

void book_delete(Book b) {
  free(b);
}

Book book_read(Stream in, Book b) {
  fscanf(in, "%s%s%le", b->name, b->author, &b->price);
  return b;
}

Book book_write(Stream out, Book b) {
  fprintf(out, "%s %s %.2f", b->name, b->author, b->price);
  return b;
}

/* Container */
typedef struct _Container {
  Book* data;
  size_t length;
  size_t capacity;
} *Container;

Container container_new(int limit) {
  Container c = (Container)calloc(1, sizeof(struct _Container));
  c->capacity = limit;
  c->length = 0;
  c->data = (Book*)calloc(limit, sizeof(Book));
  return c;
}

void container_delete(Container c) {
  free(c->data);
  free(c);
}

Book container_get(Container c, int n) {
  if (0 <= n && n < c->length) {
    return c->data[n];
  } else {
    return NULL;
  }
}

void container_add(Container c, Book b) {
  if (c->length < c->capacity) {
    c->data[c->length] = b;
    c->length++;
  }
}

Book container_remove(Container c, int n) {
  if (0 <= n && n < c->length) {
    Book o = c->data[n];
    c->length--;
    memmove(c->data + n, c->data + n + 1, c->length - n);
    return o;
  } else {
    return NULL;
  }
}

/* Function */
typedef void (*Function)(void);

Container root;

int input(char* prompt, int from, int to) {
  int n;
  do {
    printf("%s[%d-%d]: ", prompt, from, to);
    scanf("%d", &n);
  } while (n < from || to < n);
  return n;
}

void fn_welcome(void) {
  root = container_new(128);
  puts("Welcome!");
}

void fn_add(void) {
  container_add(root, book_read(stdin, book_new()));
}

void fn_edit(void) {
  book_read(stdin, container_get(root, input("id", 1, root->length) - 1));
}

void fn_delete(void) {
  book_delete(container_remove(root, input("id", 1, root->length) - 1));
}

void fn_list(void) {
  int i;
  for (i = 0; i < root->length; i++) {
    printf("%03d: ", i + 1);
    book_write(stdout, container_get(root, i));
    putchar('\n');
  }
}

void fn_search(void) {
  String name;
  int i;

  printf("name: ");
  scanf("%s", name);

  for (i = 0; i < root->length; i++) {
    Book b = container_get(root, i);
    if (strcmp(b->name, name) == 0) {
      book_write(stdout, b);
      putchar('\n');
    }
  }
}

void fn_load(void) {
  String filename;
  Stream fin;

  printf("Input path[data.txt]: ");
  scanf("%s", filename);
  fin = fopen(filename, "r");
  if (fin != NULL) {
    while (!feof(fin)) {
      container_add(root, book_read(fin, book_new()));
    }
    fclose(fin);
  }
}

void fn_save(void) {
  String filename;
  Stream fout;

  printf("Output path[data.txt]: ");
  scanf("%s", filename);
  fout = fopen(filename, "w");
  if (fout != NULL) {
    int i;
    for (i = 0; i < root->length; i++) {
      book_write(fout, container_get(root, i));
      fputc('\n', fout);
    }
    fclose(fout);
  }
}

void fn_quit(void) {
  int i;
  for (i = 0; i < root->length; i++)
    book_delete(container_get(root, i));
  container_delete(root);

  puts("Thanks!");
  exit(EXIT_SUCCESS);
}

/* Interface */
int menu(void) {
  int i;

  puts("1. add");
  puts("2. edit");
  puts("3. delete");
  puts("4. list");
  puts("5. search");
  puts("6. load");
  puts("7. save");
  puts("0. quit");
  return input("", 0, 7);
}

int main(void) {
  Function fn[] = {
    fn_quit,
    fn_add,
    fn_edit,
    fn_delete,
    fn_list,
    fn_search,
    fn_load,
    fn_save
  };

  fn_welcome();
  for (;;) {
    fn[menu()]();
  }

  return 0;
}
