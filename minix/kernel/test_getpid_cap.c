#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>       // For standard getpid()
#include <minix/ipc.h>    // For message struct, and now for ipc_send_cap prototype
#include <minix/com.h>    // For PM_PROC_NR, KERNEL
#include <minix/callnr.h> // For PM_GETPID_CAP, PM_PID_REPLY (as defined in this session)
#include <minix/ipcconst.h> // For SYS_IPC_SEND_CAP (kernel syscall number)

// The prototype for ipc_send_cap is now in <minix/ipc.h>
// extern int ipc_send_cap(int cap_idx, message *msg_ptr);

#ifndef PM_CAP_IDX
// This should ideally be in a shared header related to capabilities or PM interaction
#define PM_CAP_IDX 0
#endif

// Ensure PM_GETPID_CAP and PM_PID_REPLY are available (they are in callnr.h)

int main(int argc, char *argv[]) {
    message m_req, m_reply;
    pid_t my_pid_std;
    pid_t my_pid_cap = -1; // Initialize to an error value
    int status_ipc;
    int result;

    printf("Test app: Attempting getpid() via new capability IPC path...\n");

    // Standard getpid() for comparison
    my_pid_std = getpid();
    printf("Standard getpid() result: %d\n", my_pid_std);

    // 1. Prepare message for PM
    m_req.m_type = PM_GETPID_CAP;
    // No other payload fields needed for this specific PM call

    printf("Test app: Sending PM_GETPID_CAP (type %d) using actual ipc_send_cap to PM_CAP_IDX %d\n",
           m_req.m_type, PM_CAP_IDX);

    // 2. Send to PM using the new SYS_IPC_SEND_CAP syscall via the libc wrapper
    result = ipc_send_cap(PM_CAP_IDX, &m_req);

    if (result != OK) {
        fprintf(stderr, "Test app: ipc_send_cap to PM failed with result/status %d\n", result);
        // perror("ipc_send_cap failed"); // perror might not be accurate for non-errno results
        return 1;
    }
    // printf("Test app: ipc_send_cap call succeeded. Now waiting for reply from PM...\n");

    // 3. Receive reply from PM
    // The kernel's sys_ipc_send_cap -> ipc_fast_send -> mini_send path has sent the message.
    // The application now does a standard receive from PM.
    result = receive(PM_PROC_NR, &m_reply, &status_ipc);
    if (result != OK) {
        fprintf(stderr, "Test app: receive from PM failed with result %d\n", result);
        // perror("receive from PM failed");
        return 1;
    }
    // printf("Test app: Received reply from PM. Message type: %d, IPC status from receive: %d\n",
    //        m_reply.m_type, status_ipc);

    if (m_reply.m_type == PM_PID_REPLY) {
        mypid_cap = m_reply.m1_i1; // PID is in m1_i1 as set by PM's do_getpid_cap
        printf("SUCCESS: getpid() via capability IPC returned PID: %d\n", mypid_cap);

        if (my_pid_std != -1 && mypid_cap != my_pid_std) {
            printf("WARNING: PID mismatch! Standard getpid() = %d, Capability getpid() = %d\n",
                   my_pid_std, mypid_cap);
        } else if (mypid_cap == my_pid_std) {
            printf("PID match with standard getpid(): SUCCESS!\n");
        }
    } else {
        printf("ERROR: Received unexpected message type %d (expected %d for PM_PID_REPLY) from PM.\n",
               m_reply.m_type, PM_PID_REPLY);
        return 1;
    }

    return 0;
}
