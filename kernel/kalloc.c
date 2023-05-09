// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

// reference count for each physical page to facilitate COW
#define PA2INDEX(pa) (((uint64)pa)/PGSIZE)
int cowcount[PHYSTOP/PGSIZE];


void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    cowcount[PA2INDEX(p)] = 1; // add into free list initially
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  acquire(&kmem.lock);
  int remain = --cowcount[PA2INDEX(pa)];
  release(&kmem.lock);

  if (remain > 0) {
    // only the last reference is allowed to really free this page
    return;
  }

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r) {
    memset((char *)r, 5, PGSIZE); // fill with junk
    int idx = PA2INDEX(r);
    if (cowcount[idx] != 0) {
      panic("kalloc: cowcount[idx] != 0");
    }
    cowcount[idx] = 1;
  }
  return (void*)r;
}

// increment the reference count for a physical address by 1
void adjustref(uint64 pa, int num) {
    if (pa >= PHYSTOP) {
        panic("addref: pa too big");
    }
    acquire(&kmem.lock);
    cowcount[PA2INDEX(pa)] += num;
    release(&kmem.lock);
}
