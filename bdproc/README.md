## bdproc
This one-liner disables/enables BD PROCHOT, because it can cause performance issues. For instance, on my laptop, BD PROCHOT throttles my CPU so that it only uses *0.40 GHz* because of an aftermarket battery. There is an easy fix, though: by decrementing the 0x1FC MSR register, this can be disabled.

If it gives you an error about some command not being found, make sure you have ```rdmsr``` and ```wrmsr``` installed. On Arch Linux, they can be installed by the following package (community repo must be enabled):

```sudo pacman -S msr-tools```

## CLI arguments
```-d``` to remain disabled regardless of current state
```-e``` to remain enabled regardless of current state
