"""
@author hsc
@application: pagerank
@dataset: soc-livejournal
@description: walks distribution among blocks
"""

import matplotlib.pyplot as plt
from matplotlib.pyplot import MultipleLocator
import numpy as np

plt.style.use(['science', 'ieee', 'no-latex', 'cjk-sc-font'])

config = {
    "font.family": 'serif',
    "font.size": 6,
    "mathtext.fontset": 'stix',
    "font.serif": ['SimSun'],
}
plt.rcParams.update(config)


count = [7762, 122558, 11308, 4952, 6597, 50171, 156068, 24166,
         36173, 101039, 86888, 19200, 116746, 143300, 31094, 55287, 18145]

x = np.arange(len(count))

fig = plt.figure(figsize=(4, 2))
plt.plot(x, count, color="black", marker='o', markersize=2, linewidth=1)
plt.ticklabel_format(axis='y', style='sci', scilimits=(0, 0), useMathText=True)
plt.gca().xaxis.set_major_locator(MultipleLocator(1))
plt.ylabel("分区中随机游走数目")
plt.xlabel("子图分区编号")
plt.ylim(0, 160000)

plt.grid(True, axis='y', linestyle=':')
plt.savefig("2-5.png", dpi=300)
