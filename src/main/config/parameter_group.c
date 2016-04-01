/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stddef.h>
#include <string.h>
#include <stdint.h>

#include "parameter_group.h"
#include "common/maths.h"

const pgRegistry_t* pgFind(pgn_t pgn)
{
    PG_FOREACH(reg) {
        if (pgN(reg) == pgn) {
            return reg;
        }
    }
    return NULL;
}

const pgRegistry_t* pgMatcher(pgMatcherFuncPtr matcher, const void *criteria)
{
    PG_FOREACH(candidate) {
        if (matcher(candidate, criteria)) {
            return candidate;
        }
    }
    return NULL;
}

void pgLoad(const pgRegistry_t* reg, const void *from, int size)
{
    memset(reg->address, 0, pgSize(reg));
    const int take = MIN(size, pgSize(reg));
    memcpy(reg->address, from, take);
}

void pgResetAll(uint8_t profileCount)
{
    // Clear all configuration
    PG_FOREACH(reg) {
        // FIXME this assumes that all defaults should be 0, but this is not the case.
        const uint16_t regSize = pgSize(reg);
        if (pgIsSystem(reg)) {
            memset(reg->address, 0, regSize);
        } else {
            // reset one instance for each profile
            for (uint8_t profileIndex = 0; profileIndex < profileCount; profileIndex++) {
                uint8_t *base = reg->address + (regSize * profileIndex);
                memset(base, 0, regSize);
            }
        }
    }
}

void pgActivateProfile(uint8_t profileIndexToActivate)
{
    PG_FOREACH(reg) {
        if (!pgIsSystem(reg)) {
            uint8_t *ptr = reg->address + (profileIndexToActivate * pgSize(reg));
            *(reg->ptr) = ptr;
        }
    }
}

