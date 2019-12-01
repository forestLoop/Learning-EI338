# Project 8: Designing a Virtual Memory Manager

Designing a Virtual Memory Manager. (Operating System Concepts, 10th Edition, Chapter 10)

## Description

This project consists of writing a program that translates logical to physical addresses for a virtual address space of size 65,536 bytes. Your program will read from a file containing logical addresses and, using a TLB and a page table, will translate each logical address to its corresponding physical address and output the value of the byte stored at the translated physical address.

## Environment

- OS: Ubuntu 18.04 (Linux kernel version: 5.3.5)
- Compiler: GCC 7.4.0

## Basic Ideas

The program will basically do two things: address translation and memory access. In address translation, TLB and page table are used and if page fault occurs, demanding page will been performed.

## Details

### Address translation

According to the specification, a 32-bit logical address is composed of 16 unused bits, 8 page bits and 8 offset bits. So first extract the page number and offset from the logical address. If the page number is in the TLB, directly use the TLB; otherwise, there's a TLB miss and look it up in the page table. If this page is invalid, i.e., not in memory, then there's a page fault to handle.

```c
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
```

By the way, `GET_PAGE_NUMBER`, `GET_OFFSET` and `GET_PHYSICAL_ADDRESS` are macros for efficiency.

```c
#define GET_PAGE_NUMBER(address) ((address & ((1 << (OFFSET_BITS + PAGE_BITS))- (1 << OFFSET_BITS))) >> OFFSET_BITS)
#define GET_OFFSET(address) (address & ((1 << OFFSET_BITS) - 1))
#define GET_PHYSICAL_ADDRESS(frame, offset) ((frame_number << OFFSET_BITS) | offset)
```

### TLB

As there's no build-in mapping object in C language, TLB is implemented as an array of `Pair`. Then checking whether a page is in TLB needs to traverse the TLB, which is inefficient but practical as the `TLB_SIZE` is small. Also, I use FIFO algorithm to update the TLB.

```c
typedef struct {
    uint8_t valid;
    uint32_t page, frame;
} Pair;

Pair TLB[TLB_SIZE];

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
```

### Page table

When a page fault occurs, first select a victim frame in the memory and then load the page into that frame from the backing storage.

```c
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
```

## Result

Here're some demos of this program (FIFO as the page-replacement algorithm):

### 256 pages, 256 frames, 16 TLB entries

```bash
$ ./vm_manager addresses.txt
Virtual address: 16916 Physical address: 20 Value: 0
Virtual address: 62493 Physical address: 285 Value: 0
Virtual address: 30198 Physical address: 758 Value: 29
# ...
Virtual address: 9929 Physical address: 44745 Value: 0
Virtual address: 45563 Physical address: 46075 Value: 126
Virtual address: 12107 Physical address: 2635 Value: -46
Page fault rate: 24.40%
TLB hit rate: 5.40%
```

In this case, the output is exactly the same as the given `correct.txt`, except for the last two lines of statistics.

### 256 pages, 128 frames, 16 TLB entries

```bash
$ ./vm_manager addresses.txt
Virtual address: 16916 Physical address: 20 Value: 0
Virtual address: 62493 Physical address: 285 Value: 0
Virtual address: 30198 Physical address: 758 Value: 29
# ...
Virtual address: 9929 Physical address: 6089 Value: 0
Virtual address: 45563 Physical address: 6395 Value: 126
Virtual address: 12107 Physical address: 6475 Value: -46
Page fault rate: 53.80%
TLB hit rate: 5.40%
```

### 256 pages, 128 frames, 32 TLB entries

```bash
$ ./vm_manager addresses.txt
Virtual address: 16916 Physical address: 20 Value: 0
Virtual address: 62493 Physical address: 285 Value: 0
Virtual address: 30198 Physical address: 758 Value: 29
# ...
Virtual address: 9929 Physical address: 6089 Value: 0
Virtual address: 45563 Physical address: 6395 Value: 126
Virtual address: 12107 Physical address: 6475 Value: -46
Page fault rate: 53.80%
TLB hit rate: 11.80%
```