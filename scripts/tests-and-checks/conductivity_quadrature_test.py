import numpy as np
from scipy.integrate import quad
import matplotlib.pyplot as plt

f_visc = lambda T: 2.4e-5*(10**(248/(T - 140)))
f_perm = lambda z: 1e-12*np.exp(z/2e3)
T_init = lambda z: 300.0 - 0.005*z
rho_w = 1000.0
g = 3.72

f_K = lambda z: g*rho_w*f_perm(z)/f_visc(T_init(z))

print(quad(f_K, -500, -0))
