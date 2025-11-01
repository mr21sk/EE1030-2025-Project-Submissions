# This code gives the coefficients for T(V)
import numpy as np
import matplotlib.pyplot as plt
import sympy as sp

A = np.loadtxt('training.txt')
T_train = A[:, 0]
V_train = A[:, 1]

X_train = np.column_stack((np.ones(A.shape[0]), V_train, V_train**2,V_train**3))

theta = np.linalg.lstsq(X_train, T_train)[0].reshape(-1, 1)
print("The value of n is:")
theta=sp.Array(theta)
sp.pprint(theta)

plt.figure()
plt.plot(V_train, X_train @ theta, label='Fitted Model')
plt.plot(V_train, T_train, 'k.', label='Training Data')
plt.grid()
plt.ylabel(r'Temperature ($^{\degree}$C)')
plt.xlabel('Output voltage (V)') 
plt.legend()
plt.tight_layout()
plt.show()
plt.close()


B = np.loadtxt('validation.txt')
T_valid = B[:, 0]
V_valid = B[:, 1]

X_valid = np.column_stack((np.ones(B.shape[0]), V_valid, V_valid**2,V_valid**3))

plt.figure()
plt.plot(V_valid, X_valid @ theta, label='Fitted Model')
plt.plot(V_valid, T_valid, 'k.', label='Validation Data')
plt.grid()
plt.ylabel(r'Temperature ($^{\degree}$C)')
plt.xlabel('Output voltage (V)') 
plt.legend()
plt.tight_layout()
plt.show()
