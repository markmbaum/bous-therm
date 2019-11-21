import numpy as np
from numpy import sin, cos, exp, pi
from scipy.integrate import quad
import matplotlib.pyplot as plt

plt.rc('font', family='serif')
#plt.rc('font', size=14)
plt.rc('text', usetex=True)

def fd_grad(y, dx):
    return((y[0]/12 - 2*y[1]/3 + 2*y[3]/3 - y[4]/12)/dx)

def test(x, dx):
    xg = np.array([x - 2*dx, x - dx, x, x + dx, x + 2*dx])
    zt = f_topo(xg)
    h = f_h(xg)
    d = zt - h
    K = f_K(d)
    Kint = [f_Kint(i) for i in d]

    r1 = fd_grad(Kint,dx)
    r2 = (fd_grad(zt,dx) - fd_grad(h,dx))*(-K[2])

    return(np.abs(r1 - r2), np.abs((r1 - r2)/r1))

#domain bottom
za = -4e3
#domain top
zb = 0
#lower x domain boundary
xa = -1e6
#higher x domain boundary
xb = -xa

#topography
f_topo = lambda x: -2.5e3*cos(2*pi*(x - (xb + xa)/2)/(xb - xa))**2
#depth of water table
f_h = lambda x: (f_topo(x) - 1e3) + 1e2*sin((abs(x)/4e4)**1.5)

#conductivty function with depth
f_K = lambda d: 1e-1*(exp(-d/1e3) + 0.1*sin(((d+za)/5e2)**2) + 1)
#integrated conductivty
f_Kint = lambda d: quad(f_K, d, zb - za)[0]

N = 750
abserr = np.empty((N,))
relerr = abserr.copy()
xg = np.linspace(xa, xb, N)
for i,x in enumerate(xg):
    abserr[i], relerr[i] = test(x, 1e0)

ax1 = plt.subplot2grid((3,3), (0,0), rowspan=3)
ax2 = plt.subplot2grid((3,3), (0,1), colspan=2)
ax3 = plt.subplot2grid((3,3), (1,1), colspan=2)
ax4 = plt.subplot2grid((3,3), (2,1), colspan=2)

d = np.linspace(0, zb - za, 1000)
ax1.invert_yaxis()
ax1.plot(f_K(d), d, 'k')
ax1.set_ylabel('Depth')
ax1.set_xlabel("$K(d)$")

ax2.plot(xg, f_topo(xg), label='$z_t$')
ax2.plot(xg, f_h(xg), label='$h$')
ax2.plot(xg, f_topo(xg) - (zb - za), label='$z_b$')
ax2.set_ylabel("$z$")
ax2.legend()

ax3.semilogy(xg, abserr, 'C3.', markersize=2)
ax3.set_ylabel("Abs Err")

ax4.semilogy(xg, relerr, 'C4.', markersize=2)
ax4.set_ylabel("Rel Err")
ax4.set_xlabel('$x$')

plt.tight_layout()
plt.show()
