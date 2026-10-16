/*
 * CherryOS BSP - Board Platform Device Registration
 *
 * Copyright (c) 2026 ForgeStitch
 * SPDX-License-Identifier: MIT
 */

#include "bsp_devices.h"
#include "platform.h"
#include "cherry_vfs.h"
#include "vfs_platform.h"
#include "cherry_printk.h"

/* 1. GICv3 Interrupt Controller Platform Device */
static const struct platform_resource s_gicv3_resources[] = {
    {
        .flags = IORESOURCE_MEM,
        .start = GICD_BASE,
        .end   = GICD_BASE + 0x10000 - 1,
        .name  = "gicd",
    },
    {
        .flags = IORESOURCE_MEM,
        .start = GICR_BASE,
        .end   = GICR_BASE + 0x60000 - 1,
        .name  = "gicr",
    },
};

static struct platform_device s_bsp_gicv3_pdev = {
    .name          = "arm_gicv3",
    .id            = 0,
    .resources     = s_gicv3_resources,
    .num_resources = sizeof(s_gicv3_resources) / sizeof(s_gicv3_resources[0]),
};

/* 2. ARM Generic Architecture Timer Platform Device */
static const struct platform_resource s_timer_resources[] = {
    {
        .flags = IORESOURCE_IRQ,
        .start = 30,
        .end   = 30,
        .name  = "arch_timer_irq",
    },
};

static struct platform_device s_bsp_timer_pdev = {
    .name          = "arm_arch_timer",
    .id            = 0,
    .resources     = s_timer_resources,
    .num_resources = sizeof(s_timer_resources) / sizeof(s_timer_resources[0]),
};

/* 3. PCIe ECAM Host Controller Platform Device */
static const struct platform_resource s_pcie_resources[] = {
    {
        .flags = IORESOURCE_MEM,
        .start = PCIE_ECAM_BASE_LOW,
        .end   = PCIE_ECAM_BASE_LOW + 0x1000000 - 1,
        .name  = "pcie_ecam",
    },
};

static struct platform_device s_bsp_pcie_pdev = {
    .name          = "pcie_ecam",
    .id            = 0,
    .resources     = s_pcie_resources,
    .num_resources = sizeof(s_pcie_resources) / sizeof(s_pcie_resources[0]),
};

int bsp_register_platform_devices(void)
{
    pr_info("bsp: registering board hardware platform devices...\n");

    platform_device_register(&s_bsp_gicv3_pdev);
    platform_device_register(&s_bsp_timer_pdev);
    platform_device_register(&s_bsp_pcie_pdev);

    pr_info("bsp: registered arm_gicv3, arm_arch_timer, pcie_ecam platform devices\n");
    return 0;
}
