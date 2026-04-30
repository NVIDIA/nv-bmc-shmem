# Branch Coverage Notes

Overall branch coverage after this MR: **35.3%** (3085 / 8733 arms).

Branch coverage is structurally harder to raise than line coverage
because gcov counts every `if`/`else` arm, every ternary, every
`&&`/`||` short-circuit, and every template instantiation independently.

## Permanently uncoverable branches

| Location | Reason |
|----------|--------|
| `time_utils.hpp` — `year < 1970` | Impossible for `uint64_t` inputs; the true-arm is dead in all four template instantiations (`uint64_t/ratio<1>`, `uint64_t/milli`, `uint64_t/micro`, `time_t/ratio<1>`) |
| `metric_report_utils.hpp` lines 898, 911 | `PLATFORMDEVICEPREFIX=""` makes `deviceName.find("") != 0` always false; true-arm is dead at this build configuration |
| `metric_report_utils.hpp` line 1273 | Always-false logical condition; true-arm is dead code |
| `shmem_map` erase result check | `erase()` always returns true when the namespace exists; false-arm is unreachable dead code |

## Exception / OS-failure branches

`managed_shmem.cpp` and `shmem_map.cpp` have false-arms guarding
`mmap`, `shm_open`, and `ftruncate` failures. Triggering these
requires the OS to return errors under normal test conditions, which
is not feasible without kernel-level fault injection. These contribute
roughly 30 uncovered branch arms.

## Template instantiation inflation

A single dead or hard-to-reach branch inside a function template body
appears **once per instantiation** in the gcov denominator. For
example, the `year < 1970` guard in `toISO8061ExtendedStr` is counted
four times (one per instantiation), multiplying the apparent gap
without any corresponding coverable code path.

## Config-dependent dead branches

`config_json_reader.cpp` contains JSON parsing branches that activate
only for specific malformed or edge-case input structures not present
in the test configuration files. These add approximately 50 uncovered
branch arms that would require purpose-built invalid JSON fixtures to
exercise.

## Summary

| Category | Approx. uncovered arms |
|----------|------------------------|
| Permanently dead (build config) | ~15 |
| Template instantiation duplicates | ~40 |
| OS-failure / exception paths | ~30 |
| Config-dependent JSON branches | ~50 |
| Remaining reachable but untested | ~5513 |
