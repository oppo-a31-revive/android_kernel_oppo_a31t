/*
 * arch/arm/kernel/return_address.c
 *
 * Copyright (C) 2009 Uwe Kleine-Koenig <u.kleine-koenig@pengutronix.de>
 * for Pengutronix
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */
#include <linux/export.h>
#include <linux/ftrace.h>

#include <linux/sched.h>
#include <asm/stacktrace.h>

struct return_address_data {
    unsigned int level;
    void *addr;
};

#if defined(CONFIG_FRAME_POINTER) && !defined(CONFIG_ARM_UNWIND)
static int save_return_addr(struct stackframe *frame, void *d)
{
    struct return_address_data *data = d;

    if (!data->level) {
        data->addr = (void *)frame->pc;
        return 1;
    } else {
        --data->level;
        return 0;
    }
}
#endif

void *return_address(unsigned int level)
{
#if defined(CONFIG_FRAME_POINTER) && !defined(CONFIG_ARM_UNWIND)
    struct return_address_data data;
    struct stackframe frame;
    register unsigned long current_sp asm ("sp");

    data.level = level + 2;
    data.addr = NULL;

    frame.fp = (unsigned long)__builtin_frame_address(0);
    frame.sp = current_sp;
    frame.lr = (unsigned long)__builtin_return_address(0);
    frame.pc = (unsigned long)return_address;

    walk_stackframe(&frame, save_return_addr, &data);

    if (!data.level)
        return data.addr;
    else
        return NULL;
#else
    struct stackframe frame;
    register unsigned long current_sp asm ("sp");

    frame.fp = (unsigned long)__builtin_frame_address(0);
    frame.sp = current_sp;
    frame.lr = (unsigned long)__builtin_return_address(0);
    frame.pc = (unsigned long)return_address;

    while (level > 0) {
        if (unwind_frame(&frame) < 0)
            return NULL;
        level--;
    }

    if (unwind_frame(&frame) < 0)
        return NULL;

    return (void *)frame.pc;
#endif
}

EXPORT_SYMBOL_GPL(return_address);
