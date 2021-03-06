/*
 * Copyright (C) 2022 CNPEM (cnpem.br)
 * Author: Érico Nogueira <erico.rolim@lnls.br>
 *
 * Released according to the GNU GPL, version 3 or any later version.
 */

#include <stdio.h>

#include "pcie.h"

static void set_wb_pg(const struct pcie_bars *bars, int num)
{
    uint32_t *bar0 = bars->bar0;
    SET_PG(bar0, PCIE_CFG_REG_WB_PG, num);
}

uint32_t bar4_read(const struct pcie_bars *bars, size_t addr)
{
    uint32_t pg_num = PCIE_ADDR_WB_PG (addr);

    /* the offset defined in the SBC is aligned to 8 (3 LSB are 0),
     * but for encoding purposes the value is shifted right,
     * so we have to shift them back left to access the values */
    size_t pg_offs = PCIE_ADDR_WB_PG_OFFS (addr) << 3;

    /* set memory page in bar0, all reads need to do this */
    set_wb_pg(bars, pg_num);

    const uint32_t *barp = (void *)((unsigned char *)bars->bar4 + pg_offs);

    /* this access depends on data having the same endianness as the host */
    return *barp;
}

void bar4_read_u32s(const struct pcie_bars *bars, size_t addr, void *dest, size_t n)
{
    uint32_t *destp = dest;

    /* registers are offset in multiples of 4 */
    for (size_t i = 0; i < n; i++)
        destp[i] = bar4_read(bars, addr + i*4);
}
