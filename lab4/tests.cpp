#include "memory.h"
#include <UnitTest++/UnitTest++.h>
#include <cstring>

struct MemoryManagerFixture {
    MemoryManager* mm;

    MemoryManagerFixture() {
        mm = m_init(64); // физическая память 64 байта
    }

    ~MemoryManagerFixture() {
        free(mm->phys);
        free(mm->segment);
        free(mm->lru);
        free(mm);
    }
};

SUITE(MemoryManagerTests) {

    TEST_FIXTURE(MemoryManagerFixture, AddAndFindSegment) {
        m_add_segment(mm, 1, 16);
        Segment* seg = m_find_segment(mm, 1);
        CHECK(seg != nullptr);
        CHECK_EQUAL(1, seg->id);
        CHECK_EQUAL(16, seg->size);
        CHECK_EQUAL(0, seg->loaded);
    }

    TEST_FIXTURE(MemoryManagerFixture, LoadSegment) {
        m_add_segment(mm, 2, 32);
        Segment* seg = m_find_segment(mm, 2);
        m_load(mm, seg);
        CHECK(seg->loaded);
        CHECK(mm->phys_used == 32);
    }

    TEST_FIXTURE(MemoryManagerFixture, EvictSegment) {
        m_add_segment(mm, 3, 32);
        Segment* seg = m_find_segment(mm, 3);
        m_load(mm, seg);
        CHECK(seg->loaded);

        m_evict(mm);
        CHECK(!seg->loaded);
        CHECK_EQUAL(-1, seg->phys_offset);
        CHECK_EQUAL(0, mm->phys_used);
    }

    TEST_FIXTURE(MemoryManagerFixture, WriteAndReadData) {
        m_add_segment(mm, 4, 16);
        Segment* seg = m_find_segment(mm, 4);
        m_load(mm, seg);

        unsigned char data[5] = {'H','e','l','l','o'};
        m_write(mm, 4, 0, data, 5);

        unsigned char out[5];
        m_read(mm, 4, 0, out, 5);

        CHECK_EQUAL(0, memcmp(data, out, 5));
    }

    TEST_FIXTURE(MemoryManagerFixture, AccessLoadsSegmentIfNotLoaded) {
        m_add_segment(mm, 5, 8);
        Segment* seg = m_find_segment(mm, 5);
        CHECK(!seg->loaded);

        m_access(mm, 5);
        CHECK(seg->loaded);
    }

    TEST_FIXTURE(MemoryManagerFixture, SegmentTooBig) {
        m_add_segment(mm, 6, 128); // больше чем phys_size
        Segment* seg = m_find_segment(mm, 6);
        m_load(mm, seg);
        CHECK(!seg->loaded); // не должен загрузиться
    }
}

