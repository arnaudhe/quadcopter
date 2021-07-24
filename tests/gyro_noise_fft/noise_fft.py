import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import scipy.fftpack

imu_data = pd.read_csv("imu.csv", delimiter=";")
x = np.array(imu_data.t)
gyr = np.array(imu_data.gx)
acc = np.array(imu_data.gy)
N = len(imu_data.t)
T = 0.001

xf = np.linspace(0.0, 1.0//(2.0*T), N//2)
gyr_f = scipy.fftpack.fft(gyr)
acc_f = scipy.fftpack.fft(acc)

print(xf)

fig, ax = plt.subplots()
ax.plot(xf, 2.0/N * np.abs(gyr_f[:N//2]))
ax.plot(xf, 2.0/N * np.abs(acc_f[:N//2]))
plt.show()