#include <iostream>
using namespace std;

#define PAGES_SIZE 10
#define SINGLE_PAGE_SIZE 256
#define WHOLE_SIZE SINGLE_PAGE_SIZE * PAGES_SIZE

enum State{
    FREE,
    DEVIDED,
    FULL
};

struct Page{
    State state;
    uint8_t* start_of_page;
    uint16_t size_of_block;
    uint8_t* list_of_using;
};

class Allocator {
public:
    uint8_t* start;
    Page* pages;
    void init();
    void mem_dump();
};

void Allocator::init() {
    pages = new Page[PAGES_SIZE];
    this->start = (uint8_t*) malloc(WHOLE_SIZE);
    for (int i = 0; i < PAGES_SIZE; i++) {
        pages[i].size_of_block = FREE;
        pages[i].size_of_block = SINGLE_PAGE_SIZE;
        pages[i].start_of_page = this->start + (SINGLE_PAGE_SIZE * i);
        pages[i].list_of_using = new uint8_t[1];
    }
}

void Allocator::mem_dump() {
    cout << "MEMORY:" << endl;
    for (int i = 0; i < PAGES_SIZE; i++) {
        cout << "Page N" << i << ", Size: " << pages[i].size_of_block << endl;
        cout << "State: " << pages[i].state << endl << endl;
    }
}

int main() {
    Allocator allocator;
    allocator.init();
    allocator.mem_dump();
    return 0;
}
