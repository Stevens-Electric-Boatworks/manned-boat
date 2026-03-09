"""
NMEA Serial Simulator using Pseudo-Terminals (PTY)

Creates a virtual serial port pair. Your decoder reads from the slave end
(symlinked to /tmp/ttyUSB1_fake), while this script writes fake NMEA
sentences to the master end — simulating real GPS hardware.

Usage:
    1. Run this script in one terminal:         python nmea_serial_simulator.py
    2. Point your decoder at the fake port:     /tmp/ttyUSB1_fake
"""

import os
import pty
import time
import threading
import sys

# --- Fake NMEA sentences ---
# Add or modify sentences here to test your decoder
NMEA_SENTENCES = [
    "$GPGSV,4,1,16,02,82,150,53,11,78,139,,12,72,191,53,25,50,296,51*77",
    "$GPGSV,4,2,16,06,43,056,49,20,33,149,45,29,20,275,44,19,16,087,46*73",
    "$GPGSV,4,3,16,31,11,329,42,05,10,169,42,24,07,212,44,04,01,033,*7C",
    "$GPGSV,4,4,16,44,32,184,47,51,31,171,48,48,31,194,47,46,30,199,47*7E",
    "$GPGSA,M,3,05,02,31,06,19,29,20,12,24,25,,,0.9,0.5,0.7*35",
    "$GPGGA,202530.00,5109.0262,N,11401.8407,W,5,40,0.5,1097.36,M,-17.00,M,18,TSTR*61",
    "$GPVTG,224.592,T,224.592,M,0.003,N,0.005,K,D*20"
]

FAKE_PORT_SYMLINK = "/tmp/ttyUSB1_fake"
SEND_INTERVAL = 1.0  # seconds between sentences


def nmea_checksum(sentence: str) -> str:
    """Calculate NMEA checksum (XOR of all bytes between $ and *)."""
    data = sentence.strip().lstrip("$").split("*")[0]
    checksum = 0
    for char in data:
        checksum ^= ord(char)
    return f"{checksum:02X}"


def verify_sentence(sentence: str) -> bool:
    """Verify the checksum of an NMEA sentence."""
    if "*" not in sentence:
        return False
    body, given = sentence.strip().lstrip("$").split("*")
    calculated = 0
    for char in body:
        calculated ^= ord(char)
    return f"{calculated:02X}".upper() == given.upper()


def write_sentences(master_fd: int, stop_event: threading.Event):
    """Continuously write NMEA sentences to the master PTY fd."""
    print(f"[simulator] Starting to stream {len(NMEA_SENTENCES)} sentence types...")
    index = 0
    while not stop_event.is_set():
        sentence = NMEA_SENTENCES[index % len(NMEA_SENTENCES)]
        line = sentence + "\r\n"
        try:
            os.write(master_fd, line.encode())
            print(f"[simulator] Sent: {sentence}")
        except OSError as e:
            print(f"[simulator] Write error (decoder disconnected?): {e}")
            break
        index += 1
        time.sleep(SEND_INTERVAL)


def main():
    # Verify all sentences have valid checksums before sending
    print("[simulator] Verifying sentence checksums...")
    for s in NMEA_SENTENCES:
        status = "OK" if verify_sentence(s) else "INVALID"
        print(f"  [{status}] {s}")

    # Create the PTY pair
    master_fd, slave_fd = pty.openpty()
    slave_path = os.ttyname(slave_fd)
    print(f"\n[simulator] PTY slave device: {slave_path}")

    # Symlink the slave to a predictable path your decoder can use
    if os.path.exists(FAKE_PORT_SYMLINK):
        os.remove(FAKE_PORT_SYMLINK)
    os.symlink(slave_path, FAKE_PORT_SYMLINK)
    print(f"[simulator] Symlinked to:     {FAKE_PORT_SYMLINK}")
    print(f"\n>>> Point your decoder to: {FAKE_PORT_SYMLINK} <<<\n")

    stop_event = threading.Event()
    writer_thread = threading.Thread(
        target=write_sentences, args=(master_fd, stop_event), daemon=True
    )
    writer_thread.start()

    try:
        print("[simulator] Running. Press Ctrl+C to stop.\n")
        writer_thread.join()
    except KeyboardInterrupt:
        print("\n[simulator] Stopping...")
        stop_event.set()
    finally:
        os.close(master_fd)
        os.close(slave_fd)
        if os.path.islink(FAKE_PORT_SYMLINK):
            os.remove(FAKE_PORT_SYMLINK)
        print("[simulator] Cleaned up. Bye!")


if __name__ == "__main__":
    main()