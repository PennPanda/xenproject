/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Copyright (c) 2007, Keir Fraser
 */

#ifndef __XEN_PUBLIC_HVM_HVM_OP_H__
#define __XEN_PUBLIC_HVM_HVM_OP_H__

#include "../xen.h"
#include "../trace.h"
#include "../event_channel.h"

/* Get/set subcommands: extra argument == pointer to xen_hvm_param struct. */
#define HVMOP_set_param           0
#define HVMOP_get_param           1
struct xen_hvm_param {
    domid_t  domid;    /* IN */
    uint32_t index;    /* IN */
    uint64_t value;    /* IN/OUT */
};
typedef struct xen_hvm_param xen_hvm_param_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_param_t);

/* Set the logical level of one of a domain's PCI INTx wires. */
#define HVMOP_set_pci_intx_level  2
struct xen_hvm_set_pci_intx_level {
    /* Domain to be updated. */
    domid_t  domid;
    /* PCI INTx identification in PCI topology (domain:bus:device:intx). */
    uint8_t  domain, bus, device, intx;
    /* Assertion level (0 = unasserted, 1 = asserted). */
    uint8_t  level;
};
typedef struct xen_hvm_set_pci_intx_level xen_hvm_set_pci_intx_level_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_set_pci_intx_level_t);

/* Set the logical level of one of a domain's ISA IRQ wires. */
#define HVMOP_set_isa_irq_level   3
struct xen_hvm_set_isa_irq_level {
    /* Domain to be updated. */
    domid_t  domid;
    /* ISA device identification, by ISA IRQ (0-15). */
    uint8_t  isa_irq;
    /* Assertion level (0 = unasserted, 1 = asserted). */
    uint8_t  level;
};
typedef struct xen_hvm_set_isa_irq_level xen_hvm_set_isa_irq_level_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_set_isa_irq_level_t);

#define HVMOP_set_pci_link_route  4
struct xen_hvm_set_pci_link_route {
    /* Domain to be updated. */
    domid_t  domid;
    /* PCI link identifier (0-3). */
    uint8_t  link;
    /* ISA IRQ (1-15), or 0 (disable link). */
    uint8_t  isa_irq;
};
typedef struct xen_hvm_set_pci_link_route xen_hvm_set_pci_link_route_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_set_pci_link_route_t);

/* Flushes all VCPU TLBs: @arg must be NULL. */
#define HVMOP_flush_tlbs          5

typedef enum {
    HVMMEM_ram_rw,             /* Normal read/write guest RAM */
    HVMMEM_ram_ro,             /* Read-only; writes are discarded */
    HVMMEM_mmio_dm,            /* Reads and write go to the device model */
#if __XEN_INTERFACE_VERSION__ < 0x00040700
    HVMMEM_mmio_write_dm,      /* Read-only; writes go to the device model */
#else
    HVMMEM_unused,             /* Placeholder; setting memory to this type
                                  will fail for code after 4.7.0 */
#endif
    HVMMEM_ioreq_server
} hvmmem_type_t;

/* Following tools-only interfaces may change in future. */
#if defined(__XEN__) || defined(__XEN_TOOLS__)

/* Notify that some pages got modified by the Device Model. */
#define HVMOP_modified_memory    7
struct xen_hvm_modified_memory {
    /* Domain to be updated. */
    domid_t  domid;
    /* Number of pages. */
    uint32_t nr;
    /* First pfn. */
    uint64_aligned_t first_pfn;
};
typedef struct xen_hvm_modified_memory xen_hvm_modified_memory_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_modified_memory_t);

#define HVMOP_set_mem_type    8
/* Notify that a region of memory is to be treated in a specific way. */
struct xen_hvm_set_mem_type {
    /* Domain to be updated. */
    domid_t domid;
    /* Memory type */
    uint16_t hvmmem_type;
    /* Number of pages. */
    uint32_t nr;
    /* First pfn. */
    uint64_aligned_t first_pfn;
};
typedef struct xen_hvm_set_mem_type xen_hvm_set_mem_type_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_set_mem_type_t);

#endif /* defined(__XEN__) || defined(__XEN_TOOLS__) */

/* Hint from PV drivers for pagetable destruction. */
#define HVMOP_pagetable_dying        9
struct xen_hvm_pagetable_dying {
    /* Domain with a pagetable about to be destroyed. */
    domid_t  domid;
    uint16_t pad[3]; /* align next field on 8-byte boundary */
    /* guest physical address of the toplevel pagetable dying */
    uint64_t gpa;
};
typedef struct xen_hvm_pagetable_dying xen_hvm_pagetable_dying_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_pagetable_dying_t);

/* Get the current Xen time, in nanoseconds since system boot. */
#define HVMOP_get_time              10
struct xen_hvm_get_time {
    uint64_t now;      /* OUT */
};
typedef struct xen_hvm_get_time xen_hvm_get_time_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_get_time_t);

#define HVMOP_xentrace              11
struct xen_hvm_xentrace {
    uint16_t event, extra_bytes;
    uint8_t extra[TRACE_EXTRA_MAX * sizeof(uint32_t)];
};
typedef struct xen_hvm_xentrace xen_hvm_xentrace_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_xentrace_t);

/* Following tools-only interfaces may change in future. */
#if defined(__XEN__) || defined(__XEN_TOOLS__)

/* Deprecated by XENMEM_access_op_set_access */
#define HVMOP_set_mem_access        12

/* Deprecated by XENMEM_access_op_get_access */
#define HVMOP_get_mem_access        13

#define HVMOP_inject_trap            14
/* Inject a trap into a VCPU, which will get taken up on the next
 * scheduling of it. Note that the caller should know enough of the
 * state of the CPU before injecting, to know what the effect of
 * injecting the trap will be.
 */
struct xen_hvm_inject_trap {
    /* Domain to be queried. */
    domid_t domid;
    /* VCPU */
    uint32_t vcpuid;
    /* Vector number */
    uint32_t vector;
    /* Trap type (HVMOP_TRAP_*) */
    uint32_t type;
/* NB. This enumeration precisely matches hvm.h:X86_EVENTTYPE_* */
# define HVMOP_TRAP_ext_int    0 /* external interrupt */
# define HVMOP_TRAP_nmi        2 /* nmi */
# define HVMOP_TRAP_hw_exc     3 /* hardware exception */
# define HVMOP_TRAP_sw_int     4 /* software interrupt (CD nn) */
# define HVMOP_TRAP_pri_sw_exc 5 /* ICEBP (F1) */
# define HVMOP_TRAP_sw_exc     6 /* INT3 (CC), INTO (CE) */
    /* Error code, or ~0u to skip */
    uint32_t error_code;
    /* Intruction length */
    uint32_t insn_len;
    /* CR2 for page faults */
    uint64_aligned_t cr2;
};
typedef struct xen_hvm_inject_trap xen_hvm_inject_trap_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_inject_trap_t);

#endif /* defined(__XEN__) || defined(__XEN_TOOLS__) */

#define HVMOP_get_mem_type    15
/* Return hvmmem_type_t for the specified pfn. */
struct xen_hvm_get_mem_type {
    /* Domain to be queried. */
    domid_t domid;
    /* OUT variable. */
    uint16_t mem_type;
    uint16_t pad[2]; /* align next field on 8-byte boundary */
    /* IN variable. */
    uint64_t pfn;
};
typedef struct xen_hvm_get_mem_type xen_hvm_get_mem_type_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_get_mem_type_t);

/* Following tools-only interfaces may change in future. */
#if defined(__XEN__) || defined(__XEN_TOOLS__)

/* MSI injection for emulated devices */
#define HVMOP_inject_msi         16
struct xen_hvm_inject_msi {
    /* Domain to be injected */
    domid_t   domid;
    /* Data -- lower 32 bits */
    uint32_t  data;
    /* Address (0xfeexxxxx) */
    uint64_t  addr;
};
typedef struct xen_hvm_inject_msi xen_hvm_inject_msi_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_inject_msi_t);

/*
 * Definitions relating to DMOP_create_ioreq_server. (Defined here for
 * backwards compatibility).
 */

#define HVM_IOREQSRV_BUFIOREQ_OFF    0
#define HVM_IOREQSRV_BUFIOREQ_LEGACY 1
/*
 * Use this when read_pointer gets updated atomically and
 * the pointer pair gets read atomically:
 */
#define HVM_IOREQSRV_BUFIOREQ_ATOMIC 2

#endif /* defined(__XEN__) || defined(__XEN_TOOLS__) */

#if defined(__i386__) || defined(__x86_64__)

/*
 * HVMOP_set_evtchn_upcall_vector: Set a <vector> that should be used for event
 *                                 channel upcalls on the specified <vcpu>. If set,
 *                                 this vector will be used in preference to the
 *                                 domain global callback via (see
 *                                 HVM_PARAM_CALLBACK_IRQ).
 */
#define HVMOP_set_evtchn_upcall_vector 23
struct xen_hvm_evtchn_upcall_vector {
    uint32_t vcpu;
    uint8_t vector;
};
typedef struct xen_hvm_evtchn_upcall_vector xen_hvm_evtchn_upcall_vector_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_evtchn_upcall_vector_t);

#endif /* defined(__i386__) || defined(__x86_64__) */

#define HVMOP_guest_request_vm_event 24

/* HVMOP_altp2m: perform altp2m state operations */
#define HVMOP_altp2m 25

#define HVMOP_ALTP2M_INTERFACE_VERSION 0x00000001

struct xen_hvm_altp2m_domain_state {
    /* IN or OUT variable on/off */
    uint8_t state;
};
typedef struct xen_hvm_altp2m_domain_state xen_hvm_altp2m_domain_state_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_altp2m_domain_state_t);

struct xen_hvm_altp2m_vcpu_enable_notify {
    uint32_t vcpu_id;
    uint32_t pad;
    /* #VE info area gfn */
    uint64_t gfn;
};
typedef struct xen_hvm_altp2m_vcpu_enable_notify xen_hvm_altp2m_vcpu_enable_notify_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_altp2m_vcpu_enable_notify_t);

struct xen_hvm_altp2m_view {
    /* IN/OUT variable */
    uint16_t view;
    /* Create view only: default access type
     * NOTE: currently ignored */
    uint16_t hvmmem_default_access; /* xenmem_access_t */
};
typedef struct xen_hvm_altp2m_view xen_hvm_altp2m_view_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_altp2m_view_t);

struct xen_hvm_altp2m_set_mem_access {
    /* view */
    uint16_t view;
    /* Memory type */
    uint16_t hvmmem_access; /* xenmem_access_t */
    uint32_t pad;
    /* gfn */
    uint64_t gfn;
};
typedef struct xen_hvm_altp2m_set_mem_access xen_hvm_altp2m_set_mem_access_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_altp2m_set_mem_access_t);

struct xen_hvm_altp2m_change_gfn {
    /* view */
    uint16_t view;
    uint16_t pad1;
    uint32_t pad2;
    /* old gfn */
    uint64_t old_gfn;
    /* new gfn, INVALID_GFN (~0UL) means revert */
    uint64_t new_gfn;
};
typedef struct xen_hvm_altp2m_change_gfn xen_hvm_altp2m_change_gfn_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_altp2m_change_gfn_t);

struct xen_hvm_altp2m_op {
    uint32_t version;   /* HVMOP_ALTP2M_INTERFACE_VERSION */
    uint32_t cmd;
/* Get/set the altp2m state for a domain */
#define HVMOP_altp2m_get_domain_state     1
#define HVMOP_altp2m_set_domain_state     2
/* Set the current VCPU to receive altp2m event notifications */
#define HVMOP_altp2m_vcpu_enable_notify   3
/* Create a new view */
#define HVMOP_altp2m_create_p2m           4
/* Destroy a view */
#define HVMOP_altp2m_destroy_p2m          5
/* Switch view for an entire domain */
#define HVMOP_altp2m_switch_p2m           6
/* Notify that a page of memory is to have specific access types */
#define HVMOP_altp2m_set_mem_access       7
/* Change a p2m entry to have a different gfn->mfn mapping */
#define HVMOP_altp2m_change_gfn           8
    domid_t domain;
    uint16_t pad1;
    uint32_t pad2;
    union {
        struct xen_hvm_altp2m_domain_state       domain_state;
        struct xen_hvm_altp2m_vcpu_enable_notify enable_notify;
        struct xen_hvm_altp2m_view               view;
        struct xen_hvm_altp2m_set_mem_access     set_mem_access;
        struct xen_hvm_altp2m_change_gfn         change_gfn;
        uint8_t pad[64];
    } u;
};
typedef struct xen_hvm_altp2m_op xen_hvm_altp2m_op_t;
DEFINE_XEN_GUEST_HANDLE(xen_hvm_altp2m_op_t);

#endif /* __XEN_PUBLIC_HVM_HVM_OP_H__ */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
