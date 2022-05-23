"""
@author hsc
@application: node2vec
@dataset: soc-livejournal
@description: average walk step 
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


count = [1.13521, 1.18198, 1.29441, 1.55314, 2.44982, 9.19979, 20]

x = np.arange(len(count))
block_size = ["$\mathrm{8}$", "$\mathrm{16}$", "$\mathrm{32}$",
              "$\mathrm{64}$", "$\mathrm{128}$", "$\mathrm{256}$", "$\mathrm{512}$"]

fig = plt.figure(figsize=(4, 2))
plt.xticks(x, block_size)
plt.plot(x, count, color="black", marker='o', markersize=2, linewidth=1)
plt.xlabel("子图分区大小 $\mathrm{(MB)}$")
plt.ylabel("平均游走步长")
plt.ylim(0)

plt.grid(True, axis='y', linestyle=':')
plt.savefig("2-5.png", dpi=300)
