# Changelog

All notable changes to Internet Connection Utility are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and the project uses semantic
versioning.

## [1.3.0] — 2026-11-16

### Added
- `VERSIONINFO` resource, so File Explorer's Properties → Details shows **Internet Connection
  Utility** as the product name, with company, description, file version 1.3.0.0 and the copyright
  line instead of a blank description.
- Copyright and license notice in the sidebar footer (`© 2026 Charles McDonald`,
  `Proprietary · all rights reserved`), which doubles as the entry point to a new **About**
  panel listing the product name, version, license terms and privacy note.
- Screenshots of all four tabs (Overview, Wi-Fi, Repairs, Activity) plus the About panel in
  `docs/`, embedded in the README with captions; the About shot sits in the License section.
- Both build paths are documented in the README: the one-step `build.bat` script and an
  out-of-source CMake + Ninja build that keeps `build\` disposable.

### Fixed
- The CMake build did not work at all. Generation rejected the relative `res` include path, and
  the resource step passed that directory to `windres` as `-I`, which word-splits the value and
  fails whenever the checkout lives under a path containing a space. The include directory is now
  C++-only and absolute, so `app.rc` resolves `resource.h` through its own quoted include and the
  CMake build works from any directory.

### Changed
- Settled on the final product name, **Internet Connection Utility**. The window title, sidebar
  brand, dialogs, report header, README, license and every source header use it, and the
  executable is now `InternetConnectionUtility.exe`.
- The `ICU` monogram already reads as the product initials, so the icon, its generator and the
  sidebar brand mark needed no change.
- Internal identifiers were aligned with the name: the window class, the WinHTTP user agent, the
  pending repair queue (`internet-connection-utility-pending.txt`, `%ProgramData%\InternetConnectionUtility`)
  and the hosts backup suffix.

## [1.2.0] — 2026-11-16

### Added
- Repairs requested before a UAC relaunch are written to a pending queue and applied
  automatically by the elevated instance, so elevating no longer discards a single repair or a
  **Fix all** batch.
- Every repair batch now ends with a re-scan and an explicit result: the toast and the activity
  log report what was applied and list any checks that are still failing.

### Fixed
- **TCP receive window auto-tuning** never cleared. The repair ran `netsh`, which changes the
  auto-tuning level, while the check reads `Tcp1323Opts`. The repair now also writes
  `Tcp1323Opts = 3` and removes `DisableTaskOffload`, so the finding actually resolves.
- **Path MTU** repair always forced 1500 bytes, which cannot fix a 1492 or 1400 byte path. It now
  sets the adapter MTU to the measured path MTU, and the check compares the adapter against the
  path instead of treating every reduced path as a fault.
- Checks that offered a repair which could not affect them no longer show an apply button:
  *Default routes & metric*, *Virtual / VPN capture*, *Wi-Fi signal strength* and *Wi-Fi band &
  channel* are manual guidance now.
- **Fix all** reported an elevated repair as applied even when it was skipped for a standard user;
  the batch now separates applied, failed and still-failing results.

## [1.1.0] — 2026-11-16

### Added
- **Relaunch as administrator** button in the action bar, with a lock glyph, shown whenever
  Internet Connection Utility is running as a standard user. It relaunches the app through the UAC prompt and the
  new elevated instance re-scans automatically.
- `ELEVATE` affordance on the sidebar privilege pill so elevation is discoverable from the
  place where privilege status is displayed.
- ELEVATE action on individual admin-only repairs now reports a cancelled elevation instead of
  failing silently.

### Changed
- **Fix all** now inspects which failing repairs need elevation. It offers to relaunch elevated
  and apply everything, or to apply only the repairs available to a standard user, instead of
  running admin repairs and logging a refusal for each.
- Engine gained a `skipAdminFixes` path so the repair queue can be filtered by privilege.
- Licensing moved from MIT to a proprietary license (`LICENSE.md`); every source and header
  file carries the matching copyright and SPDX notice.

### Fixed
- IPv6 resolvers were reported as "no answer" because the DNS probe is IPv4-only. IPv6 servers
  are now validated through the system resolver and no longer lower the score.
- Demand-start services (**Network Connections**) were flagged as failures even though stopped
  is their normal state; only a disabled service is reported now.
- **Network Location Awareness** stopping is treated as a warning rather than a critical fault.
- Wi-Fi signal, channel, security and power checks no longer emit empty "Not associated" rows
  when Ethernet is the active link; they run only when a wireless network is associated.
- Sidebar footer no longer collides with the action bar at smaller window heights.
- Expanded rows with nothing to repair no longer claim to "follow the steps above".
- DNS server list in the throughput probe no longer includes non-IPv4 entries.

## [1.0.0] — 2026-11-16

### Added
- Initial release: native C++17 Win32/GDI+ application with a single self-contained
  `InternetConnectionUtility.exe` and no third-party dependencies.
- Diagnostic engine covering the protocol stack, adapter and link state, addressing, routing,
  interface errors, DNS, internet reachability, TLS egress, captive portals, throughput,
  retransmissions, path MTU, Windows TCP tuning, adapter power management, services, proxies,
  hosts file, drivers, firewall and Wi-Fi.
- 0–100 connection score with severity-ranked findings, plain-English explanations and
  step-by-step remediation guidance.
- Nineteen repairs with `ADMIN` / `RESTART` / `CHANGES` labelling, risk confirmation and
  automatic re-scan after applying.
- Custom dark interface: animated score ring, latency sparkline, metric tiles, expandable
  findings, Wi-Fi analyser with neighbour list and channel-occupancy chart, repair catalogue and
  timestamped activity log.
- Clipboard report export, `F5` rescan, per-monitor DPI awareness and dark title bar.
- Custom application icon - an ICU monogram over connection signal arcs - embedded at 16-256 px
  and used for the window, taskbar, Alt-Tab and Explorer.
