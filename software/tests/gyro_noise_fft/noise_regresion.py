import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from sklearn.linear_model import LinearRegression

data = pd.read_csv("thurst_frequency.csv", delimiter=";")
thurst = np.array(data.thurst).reshape(-1, 1)
frequency = np.array(data.frequency).reshape(-1, 1)

model = LinearRegression().fit(thurst, frequency)

r_sq = model.score(thurst, frequency)
print('coefficient of determination:', r_sq)
print('intercept:', model.intercept_)
print('slope:', model.coef_)

freq_pred = model.predict(thurst)

fig, ax = plt.subplots()
ax.plot(thurst, frequency)
ax.plot(thurst, freq_pred)
plt.show()