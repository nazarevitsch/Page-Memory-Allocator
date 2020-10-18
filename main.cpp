#include <iostream>
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
    uint16_t size_of_block;
    uint16_t size_of_class;
    uint8_t list_of_using[MAX_BLOCKS_ON_PAGE];
};

class Allocator {
public:
    uint8_t* start;
    Page* pages;
    void init();
    void mem_dump();
//    void createPage(Page* page, State state, uint16_t page_size, );
};

void Allocator::init() {
    pages = new Page[PAGES_SIZE];
    this->start = (uint8_t*) malloc(WHOLE_SIZE);
    for (int i = 0; i < PAGES_SIZE; i++) {
        pages[i].state = State::FREE;
        pages[i].size_of_block = SINGLE_PAGE_SIZE;
        pages[i].start_of_page = this->start + (SINGLE_PAGE_SIZE * i);
        pages[i].size_of_class = 256;
        for (int j = 0; j < sizeof(pages[i].list_of_using)/sizeof(*pages[i].list_of_using); j++) {
            pages[i].list_of_using[j] = 0;
        }
        cout << endl;
    }
}

void Allocator::mem_dump() {
    cout << "MEMORY:" << endl;
    for (int i = 0; i < PAGES_SIZE; i++) {
        cout << "Page N" << i << ", Size: " << pages[i].size_of_block << endl;
        cout << "Start of page: " << (uint8_t**)pages[i].start_of_page << endl;
        cout << "State of page: " << (pages[i].state == State::FREE ? "FREE" : pages[i].state == State::SAME_BLOCKS ? "SAME BLOCKS" : "MULTY") << endl;
        for (int j = 0; j < pages[i].size_of_block/pages[i].size_of_class; j++) {
            cout << "Block N" << j << ", using: " << (int)pages[i].list_of_using[j] << endl;
        }
        cout << endl;
    }
}

int main() {
    Allocator allocator;
    allocator.init();
    allocator.mem_dump();
    return 0;
}
