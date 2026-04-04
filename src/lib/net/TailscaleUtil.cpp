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

#if SYSAPI_WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <iphlpapi.h>
#  pragma comment(lib, "iphlpapi.lib")
#else
#  include <ifaddrs.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#endif

#include <cstdint>

namespace barrier {

// Tailscale uses the 100.64.0.0/10 CGNAT block:
//   first address: 100.64.0.0  → 0x64400000
//   last  address: 100.127.255.255 → 0x647FFFFF
namespace detail {
bool is_tailscale_addr(uint32_t addr_host_order)
{
    return (addr_host_order >= 0x64400000u) && (addr_host_order <= 0x647FFFFFu);
}
} // namespace detail

std::string get_tailscale_address()
{
#if SYSAPI_WIN32
    // Use GetAdaptersAddresses to enumerate IPv4 addresses on Windows.
    ULONG bufLen = 15000;
    auto buf = std::make_unique<char[]>(bufLen);
    IP_ADAPTER_ADDRESSES* adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buf.get());

    ULONG ret = GetAdaptersAddresses(AF_INET,
        GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
        nullptr, adapters, &bufLen);

    if (ret == ERROR_BUFFER_OVERFLOW) {
        buf = std::make_unique<char[]>(bufLen);
        adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buf.get());
        ret = GetAdaptersAddresses(AF_INET,
            GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
            nullptr, adapters, &bufLen);
    }

    if (ret != NO_ERROR) {
        return {};
    }

    for (auto* a = adapters; a != nullptr; a = a->Next) {
        for (auto* ua = a->FirstUnicastAddress; ua != nullptr; ua = ua->Next) {
            auto* sin = reinterpret_cast<sockaddr_in*>(ua->Address.lpSockaddr);
            uint32_t addr = ntohl(sin->sin_addr.s_addr);
            if (detail::is_tailscale_addr(addr)) {
                char buf[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf));
                return buf;
            }
        }
    }
    return {};

#else
    // Use getifaddrs on Unix/macOS.
    struct ifaddrs* ifap = nullptr;
    if (getifaddrs(&ifap) != 0) {
        return {};
    }

    std::string result;
    for (auto* ifa = ifap; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET) {
            continue;
        }
        auto* sin = reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr);
        uint32_t addr = ntohl(sin->sin_addr.s_addr);
        if (detail::is_tailscale_addr(addr)) {
            char buf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf));
            result = buf;
            break;
        }
    }
    freeifaddrs(ifap);
    return result;
#endif
}

} // namespace barrier
