void * _malloc(size_t size);

void _free(void * ptr);
void * firstAlloc(size_t size);



struct block {
  size_t size;
  int free; //1 if free, 0 if not
  struct block *next;
  struct block *prev;
};

typedef struct block Block;
extern Block *head;
