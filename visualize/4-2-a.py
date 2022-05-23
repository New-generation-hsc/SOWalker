"""
@author hsc
@application: second-order pagerank
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

sowalker_bar = [9.043741, 87.62475, 180.472458, 34.82584, 243.638302, 400.856803]
graphwalker_bar = [47.828795, 2425.082846, 6268.875002, 293.731799, 5314.42, 2253.891014]
auto_ratio = [0.810914304, 0.963867317, 0.971211348, 0.881436603, 0.954155241, 0.822148986]
dataset = ["$\mathrm{SL}$", "$\mathrm{RM27}$", "$\mathrm{TW}$", "$\mathrm{CF}$", "$\mathrm{RM28}$", "$\mathrm{UK}$"]

x1 = np.arange(len(sowalker_bar))
x2 = [x + bar_width for x in x1]

fig, axes1 = plt.subplots()
lns1 = axes1.bar(x1, sowalker_bar, color='white', width=bar_width,
                 label=r"$\mathrm{SOWalker}$", edgecolor='black', hatch="**")
lns2 = axes1.bar(x2, graphwalker_bar, color='white', width=bar_width,
                 label=r"$\mathrm{GraphWalker}$", edgecolor='black', hatch='//')

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
lns3 = axes2.plot(x1, auto_ratio, 'g', marker='o',
                  markersize=2, label=r"缩短比例$\mathrm{\beta}$")
axes2.set_ylabel("缩短比例")
axes2.set_ylim(0, 1.0)

axes1.legend(handles=[lns1, lns2, lns3[0]], loc="upper right",
             bbox_to_anchor=(0.42, 1), prop={'size': 8})
plt.savefig("4-1-b.png", dpi=300)
