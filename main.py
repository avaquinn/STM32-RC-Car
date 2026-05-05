#####################################################################
# DISCLAIMER:
# This code was AI-generated and has not been reviewed or tested for
# unsafe behavior or security vulnerabilities. It is provided
# as a basic control interface and should be used with caution.
#                                                          - Ava Q.
#####################################################################

import serial
import time
from pynput import keyboard

PORT = "/dev/cu.DSDTECHHC-05"
BAUD = 9600

SEND_PERIOD = 0.10

current_cmd = "Q"   # Q means stop
running = True


def on_press(key):
    global current_cmd, running

    try:
        k = key.char.lower()

        if k == "w":
            current_cmd = "W"   # forward
        elif k == "a":
            current_cmd = "A"   # left
        elif k == "s":
            current_cmd = "S"   # backward
        elif k == "d":
            current_cmd = "D"   # right
        elif k == "q":
            current_cmd = "Q"   # stop

    except AttributeError:
        if key == keyboard.Key.esc:
            current_cmd = "Q"
            running = False


def on_release(key):
    global current_cmd

    try:
        k = key.char.lower()

        # Releasing any movement key stops the car
        if k in ["w", "a", "s", "d"]:
            current_cmd = "Q"

    except AttributeError:
        pass


def read_from_stm32(bt):
    while bt.in_waiting > 0:
        msg = bt.readline().decode(errors="ignore").strip()
        if msg:
            print("STM32:", msg)


def main():
    global running

    bt = serial.Serial(PORT, BAUD, timeout=0.1)
    time.sleep(2)

    print("Connected to HC-05.")
    print("Hold keys to drive:")
    print("  W = forward")
    print("  A = left")
    print("  S = backward")
    print("  D = right")
    print("  Q = stop")
    print("  Esc = quit script")

    listener = keyboard.Listener(on_press=on_press, on_release=on_release)
    listener.start()

    try:
        while running:
            bt.write(current_cmd.encode())
            print("Sent:", current_cmd)

            # Read anything STM32 sent back
            read_from_stm32(bt)

            time.sleep(SEND_PERIOD)

    finally:
        bt.write(b"Q")  # stop before disconnecting
        time.sleep(0.1)
        read_from_stm32(bt)

        bt.close()
        listener.stop()
        print("Stopped and disconnected.")


if __name__ == "__main__":
    main()