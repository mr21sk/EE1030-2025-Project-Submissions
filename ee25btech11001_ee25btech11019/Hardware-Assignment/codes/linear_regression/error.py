import numpy as np
import matplotlib.pyplot as plt

A = np.loadtxt('../linear_regression/training_data.txt')
X = np.hstack((np.ones((A.shape[0],1)), A[:,[0]], A[:,[0]]**2))
T = A[:,[0]]
C = A[:,[1]]

n_lsq = np.linalg.lstsq(X, C, rcond=None)[0].reshape(-1, 1)

C_pred = X @ n_lsq

error_train = C - C_pred
mse_train = np.mean(error_train**2)
print(f"Training Quadratic Loss (MSE): {mse_train:.8e}")

plt.figure(figsize=(7,5))
plt.scatter(T, error_train, color='red', label='Error points')
plt.plot(T, error_train, color='blue', linewidth=1, label='Error curve')
plt.axhline(0, color='black', linestyle='--')
plt.xlabel('Temperature (°C)')
plt.ylabel('Error (V_actual - V_pred)')
plt.title(f'Training Data Error (MSE = {mse_train:.6f})')
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig('../../figs/train_error.png', dpi=300)
plt.close()

B = np.loadtxt('../linear_regression/validation_data.txt')
Xv = np.hstack((np.ones((B.shape[0],1)), B[:,[0]], B[:,[0]]**2))
Tv = B[:,[0]]
Cv = B[:,[1]]

Cv_pred = Xv @ n_lsq
error_valid = Cv - Cv_pred
mse_valid = np.mean(error_valid**2)
print(f"Validation Quadratic Loss (MSE): {mse_valid:.8e}")

plt.figure(figsize=(7,5))
plt.scatter(Tv, error_valid, color='green', label='Error points')
plt.plot(Tv, error_valid, color='orange', linewidth=1, label='Error curve')
plt.axhline(0, color='black', linestyle='--')
plt.xlabel('Temperature (°C)')
plt.ylabel('Error (V_actual - V_pred)')
plt.title(f'Validation Data Error (MSE = {mse_valid:.6f})')
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig('../../figs/valid_error.png', dpi=300)
plt.close()

