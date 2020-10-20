#include <iostream>
#include <cmath>
using namespace std;

#define PAGES_SIZE 10
#define SINGLE_PAGE_SIZE 256
#define MAX_BLOCKS_ON_PAGE 16
#define WHOLE_SIZE SINGLE_PAGE_SIZE * PAGES_SIZE

enum class State{
    FREE,
    SAME_BLOCKS,
    BLOCK_FROM_MULTIPLY_PAGES
};

struct Page{
    State state;
    uint8_t* start_of_page;
    uint16_t size_of_class;
    uint8_t list_of_using[MAX_BLOCKS_ON_PAGE];
};

class Allocator {
public:
    uint8_t* start;
    Page* pages;
    void init();
    void mem_dump();
    void mem_free(void* adr);
    void* mem_realloc(void* adr, uint16_t size);
    void* mem_alloc(uint16_t size);
    uint16_t align(uint16_t size);
    int find_free_pages_for_multi_block(uint16_t size);
    void unite_page(uint16_t page_index);
};

void Allocator::init() {
    this->pages = new Page[PAGES_SIZE];
    this->start = (uint8_t*) malloc(WHOLE_SIZE);
    for (int i = 0; i < PAGES_SIZE; i++) {
        this->pages[i].state = State::FREE;
        this->pages[i].start_of_page = this->start + (SINGLE_PAGE_SIZE * i);
        this->pages[i].size_of_class = SINGLE_PAGE_SIZE;
        for (int j = 0; j < sizeof(pages[i].list_of_using)/sizeof(*pages[i].list_of_using); j++) {
            this->pages[i].list_of_using[j] = 0;
        }
    }
}

void* Allocator::mem_alloc(uint16_t size) {
    if (align(size) > (SINGLE_PAGE_SIZE / 2)){
        int start_index = find_free_pages_for_multi_block(size);
        if (start_index != -1){
            uint16_t times = align(size) / SINGLE_PAGE_SIZE;
            for (int i = start_index; i < start_index + times; i++) {
                this->pages[i].state = State::BLOCK_FROM_MULTIPLY_PAGES;
                this->pages[i].size_of_class = align(size);
            }
            return pages[start_index].start_of_page;
        } else return NULL;
    } else {
        for (int i = 0; i < PAGES_SIZE; i++) {
            if (this->pages[i].state == State::SAME_BLOCKS && this->pages[i].size_of_class == align(size)) {
                for (int j = 0; j < SINGLE_PAGE_SIZE / pages[i].size_of_class; j++) {
                    if (pages[i].list_of_using[j] == 0) {
                        pages[i].list_of_using[j] = 1;
                        return pages[i].start_of_page + (pages[i].size_of_class * j);
                    }
                }
            }
        }
        for (int i = 0; i < PAGES_SIZE; i++) {
            if (this->pages[i].state == State::FREE) {
                this->pages[i].size_of_class = align(size);
                for (int j = 0; j < SINGLE_PAGE_SIZE / pages[i].size_of_class; j++) {
                    pages[i].list_of_using[0] = 1;
                    pages[i].state = State::SAME_BLOCKS;
                    return pages[i].start_of_page + (pages[i].size_of_class * j);
                }
            }
        }
    }
    return NULL;
}

void* Allocator::mem_realloc(void *adr, uint16_t size) {
    if (adr == NULL) {
        return mem_alloc(size);
    } else {
        for (int i = 0; i < PAGES_SIZE; i++) {
            if (pages[i].start_of_page <= adr && (pages[i].start_of_page + SINGLE_PAGE_SIZE - 1) > adr) {
                if (align(size) == pages[i].size_of_class) { return adr; }
                else {
                    if (align(size) < pages[i].size_of_class) {
                        mem_free(adr);
                        return mem_alloc(size);
                    } else {
                        uint16_t block_size = pages[i].size_of_class;
                        for (int j = 0; j < SINGLE_PAGE_SIZE / pages[i].size_of_class; j++) {
                            if ((uint8_t **) (pages[i].start_of_page + (pages[i].size_of_class * j)) <= adr &&
                                (uint8_t **) (pages[i].start_of_page + ((pages[i].size_of_class * (j + 1)) - 1)) >= adr) {
                                mem_free(adr);
                                void* new_adr = mem_alloc(size);
                                if (new_adr != NULL){ return new_adr; }
                                else {
                                    pages[i].size_of_class = block_size;
                                    pages[i].list_of_using[j] = 1;
                                    return NULL;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Allocator::mem_free(void *adr) {
    for (int i = 0; i < PAGES_SIZE; i++) {
        if (((uint8_t*) adr) >= this->pages[i].start_of_page && ((uint8_t*) adr) <= this->pages[i].start_of_page + (SINGLE_PAGE_SIZE - 1)){
            if (pages[i].state == State::SAME_BLOCKS){
                for (int j = 0; j < SINGLE_PAGE_SIZE / pages[i].size_of_class; j++) {
                    if ((uint8_t**)(pages[i].start_of_page + (pages[i].size_of_class * j)) <= adr && (uint8_t**)(pages[i].start_of_page + ((pages[i].size_of_class * (j + 1)) - 1)) >= adr){
                        pages[i].list_of_using[j] = 0;
                        unite_page(i);
                        return;
                    }
                }
            } else {
                uint16_t times = pages[i].size_of_class / SINGLE_PAGE_SIZE;
                for (int j = 0; j < times; j++) {
                    pages[i + j].size_of_class = SINGLE_PAGE_SIZE;
                    pages[i + j].state = State::FREE;
                }
            }
            break;
        }
    }
}

void Allocator::unite_page(uint16_t page_index) {
    for (int i = 0; i < SINGLE_PAGE_SIZE / pages[page_index].size_of_class; i++) {
        if (pages[page_index].list_of_using[i] == 1) return;
        if (i == (SINGLE_PAGE_SIZE / pages[page_index].size_of_class) - 1){
            pages[page_index].state = State::FREE;
            pages[page_index].size_of_class = SINGLE_PAGE_SIZE;
        }
    }
}

int Allocator::find_free_pages_for_multi_block(uint16_t size) {
    uint16_t times = align(size) / SINGLE_PAGE_SIZE;
    int index = -1;
    for (int i = 0; i < PAGES_SIZE; i++){
        if (this->pages[i].state == State::FREE && (i + times) < PAGES_SIZE) {
            if (times == 1) return i;
            for (int j = 1; j < times; j++) {
                if (this->pages[i + j].state != State::FREE) {
                    break;
                } else {
                    if ((times - 1) == j) {
                        return i;
                    }
                }
            }
        }
    }
    return index;
}

void Allocator::mem_dump() {
    cout << "MEMORY:" << endl;
    for (int i = 0; i < PAGES_SIZE; i++) {
        cout << "Page N" << (i + 1) << ", Size: " << SINGLE_PAGE_SIZE << endl;
        cout << "Start of page: " << (uint8_t**)pages[i].start_of_page << endl;
        cout << "State of page: " << (pages[i].state == State::FREE ? "FREE" : pages[i].state == State::SAME_BLOCKS ? "SAME BLOCKS" : "MULTY") << endl;
        if (pages[i].state == State::SAME_BLOCKS) {
            cout << "Size of Class: " << pages[i].size_of_class << endl;
            for (int j = 0; j < SINGLE_PAGE_SIZE / pages[i].size_of_class; j++) {
                cout << "Block N" << (j + 1) << ", using: " << (int)pages[i].list_of_using[j] << endl;
//                for (uint8_t k = 0; k < pages[i].size_of_class; k++) {
//                    cout << "I: " << (int)k << ", N: " << (uint8_t**)(pages[i].start_of_page + (pages[i].size_of_class * j) + k) << endl;
//                }
            }
        }
        if (pages[i].state == State::BLOCK_FROM_MULTIPLY_PAGES) {
            cout << "Size of Class: " << pages[i].size_of_class << endl;
        }
        cout << endl;
    }
}

uint16_t Allocator::align(uint16_t size) {
    uint16_t a = 0;
    while (true){
        if (size <= pow(2,(4 + a))) return pow(2,(4 + a));
        else a++;
    }
}

int main() {
    Allocator allocator;
    allocator.init();
    void * adr1 = allocator.mem_alloc(63);
    void * adr2 = allocator.mem_alloc(63);
    void * adr3 = allocator.mem_alloc(35);
    void * adr4 = allocator.mem_alloc(11);
    void * adr5 = allocator.mem_alloc(69);
    void * adr6 = allocator.mem_alloc(259);
//    allocator.mem_free(adr6);
//    allocator.mem_free(adr4);
//    allocator.mem_free(adr3);
//    allocator.mem_free(adr2);
//    allocator.mem_free(adr1);


allocator.mem_realloc(adr3, 79);
    allocator.mem_dump();
    return 0;
}
