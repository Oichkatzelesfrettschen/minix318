#define _SYSTEM 1 // For libsys
#include "ramdiskfs.h"

ramdisk_file_t open_files[MAX_RAMDISK_FILES];
message m_in;
message m_out; // Defined as extern in ramdiskfs.h, so provide storage here
endpoint_t who_e;
int call_nr;

// Forward declaration for SEF startup
static void sef_local_startup(void);
static int sef_cb_init_fresh(int type, sef_init_info_t *info);
static int sef_cb_lu_state_save(int);
static int sef_cb_signal_handler(int signo, sigset_t *new_set);


int main(void) {
    sef_local_startup(); // Initialize with SEF

    printf("RamdiskFS: Starting up.\n");

    for(int i=0; i < MAX_RAMDISK_FILES; ++i) {
        open_files[i].in_use = 0;
        open_files[i].data_ptr = NULL;
        open_files[i].allocated_size = 0;
        open_files[i].current_size = 0;
        memset(open_files[i].name, 0, MAX_FILENAME_LEN);
    }

    while(1) {
        int r;
        if ((r = sef_receive_status(ANY, &m_in, &r)) != OK) { // Use sef_receive_status
            printf("RamdiskFS: sef_receive_status failed (%d)\n", r);
            continue;
        }
        who_e = m_in.m_source;
        call_nr = m_in.m_type;

        // Clear m_out before handling, to avoid stale data from previous replies
        memset(&m_out, 0, sizeof(m_out));
        m_out.m_source = SELF; // Set our endpoint as the source of the reply


        printf("RamdiskFS: Received message type %d from %d\n", call_nr, who_e);

        int result = ENOSYS; // Default error
        switch(call_nr) {
            case RAMDISKFS_OPEN:
                result = handle_open();
                break;
            case RAMDISKFS_READ:
            case RAMDISKFS_WRITE:
                result = handle_readwrite();
                break;
            case RAMDISKFS_CLOSE:
                result = handle_close();
                break;
            default:
                printf("RamdiskFS: Unknown call_nr %d\n", call_nr);
                result = EBADREQUEST; // Or ENOSYS
        }

        if (result != SUSPEND) { // SUSPEND means handler will reply later
            m_out.m_type = result; // Handler sets result (e.g. OK, EIO, or bytes_read/written)
                                   // For OPEN, OK means success, cap_idx is in m_out fields.
                                   // For READ/WRITE, result is bytes transferred or error.
                                   // For CLOSE, result is OK or error.
            if (sef_sendnb(who_e, &m_out) != OK) {
                printf("RamdiskFS: sef_sendnb to %d failed!\n", who_e);
            }
        }
    }
    return 0; // Should not reach here
}

static void sef_local_startup(void) {
    sef_setcb_init_fresh(sef_cb_init_fresh);
    sef_setcb_init_lu(sef_cb_init_fresh); // Simple restart for LU for now
    sef_setcb_init_restart(sef_cb_init_fresh);

    sef_setcb_lu_state_save(sef_cb_lu_state_save);
    sef_setcb_signal_handler(sef_cb_signal_handler);

    sef_startup();
}

static int sef_cb_init_fresh(int type, sef_init_info_t *info) {
    (void)type; (void)info;
    // Nothing special for RamdiskFS init for now beyond main() zeroing
    // and global variable initialization.
    printf("RamdiskFS: SEF Initialized (type %d)\n", type);
    return OK;
}

static int sef_cb_lu_state_save(int UNUSED(state)) {
    // RamdiskFS is stateless beyond its in-memory file representation for now.
    // No specific state saving needed for live update beyond what SEF does.
    // If we had persistent state or needed complex cleanup/reinit, it'd go here.
    return OK;
}

static int sef_cb_signal_handler(int signo, sigset_t *UNUSED(new_set)) {
    printf("RamdiskFS: Received signal %d\n", signo);
    if (signo == SIGTERM) {
        // Perform cleanup if necessary, e.g., free allocated memory
        for(int i=0; i < MAX_RAMDISK_FILES; ++i) {
            if (open_files[i].in_use && open_files[i].data_ptr != NULL) {
                // FIXME TODO: Replace with Exokernel memory free call
                // exo_free_mem_pages_placeholder(open_files[i].data_ptr, open_files[i].allocated_size);
                free(open_files[i].data_ptr); // Placeholder for Phase 0.5
                open_files[i].in_use = 0;
            }
        }
        printf("RamdiskFS: Terminating on SIGTERM.\n");
        sef_exit(0); // Graceful exit
    }
    return OK; // Other signals ignored for now
}
