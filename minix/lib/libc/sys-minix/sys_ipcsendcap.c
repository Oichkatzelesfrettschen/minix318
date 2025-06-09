#include "syslib.h" // For _syscall and message type
#include <minix/ipc.h> // For message struct definition (though syslib.h might bring it)
#include <minix/ipcconst.h> // For SYS_IPC_SEND_CAP definition
#include <minix/com.h>      // For KERNEL endpoint

// int ipc_send_cap(int cap_idx, message *msg_ptr); // Prototype usually in a public header

/*
 * This function implements the libc wrapper for the SYS_IPC_SEND_CAP kernel call.
 * It uses the generic _syscall mechanism.
 * For IPC verbs handled directly by do_ipc (like SEND, RECEIVE, NOTIFY, SYS_IPC_SEND_CAP),
 * the typical _syscall(endpoint, call_number, message_struct_ptr) convention is:
 * - 'call_number' (e.g., SYS_IPC_SEND_CAP) is moved into register r1 (eax).
 * - 'endpoint' (the first argument to _syscall) is moved into register r2 (ebx).
 * - 'message_struct_ptr' (the third argument to _syscall) is moved into register r3 (ecx).
 *
 * The kernel's do_ipc(r1, r2, r3) then receives these.
 * For SYS_IPC_SEND_CAP, do_ipc expects:
 * - r1 (call_nr) = SYS_IPC_SEND_CAP
 * - r2 (src_dst for some calls) = cap_idx (capability index)
 * - r3 (m_ptr) = user_msg_ptr (pointer to the message payload)
 *
 * So, we map:
 * _syscall's 'endpoint' argument -> cap_idx
 * _syscall's 'call_number' argument -> SYS_IPC_SEND_CAP
 * _syscall's 'message_struct_ptr' argument -> msg_ptr (the user's message)
 * The first argument to _syscall (traditionally an endpoint) will be used to pass cap_idx.
 * KERNEL is not used as the first argument here because that slot is taken by cap_idx.
 * The kernel knows it's a kernel call (IPC verb) by the SYS_IPC_SEND_CAP number itself.
 */
int ipc_send_cap(int cap_idx, message *msg_ptr)
{
    return _syscall((endpoint_t)cap_idx, SYS_IPC_SEND_CAP, msg_ptr);
}
