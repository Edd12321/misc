#!/usr/bin/env python
import json
import argparse
import time
import os
import sys
import traceback
import math
import signal
import termios
import select

parser = argparse.ArgumentParser(
	prog='ascjs.py',
	description='Play ascii json in terminal',
	epilog='aboba')
parser.add_argument('file')
parser.add_argument('--fps')
parser.add_argument('--no-loop', action='store_true')
parser.add_argument('--no-fit', action='store_true')
parser.add_argument('--clear', action='store_true')
args = parser.parse_args()

HIDECUR = '\033[?25l'
SHOWCUR = '\033[?25h'
CLRSCR = '\033[H'
CLRSCRFULL = '\033[2J' + CLRSCR
if args.clear:
	CLRSCR = CLRSCRFULL

winched = True
def winch(sig, frame):
	global winched
	winched = True

with open(args.file, 'r') as fin:
	tab = json.loads(fin.read())
	if args.fps == None:
		args.fps = 60
	if 'frameCount' not in tab:
		tab['frameCount'] = len(tab['frames'])
	sltime = 1 / int(args.fps)
	
	try:
		signal.signal(signal.SIGWINCH, winch)
		newt = oldt = termios.tcgetattr(sys.stdin.fileno())
		newt[3] &= ~termios.ICANON & ~termios.ECHO
		termios.tcsetattr(sys.stdin.fileno(), termios.TCSAFLUSH, newt)

		if not args.no_fit:
			maxl = maxc = 1
			for i in range(tab['frameCount']):
				cc = lc = 0
				for c in tab['frames'][i]:
					if c != '\n':
						cc += 1
						maxc = max(maxc, cc)
					else:
						lc += 1
						cc = 0
						maxl = max(maxl, lc)
				
				lc += 1
				cc = 0
				maxl = max(maxl, lc)
				maxc = max(maxc, cc)

		paused = False
		print(HIDECUR)
		while True:
			i = 0
			while i < len(tab['frames']):
				if select.select([sys.stdin], [], [], 0)[0] != []: # kbhit
					###########################################################
					# optiuni
					opt = sys.stdin.read(1)
					if opt == ' ':
						paused = not paused
					elif opt == 'q':
						exit()
					###########################################################

				ok = False
				if winched and not args.no_fit:
					tc, tl = os.get_terminal_size()
					skipc = max(1, int(math.ceil(maxc / tc)))
					skipl = max(1, int(math.ceil(maxl / tl)))
					print(CLRSCRFULL)
					winched = False
					ok = True

				if not paused or ok:
					print(CLRSCR)
					if not args.no_fit:
						cc = lc = 0
						for j in range(len(tab['frames'][i])):
							c = tab['frames'][i][j]
							if c != '\n':
								cc += 1
							else:
								lc += 1
								cc = 0
							if cc % skipc == 0 and lc % skipl == 0:
								print(c, end='')					
					else:
						print(tab['frames'][i])

					if not paused:
						i += 1
				
				time.sleep(sltime)
			
			sys.stdout.flush()
			if args.no_loop:
				break
	except:
		traceback.print_exc()
	finally:
		print(SHOWCUR, CLRSCRFULL)
		termios.tcsetattr(sys.stdin.fileno(), termios.TCSAFLUSH, oldt)
