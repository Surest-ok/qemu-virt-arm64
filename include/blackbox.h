/*
 * Blackbox Flight Recorder & Callstack Unwinder
 *
 * Copyright (c) 2026 ForgeStitch
 * SPDX-License-Identifier: MIT
 */

#ifndef _BLACKBOX_H_
#define _BLACKBOX_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BLACKBOX_MAGIC        0x42424F58 /* "BBOX" */
#define BLACKBOX_MAX_RECORDS  16
#define BLACKBOX_MAX_FRAMES   12

struct backtrace_frame {
    uint64_t pc;
    uint64_t fp;
    char sym_name[48];
    uint64_t sym_offset;
};

struct blackbox_record {
    uint32_t magic;
    uint64_t timestamp_ms;
    uint32_t core_id;
    char task_name[32];
    char reason[32];
    uint64_t esr;
    uint64_t elr;
    uint64_t far;
    uint64_t sp;
    uint32_t frame_count;
    struct backtrace_frame frames[BLACKBOX_MAX_FRAMES];
};

void blackbox_init(void);
void blackbox_record_event(const char *reason, uint64_t esr, uint64_t elr, uint64_t far, uint64_t sp, uint64_t fp);
int  blackbox_get_count(void);
const struct blackbox_record *blackbox_get_record(int idx);
void blackbox_dump(void (*print_fn)(const char *fmt, ...));
int  arch_unwind_stack(uint64_t fp, uint64_t lr, struct backtrace_frame *frames, int max_frames);
bool blackbox_run_self_test(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLACKBOX_H_ */
