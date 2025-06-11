#include "kernel/system.h"
#include "kernel/proc.h"   // For proc_addr, RS_PROC_NR, PM_PROC_NR, proc_ptr, get_cpulocal_var
#include <minix/com.h>      // For RS_DEBUG_RESTART_SERVICE_RQ, PM_PROC_NR, SYSTEM, KERNEL_CALL, message
#include <minix/syslib.h>   // For _ENDPOINT_P
#include <string.h>         // For memset
#include <assert.h>         // For KASSERT (if used, though kprintf_stub is more common for kernel debug)

// Handler for SYS_DEBUG_RS_RESTART_PM kernel call
// This syscall is called by a test program. It tells the kernel to ask RS
// to simulate a restart of PM for epoch testing.
// Note: The 'caller' argument is the process calling this syscall (the test program).
// 'm_ptr' is the message from the test program, not used for parameters in this version.
int do_debug_rs_restart_pm(struct proc *caller, message *__unused m_ptr) {
    message send_msg;
    struct proc *rs_ptr;
    endpoint_t rs_ep = RS_PROC_NR; // RS_PROC_NR is already an endpoint_t
    int r;

    // FIXME TODO PLACEHOLDER: Add privilege check if only certain processes can trigger this.
    // For Phase 0.25, assuming any process can call this debug syscall.
    kprintf_stub("KERNEL: do_debug_rs_restart_pm: Received request from EP %d.\n", caller->p_endpoint);

    rs_ptr = proc_addr(_ENDPOINT_P(rs_ep));

    if (!rs_ptr || (rs_ptr->p_rts_flags & RTS_SLOT_FREE) || (rs_ptr->p_rts_flags & RTS_NO_ENDPOINT) ) {
        kprintf_stub("KERNEL: RS process (EP %d) not found or not active.\n", rs_ep);
        return ESRCH; // RS not available
    }

    memset(&send_msg, 0, sizeof(send_msg));
    send_msg.m_type = RS_DEBUG_RESTART_SERVICE_RQ;
    // We need to tell RS which service to "restart" (bump epoch for). For this debug call, it's PM.
    send_msg.m1_i1 = PM_PROC_NR; // Pass PM's endpoint as a parameter

    kprintf_stub("KERNEL: Sending RS_DEBUG_RESTART_SERVICE_RQ to RS (EP %d) for PM (EP %d).\n",
                 rs_ep, PM_PROC_NR);

    // Use mini_send to send a message from KERNEL (SYSTEM task) to RS.
    // This is an asynchronous send. RS will handle it and reply to KERNEL.
    // The kernel_call path in system.c expects this function to return a result
    // for the original caller (test program). So we need a way to get RS's reply.
    // For simplicity in Phase 0.25, we might make this a one-way notification to RS,
    // and the test program just assumes it worked or has a timeout.
    // However, the prompt's conceptual sendrec_kern implied waiting for RS.
    // To achieve that, KERNEL (SYSTEM task) would effectively do a SENDREC.
    // This is complex for SYSTEM task.
    //
    // Alternative: Use a synchronous send from kernel, which is also tricky.
    // Let's use asyn_send for now, which means the test program gets an immediate OK
    // if the message is queued to RS, not confirmation of RS action.
    // This means the test program's call to debug_rs_simulate_pm_restart() will return
    // almost immediately. The usleep() in the test program becomes important.

    // Re-evaluating based on prompt's use of sendrec_kern:
    // Kernel calls are usually synchronous from the caller's perspective.
    // The SYSTEM task itself doesn't usually block on sendrec.
    // A common pattern for kernel asking another server and waiting is:
    // 1. Kernel sends request.
    // 2. Kernel sets a flag or state indicating it's waiting for this specific reply.
    // 3. When the reply comes to SYSTEM, a handler matches it to the pending request.
    // This is too complex for Phase 0.25 debug call.
    //
    // Let's try to use ipc_sendrec, but it needs careful handling as KERNEL
    // is not a typical process. The `sendrec_kern` is not a standard MINIX primitive.
    // The standard way for kernel to initiate IPC is often by setting up a message
    // in a process slot and then scheduling that process to send it.
    //
    // Given the constraints, a simplified approach for this *debug* syscall:
    // We will attempt a synchronous-like interaction by having the kernel send
    // and then immediately try to receive. This is not robust but might work for a debug scenario
    // if RS replies quickly. This is still a bit of a conceptual stretch.
    // The most straightforward for a debug call is often just a send-notify and hope for the best,
    // or RS could notify back to the test program directly if it knew its endpoint.

    // For now, let's stick to the prompt's conceptual sendrec_kern, meaning the kernel call
    // should block until RS replies. This implies a kernel internal mechanism for this.
    // Since _kernel_call is used by RS to call SYSTEM, SYSTEM itself calling _kernel_call
    // to another server is not typical.
    // Let's use a direct send and assume the test program's usleep is the main synchronization.
    // The kernel call will return OK if the message is sent to RS.

    // If we are to implement the sendrec behavior as implied by the prompt:
    // This would require the SYSTEM task to be able to do a blocking send-receive.
    // The current kernel architecture might not directly support SYSTEM task blocking on sendrec.
    // Let's assume `sendrec_kern` is a placeholder for a future robust mechanism.
    // For Phase 0.25, we can simulate by sending and then having the kernel side of this call
    // not wait for an explicit reply from RS. The test program will just sleep.
    // The `caller` here is the test program. `proc_ptr` is the test program.
    // The message is being sent *from* SYSTEM *to* RS.

    // Simplification for Phase 0.25: Send an ASYNCHRONOUS message from SYSTEM to RS.
    // The test program will get OK if this send succeeds.
    // The test program's usleep() will allow RS to process it.

    // Correct way to send from kernel to a server:
    // 1. Lock the destination.
    // 2. Copy the message.
    // 3. Unlock destination.
    // 4. Notify destination.
    // This is essentially what mini_send does when caller is KERNEL.
    // The SYSTEM process is effectively KERNEL for this purpose.

    // Set source endpoint for the message to SYSTEM
    send_msg.m_source = SYSTEM;

    r = mini_send(proc_addr(SYSTEM_PROC_NR), rs_ep, &send_msg, FROM_KERNEL);

    if (r != OK) {
        kprintf_stub("KERNEL: Failed to send RS_DEBUG_RESTART_SERVICE_RQ to RS. Error: %d\n", r);
        return r;
    }

    // For this debug call, we don't wait for a reply from RS to SYSTEM.
    // The test program will get OK if the message was successfully sent to RS.
    // The actual epoch update's success/failure will be observed by the test program's
    // subsequent getpid_cap() call.
    kprintf_stub("KERNEL: RS_DEBUG_RESTART_SERVICE_RQ sent to RS. Test program should see effect after delay.\n");
    return OK;
}
