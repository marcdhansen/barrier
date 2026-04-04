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

#pragma once

#include <cstdint>
#include <string>

namespace barrier {

// Returns the first IPv4 address in the Tailscale CGNAT range
// (100.64.0.0/10, i.e. 100.64.x.x – 100.127.x.x) found on a local
// network interface.  Returns an empty string if Tailscale is not
// running or no such address is present.
std::string get_tailscale_address();

namespace detail {
// Returns true if addr (in host byte order) falls within the Tailscale
// CGNAT range 100.64.0.0/10 (0x64400000–0x647FFFFF).
bool is_tailscale_addr(uint32_t addr_host_order);
} // namespace detail

} // namespace barrier
