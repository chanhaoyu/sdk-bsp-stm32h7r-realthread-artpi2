/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-02-16     howie        first version
 */

#ifndef __I2C_CONFIG_H__
#define __I2C_CONFIG_H__

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BSP_USING_HARD_I2C1)
#ifndef I2C1_BUS_CONFIG
#define I2C1_BUS_CONFIG                                                 \
    {                                                               \
        .name = "i2c1",                                             \
        .Instance = I2C1,                                           \
        .evirq_type = I2C1_EV_IRQn,                                   \
        .erirq_type = I2C1_ER_IRQn,                                   \
    }
#endif /* I2C1_BUS_CONFIG */
#endif

#if defined(BSP_USING_HARD_I2C2)
#ifndef I2C2_BUS_CONFIG
#define I2C2_BUS_CONFIG                                                 \
    {                                                               \
        .name = "i2c2",                                             \
        .Instance = I2C2,                                           \
        .evirq_type = I2C2_EV_IRQn,                                   \
        .erirq_type = I2C2_ER_IRQn,                                   \
    }
#endif /* I2C2_BUS_CONFIG */
#endif

#if defined(BSP_USING_HARD_I2C3)
#ifndef I2C3_BUS_CONFIG
#define I2C3_BUS_CONFIG                                                 \
    {                                                               \
        .name = "i2c3",                                             \
        .Instance = I2C3,                                           \
        .evirq_type = I2C3_EV_IRQn,                                   \
        .erirq_type = I2C3_ER_IRQn,                                   \
    }
#endif /* I2C3_BUS_CONFIG */
#endif

#ifdef __cplusplus
}
#endif

#endif