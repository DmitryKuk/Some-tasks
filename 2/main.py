#!/usr/bin/python

# Usage: python filename.py N M

import sys


if len(sys.argv) != 3:
	print("Usage: %s N M" % sys.argv[0])
	exit(1)

N = int(sys.argv[1])
M = int(sys.argv[2])


def f(N, M):
	s = N % M
	return (s // 2, s - s // 2)

print(f(N, M))
