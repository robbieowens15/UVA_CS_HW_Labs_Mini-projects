#!/usr/bin/env python3
import os
from subprocess import run

os.chdir('/Users/robbieowens/Documents/Documents/SchoolWork/College/7th Semester/Crypto/P2')

p = 43
o = 31
Gx = 25
Gy = 25
gen_key_command = f"./ecdsa.sh genkey {str(p)} {str(o)} {str(Gx)} {str(Gy)}"
gen_key_output = run(gen_key_command.split(" "), capture_output=True)

print(f"{gen_key_command}\texit code: {gen_key_output.returncode}")
if (gen_key_output.returncode != 0):
    print("stdout:", gen_key_output.stdout.decode())
    print("stderr:", gen_key_output.stderr.decode())
    exit()
else:
    out = gen_key_output.stdout.decode().split("\n")
    d = out[0].strip()
    Qx = out[1].strip()
    Qy = out[2].strip()
    print(f"Output: (d, Qx, Qy) = ({d}, {Qx}, {Qy})")

print()

h = 30
sign_command = f"./ecdsa.sh sign {str(p)} {str(o)} {str(Gx)} {str(Gy)} {d} {h}"
sign_output = run(sign_command.split(" "), capture_output=True)

print(f"{sign_command}\texit code: {sign_output.returncode}")
if (sign_output.returncode != 0):
    print("stdout:", sign_output.stdout.decode())
    print("stderr:", sign_output.stderr.decode())
    exit()
else:
    output = sign_output.stdout.decode().split("\n")
    r = output[0].strip()
    s = output[1].strip()
    print(f"Output: (r, s) = ({r}, {s})")

print()

print("\t\t\tTrue Hash")
verify_command = f"./ecdsa.sh verify {str(p)} {str(o)} {str(Gx)} {str(Gy)} {Qx} {Qy} {r} {s} {h}"
verify_output = run(verify_command.split(" "), capture_output=True)

print(f"{verify_command}\texit code: {verify_output.returncode}")
if (verify_output.returncode != 0):
    print("stdout:", verify_output.stdout.decode())
    print("stderr:", verify_output.stderr.decode())
else:
    output = verify_output.stdout.decode().split("\n")
    b = output[0].strip()
    print(f"Output: {b}")

print("\t\t\tFalse Hash")
verify_command = f"./ecdsa.sh verify {str(p)} {str(o)} {str(Gx)} {str(Gy)} {Qx} {Qy} {r} {s} {h-1}"
verify_output = run(verify_command.split(" "), capture_output=True)

print(f"{verify_command}\texit code: {verify_output.returncode}")
if (verify_output.returncode != 0):
    print("stdout:", verify_output.stdout.decode())
    print("stderr:", verify_output.stderr.decode())
else:
    output = verify_output.stdout.decode().split("\n")
    b = output[0].strip()
    print(f"Output: {b}")