# Sample observed and calculated temperature values
observed = [66.3, 50.1, 74.9, 86.0, 37.6]
calculated = [67.4, 49.7, 74.9, 84.5, 35.1]

# Lists to store errors
absolute_error = []
percentage_error = []

for o, c in zip(observed, calculated):
    abs_err = abs(o - c)
    percent_err = (abs_err / o) * 100

    absolute_error.append(abs_err)
    percentage_error.append(percent_err)

# Print results
print("Observed | Calculated | Absolute Error | Percentage Error (%)")
for o, c, ae, pe in zip(observed, calculated, absolute_error, percentage_error):
    print(f"{o:.2f}      {c:.3f}        {ae:.3f}           {pe:.2f}%")
