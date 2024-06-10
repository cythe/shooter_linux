#include "type.h"
#include "buffer.h"

struct _sector_bullet sectors[SECTOR_SIZE];

void print_sector(struct _sector_bullet* s)
{
    printf("%p: start: \t%d\n"
	    "angle: \t%d\n"
	    "delta: \t%d\n"
	    "ch_di: \t%d\n"
	    "curre: \t%d\n"
	    "count: \t%d\n"
	    "status:\t%d\n",
	    s, s->start, s->angle, s->delta, s->change_direction, s->current, s->count, s->status);
}

struct _sector_bullet* get_sector(void)
{
    for(int i = 0; i < SECTOR_SIZE; i++)
    {
	if (sectors[i].count == 0 && sectors[i].status == 0) {
	    memset(&sectors[i], 0, sizeof(struct _sector_bullet));
	    return &sectors[i];
	}
    }

    printf("Please increase SECTOR_SIZE.\n");
    return NULL;
}

void put_sector(struct _sector_bullet* sector)
{
    sector->status = 0;
    sector->count = 0;
}
