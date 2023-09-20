import random
import sys

"""
ELLIPTIC CURVE OPERATIONS

add_self
add_two_points
scalar_point_multiplication

utility function:
- get_slope_at_point
- get_slope_between_points
"""
def get_slope_at_point(x, y, field_size): #! may need to make adjustments for finite field!
	rise = multiplication(3, exponentiation(x, 2, field_size), field_size)
	run = multiplication(2, y, field_size)
	return division(rise, run, field_size)
	# return (3* (x ** 2) / (2 * y))

def get_slope_between_points(x1, y1, x2, y2, field_size): #! may need to make adjustments for finite field!
	rise = subtraction(y2, y1, field_size)
	run = subtraction(x2, x1, field_size)
	return division(rise, run, field_size)
	# return (y2 - y1) / (x2 - x1)

# Q = P + P
def add_self(P, field_size):
	slope = get_slope_at_point(P[0], P[1], field_size)
	x3 = subtraction(subtraction(exponentiation(slope, 2, field_size), P[0], field_size), P[0], field_size)
	y3 = subtraction(multiplication(slope, subtraction(P[0], x3, field_size), field_size), P[1], field_size)
	return (x3,y3)

# P = Q + R
def add_two_points(Q, R, field_size):
	if Q == R: # corner case
		return add_self(Q, field_size)
	slope = get_slope_between_points(Q[0], Q[1], R[0], R[1], field_size)
	x3 = subtraction(subtraction(exponentiation(slope, 2, field_size), Q[0], field_size), R[0], field_size)
	y3 = subtraction(multiplication(slope, subtraction(Q[0], x3, field_size), field_size), Q[1], field_size)
	return (x3,y3)
# P = k x Q
#! NB THIS MUST BE A LOG TIME ALGORITHM
def scalar_point_multiplication(k, Q, field_size):
	bit_arr = format(k, "b")[::-1]
	nQ = [(0,0)] * len(bit_arr)
	nQ[0] = Q
	for i in range(1, len(nQ)):
		nQ[i] = add_self(nQ[i-1], field_size)
	P = None
	for i in range(len(bit_arr)):
		if bit_arr[i] == "1" and P is None:
			P = nQ[i]
		elif bit_arr[i] == "1":
			P = add_two_points(P, nQ[i], field_size)

	return P


"""
FINITE FIELDS OPERATIONS

Addition
Subtraction
Multiplication
Exponentiation
Division

Utilities:
- get_additive_inverse
- get_multiplicative_inverse
"""

def addition(x, y, field_size):
	return (x + y) % field_size

def multiplication(x, y, field_size):
	return (x * y) % field_size

def exponentiation(x, y, field_size):
	return (x ** y) % field_size

def subtraction(x, y, field_size):
	add_inv = get_additive_inverse(y, field_size)
	ans = addition(x, add_inv, field_size)
	while ans < 0:
		ans += field_size
	return ans

def division(x, y, field_size):
	mul_inv = get_multiplicative_inverse(y, field_size)
	return (x * mul_inv) % field_size

def get_additive_inverse(x, field_size):
	return field_size - x

def get_multiplicative_inverse(x, field_size):
	return (x ** (field_size -2)) % field_size

"""
ECDSA

genkey
sign
verify
"""

def generate_key(p, o, Gx, Gy):
	d = random.randint(1, o-1) # private key
	Q = scalar_point_multiplication(d, (Gx, Gy), p) # public key

	# Conditions to regenerate point (two points on vertical line)
	if (Q == (0,0) or add_two_points((Gx,Gy), Q, p) == (Gx,Gy)):
		return generate_key(p, o, Gx, Gy)

	return (d, Q[0], Q[1])

def sign(p, o, Gx, Gy, d, h):
	k = random.randint(1, o-1)
	k_inv = get_multiplicative_inverse(k, o)
	R = scalar_point_multiplication(k, (Gx, Gy), p)
	r = R[0]
	s = multiplication(k_inv, addition(h, multiplication(r, d, o),o),o)
	# Conditions to regenerate signature (two points on vertical line)
	if r == 0 or s == 0:
		return sign(p, o, Gx, Gy, d, h)
	return (r,s)

def verify(p, o, Gx, Gy, Qx, Qy, r, s, h):
	s_inv = get_multiplicative_inverse(s, o)
	# left_scalar = s_inv * h
	left_scalar = multiplication(s_inv, h, o)
	LEFT = scalar_point_multiplication(left_scalar, (Gx, Gy), p)
	# right_scalar = s_inv * r
	right_scalar = multiplication(s_inv, r, o)
	RIGHT = scalar_point_multiplication(right_scalar, (Qx, Qy), p)
	R = add_two_points(LEFT, RIGHT, p)
	return R[0] == r

if __name__ == "__main__":
	mode = sys.argv[1]
	if mode == "userid":
		print("rao7utn")
	elif mode == "genkey":
		p = int(sys.argv[2])
		o = int(sys.argv[3])
		Gx = int(sys.argv[4])
		Gy = int(sys.argv[5])
		d, Qx, Qy = generate_key(p, o, Gx, Gy)
		print(d)
		print(Qx)
		print(Qy)
	elif mode == "sign": 
		p = int(sys.argv[2])
		o = int(sys.argv[3])
		Gx = int(sys.argv[4])
		Gy = int(sys.argv[5])
		d = int(sys.argv[6])
		h = int(sys.argv[7])
		r, s = sign(p, o, Gx, Gy, d, h)
		print(str(r))
		print(str(s))
	elif mode == "verify": #NB occasionally invalid signatures will validate as true. expected behavior
		p = int(sys.argv[2])
		o = int(sys.argv[3])
		Gx = int(sys.argv[4])
		Gy = int(sys.argv[5])
		Qx = int(sys.argv[6])
		Qy = int(sys.argv[7])
		r = int(sys.argv[8])
		s = int(sys.argv[9])
		h = int(sys.argv[10])
		if (verify(p, o, Gx, Gy, Qx, Qy, r, s, h)):
			print("True")
		else:
			print("False")
	elif mode == "add": #! Note required in final submission
		x = int(sys.argv[2])
		y = int(sys.argv[3])
		field_size = int(sys.argv[4])
		print(addition(x, y, field_size))
	elif mode == "mul": #! Note required in final submission
		x = int(sys.argv[2])
		y = int(sys.argv[3])
		field_size = int(sys.argv[4])
		print(multiplication(x, y, field_size))
	elif mode == "exp": #! Note required in final submission
		x = int(sys.argv[2])
		y = int(sys.argv[3])
		field_size = int(sys.argv[4])
		print(exponentiation(x, y, field_size))
	elif mode == "add_inv": #! Note required in final submission
		x = int(sys.argv[2])
		field_size = int(sys.argv[3])
		print(get_additive_inverse(x, field_size))
	elif mode == "sub": #! Note required in final submission
		x = int(sys.argv[2])
		y = int(sys.argv[3])
		field_size = int(sys.argv[4])
		print(subtraction(x, y, field_size))
	elif mode == "mul_inv": #! Note required in final submission
		x = int(sys.argv[2])
		field_size = int(sys.argv[3])
		print(get_multiplicative_inverse(x, field_size))
	elif mode == "div": #! Note required in final submission
		x = int(sys.argv[2])
		y = int(sys.argv[3])
		field_size = int(sys.argv[4])
		print(division(x, y, field_size))
	elif mode == "elp_add": #! Note required in final submission
		x1 = int(sys.argv[2])
		y1 = int(sys.argv[3])
		x2 = int(sys.argv[4])
		y2 = int(sys.argv[5])
		field_size = int(sys.argv[6])
		print(add_two_points((x1, y1), (x2, y2), field_size))
	elif mode == "elp_add_self": #! Note required in final submission
		x = int(sys.argv[2])
		y = int(sys.argv[3])
		field_size = int(sys.argv[4])
		print(add_self((x, y), field_size))
	elif mode == "elp_k_mul_self": #! Note required in final submission
		x = int(sys.argv[2])
		y = int(sys.argv[3])
		k = int(sys.argv[4])
		field_size = int(sys.argv[5])
		print(scalar_point_multiplication(k, (x, y), field_size))
	else:
		print(f"Unrecognized mode: {mode}")
	
