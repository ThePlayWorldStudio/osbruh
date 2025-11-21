#include <stdio.h> 
#include <string.h>
#include "memory.h" 
int main() {
       	// 1. Инициализация менеджера
	 MemoryManager* mm = m_init(64); 
	 printf("Init: phys_size=%d, phys_used=%d\n", mm->phys_size, mm->phys_used); 
	 
	 // 2. Добавление сегментов
	 m_add_segment(mm, 1, 16);
	 m_add_segment(mm, 2, 32);
	 m_add_segment(mm, 3, 24);
	 printf("Segments added: count=%d\n", mm->seg_count);
	 
	 // 3. Доступ к сегменту (автоматическая загрузка)
	 m_access(mm, 1);
	 printf("Access seg1: loaded=%d, offset=%d\n", mm->segment[0].loaded, mm->segment[0].phys_offset);
	 
	 // 4. Запись и чтение
	 unsigned char data[5] = {'H','e','l','l','o'};
	 m_write(mm, 1, 0, data, 5);
	 unsigned char out[5];
	 m_read(mm, 1, 0, out, 5);
	 printf("Read seg1: %.*s\n", 5, out);
	 
	 // 5. Загрузка второго сегмента
	 m_access(mm, 2);
	 printf("Access seg2: loaded=%d, offset=%d\n", mm->segment[1].loaded, mm->segment[1].phys_offset);
	 
	 // 6. Попытка загрузить третий сегмент (вытеснение)
	 m_access(mm, 3);
	 printf("Access seg3: loaded=%d, offset=%d\n", mm->segment[2].loaded, mm->segment[2].phys_offset);
	 
	 // Проверим, что LRU вытеснил первый сегмент
	 printf("Seg1 after eviction: loaded=%d\n", mm->segment[0].loaded);
	 // 7. Повторный доступ к первому сегменту (должен снова загрузиться) 
	 m_access(mm, 1);
	 printf("Seg1 reloaded: loaded=%d, offset=%d\n", mm->segment[0].loaded, mm->segment[0].phys_offset);
	 return 0; 
}
