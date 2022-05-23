"""
@author hsc
@application: second-order pagerank
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

sowalker_bar = [2423048.522, 253873.8194, 129956.0257, 2579712.47, 204677.9451, 1914402.471]
graphwalker_bar = [427039.5006, 8290.423799, 3258.011345, 395922.1915, 5300.863776, 86360.14934]

dataset = ["$\mathrm{SL}$", "$\mathrm{RM27}$", "$\mathrm{TW}$", "$\mathrm{CF}$", "$\mathrm{RM28}$", "$\mathrm{UK}$"]

x1 = np.arange(len(sowalker_bar))
x2 = [x + bar_width for x in x1]
plt.bar(x1, sowalker_bar, width=bar_width, color="white", label=r"$\mathrm{SOWalker}$", edgecolor="black", hatch="**")
plt.bar(x2, graphwalker_bar, width=bar_width, color="white", label=r"$\mathrm{GraphWalker}$", edgecolor="black", hatch="//")

plt.xticks([r + bar_width / 2 for r in range(len(sowalker_bar))], dataset)
plt.ticklabel_format(axis='y', style='sci', scilimits=(0,0), useMathText=True)
plt.ylabel("$\mathrm{TEPS}$ $\mathrm{(steps/s)}$")
plt.xlabel("数据集")
plt.legend(loc="upper right", bbox_to_anchor=(0.52, 1), prop={'size': 8})

plt.yscale("log")
plt.ylim(1, 10000000)


plt.grid(True, axis='y', linestyle=':')
plt.savefig("4-2-b.png", dpi=300)