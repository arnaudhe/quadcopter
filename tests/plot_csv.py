#! /usr/bin/env python3

import sys
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv(sys.stdin, sep=';')
df.plot()
plt.show()
