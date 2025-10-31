import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# -----------------------------
# Step 1. Coefficients from training (example values)
# Replace with your actual ones from training
# Suppose we had: T = a0 + a1*V + a2*V^2
a0 = 1411.268101
a1 = -1708.333692
a2 = 530.375983

# -----------------------------
# Step 2. Validation data (Voltage and actual Temperature)
V_test = np.array([1.89, 1.88, 1.85, 1.84, 1.84, 1.83, 1.83, 1.82, 1.81, 1.79])
T_actual = np.array([72.90, 71.89, 65.43, 65.22, 64.22, 63.03, 61.88, 57.50, 55.00, 53.10])

# -----------------------------
# Step 3. Predict temperature from voltage
T_pred = a0 + a1*V_test + a2*(V_test**2)

# -----------------------------
# Step 4. Create validation table
df = pd.DataFrame({
    "Voltage (V)": V_test,
    "Temperature Pred (°C)": T_pred.round(2),
    "Temperature Actual (°C)": T_actual
})

print("Validation Table:")
print(df)

# -----------------------------
# Step 5. Plot measured vs predicted data
plt.figure(figsize=(8,6))
plt.scatter(T_actual, V_test, color='red', label='Measured Data')
plt.plot(T_pred, V_test, color='black', label='Best Fit Curve')
plt.xlabel('Temperature (°C)')
plt.ylabel('Output Voltage (V)')
plt.title('Voltage vs Temperature (PT100 Sensor Calibration)')
plt.legend()
plt.grid(True)

plt.savefig("figs/Validation_Curve.png", dpi=300, bbox_inches='tight')
plt.show()
