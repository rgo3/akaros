#ifndef _ROS_ARCH_MMU_H
#define _ROS_ARCH_MMU_H

// All physical memory mapped at this address
#define KERNBASE        0x80000000
#define KERN_LOAD_ADDR  KERNBASE
#define ULIM            0x70000000

/* All arches must define this, which is the lower limit of their static
 * mappings, and where the dynamic mappings will start. */
#define KERN_DYN_TOP    KERNBASE

// Use this if needed in annotations
#define IVY_KERNBASE (0x8000U << 16)

#define L3PGSHIFT   12
#define L3PGSIZE    (1<<L3PGSHIFT)

#define L2PGSHIFT   (12+6)
#define L2PGSIZE    (1<<L2PGSHIFT)

#define L1PGSHIFT   (12+6+6)
#define L1PGSIZE    (1<<L1PGSHIFT)

#define PGSHIFT L3PGSHIFT
#define PGSIZE (1 << PGSHIFT)
#define PTSIZE L1PGSIZE

#define NOVPT

#ifndef __ASSEMBLER__
typedef unsigned long pte_t;
typedef unsigned long pde_t;
#endif

#endif
