#include "blackbox.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "arch_api/arch.h"
#include "arch_api/exception.h"
#include "uart.h"

struct syscall_frame {
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;
    uint64_t x30;
    uint64_t sp;
    uint64_t pc;
    uint64_t pstate;
};

static inline void get_simd_reg(int reg, uint8_t *buf)
{
    uint64_t low = 0, high = 0;
    switch (reg) {
    case 0:  __asm__ volatile("fmov %0, d0\n\tmov %1, v0.d[1]"  : "=r"(low), "=r"(high)); break;
    case 1:  __asm__ volatile("fmov %0, d1\n\tmov %1, v1.d[1]"  : "=r"(low), "=r"(high)); break;
    case 2:  __asm__ volatile("fmov %0, d2\n\tmov %1, v2.d[1]"  : "=r"(low), "=r"(high)); break;
    case 3:  __asm__ volatile("fmov %0, d3\n\tmov %1, v3.d[1]"  : "=r"(low), "=r"(high)); break;
    case 4:  __asm__ volatile("fmov %0, d4\n\tmov %1, v4.d[1]"  : "=r"(low), "=r"(high)); break;
    case 5:  __asm__ volatile("fmov %0, d5\n\tmov %1, v5.d[1]"  : "=r"(low), "=r"(high)); break;
    case 6:  __asm__ volatile("fmov %0, d6\n\tmov %1, v6.d[1]"  : "=r"(low), "=r"(high)); break;
    case 7:  __asm__ volatile("fmov %0, d7\n\tmov %1, v7.d[1]"  : "=r"(low), "=r"(high)); break;
    case 8:  __asm__ volatile("fmov %0, d8\n\tmov %1, v8.d[1]"  : "=r"(low), "=r"(high)); break;
    case 9:  __asm__ volatile("fmov %0, d9\n\tmov %1, v9.d[1]"  : "=r"(low), "=r"(high)); break;
    case 10: __asm__ volatile("fmov %0, d10\n\tmov %1, v10.d[1]": "=r"(low), "=r"(high)); break;
    case 11: __asm__ volatile("fmov %0, d11\n\tmov %1, v11.d[1]": "=r"(low), "=r"(high)); break;
    case 12: __asm__ volatile("fmov %0, d12\n\tmov %1, v12.d[1]": "=r"(low), "=r"(high)); break;
    case 13: __asm__ volatile("fmov %0, d13\n\tmov %1, v13.d[1]": "=r"(low), "=r"(high)); break;
    case 14: __asm__ volatile("fmov %0, d14\n\tmov %1, v14.d[1]": "=r"(low), "=r"(high)); break;
    case 15: __asm__ volatile("fmov %0, d15\n\tmov %1, v15.d[1]": "=r"(low), "=r"(high)); break;
    case 16: __asm__ volatile("fmov %0, d16\n\tmov %1, v16.d[1]": "=r"(low), "=r"(high)); break;
    case 17: __asm__ volatile("fmov %0, d17\n\tmov %1, v17.d[1]": "=r"(low), "=r"(high)); break;
    case 18: __asm__ volatile("fmov %0, d18\n\tmov %1, v18.d[1]": "=r"(low), "=r"(high)); break;
    case 19: __asm__ volatile("fmov %0, d19\n\tmov %1, v19.d[1]": "=r"(low), "=r"(high)); break;
    case 20: __asm__ volatile("fmov %0, d20\n\tmov %1, v20.d[1]": "=r"(low), "=r"(high)); break;
    case 21: __asm__ volatile("fmov %0, d21\n\tmov %1, v21.d[1]": "=r"(low), "=r"(high)); break;
    case 22: __asm__ volatile("fmov %0, d22\n\tmov %1, v22.d[1]": "=r"(low), "=r"(high)); break;
    case 23: __asm__ volatile("fmov %0, d23\n\tmov %1, v23.d[1]": "=r"(low), "=r"(high)); break;
    case 24: __asm__ volatile("fmov %0, d24\n\tmov %1, v24.d[1]": "=r"(low), "=r"(high)); break;
    case 25: __asm__ volatile("fmov %0, d25\n\tmov %1, v25.d[1]": "=r"(low), "=r"(high)); break;
    case 26: __asm__ volatile("fmov %0, d26\n\tmov %1, v26.d[1]": "=r"(low), "=r"(high)); break;
    case 27: __asm__ volatile("fmov %0, d27\n\tmov %1, v27.d[1]": "=r"(low), "=r"(high)); break;
    case 28: __asm__ volatile("fmov %0, d28\n\tmov %1, v28.d[1]": "=r"(low), "=r"(high)); break;
    case 29: __asm__ volatile("fmov %0, d29\n\tmov %1, v29.d[1]": "=r"(low), "=r"(high)); break;
    case 30: __asm__ volatile("fmov %0, d30\n\tmov %1, v30.d[1]": "=r"(low), "=r"(high)); break;
    case 31: __asm__ volatile("fmov %0, d31\n\tmov %1, v31.d[1]": "=r"(low), "=r"(high)); break;
    }
    for (int i = 0; i < 8; i++) buf[i] = ((uint8_t *)&low)[i];
    for (int i = 0; i < 8; i++) buf[8 + i] = ((uint8_t *)&high)[i];
}

static inline void set_simd_reg(int reg, const uint8_t *buf)
{
    uint64_t low = 0, high = 0;
    for (int i = 0; i < 8; i++) ((uint8_t *)&low)[i] = buf[i];
    for (int i = 0; i < 8; i++) ((uint8_t *)&high)[i] = buf[8 + i];
    switch (reg) {
    case 0:  __asm__ volatile("fmov d0, %0\n\tmov v0.d[1], %1"  :: "r"(low), "r"(high)); break;
    case 1:  __asm__ volatile("fmov d1, %0\n\tmov v1.d[1], %1"  :: "r"(low), "r"(high)); break;
    case 2:  __asm__ volatile("fmov d2, %0\n\tmov v2.d[1], %1"  :: "r"(low), "r"(high)); break;
    case 3:  __asm__ volatile("fmov d3, %0\n\tmov v3.d[1], %1"  :: "r"(low), "r"(high)); break;
    case 4:  __asm__ volatile("fmov d4, %0\n\tmov v4.d[1], %1"  :: "r"(low), "r"(high)); break;
    case 5:  __asm__ volatile("fmov d5, %0\n\tmov v5.d[1], %1"  :: "r"(low), "r"(high)); break;
    case 6:  __asm__ volatile("fmov d6, %0\n\tmov v6.d[1], %1"  :: "r"(low), "r"(high)); break;
    case 7:  __asm__ volatile("fmov d7, %0\n\tmov v7.d[1], %1"  :: "r"(low), "r"(high)); break;
    case 8:  __asm__ volatile("fmov d8, %0\n\tmov v8.d[1], %1"  :: "r"(low), "r"(high)); break;
    case 9:  __asm__ volatile("fmov d9, %0\n\tmov v9.d[1], %1"  :: "r"(low), "r"(high)); break;
    case 10: __asm__ volatile("fmov d10, %0\n\tmov v10.d[1], %1":: "r"(low), "r"(high)); break;
    case 11: __asm__ volatile("fmov d11, %0\n\tmov v11.d[1], %1":: "r"(low), "r"(high)); break;
    case 12: __asm__ volatile("fmov d12, %0\n\tmov v12.d[1], %1":: "r"(low), "r"(high)); break;
    case 13: __asm__ volatile("fmov d13, %0\n\tmov v13.d[1], %1":: "r"(low), "r"(high)); break;
    case 14: __asm__ volatile("fmov d14, %0\n\tmov v14.d[1], %1":: "r"(low), "r"(high)); break;
    case 15: __asm__ volatile("fmov d15, %0\n\tmov v15.d[1], %1":: "r"(low), "r"(high)); break;
    case 16: __asm__ volatile("fmov d16, %0\n\tmov v16.d[1], %1":: "r"(low), "r"(high)); break;
    case 17: __asm__ volatile("fmov d17, %0\n\tmov v17.d[1], %1":: "r"(low), "r"(high)); break;
    case 18: __asm__ volatile("fmov d18, %0\n\tmov v18.d[1], %1":: "r"(low), "r"(high)); break;
    case 19: __asm__ volatile("fmov d19, %0\n\tmov v19.d[1], %1":: "r"(low), "r"(high)); break;
    case 20: __asm__ volatile("fmov d20, %0\n\tmov v20.d[1], %1":: "r"(low), "r"(high)); break;
    case 21: __asm__ volatile("fmov d21, %0\n\tmov v21.d[1], %1":: "r"(low), "r"(high)); break;
    case 22: __asm__ volatile("fmov d22, %0\n\tmov v22.d[1], %1":: "r"(low), "r"(high)); break;
    case 23: __asm__ volatile("fmov d23, %0\n\tmov v23.d[1], %1":: "r"(low), "r"(high)); break;
    case 24: __asm__ volatile("fmov d24, %0\n\tmov v24.d[1], %1":: "r"(low), "r"(high)); break;
    case 25: __asm__ volatile("fmov d25, %0\n\tmov v25.d[1], %1":: "r"(low), "r"(high)); break;
    case 26: __asm__ volatile("fmov d26, %0\n\tmov v26.d[1], %1":: "r"(low), "r"(high)); break;
    case 27: __asm__ volatile("fmov d27, %0\n\tmov v27.d[1], %1":: "r"(low), "r"(high)); break;
    case 28: __asm__ volatile("fmov d28, %0\n\tmov v28.d[1], %1":: "r"(low), "r"(high)); break;
    case 29: __asm__ volatile("fmov d29, %0\n\tmov v29.d[1], %1":: "r"(low), "r"(high)); break;
    case 30: __asm__ volatile("fmov d30, %0\n\tmov v30.d[1], %1":: "r"(low), "r"(high)); break;
    case 31: __asm__ volatile("fmov d31, %0\n\tmov v31.d[1], %1":: "r"(low), "r"(high)); break;
    }
}

static inline uint64_t get_gp_reg(struct syscall_frame *frame, int reg)
{
    if (reg >= 0 && reg < 31) return (&frame->x0)[reg];
    if (reg == 31) return frame->sp;
    return 0;
}

static inline void set_gp_reg(struct syscall_frame *frame, int reg, uint64_t val)
{
    if (reg >= 0 && reg < 31) (&frame->x0)[reg] = val;
    else if (reg == 31) frame->sp = val;
}

int arch_fixup_alignment_fault(struct syscall_frame *frame, uint64_t esr, uint64_t elr, uint64_t far)
{
    uint32_t ec = (uint32_t)((esr >> 26) & 0x3f);
    uint32_t dfsc = (uint32_t)(esr & 0x3f);
    if (ec != 0x25 && ec != 0x24) return 0; /* Data Abort */
    if (dfsc != 0x21) return 0;             /* Alignment fault */

    uint32_t insn = *(volatile uint32_t *)elr;
    volatile uint8_t *addr = (volatile uint8_t *)far;

    /* 1. Load/Store Pair: ((insn >> 27) & 0x7) == 0b101 && ((insn >> 25) & 1) == 0 */
    if ((insn & 0x3a000000) == 0x28000000) {
        bool is_load = (insn & (1 << 22)) != 0;
        bool is_simd = (insn & (1 << 26)) != 0;
        uint32_t opc = (insn >> 30) & 3;
        int rt  = insn & 0x1f;
        int rt2 = (insn >> 10) & 0x1f;
        int rn  = (insn >> 5) & 0x1f;

        size_t sz = 0;
        if (is_simd) {
            if (opc == 0) sz = 4;
            else if (opc == 1) sz = 8;
            else if (opc == 2) sz = 16;
        } else {
            sz = (opc & 2) ? 8 : 4;
        }
        if (!sz) return 0;

        if (is_load) {
            if (is_simd && sz == 16) {
                uint8_t b1[16], b2[16];
                for (int i = 0; i < 16; i++) b1[i] = addr[i];
                for (int i = 0; i < 16; i++) b2[i] = addr[16 + i];
                set_simd_reg(rt, b1);
                set_simd_reg(rt2, b2);
            } else if (is_simd && sz == 8) {
                uint8_t b1[16] = {0}, b2[16] = {0};
                for (int i = 0; i < 8; i++) b1[i] = addr[i];
                for (int i = 0; i < 8; i++) b2[i] = addr[8 + i];
                set_simd_reg(rt, b1);
                set_simd_reg(rt2, b2);
            } else if (is_simd && sz == 4) {
                uint8_t b1[16] = {0}, b2[16] = {0};
                for (int i = 0; i < 4; i++) b1[i] = addr[i];
                for (int i = 0; i < 4; i++) b2[i] = addr[4 + i];
                set_simd_reg(rt, b1);
                set_simd_reg(rt2, b2);
            } else if (!is_simd && sz == 8) {
                uint64_t v1 = 0, v2 = 0;
                for (int i = 0; i < 8; i++) ((uint8_t *)&v1)[i] = addr[i];
                for (int i = 0; i < 8; i++) ((uint8_t *)&v2)[i] = addr[8 + i];
                if (rt < 31) set_gp_reg(frame, rt, v1);
                if (rt2 < 31) set_gp_reg(frame, rt2, v2);
            } else if (!is_simd && sz == 4) {
                uint32_t v1 = 0, v2 = 0;
                for (int i = 0; i < 4; i++) ((uint8_t *)&v1)[i] = addr[i];
                for (int i = 0; i < 4; i++) ((uint8_t *)&v2)[i] = addr[4 + i];
                if (rt < 31) set_gp_reg(frame, rt, (uint64_t)v1);
                if (rt2 < 31) set_gp_reg(frame, rt2, (uint64_t)v2);
            } else {
                return 0;
            }
        } else {
            /* Store Pair */
            if (is_simd && sz == 16) {
                uint8_t b1[16], b2[16];
                get_simd_reg(rt, b1);
                get_simd_reg(rt2, b2);
                for (int i = 0; i < 16; i++) addr[i] = b1[i];
                for (int i = 0; i < 16; i++) addr[16 + i] = b2[i];
            } else if (is_simd && sz == 8) {
                uint8_t b1[16], b2[16];
                get_simd_reg(rt, b1);
                get_simd_reg(rt2, b2);
                for (int i = 0; i < 8; i++) addr[i] = b1[i];
                for (int i = 0; i < 8; i++) addr[8 + i] = b2[i];
            } else if (is_simd && sz == 4) {
                uint8_t b1[16], b2[16];
                get_simd_reg(rt, b1);
                get_simd_reg(rt2, b2);
                for (int i = 0; i < 4; i++) addr[i] = b1[i];
                for (int i = 0; i < 4; i++) addr[4 + i] = b2[i];
            } else if (!is_simd && sz == 8) {
                uint64_t v1 = (rt < 31) ? get_gp_reg(frame, rt) : 0;
                uint64_t v2 = (rt2 < 31) ? get_gp_reg(frame, rt2) : 0;
                for (int i = 0; i < 8; i++) addr[i] = ((uint8_t *)&v1)[i];
                for (int i = 0; i < 8; i++) addr[8 + i] = ((uint8_t *)&v2)[i];
            } else if (!is_simd && sz == 4) {
                uint32_t v1 = (rt < 31) ? (uint32_t)get_gp_reg(frame, rt) : 0;
                uint32_t v2 = (rt2 < 31) ? (uint32_t)get_gp_reg(frame, rt2) : 0;
                for (int i = 0; i < 4; i++) addr[i] = ((uint8_t *)&v1)[i];
                for (int i = 0; i < 4; i++) addr[4 + i] = ((uint8_t *)&v2)[i];
            } else {
                return 0;
            }
        }

        /* Writeback: post-index (bits 24:23 == 01) or pre-index (bits 24:23 == 11) */
        uint32_t wback_mode = (insn >> 23) & 3;
        if (wback_mode == 1 || wback_mode == 3) {
            int32_t imm7 = (int32_t)((insn >> 15) & 0x7f);
            if (imm7 & 0x40) imm7 -= 0x80;
            int64_t wb_offset = (int64_t)imm7 * (int64_t)sz;
            set_gp_reg(frame, rn, get_gp_reg(frame, rn) + wb_offset);
        }

        frame->pc = elr + 4;
        return 1;
    }

    /* 2. Load/Store Single Register: ((insn >> 27) & 0x7) == 0b111 && ((insn >> 25) & 1) == 0 */
    if ((insn & 0x3a000000) == 0x38000000) {
        bool is_load = (insn & (1 << 22)) != 0;
        bool is_simd = (insn & (1 << 26)) != 0;
        uint32_t size = (insn >> 30) & 3;
        int rt = insn & 0x1f;
        int rn = (insn >> 5) & 0x1f;

        size_t sz = 1 << size;
        if (is_simd && (insn & (1 << 23))) sz = 16;

        if (is_load) {
            if (is_simd && sz == 16) {
                uint8_t b[16];
                for (size_t i = 0; i < 16; i++) b[i] = addr[i];
                set_simd_reg(rt, b);
            } else {
                uint64_t val = 0;
                for (size_t i = 0; i < sz; i++) ((uint8_t *)&val)[i] = addr[i];
                if (rt < 31) set_gp_reg(frame, rt, val);
            }
        } else {
            if (is_simd && sz == 16) {
                uint8_t b[16];
                get_simd_reg(rt, b);
                for (size_t i = 0; i < 16; i++) addr[i] = b[i];
            } else {
                uint64_t val = (rt < 31) ? get_gp_reg(frame, rt) : 0;
                for (size_t i = 0; i < sz; i++) addr[i] = ((uint8_t *)&val)[i];
            }
        }

        /* Writeback: post-indexed (bit 24 == 0, bit 21 == 0, bit 10 == 1) or pre-indexed (bit 11 == 1) */
        if (!(insn & (1 << 24)) && !(insn & (1 << 21))) {
            if ((insn & (1 << 11)) || (insn & (1 << 10))) {
                int32_t imm9 = (int32_t)((insn >> 12) & 0x1ff);
                if (imm9 & 0x100) imm9 -= 0x200;
                set_gp_reg(frame, rn, get_gp_reg(frame, rn) + imm9);
            }
        }

        frame->pc = elr + 4;
        return 1;
    }

    return 0;
}

extern void *pxCurrentTCBs[];

void vPortFatalException(uint64_t esr, uint64_t elr, uint64_t far, uint64_t spsr, uint64_t lr, uint64_t sp)
{
    arch_exception_frame_t frame = {
        .elr = elr,
        .spsr = spsr,
        .esr = esr,
        .far = far,
    };

    arch_exception_record(&frame);
    blackbox_record_event("exception", esr, elr, far, sp, lr);
    blackbox_dump(NULL);
    uart_log(0, "freertos", 
             "[panic] core=%d reason=exception esr=0x%lx elr=0x%lx far=0x%lx spsr=0x%lx lr=0x%lx sp=0x%lx\r\n",
             (int)arch_curr_cpu_id(),
             (unsigned long)esr,
             (unsigned long)elr,
             (unsigned long)far,
             (unsigned long)spsr,
             (unsigned long)lr,
             (unsigned long)sp);

    if (sp >= 0x40000000 && sp < 0x45000000) {
        uint64_t *s = (uint64_t *)sp;
        for (int i = 0; i < 8; i++) {
            uart_log(0, "freertos", "  sp[%d] = 0x%016lx\r\n", i, (unsigned long)s[i]);
        }
    }


    for (;;) {
        __asm__ volatile("wfi");
    }
}
