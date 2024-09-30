#include "import.h"
#include <lib/debug.h>

#define PAGESIZE 4096
#define VM_USERLO 0x40000000
#define VM_USERHI 0xF0000000
#define VM_USERLO_PI (VM_USERLO / PAGESIZE)
#define VM_USERHI_PI (VM_USERHI / PAGESIZE)

/**
 * The initialization function for the allocation table AT.
 * It contains two major parts:
 * 1. Calculate the actual physical memory of the machine, and sets the number
 *    of physical pages (NUM_PAGES).
 * 2. Initializes the physical allocation table (AT) implemented in the MATIntro
 * layer based on the information available in the physical memory map table.
 *    Review import.h in the current directory for the list of available
 *    getter and setter functions.
 */
void pmem_init(unsigned int mbi_addr) {
  unsigned int nps;

  // TODO: Define your local variables here.
  unsigned int s_rows;          // rows in the physical table
  unsigned int highest_address; // highest address in the ranges
  unsigned int page_size = 4096;

  // Calls the lower layer initialization primitive.
  // The parameter mbi_addr should not be used in the further code.
  devinit(mbi_addr);

  /**
   * Calculate the total number of physical pages provided by the hardware and
   * store it into the local variable nps.
   * Hint: Think of it as the highest address in the ranges of the memory map
   * table, divided by the page size.
   */
  // TODO

  s_rows = get_size();
  highest_address = get_mms(s_rows - 1) + get_mml(s_rows - 1);
  nps = highest_address / page_size;
  set_nps(nps); // Setting the value computed above to NUM_PAGES.

  /**
   * Initialization of the physical allocation table (AT).
   *
   * In CertiKOS, all addresses < VM_USERLO or >= VM_USERHI are reserved by the
   * kernel. That corresponds to the physical pages from 0 to VM_USERLO_PI - 1,
   * and from VM_USERHI_PI to NUM_PAGES - 1.
   * The rest of the pages that correspond to addresses [VM_USERLO, VM_USERHI)
   * can be used freely ONLY IF the entire page falls into one of the ranges in
   * the memory map table with the permission marked as usable.
   *
   * Hint:
   * 1. You have to initialize AT for all the page indices from 0 to NPS - 1.
   * 2. For the pages that are reserved by the kernel, simply set its permission
   * to 1. Recall that the setter at_set_perm also marks the page as
   * unallocated. Thus, you don't have to call another function to set the
   * allocation flag.
   * 3. For the rest of the pages, explore the memory map table to set its
   * permission accordingly. The permission should be set to 2 only if there is
   * a range containing the entire page that is marked as available in the
   * memory map table. Otherwise, it should be set to 0. Note that the ranges in
   * the memory map are not aligned by pages, so it may be possible that for
   * some pages, only some of the addresses are in a usable range. Currently, we
   * do not utilize partial pages, so in that case, you should consider those
   * pages as unavailable.
   */
  // TODO

  // Initialize kernel-reserved low memory
  for (int i = 0; i < VM_USERLO_PI; i++) {
    at_set_perm(i, 1);
  }
  // Initialize user space memory
  for (unsigned int i = VM_USERLO_PI; i < VM_USERHI_PI; i++) {
    unsigned int page_start = i * PAGESIZE;
    unsigned int page_end = page_start + PAGESIZE - 1;
    int is_usable_page = 0;

    // Check if the entire page falls within a usable range
    for (unsigned int j = 0; j < s_rows; j++) {
      unsigned int range_start = get_mms(j);
      unsigned int range_end = range_start + get_mml(j) - 1;

      if (is_usable(j) && page_start >= range_start && page_end <= range_end) {
        is_usable_page = 1;
        break;
      }
    }

    at_set_perm(i, is_usable_page ? 2 : 0);
  }

  // Initialize kernel-reserved high memory
  for (unsigned int i = VM_USERHI_PI; i < nps; i++) {
    at_set_perm(i, 1);
  }
}
