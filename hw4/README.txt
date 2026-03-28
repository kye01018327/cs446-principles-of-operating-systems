1)

The number of CPUs in my system is 16
It seems each thread tends to be in a state of either relatively low latency, to sudden spikes of maxed out latency based on the bars.

2)

Watching the 4th lowest value PID of the 16 threads, with Normal the number of involuntary context switches changes in the order of two figures per update, with frequent 3 or 4 figure spikes.

Changing this to FIFO with a Priority of 1, the rate of involuntary context switches slows to a very slow 1 context switch every few seconds.

Changing to RR and keeping Priority at 1 seems to keep the rate of context switches, with some two figure bursts every several seconds or so.

Keeping it at RR and changing Priority to 99, the screen and mouse froze for a couple of seconds, but resumed afterwards, with the rate of context switches slowing slightly, with occasional 2 figure speed ups.

Changing the policy to FIFO and keeping Priority at 99, the rate of context switches stayed mostly the same, maybe the most stable out of all of the real-time options and priorities, though by a very small amount.

Changing back to Normal significantly increased the rate of context switches, back to 2 to 3 figures of change per update, with spikes.

Sequence of commands:

./sched 16
watch -n.5 grep ctxt /proc/10786/status
sudo chrt -p -o 0 10786
sudo chrt -p -f 1 10786
sudo chrt -p -r 1 10786
sudo chrt -p -r 99 10786
sudo chrt -p -f 99 10786
sudo chrt -p -o 0 10786

3)

Sequence of commands:

sudo cset set -c 0-14 system
sudo cset set -c 15 dedicated
sudo cset proc -m -f root -t system
sudo cset proc -k -f root -t system
./sched 16
watch -n.5 grep ctxt /proc/27458/status
sudo cset proc -m -p 27458 -t dedicated

It seems like with a Normal policy, the dedicated thread experiences less spikes, but there is still a 2 to 3 figure rate of involuntary context switches per half second update.

4)

Sequence of commands:
sudo chrt -p -o 0 27458
sudo chrt -p -f 1 27458
sudo chrt -p -r 1 27458
sudo chrt -p -r 99 27458
sudo chrt -p -f 99 27458
sudo chrt -p -o 0 27458

It seems like regardless of policy or Priority level, the rate of context switches seems to be once every 4 seconds. Maybe with FIFO on a Priority of 1, the timing could be slightly more varied, with a context switch very occasionally occurring slightly faster than 4 seconds.