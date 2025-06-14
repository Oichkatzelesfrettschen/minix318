#ifndef KERNEL_REPLY_OBJ_H
#define KERNEL_REPLY_OBJ_H

#include <minix/com.h> // For pid_t, if needed, or other types

// Forward declaration of struct proc, if we can't include proc.h
// struct proc;

// NR_PROCS should match the actual system limit if used for array sizing.
// It's used in proc.c for reply_objects_table.
#define NR_PROCS 64 // Example, should be from a config file eventually / same as in proc.c

#define NO_REPLY_OBJ -1 // Indicates no reply object is associated or slot is free

struct reply_object {
    int ro_caller_proc_nr;      // Process number of the SENDREC caller
    int ro_target_proc_nr;      // Process number of the target (potential replier)
    endpoint_t ro_caller_ep;    // Endpoint of the SENDREC caller
    message ro_reply_message;   // Buffer to store the reply message contents (used by SYS_REPLY)
    int ro_in_use;              // Flag: 0 = free, 1 = in use
    // Add other relevant fields as needed:
    // void *ro_message_addr_caller_userspace; // Original message buffer in caller's userspace (for debug or advanced features)
    // int ro_message_len_caller;    // Original message length
};

// Global array of reply objects is defined in proc.c
extern struct reply_object reply_objects_table[NR_PROCS];

// Function prototypes for managing reply objects (defined in proc.c)
// These are PRIVATE to proc.c for now. If called from other files, declare PUBLIC in proc.c and add to a .h file.
// void init_reply_objects(void); // Called from main.c
// int alloc_reply_object(int caller_nr, int target_nr, endpoint_t caller_ep);
// void free_reply_object(int obj_index);
// struct reply_object *get_reply_object(int obj_index); // Not implemented yet, direct access for now

#endif /* KERNEL_REPLY_OBJ_H */
