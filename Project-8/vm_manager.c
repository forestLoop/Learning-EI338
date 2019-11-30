#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define OFFSET_BITS 8
#define PAGE_BITS 8
#define FRAME_BITS 7    // the physical memory is smaller than the virtual one

#define PAGE_SIZE (1 << OFFSET_BITS)
#define FRAME_SIZE PAGE_SIZE
#define TLB_SIZE 16

#define NUMBER_OF_PAGES (1 << PAGE_BITS)
#define NUMBER_OF_FRAMES (1 << FRAME_BITS)
#define MEMORY_SIZE (FRAME_SIZE * NUMBER_OF_FRAMES)

#define GET_PAGE_NUMBER(address) ((address & ((1 << (OFFSET_BITS + PAGE_BITS))- (1 << OFFSET_BITS))) >> OFFSET_BITS)
#define GET_OFFSET(address) (address & ((1 << OFFSET_BITS) - 1))
#define GET_PHYSICAL_ADDRESS(frame, offset) ((frame_number << OFFSET_BITS) | offset)

#define BACKING_STORAGE_FILE "BACKING_STORE.bin"

typedef struct {
    uint8_t valid;
    uint32_t page, frame;
} Pair;

Pair TLB[TLB_SIZE];

int8_t memory[MEMORY_SIZE];
uint32_t page_table[NUMBER_OF_PAGES], next_available_frame, next_available_TLB;
uint8_t page_valid[NUMBER_OF_PAGES];
FILE *backing_storage, *input_file;

uint32_t total_cnt, page_fault_cnt, TLB_miss_cnt;

uint32_t select_victim_frame() {
    // FIFO
    if(next_available_frame < NUMBER_OF_FRAMES) {
        return next_available_frame++;
    }
    uint32_t victim = (next_available_frame++) % NUMBER_OF_FRAMES;
    for(size_t i = 0; i != NUMBER_OF_PAGES; ++i) {  // invalidate the victim page
        if(page_valid[i] && page_table[i] == victim) {
            page_valid[i] = 0;
            break;
        }
    }
    return victim;
}

void handle_page_fault(uint32_t page_number) {
    page_table[page_number] = select_victim_frame();
    fseek(backing_storage, page_number * PAGE_SIZE, SEEK_SET);
    fread(memory + page_table[page_number] * PAGE_SIZE, sizeof(int8_t), PAGE_SIZE, backing_storage);
    page_valid[page_number] = 1;
    ++page_fault_cnt;
}

int check_TLB(uint32_t page_number, uint32_t *frame_number) {
    for(size_t i = 0; i != TLB_SIZE; ++i) {
        if(TLB[i].valid && TLB[i].page == page_number) {
            *frame_number = TLB[i].frame;
            return 1;
        }
    }
    return 0;
}

void update_TLB(uint32_t page_number, uint32_t frame_number) {
    // FIFO
    size_t victim = next_available_TLB % TLB_SIZE;
    next_available_TLB = (next_available_TLB + 1) % TLB_SIZE;
    TLB[victim].valid = 1;
    TLB[victim].page = page_number, TLB[victim].frame = frame_number;
}


uint32_t translate_address(uint32_t logical) {
    ++total_cnt;
    uint32_t page_number, offset, frame_number;
    page_number = GET_PAGE_NUMBER(logical);
    offset = GET_OFFSET(logical);
    if(!check_TLB(page_number, &frame_number)) {    // TLB miss
        ++TLB_miss_cnt;
        if(page_valid[page_number] == 0) {    // page fault
            handle_page_fault(page_number);
        }
        frame_number = page_table[page_number];
        update_TLB(page_number, frame_number);
    }
    return GET_PHYSICAL_ADDRESS(frame_number, offset);
}

char access_memory(uint32_t physical) {
    return memory[physical];
}

int init(int argc, char **argv) {
    if(argc != 2) {
        printf("Incorrect number of arguments.\n");
        return -1;
    }
    backing_storage = fopen(BACKING_STORAGE_FILE, "rb");
    if(backing_storage == NULL) {
        printf("Unable to open the backing storage file: %s\n", BACKING_STORAGE_FILE);
        return -2;
    }
    input_file = fopen(argv[1], "r");
    if(input_file == NULL) {
        printf("Unable to open the input file: %s\n", argv[1]);
        return -3;
    }
    // initially, both the memory and page table are empty
    memset(page_valid, 0, sizeof(uint8_t) * NUMBER_OF_PAGES);
    next_available_frame = next_available_TLB = 0;
    return 0;
}

void clean_up() {
    if(input_file) {
        fclose(input_file);
    }
    if(backing_storage) {
        fclose(backing_storage);
    }
}

void display_usage() {
    printf("Usage:\n");
    printf("    ./vm_manager <input file>\n");
}

void display_statistics() {
    printf("Page fault rate: %.2f%%\n", (float)page_fault_cnt / total_cnt * 100);
    printf("TLB hit rate: %.2f%%\n", (float)(total_cnt - TLB_miss_cnt) / total_cnt * 100);
}


int main(int argc, char **argv) {
    if(init(argc, argv) != 0) {
        display_usage();
        clean_up();
        return 0;
    }
    char line[8];
    while(fgets(line, 8, input_file)) {
        uint32_t logical, physical;
        int8_t value;
        sscanf(line, "%u", &logical);
        physical = translate_address(logical);
        value = access_memory(physical);
        printf("Virtual address: %u Physical address: %u Value: %d\n", logical, physical, value);
    }
    display_statistics();
    clean_up();
    return 0;
}
