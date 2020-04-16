#include "paging.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/*  --------------------------------- PROBLEM 6 --------------------------------------
    Checkout PDF section 7 for this problem

    Page fault handler.

    When the CPU encounters an invalid address mapping in a page table,
    it invokes the OS via this handler.

    Your job is to put a mapping in place so that the translation can
    succeed. You can use free_frame() to make an available frame.
    Update the page table with the new frame, and don't forget
    to fill in the frame table.

    When you map a frame, you should update its LRU timestamp.

    Lastly, you must fill your newly-mapped page with data. If the page
    has never mapped before, just zero the memory out. Otherwise, the
    data will have been swapped to the disk when the page was
    evicted. Call swap_read() to pull the data back in.

    HINTS:
         - You will need to use the global variable current_process when
           setting the frame table entry.

    ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t address) {
    /* First, split the faulting address and locate the page table entry.
       Remember to keep a pointer to the entry so you can modify it later. */
    stats.page_faults += 1;
    vpn_t new_vpn = vaddr_vpn(address);
    pte_t* page_table_entry = (pte_t*)(mem + PTBR * PAGE_SIZE) + new_vpn;


    /* It's a page fault, so the entry obviously won't be valid. Grab
       a frame to use by calling free_frame(). */

    pfn_t ff = free_frame();

    /* Update the page table entry. Make sure you set any relevant values. */

    page_table_entry->pfn = ff;
    page_table_entry->valid = 1;
    page_table_entry->dirty = 0;

    /* Update the frame table. Make sure you set any relevant values. */
    fte_t* frame_table_entry = frame_table + (page_table_entry -> pfn);
    frame_table_entry -> vpn = new_vpn;
    frame_table_entry -> mapped = 1;
    frame_table_entry -> process = current_process;
    frame_table_entry -> protected = 0;



    /* Update the timestamp of the appropriate frame table entry with the provided get_current_timestamp function. */

    frame_table_entry -> timestamp = get_current_timestamp();

    /* Initialize the page's memory. On a page fault, it is not enough
     * just to allocate a new frame. We must load in the old data from
     * disk into the frame. If there was no old data on disk, then
     * we need to clear out the memory (why?).
     *
     * 1) Get a pointer to the new frame in memory.
     * 2) If the page has swap set, then we need to load in data from memory
     *    using swap_read().
     * 3) Else, just zero the page's memory. If the page is later written
     *    back, swap_write() will automatically allocate a swap entry.
     */
    void* new_page_frame = mem + ((page_table_entry -> pfn) * PAGE_SIZE);
    if (swap_exists(page_table_entry)) {
        swap_read(page_table_entry, new_page_frame);
    } else {
        memset(mem + ((page_table_entry -> pfn) * PAGE_SIZE), 0, PAGE_SIZE);
    }

}

#pragma GCC diagnostic pop
