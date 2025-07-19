# Ziegler–Nichols PID Tuning Formula

# Hypothetical values – adjust based on actual motor behavior
Ku = 2.5  # Ultimate gain (where the system starts to oscillate)
Pu = 0.8  # Ultimate period in seconds

# Classic Ziegler–Nichols PID values
Kp = 0.6 * Ku
Ki = 2 * Kp / Pu
Kd = Kp * Pu / 8

print(f"Kp: {Kp:.3f}, Ki: {Ki:.3f}, Kd: {Kd:.3f}")
