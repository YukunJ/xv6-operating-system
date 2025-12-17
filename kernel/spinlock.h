// Mutual exclusion lock.
#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "types.h"

struct cpu; // forward declaration

struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

#endif // SPINLOCK_H

