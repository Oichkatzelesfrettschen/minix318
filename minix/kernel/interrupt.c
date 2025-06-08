/**
 * @file interrupt.c
 * @brief Minix hardware interrupt system management
 * 
 * This module provides functionality for managing hardware interrupt handlers
 * in the Minix operating system. It maintains a registry of interrupt handlers
 * and provides mechanisms for registering, unregistering, and handling interrupts.
 * 
 * The system supports multiple handlers per IRQ line through a linked list
 * structure, with each handler having a unique ID bit for tracking active states.
 * 
 * Key Features:
 * - Dynamic registration/deregistration of interrupt handlers
 * - Support for shared IRQ lines with multiple handlers
 * - Spurious interrupt detection and reporting
 * - Hardware interrupt masking/unmasking coordination
 * - Active handler tracking via bitmask system
 * 
 * @author Minix Development Team
 * @version 3.1.8
 */

/**
 * @brief Array of IRQ handler lists, one per supported interrupt vector
 * 
 * Each element points to the head of a linked list of interrupt handlers
 * for the corresponding IRQ line. NULL indicates no handlers registered.
 */

/**
 * @brief Register an interrupt handler for a specific IRQ line
 * 
 * Adds a new interrupt handler to the linked list for the specified IRQ.
 * Automatically assigns a unique ID bit to the handler and enables the
 * hardware interrupt line if this is the first handler.
 * 
 * @param hook Pointer to the IRQ hook structure to register
 * @param irq IRQ number (must be 0 <= irq < NR_IRQ_VECTORS)
 * @param handler Function pointer to the interrupt handler routine
 * 
 * @pre hook must point to valid irq_hook_t structure
 * @pre irq must be valid IRQ number within supported range
 * @pre handler must be valid function pointer
 * 
 * @post Handler is added to IRQ list with unique ID
 * @post Hardware IRQ line is enabled if first handler
 * 
 * @panic If IRQ number is out of valid range
 * @panic If too many handlers registered for single IRQ (ID overflow)
 */

/**
 * @brief Unregister an interrupt handler from its IRQ line
 * 
 * Removes the specified handler from the IRQ handler list and disables
 * the hardware interrupt line if no other handlers remain registered.
 * Also clears any active state for the handler.
 * 
 * @param hook Pointer to the IRQ hook structure to remove
 * 
 * @pre hook must point to previously registered irq_hook_t
 * @pre hook->irq must be valid IRQ number
 * 
 * @post Handler removed from IRQ list
 * @post Hardware IRQ masked if no handlers remain
 * @post Hardware IRQ unmasked if other inactive handlers exist
 * 
 * @panic If IRQ number in hook is out of valid range
 */

/**
 * @brief Handle a hardware interrupt for the specified IRQ line
 * 
 * Called by system-dependent code when a hardware interrupt occurs.
 * Iterates through all registered handlers for the IRQ, marking each
 * active and calling their handler functions. Manages hardware interrupt
 * masking to prevent spurious interrupts during processing.
 * 
 * @param irq IRQ number of the interrupt to handle
 * 
 * @pre irq must be valid IRQ number (0 <= irq < NR_IRQ_VECTORS)
 * 
 * @post All registered handlers called with appropriate ID marking
 * @post Hardware IRQ unmasked only if all handlers completed successfully
 * @post Spurious interrupt logged if no handlers registered
 * 
 * @note Spurious interrupts are tracked and reported periodically
 * @note IRQ remains masked after spurious interrupt detection
 */

/**
 * @brief Enable interrupt processing for a specific handler
 * 
 * Clears the active bit for the specified handler and unmasks the
 * hardware IRQ line if no other handlers remain active.
 * 
 * @param hook Pointer to the IRQ hook to enable
 * 
 * @pre hook must point to valid registered irq_hook_t
 * 
 * @post Handler's active bit cleared in irq_actids
 * @post Hardware IRQ unmasked if no active handlers remain
 */

/**
 * @brief Disable interrupt processing for a specific handler
 * 
 * Sets the active bit for the specified handler and masks the
 * hardware IRQ line to prevent further interrupts.
 * 
 * @param hook Pointer to the IRQ hook to disable
 * @return int TRUE if interrupt was previously enabled, 0 if already disabled
 * 
 * @pre hook must point to valid registered irq_hook_t
 * 
 * @post Handler's active bit set in irq_actids
 * @post Hardware IRQ masked
 */
/*
 *   The Minix hardware interrupt system.
 *   
 *   This file contains routines for managing the interrupt
 *   controller.
 *  
 *   put_irq_handler: register an interrupt handler.
 *   rm_irq_handler:  deregister an interrupt handler.
 *   irq_handle:     handle a hardware interrupt.
 *                    called by the system dependent part when an
 *                    external interrupt occurs.
 *   enable_irq:      enable hook for IRQ.
 *   disable_irq:     disable hook for IRQ.
 */

// #include <assert.h> // Replaced

#include "kernel/kernel.h"
#include "hw_intr.h"

// Added kernel headers
#include <minix/kernel_types.h>
#include <sys/kassert.h>
#include <klib/include/kprintf.h> // For KASSERT_PLACEHOLDER and kprintf_stub
#include <klib/include/kstring.h> // Precautionary
#include <klib/include/kmemory.h> // Precautionary


/* number of lists of IRQ hooks, one list per supported line. */
static irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
void put_irq_handler( irq_hook_t* hook, int irq,
  const irq_handler_t handler)
{
  int id;
  irq_hook_t **line;
  unsigned long bitmap;

  if( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  line = &irq_handlers[irq];

  bitmap = 0;
  while ( *line != NULL ) {
	if(hook == *line) return; /* extra initialization */
	bitmap |= (*line)->id;	/* mark ids in use */
	line = &(*line)->next;
  }

  /* find the lowest id not in use */
  for (id = 1; id != 0; id <<= 1)
  	if (!(bitmap & id)) break;

  if(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->next = NULL;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *line = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  if((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
void rm_irq_handler( const irq_hook_t* hook ) {
  const int irq = hook->irq; 
  const int id = hook->id;
  irq_hook_t **line;

  if( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  line = &irq_handlers[irq];
  while( (*line) != NULL ) {
	if((*line)->id == id) {
		(*line) = (*line)->next;
		if (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	else {
		line = &(*line)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  if (irq_handlers[irq] == NULL) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  else if (irq_actids[irq] == 0) {
	hw_intr_unmask(irq);
  }
}

/*===========================================================================*
 *				irq_handle				     *
 *===========================================================================*/
/*
 * The function first disables interrupt is need be and restores the state at
 * the end. Before returning, it unmasks the IRQ if and only if all active ID
 * bits are cleared, and restart a process.
 */
void irq_handle(int irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  KASSERT(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  if(hook == NULL) {
      static int nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      if(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
	kprintf_stub("irq_handle: spurious irq %d (count: %d); keeping masked\n", // MODIFIED
		irq, nspurious[irq]);
	if(report_interval < (0x7FFFFFFF/2) /* INT_MAX might be undefined */ ) // MODIFIED INT_MAX with a large number
		report_interval *= 2;
      }
      return;
  }

  /* Call list of handlers for an IRQ. */
  while( hook != NULL ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    if( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  if (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
void enable_irq(const irq_hook_t *hook)
{
  if((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
int disable_irq(const irq_hook_t *hook)
{
  if(irq_actids[hook->irq] & hook->id)  /* already disabled */
    return 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  return TRUE;
}
