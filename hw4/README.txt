1)

The number of CPUs in my system is 16
It seems each thread tends to be in a state of either relatively low latency, to sudden spikes of maxed out latency based on the bars.

2)

Watching the lowest value PID of the 16 threads, with Normal, the number of involuntary context switches changes in the order of two figures per update, with frequent 3 or 4 figure spikes.

Changing this to FIFO with a Priority of 1, the rate of involuntary context switches slows to 1 figure per update, with occasional two figure bursts.

Changing to RR and keeping Priority at 1 seems to keep the rate of context switches at a 1 to 2 figure rate, with occasional 3 figure bursts.

Keeping it at RR and changing Priority to 99, the screen and mouse froze for a couple of seconds, but resumed afterwards, with the rate of context switches slowing slightly, with occasional 2 figure speed ups.

Changing the policy to FIFO and keeping Priority at 99, the rate of context switches stayed mostly the same, maybe slightly slower.

Changing back to Normal significantly increased the rate of context switches, back to 2 to 3 figures of change per update.

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


sudo cset proc -m -p 27458 -t dedicated
sudo chrt -p -o 0 27458
sudo chrt -p -f 1 27458
sudo chrt -p -r 1 27458
sudo chrt -p -r 99 27458
sudo chrt -p -f 99 27458
sudo chrt -p -o 0 27458

Starting with Normal policy, the rate of context switches is at a very very stable 2 figure rate per update, around 20 to 40 context switches per 0.5 second update, a complete absence of 3 to 4 figure spikes.

Changing policy to FIFO with Priority level of 1, there are several update cycles where no context switches occur, maybe one context switch every dozen or so updates.

Changing policy to RR, again no context switches occur for several cycles. Notably, it one context switch occurs what seems like every 4 seconds. I explicitly counted going 'one mississippi two mississipi...', and it switches once right at 4 every time.

Changing the Priority level to 99 doesn't seem to change much for RR.
With Priority 1 using policy FIFO, the rate of context switches are slightly faster and less uniform / stable as RR, however setting Priority to 99 with FIFO seems to bring FIFO's rate of context switches more in line with the behavior exhibited by RR, with very rare and occasional early context switches that deviate from the context switch every 4 second pattern.
