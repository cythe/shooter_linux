#ifndef BUFFER_H
#define BUFFER_H

#define SECTOR_SIZE 10 
extern struct _sector_bullet sectors[SECTOR_SIZE];

void print_sector(struct _sector_bullet* s);
struct _sector_bullet* get_sector();
void put_sector(struct _sector_bullet* sector);

#endif
