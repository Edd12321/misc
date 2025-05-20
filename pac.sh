#!/bin/sh
# To be run as root
# Creates dumb pacman wrapper
# sudo ./break_ur_pc.sh
TMPSRC=$(mktemp)
rm $TMPSRC
echo '
#include <unistd.h>
int main(int argc, char *argv[])
{
	setuid(0); setgid(0);
	execv("/usr/bin/pacman", argv);
}
' > $TMPSRC.c
gcc $TMPSRC.c -o pac
chown root:root pac && chmod 4755 pac
echo "Done! Now run ./pac (args...)"
