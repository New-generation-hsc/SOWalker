"""
@author hsc
@application: node2vec
@dataset: all dataset
@description: total execution time for different partition scheduling algorithm
"""

import matplotlib.pyplot as plt
import numpy as np

plt.style.use(['science', 'ieee', 'no-latex', 'cjk-sc-font'])

config = {
    "font.family":'serif',
    "font.size": 10,
    "mathtext.fontset":'stix',
    "font.serif": ['SimSun'],
}
plt.rcParams.update(config)

# width of the bars
bar_width = 0.3

acc_sowalker_bar = [35.2419, 1099.44, 477.119698, 787.425206, 70701.22502, 101805]
sowalker_bar = [28.7867, 1129.97, 560.56, 806.326, 2413.91, 1759.92]


dataset = ["$\mathrm{SL}$", "$\mathrm{RM27}$", "$\mathrm{TW}$", "$\mathrm{CF}$", "$\mathrm{RM28}$", "$\mathrm{UK}$"]

x1 = np.arange(len(sowalker_bar))
x2 = [x + bar_width for x in x1]
f, (ax1, ax2) = plt.subplots(2, 1, sharex=True)
ax1.spines['bottom'].set_visible(False)
ax2.spines['top'].set_visible(False)
ax1.xaxis.tick_top()
ax2.xaxis.tick_bottom()

d = .02
kwargs = dict(transform=ax1.transAxes, color='k', clip_on=False)
ax1.plot((-d, +d), (-d, +d), **kwargs)        # top-left diagonal
ax1.plot((1 - d, 1 + d), (-d, +d), **kwargs)  # top-right diagonal

kwargs.update(transform=ax2.transAxes)  # switch to the bottom axes
ax2.plot((-d, +d), (1 - d, 1 + d), **kwargs)  # bottom-left diagonal
ax2.plot((1 - d, 1 + d), (1 - d, 1 + d), **kwargs)  # bottom-right diagonal

plt.subplots_adjust(hspace=0.05)

ax1.bar(x1, sowalker_bar, width=bar_width, color="white", label=r"$\mathrm{SA + SOWalker}$", edgecolor="black", hatch="**")
ax1.bar(x2, acc_sowalker_bar, width=bar_width, color="white", label=r"$\mathrm{ILP + SOWalker}$", edgecolor="black", hatch="//")
ax1.set_ylim(60000, 120000)
ax1.legend(loc=2, prop={'size' : 8})

ax2.bar(x1, sowalker_bar, width=bar_width, color="white", label=r"$\mathrm{SA + SOWalker}$", edgecolor="black", hatch="**")
ax2.bar(x2, acc_sowalker_bar, width=bar_width, color="white", label=r"$\mathrm{ILP + SOWalker}$", edgecolor="black", hatch="//")

ax2.set_xticks([r + bar_width / 2 for r in range(len(sowalker_bar))], dataset)
ax2.set_ylim(0, 2500)

ax2.set_ylabel("执行时间$\mathrm{(s)}$")
ax2.set_xlabel("数据集")
ax2.yaxis.set_label_coords(-0.2,1)


ax1.grid(True, axis='y', linestyle=':')
ax2.grid(True, axis='y', linestyle=':')
plt.savefig("4-6-b.png", dpi=300)