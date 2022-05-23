"""
@author hsc
@application: node2vec
@dataset: all dataset
@description: walk speed
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

sowalker_bar = [3408873.137, 2396745.145, 2223368.537, 3204929.746, 2340931.468, 2272303.624]
graphwalker_bar = [3020716.923, 1310928.131, 1680523.519, 2139254.22, 1480485.24, 2197351.662]


dataset = ["$\mathrm{SL}$", "$\mathrm{RM27}$", "$\mathrm{TW}$", "$\mathrm{CF}$", "$\mathrm{RM28}$", "$\mathrm{UK}$"]

x1 = np.arange(len(sowalker_bar))
x2 = [x + bar_width for x in x1]
plt.bar(x1, sowalker_bar, width=bar_width, color="white", label=r"$\mathrm{SOWalker}$", edgecolor="black", hatch="**")
plt.bar(x2, graphwalker_bar, width=bar_width, color="white", label=r"$\mathrm{GraphWalker}$", edgecolor="black", hatch="//")

plt.xticks([r + bar_width / 2 for r in range(len(sowalker_bar))], dataset)
plt.ticklabel_format(axis='y', style='sci', scilimits=(0,0), useMathText=True)
plt.ylabel("$\mathrm{TEPS}$ $\mathrm{(steps/s)}$")
plt.xlabel("数据集")
plt.legend(loc="upper right", prop={'size': 8})

plt.grid(True, axis='y', linestyle=':')
plt.savefig("4-1-b.png", dpi=300)