# Bench Log

This file records measurements and experiments made on the real hardware. It is intentionally separate from design documents so measured results are not confused with planned values.

## ATmega328P logic-probe target

Target: ATmega328P-AU, TQFP-32, installed on the hand-built logic-probe/test instrument.

### Power measurements

| Measurement | Observed value | Status |
|---|---:|---|
| Target VCC | ~3.2 V | Measured |
| ATmega328P AVCC, pin 18 | 3.04 V | Measured |
| RESET, normal state | ~2.8 V | Measured |

These values were measured on the physical target before ISP connection. Repeat measurements should be appended with date/time and test conditions rather than overwriting previous results.

## First ISP test

Status: **Pending physical connection and firmware test**.

The first firmware operation is signature identification only. No erase, Flash write, fuse write, or lock-bit modification is permitted during this milestone.

Expected signature:

`1E 95 0F`

## How to append a test

Use this format:

```text
Date/time:
Target:
Firmware revision/commit:
Supply voltage:
Clock condition:
Wiring:
Operation:
Result:
Observed output:
Notes:
```

Do not delete failed tests. Failed tests are part of the project's engineering history.
