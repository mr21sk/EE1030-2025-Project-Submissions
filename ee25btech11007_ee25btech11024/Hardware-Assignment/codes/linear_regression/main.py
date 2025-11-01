import numpy as np
import matplotlib.pyplot as plt


T_train = np.array([23.6, 42.7, 46.5, 60.8, 81.5], float)
V_train = np.array([2.688, 2.727, 2.732, 2.781, 2.864], float)


X = np.column_stack([np.ones_like(T_train), T_train, T_train**2])
beta, *_ = np.linalg.lstsq(X, V_train, rcond=None)
b0, b1, b2 = beta
print("V(T) coeffs:", b0, b1, b2)


t_line = np.linspace(T_train.min() - 2, T_train.max() + 2, 300)
v_line = b0 + b1 * t_line + b2 * t_line**2

plt.figure()
plt.plot(t_line, v_line, label="Quadratic fit")
plt.plot(T_train, V_train, "ko", label="Training points")
plt.xlabel("Temperature (°C)")
plt.ylabel("Output Voltage (V)")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
