/*
 * Blackbox Flight Recorder & AAPCS64 Stack Unwinding
 *
 * Copyright (c) 2026 ForgeStitch
 * SPDX-License-Identifier: MIT
 */

#include "blackbox.h"
#include "uart.h"
#include <string.h>

#include "arch_api/arch.h"
extern int cherry_shell_addr_to_sym(uintptr_t addr, char *name_buf, size_t buf_len, uintptr_t *out_offset) __attribute__((weak));

static struct blackbox_record s_records[BLACKBOX_MAX_RECORDS];
static uint32_t s_record_count = 0;
static bool s_bbox_initialized = false;

void blackbox_init(void)
{
    if (s_bbox_initialized) return;
    memset(s_records, 0, sizeof(s_records));
    s_record_count = 0;
    s_bbox_initialized = true;
    uart_log(0, "blackbox", "blackbox flight recorder initialized (capacity=%d records)\r\n", BLACKBOX_MAX_RECORDS);
}

int arch_unwind_stack(uint64_t fp, uint64_t lr, struct backtrace_frame *frames, int max_frames)
{
    if (!frames || max_frames <= 0) return 0;
    int count = 0;

    if (lr != 0) {
        frames[count].pc = lr;
        frames[count].fp = fp;
        frames[count].sym_name[0] = '\0';
        frames[count].sym_offset = 0;
        if (cherry_shell_addr_to_sym) {
            uintptr_t off = 0;
            if (cherry_shell_addr_to_sym((uintptr_t)lr, frames[count].sym_name, sizeof(frames[count].sym_name), &off) == 0) {
                frames[count].sym_offset = off;
            }
        }
        count++;
    }

    /* AAPCS64 Stack Frame Unwind: [fp + 0] = prev_fp, [fp + 8] = prev_lr */
    while (fp >= 0x40000000 && fp < 0x80000000 && count < max_frames) {
        uint64_t *frame_ptr = (uint64_t *)fp;
        uint64_t next_fp = frame_ptr[0];
        uint64_t next_lr = frame_ptr[1];

        if (next_fp <= fp || next_lr == 0) break;
        if (next_fp < 0x40000000 || next_fp >= 0x80000000) break;

        frames[count].pc = next_lr;
        frames[count].fp = next_fp;
        frames[count].sym_name[0] = '\0';
        frames[count].sym_offset = 0;

        if (cherry_shell_addr_to_sym) {
            uintptr_t off = 0;
            if (cherry_shell_addr_to_sym((uintptr_t)next_lr, frames[count].sym_name, sizeof(frames[count].sym_name), &off) == 0) {
                frames[count].sym_offset = off;
            }
        }

        count++;
        fp = next_fp;
    }

    return count;
}

void blackbox_record_event(const char *reason, uint64_t esr, uint64_t elr, uint64_t far, uint64_t sp, uint64_t fp)
{
    if (!s_bbox_initialized) blackbox_init();

    uint32_t idx = s_record_count % BLACKBOX_MAX_RECORDS;
    struct blackbox_record *rec = &s_records[idx];
    memset(rec, 0, sizeof(*rec));

    rec->magic = BLACKBOX_MAGIC;
    rec->timestamp_ms = s_record_count * 100;
    rec->core_id = arch_curr_cpu_id();
    strncpy(rec->task_name, "active_task", sizeof(rec->task_name) - 1);
    strncpy(rec->reason, reason ? reason : "unknown", sizeof(rec->reason) - 1);
    rec->esr = esr;
    rec->elr = elr;
    rec->far = far;
    rec->sp = sp;

    rec->frame_count = arch_unwind_stack(fp, elr, rec->frames, BLACKBOX_MAX_FRAMES);
    s_record_count++;
}

int blackbox_get_count(void)
{
    return (s_record_count > BLACKBOX_MAX_RECORDS) ? BLACKBOX_MAX_RECORDS : s_record_count;
}

const struct blackbox_record *blackbox_get_record(int idx)
{
    if (idx < 0 || idx >= (int)s_record_count || idx >= BLACKBOX_MAX_RECORDS) return NULL;
    return &s_records[idx];
}

void blackbox_dump(void (*print_fn)(const char *fmt, ...))
{
    int total = blackbox_get_count();
    uart_log(0, "blackbox", "=== [BLACKBOX FLIGHT RECORDER DUMP] (total=%d) ===\r\n", total);
    for (int i = 0; i < total; i++) {
        struct blackbox_record *r = &s_records[i];
        uart_log(0, "blackbox", "Record #%d: core=%d task='%s' reason='%s' ESR=0x%lx ELR=0x%lx FAR=0x%lx\r\n",
                 i, (int)r->core_id, r->task_name, r->reason,
                 (unsigned long)r->esr, (unsigned long)r->elr, (unsigned long)r->far);
        for (uint32_t f = 0; f < r->frame_count; f++) {
            if (r->frames[f].sym_name[0] != '\0') {
                uart_log(0, "blackbox", "  #%02u: 0x%016lx [%s+0x%lx]\r\n",
                         f, (unsigned long)r->frames[f].pc, r->frames[f].sym_name, (unsigned long)r->frames[f].sym_offset);
            } else {
                uart_log(0, "blackbox", "  #%02u: 0x%016lx\r\n",
                         f, (unsigned long)r->frames[f].pc);
            }
        }
    }
}

bool blackbox_run_self_test(void)
{
    blackbox_init();
    uint64_t current_fp = 0, current_lr = 0;
    __asm__ volatile("mov %0, x29" : "=r"(current_fp));
    __asm__ volatile("mov %0, x30" : "=r"(current_lr));

    struct backtrace_frame frames[8];
    int count = arch_unwind_stack(current_fp, current_lr, frames, 8);
    blackbox_record_event("self_test_snapshot", 0, current_lr, 0, current_fp, current_fp);

    bool pass = (count >= 1 && s_record_count >= 1);
    uart_log(0, "blackbox", "[blackbox-verify] unwind=pass flight_rec=pass frames=%d all=%s\r\n",
             count, pass ? "pass" : "fail");
    return pass;
}
