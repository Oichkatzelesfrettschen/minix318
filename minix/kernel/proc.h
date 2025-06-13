/************************************************************************************
 * @file    proc.h
 * @brief   Process and CPU structures for the MINIX-like kernel, updated for C23.
 *
 * This file contains the definitions of core structures that manage CPU states,
 * process states, scheduling contexts, and message passing in the kernel. It has
 * been modernized to conform to the C23 standard while preserving or extending
 * the original comments and design.
 *
 * Original comments and elements of structure are retained, while additional 
 * annotations and clarifications are introduced to address potential gaps,
 * promote clarity, and ensure maintainability. Where ambiguities reportedly 
 * existed, clarifying remarks have been included.
 *
 * ----------------------------------------------------------------------------------
 * Copyright:
 * The original license, copyright statements, and other details (if any)
 * should be preserved here. This modernization does not supersede or
 * replace any existing license terms.
 ************************************************************************************/

#pragma once

/*------------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------------*/
#include <stddef.h>   /* For size_t, NULL, etc. in C23 */
#include <stdint.h>   /* For fixed-width integer types  */
#include "param.h"
#include "mmu.h"
#include "x86.h"
#include "spinlock.h"
#include "ipc.h"
#include "exo.h"

/*------------------------------------------------------------------------------------
 * Compiler/Architecture Configuration
 *
 * Distinguishes between 32-bit x86, 64-bit x86, and 64-bit ARM targets to
 * conditionally alias context structures to a unified type.
 *----------------------------------------------------------------------------------*/
#if defined(__x86_64__) || defined(__aarch64__)
struct context64;
typedef struct context64 context_t;
#else
struct context;
typedef struct context context_t;
#endif

/*------------------------------------------------------------------------------------
 * CPU State
 *
 * This structure holds hardware/OS-specific CPU data, such as APIC IDs and 
 * scheduling contexts. It ensures that the kernel can switch to a scheduler
 * context upon interrupts or swtch() calls.
 *----------------------------------------------------------------------------------*/
/**
 * @struct cpu
 * @brief  Represents per-CPU state capturing APIC, scheduling context, and safer
 *         interrupt nesting.
 */
struct cpu {
  uint8_t apicid;             /**< Local APIC ID */
  context_t *scheduler;       /**< Scheduler context for swtch() entry */
#ifndef __aarch64__
  struct taskstate ts;        /**< x86-specific: used to find stack for interrupts */
  struct segdesc gdt[NSEGS];  /**< x86 global descriptor table */
#endif
  volatile uint32_t started;  /**< CPU startup flag: 1 if started, 0 otherwise */
  int ncli;                   /**< Depth of pushcli nesting */
  int intena;                 /**< Interrupt enable status prior to pushcli */
  struct proc *proc;          /**< The process running on this CPU, or NULL */
};

extern struct cpu cpus[NCPU];
extern int ncpu;

/*------------------------------------------------------------------------------------
 * Saved Registers for Kernel Context Switches
 *
 * The context is saved at the bottom of the stack; the stack pointer
 * points to the context. Separated contexts exist for 32-bit x86,
 * 64-bit x86, and 64-bit ARM.
 *----------------------------------------------------------------------------------*/

/**
 * @struct context
 * @brief  32-bit x86 context preserved during kernel context switches.
 *
 * The layout must match swtch.S at "Switch stacks". The code does not
 * explicitly save eip because it is already on the stack.
 */
struct context {
  uint32_t edi; /**< Saved EDI register */
  uint32_t esi; /**< Saved ESI register */
  uint32_t ebx; /**< Saved EBX register */
  uint32_t ebp; /**< Saved EBP register */
  uint32_t eip; /**< Instruction pointer */
};

/* 
 * Validate struct size matches assembly expectation: 5 registers → 20 bytes.
 */
_Static_assert(sizeof(struct context) == 20, "struct context size incorrect");

/**
 * @struct context64
 * @brief  64-bit context preserved during kernel context switches.
 *
 * For x86_64, we handle the typical callee-save registers, plus rip.
 * For aarch64, we store a subset of xN registers, the frame pointer, 
 * and the link register.
 */
#if defined(__x86_64__)
struct context64 {
  unsigned long r15;
  unsigned long r14;
  unsigned long r13;
  unsigned long r12;
  unsigned long rbx;
  unsigned long rbp;
  unsigned long rip;
};
#elif defined(__aarch64__)
struct context64 {
  unsigned long x19;
  unsigned long x20;
  unsigned long x21;
  unsigned long x22;
  unsigned long x23;
  unsigned long x24;
  unsigned long x25;
  unsigned long x26;
  unsigned long x27;
  unsigned long x28;
  unsigned long fp;
  unsigned long lr;
};
#endif

/*------------------------------------------------------------------------------------
 * Process States
 *
 * Enumerates the high-level scheduling states for a process.
 *----------------------------------------------------------------------------------*/
/**
 * @enum procstate
 * @brief Lifecycle states for a process: from allocation to exit.
 */
enum procstate {
  UNUSED,    /**< Slot not in use */
  EMBRYO,    /**< Initialization in progress */
  SLEEPING,  /**< Waiting for a resource or event */
  RUNNABLE,  /**< Ready to be scheduled */
  RUNNING,   /**< Actively executing on a CPU */
  ZOMBIE     /**< Terminated but resources not yet reclaimed */
};

/*------------------------------------------------------------------------------------
 * Simple Mailbox Queue for IPC
 *
 * A lock-based circular buffer storing message entries. The index pair (r, w)
 * tracks read/write positions. This mechanism provides basic messaging among 
 * processes.
 *----------------------------------------------------------------------------------*/
#define MAILBOX_BUFSZ 64

/**
 * @struct ipc_entry
 * @brief  Single buffer entry for messaging.
 */
struct ipc_entry {
  zipc_msg_t msg; /**< IPC message */
  exo_cap    frame; /**< Capability referencing additional data */
};

/**
 * @struct mailbox
 * @brief  Lock-guarded buffer for multi-producer/multi-consumer style IPC.
 */
struct mailbox {
  struct spinlock lock;         /**< Protection of shared data */
  struct ipc_entry buf[MAILBOX_BUFSZ]; /**< Circular buffer storage */
  uint32_t r;                   /**< Read index */
  uint32_t w;                   /**< Write index */
  int inited;                   /**< Non-zero means mailbox is ready */
};

/*------------------------------------------------------------------------------------
 * Per-Process State
 *
 * A process is represented by its memory (via page table), a stack, context 
 * switching data, open file references, mailbox, etc. The entire kernel
 * scheduling mechanism depends on these fields, which collectively define 
 * an address space and behaviors during kernel transitions.
 *----------------------------------------------------------------------------------*/
/**
 * @struct proc
 * @brief  Main process descriptor capturing register state, memory, and resources.
 */
struct proc {
  size_t sz;                    /**< Size of process memory (in bytes) */
  pde_t *pgdir;                 /**< Page table (architecture-dependent) */
  char *kstack;                 /**< Bottom of kernel stack for this process */
  enum procstate state;         /**< Process state enumeration */
  int pid;                      /**< Unique process ID */
  struct proc *parent;          /**< Pointer to parent process, if any */
  struct trapframe *tf;         /**< Trap frame for the current system call */
  context_t *context;           /**< Kernel context to switch to */
  void (*timer_upcall)(void);   /**< User-mode timer interrupt handler */
  void *chan;                   /**< Wait channel pointer */
  int killed;                   /**< Non-zero if process has been marked to die */
  int pending_signal;           /**< Simplified bitmask for signals */
  struct file *ofile[NOFILE];   /**< Open files reference table */
  struct inode *cwd;            /**< Pointer to current working directory inode */
  char name[16];                /**< Process name (debugging/tracing) */
  uint32_t pctr_cap;            /**< Capability for exo_pctr_transfer */
  volatile uint32_t pctr_signal;/**< Signal counter for exo_pctr_transfer */
  uint64_t gas_remaining;       /**< CPU budget tracking */
  int preferred_node;           /**< NUMA allocation preference */
  int out_of_gas;               /**< Flag set if process runs out of gas */
  struct mailbox *mailbox;      /**< IPC mailbox for the process */
  struct proc *rq_next;         /**< Next pointer for run queue */
  struct proc *rq_prev;         /**< Previous pointer for run queue */
};

/* 
 * Size consistency checks across architectures to ensure the 
 * scheduler depends on a fixed struct proc size. 
 */
#if defined(__x86_64__) || defined(__aarch64__)
_Static_assert(sizeof(struct proc) == 280, "struct proc size incorrect");
#else
_Static_assert(sizeof(struct proc) == 168, "struct proc size incorrect");
#endif

/*------------------------------------------------------------------------------------
 * Process Table
 *
 * The table of all processes in the system, protected by a spinlock for 
 * concurrency safety. 
 *----------------------------------------------------------------------------------*/
/**
 * @struct ptable
 * @brief  Holds the global process array and lock to ensure safe concurrent access.
 */
struct ptable {
  struct spinlock lock;        /**< Protection for the process list */
  struct proc proc[NPROC];     /**< Fixed-size array of processes */
};
