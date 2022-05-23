"""
@author hsc
@application: node2vec
@dataset: all dataset
@description: total execution time
"""

import matplotlib.pyplot as plt
import numpy as np

plt.style.use(['science', 'ieee', 'no-latex', 'cjk-sc-font'])

config = {
    "font.family": 'serif',
    "font.size": 10,
    "mathtext.fontset": 'stix',
    "font.serif": ['SimSun'],
}
plt.rcParams.update(config)

# width of the bars
bar_width = 0.3

sowalker_bar = [32.4159, 1129.97, 560.56, 806.326, 2413.91, 1759.92]
graphwalker_bar = [33.3137, 2087.52, 874.026, 1490.45, 5844.59, 5728.61]
ratio = [0.111957404, 0.458702192, 0.359379036, 0.459004998, 0.58698386, 0.692784113]
dataset = ["$\mathrm{SL}$", "$\mathrm{RM27}$", "$\mathrm{TW}$", "$\mathrm{CF}$", "$\mathrm{RM28}$", "$\mathrm{UK}$"]

x1 = np.arange(len(sowalker_bar))
x2 = [x + bar_width for x in x1]

fig, axes1 = plt.subplots()
lns1 = axes1.bar(x1, sowalker_bar, color='white', width=bar_width,
                 label=r"$\mathrm{SOWalker}$", edgecolor='black',  hatch="**")
lns2 = axes1.bar(x2, graphwalker_bar, color='white', width=bar_width,
                 label=r"$\mathrm{GraphWalker}$", edgecolor='black', hatch="//")

axes1.set_xticks(
    [r + bar_width / 2 for r in range(len(sowalker_bar))], dataset)
axes1.ticklabel_format(axis='y', style='sci',
                       scilimits=(0, 0), useMathText=True)
axes1.set_ylabel("执行时间 $\mathrm{(s)}$")
axes1.set_xlabel("数据集")
axes1.set_yscale("log")
axes1.grid(True, axis='y', linestyle=':')
axes1.set_ylim(1, 50000)
axes2 = axes1.twinx()
lns3 = axes2.plot(x1, ratio, 'g', marker='o', markersize=2,
                  label=r"缩短比例$\mathrm{\beta}$")
axes2.set_ylabel("缩短比例")
axes2.set_ylim(0, 1.0)

axes1.legend(handles=[lns1, lns2, lns3[0]], prop={'size': 8})
plt.savefig("4-1-a.png", dpi=300)
