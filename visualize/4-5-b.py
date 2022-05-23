"""
@author hsc
@application: second-order pagerank
@dataset: all dataset
@description: average walk step
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

sowalker_bar = [20, 20, 5.10981, 4.03585, 2.11033, 5.92797]
graphwalker_bar = [6.33028, 1.05706, 1.09761, 1.18876, 1.02996, 4.63347]
dataset = ["$\mathrm{SL}$", "$\mathrm{RM27}$", "$\mathrm{TW}$", "$\mathrm{CF}$", "$\mathrm{RM28}$", "$\mathrm{UK}$"]

x1 = np.arange(len(sowalker_bar))
x2 = [x + bar_width for x in x1]
plt.xticks([r + bar_width/2 for r in range(len(sowalker_bar))], dataset)
plt.bar(x1, sowalker_bar, width=bar_width, color="white", label=r"$\mathrm{SOWalker}$", edgecolor="black", hatch="**")
plt.bar(x2, graphwalker_bar, width=bar_width, color="white", label=r"$\mathrm{GraphWalker}$", edgecolor="black", hatch="//")
plt.ylabel("单次平均游走步长")
plt.xlabel("数据集")
plt.ylim(0)
plt.legend(prop={'size' : 8})
plt.grid(True, axis='y', linestyle=':')
plt.savefig("4-5-b.png", dpi=300)