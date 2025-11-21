#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

MemoryManager* m_init(int phys_size) {
	MemoryManager* mm = malloc(sizeof(MemoryManager));
	mm->phys = malloc(phys_size);
	mm->phys_used = 0;
	mm->segment = NULL;
	mm->seg_count = 0;
	mm->lru = NULL;
	mm->phys_size = phys_size;
	mm->lru_count = 0;
	memset(mm->phys, 0, phys_size);
	return mm;
}

void m_add_segment(MemoryManager* mm, int id, int size){
	mm->segment = realloc(mm->segment, (mm->seg_count+1)*sizeof(Segment));
	mm->segment[mm->seg_count].id = id;
	mm->segment[mm->seg_count].size = size;
	mm->segment[mm->seg_count].loaded = 0;
	mm->segment[mm->seg_count].phys_offset = -1;
	mm->seg_count++;
}

Segment* m_find_segment(MemoryManager* mm, int id){
	for(int i = 0; i<mm->seg_count; i++){
		if (mm->segment[i].id == id) return &mm->segment[i];
	}
	return NULL;
}

void m_evict(MemoryManager* mm){
	if(mm->lru_count == 0) return;
	int bro_id = mm->lru[0];
	Segment* bro = m_find_segment(mm, bro_id);
	if(bro && bro->loaded){
		mm->phys_used -= bro->size;
		bro->loaded = 0;
		bro->phys_offset = -1;
	}
	for(int i = 1; i<mm->lru_count; i++){
		mm->lru[i-1] = mm->lru[i];
	} 
	mm->lru_count--;
}

void m_load(MemoryManager* mm, Segment* seg){
	if(seg->size > mm->phys_size){
		printf("Segment is too big\n");
	}
        while(mm->phys_used + seg->size > mm->phys_size) {
            if(mm->lru_count == 0) {
                printf("No space to load segment %d\n", seg->id);
                return;            }
            m_evict(mm);
        }
	seg->phys_offset = mm->phys_used;
	seg->loaded = 1;
	mm->phys_used += seg->size;
	mm->lru = realloc(mm->lru, (mm->lru_count+1)*sizeof(int));
	mm->lru[mm->lru_count++] = seg->id;
	memset(mm->phys + seg->phys_offset, 0, seg->size);
}

void m_access(MemoryManager* mm, int id){
	Segment* seg = m_find_segment(mm, id);
	if(!seg) {printf("Not found segment with id %d \n", id); return;};
	if(!seg->loaded) m_load(mm, seg);
	printf("Founded segment with id %d \n", id);
}

void m_write(MemoryManager* mm, int id, int offset, unsigned char *data, int len){
	Segment* seg = m_find_segment(mm, id);
	if(!seg || !seg->loaded) return;
	if(offset+len > seg->size) return;
	memcpy(mm->phys + seg->phys_offset + offset, data, len);
}

void m_read(MemoryManager* mm, int id, int offset, unsigned char *out, int len){
	Segment* seg = m_find_segment(mm, id);
	if(!seg || !seg->loaded) return;
	if(offset+len > seg->size) return;
	memcpy(out, mm->phys + seg->phys_offset + offset, len);
}

