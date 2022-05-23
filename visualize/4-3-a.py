"""
@author hsc
@application: node2vec
@dataset: all dataset
@description: io count
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

sowalker_bar = [2, 17, 40, 128, 555, 3410]
graphwalker_bar = [2, 17, 1157, 2993, 17251, 45734]
ratio = [0, 0, 0.965427831, 0.957233545, 0.967827952, 0.925438405]
dataset = ["$\mathrm{SL}$", "$\mathrm{RM27}$", "$\mathrm{TW}$", "$\mathrm{CF}$", "$\mathrm{RM28}$", "$\mathrm{UK}$"]

x1 = np.arange(len(sowalker_bar))
x2 = [x + bar_width for x in x1]

fig, axes1 = plt.subplots()
lns1 = axes1.bar(x1, sowalker_bar, width=bar_width, color="white", label=r"$\mathrm{SOWalker}$", edgecolor="black", hatch="**")
lns2 = axes1.bar(x2, graphwalker_bar, width=bar_width, color="white", label=r"$\mathrm{GraphWalker}$", edgecolor="black", hatch="//")

axes1.set_xticks([r + bar_width / 2 for r in range(len(sowalker_bar))], dataset)
axes1.ticklabel_format(axis='y', style='sci', scilimits=(0,0), useMathText=True)
axes1.set_ylabel("$\mathrm{I/O}$ 次数")
axes1.set_xlabel("数据集")
axes1.set_yscale("log")
axes1.grid(True, axis='y', linestyle=':')
axes1.set_ylim(1, 50000)

axes2 = axes1.twinx()
lns3 = axes2.plot(x1, ratio, 'g', marker='o', markersize=2, label=r"缩短比例$\mathrm{\beta}$")
axes2.set_ylabel("缩短比例")
axes2.set_ylim(0, 1.0)

axes1.legend(handles=[lns1, lns2, lns3[0]], prop={'size' : 8})
plt.savefig("4-3-a.png", dpi=300)