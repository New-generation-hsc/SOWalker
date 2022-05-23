"""
@author hsc
@application: node2vec
@dataset: all dataset
@description: io count for different partition scheduling algorithm
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

acc_sowalker_bar = [2, 17, 40, 101, 376, 1589]
sowalker_bar = [2, 17, 40, 101, 429, 1847]
graphwalker_bar = [2, 17, 1157, 2993, 17251, 45734]
ratio = [0, 0, 0.965427831, 0.957233545, 0.967827952, 0.925438405]
dataset = ["$\mathrm{SL}$", "$\mathrm{RM27}$", "$\mathrm{TW}$", "$\mathrm{CF}$", "$\mathrm{RM28}$", "$\mathrm{UK}$"]

x1 = np.arange(len(sowalker_bar))
x2 = [x + bar_width for x in x1]
plt.bar(x1, sowalker_bar, width=bar_width, color="white", label=r"$\mathrm{SA + SOWalker}$", edgecolor="black", hatch="**")
plt.bar(x2, acc_sowalker_bar, width=bar_width, color="white", label=r"$\mathrm{ILP + SOWalker}$", edgecolor="black", hatch="//")

plt.xticks([r + bar_width / 2 for r in range(len(sowalker_bar))], dataset)
plt.ticklabel_format(axis='y', style='sci', scilimits=(0,0), useMathText=True)
plt.ylabel("$\mathrm{I/O}$ 次数")
plt.xlabel("数据集")
plt.legend(prop={'size' : 8})
plt.yscale("log")
plt.ylim(1, 3000)
plt.grid(True, axis='y', linestyle=':')
plt.savefig("4-6-a.png", dpi=300)