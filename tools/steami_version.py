#!/usr/bin/env python3
"""Ask a STeaMi DAPLink for its release with the vendor command 0x9F.

The reply is 0x9F, the mark "STM1", the release number and the release date
(YYYYMMDD), both uint32 little-endian. A DAPLink without the command answers
0x9F only; the rest of the 64-byte report is whatever was in the buffer, so
the mark is what tells a real reply.

Linux only: talks to the /dev/hidrawN node of the "STeaMi CMSIS-DAP"
interface (read/write access needed, e.g. through the plugdev group).

    python tools/steami_version.py [/dev/hidrawN]
"""

import glob
import os
import select
import struct
import sys

COMMAND = 0x9F
MARK = b"STM1"
REPORT_SIZE = 64


def find_hidraw():
    for uevent in sorted(glob.glob("/sys/class/hidraw/hidraw*/device/uevent")):
        with open(uevent) as f:
            if "STeaMi CMSIS-DAP" in f.read():
                return "/dev/" + uevent.split("/")[4]
    sys.exit("no STeaMi CMSIS-DAP HID interface found")


def query(device):
    fd = os.open(device, os.O_RDWR)
    try:
        while select.select([fd], [], [], 0.05)[0]:  # drop stale reports
            os.read(fd, REPORT_SIZE)
        os.write(fd, bytes([0x00, COMMAND]) + bytes(REPORT_SIZE - 1))
        if not select.select([fd], [], [], 2)[0]:
            sys.exit("no reply from " + device)
        return os.read(fd, REPORT_SIZE)
    finally:
        os.close(fd)


def main():
    device = sys.argv[1] if len(sys.argv) > 1 else find_hidraw()
    reply = query(device)
    print("device:", device)
    print("reply: ", reply[:13].hex(" "))
    if reply[0] != COMMAND:
        sys.exit("unexpected command ID 0x%02X" % reply[0])
    if reply[1:5] != MARK:
        print("mark absent: DAPLink without the 0x9F command (older firmware)")
        return
    number, date = struct.unpack_from("<II", reply, 5)
    if number == 0:
        print("STM1: development build (release 0, date %d)" % date)
    else:
        print("STM1: release %d, date %d" % (number, date))


if __name__ == "__main__":
    main()
