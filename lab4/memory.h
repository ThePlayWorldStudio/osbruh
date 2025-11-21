#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>

typedef struct{
	int id;
	int size;
	int loaded;
	int phys_offset;
} Segment;

typedef struct{
	unsigned char *phys;
	int phys_size;
	int phys_used;

	Segment *segment;
	int seg_count;

	int *lru;
	int lru_count;
} MemoryManager;

MemoryManager* m_init(int phys_size);

void m_add_segment(MemoryManager* mm,int id, int size);

Segment* m_find_segment(MemoryManager* mm, int id);

void m_evict(MemoryManager* mm);

void m_load(MemoryManager* mm, Segment* seg);

void m_access(MemoryManager* mm, int id);

void m_write(MemoryManager* mm, int id, int offset, unsigned char *data, int len);

void m_read(MemoryManager* mm, int id, int offset, unsigned char *out, int len);

#endif
