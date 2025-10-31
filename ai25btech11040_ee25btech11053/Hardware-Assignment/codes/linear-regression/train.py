import numpy as np
import matplotlib.pyplot as plt

A = np.loadtxt('./training.csv', delimiter=',')
A = A[A[:,0].argsort()]
X = np.hstack((np.ones((A.shape[0],1)),A[:,[1]],A[:,[1]]**2))
T = A[:,[0]]
C = A[:,[1]]

#Least squares method
a, b, c = map(lambda x: x[0], np.linalg.lstsq(X, T, rcond=None)[0])
lsq = np.array([
    [a],
    [b],
    [c]
])
print(f"Coefficients: {a}, {b}, {c}")

#Plot both the results
plt.plot(C, X@lsq)
plt.plot(C, T, 'k.')
plt.grid()
plt.xlabel('Voltage (V)')
plt.ylabel('Temperature ($^{\\circ}$C)')
# plt.show()
plt.savefig('../../figs/training.png')

#Close current figure(s)
plt.close('all')

#Plot for validation
# B = np.loadtxt('validation.csv', delimiter=',')
# Xv = np.hstack((np.ones((B.shape[0],1)),B[:,[0]],B[:,[0]]**2))
# Cv = B[:,[1]]
# Tv = B[:,[0]]
# plt.plot(Tv, Xv@lsq)
# plt.plot(Tv, Cv, 'k.')
# plt.ylabel('Output Voltage (V)')
# plt.xlabel('Temperature ($^{\\circ}$C)')
# plt.grid()
# plt.savefig('figs/validation.png')