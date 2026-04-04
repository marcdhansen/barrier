/*
 * barrier -- mouse and keyboard sharing utility
 * Copyright (C) 2024 Barrier Contributors
 *
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "net/TailscaleUtil.h"

#include "test/global/gtest.h"

namespace barrier {
namespace detail {

// Tailscale CGNAT range: 100.64.0.0/10
//   first: 100.64.0.0   = 0x64400000
//   last:  100.127.255.255 = 0x647FFFFF

TEST(TailscaleUtilTest, FirstAddressInRange)
{
    EXPECT_TRUE(is_tailscale_addr(0x64400000u)); // 100.64.0.0
}

TEST(TailscaleUtilTest, LastAddressInRange)
{
    EXPECT_TRUE(is_tailscale_addr(0x647FFFFFu)); // 100.127.255.255
}

TEST(TailscaleUtilTest, TypicalTailscaleAddress)
{
    // 100.100.100.100 = 0x64646464
    EXPECT_TRUE(is_tailscale_addr(0x64646464u));
}

TEST(TailscaleUtilTest, AddressJustBelowRange)
{
    EXPECT_FALSE(is_tailscale_addr(0x643FFFFFu)); // 100.63.255.255
}

TEST(TailscaleUtilTest, AddressJustAboveRange)
{
    EXPECT_FALSE(is_tailscale_addr(0x64800000u)); // 100.128.0.0
}

TEST(TailscaleUtilTest, LoopbackNotInRange)
{
    EXPECT_FALSE(is_tailscale_addr(0x7F000001u)); // 127.0.0.1
}

TEST(TailscaleUtilTest, PrivateRfc1918NotInRange)
{
    EXPECT_FALSE(is_tailscale_addr(0xC0A80001u)); // 192.168.0.1
    EXPECT_FALSE(is_tailscale_addr(0x0A000001u)); // 10.0.0.1
    EXPECT_FALSE(is_tailscale_addr(0xAC100001u)); // 172.16.0.1
}

TEST(TailscaleUtilTest, ZeroAddressNotInRange)
{
    EXPECT_FALSE(is_tailscale_addr(0x00000000u)); // 0.0.0.0
}

} // namespace detail
} // namespace barrier
