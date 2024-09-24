#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np

class CubicSpline:
    def __init__(self, x : list[float], y : list[float]):
        n = len(x)
        self.__x = x
        self.__y = y

        h = [0] + [(x[i] - x[i-1]) for i in range(1, n)]

        a = [h[i] for i in range(1, n-1)]
        b = [2 * (h[i] + h[i+1]) for i in range(1, n-1)]
        c = [h[i+1] for i in range(1, n-1)]

        f = [6 * ((y[i+1] - y[i]) / h[i+1] - (y[i] - y[i-1]) / h[i]) for i in range(1, n-1)]

        self.c_coefs = [0] + solve_tridiagonal_sle(a, b, c, f) + [0]
        self.d_coefs = [0] + [(self.c_coefs[i] - self.c_coefs[i-1]) / h[i] for i in range(1, n)]
        self.b_coefs = [0] + [None] * (n-1)
        for i in range(1, n):
            self.b_coefs[i] = h[i] * self.c_coefs[i] / 2
            self.b_coefs[i] -= (h[i] ** 2) * self.d_coefs[i] / 6
            self.b_coefs[i] += (y[i] - y[i-1]) / h[i]
            
        self.a_coefs = y

    @np.vectorize
    def __call__(self, x : float):
        # find interval to which x belongs
        i = np.searchsorted(self.__x, [x], side='left')[0]

        return self.a_coefs[i] + self.b_coefs[i] * (x - self.__x[i]) + self.c_coefs[i] / 2 * (x - self.__x[i]) ** 2 + self.d_coefs[i] / 6 * (x - self.__x[i]) ** 3


class NewtonPolynomial:
    def __init__(self, x : list[float], y : list[float]):
        self.__divided_diffs = []
        for i in range(len(x)):
            self.__divided_diffs += [divided_difference(x[0:i+1], y[0:i+1])]

        self.__x = x
        self.__n = len(x)

    @np.vectorize
    def __call__(self, x : float) -> float:
        ret = 0
        for i in range(self.__n):
            p = np.prod([x - self.__x[j] for j in range(i)])
            ret += self.__divided_diffs[i] * p;

        return ret


# Input: SLE in form a[i]y[i-1] + b[i]y[i] + c[i]y[i+1] = f[i]
def solve_tridiagonal_sle(a, b, c, f):
    n = len(a)
    alpha = [-c[0] / b[0]] + [None] * (n - 1)
    beta = [f[0] / b[0]] + [None] * (n - 1)
    for i in range(1, n):
        y_i = b[i] + a[i] * alpha[i-1]
        alpha[i] = -c[i] / y_i
        beta[i] = (f[i] - a[i] * beta[i-1]) / y_i

    x = [None] * (n-1) + [beta[n-1]]
    for i in range(n-2, -1, -1):
        x[i] = alpha[i] * x[i+1] + beta[i]

    return x

def divided_difference(x : list[float], y : list[float]) -> float:
    ret = 0
    for i in range(0, len(x)):
        p = 1
        for j in range(0, len(x)):
            if i != j:
                p *= (x[i] - x[j])
        ret += y[i] / p

    return ret


if __name__ == "__main__":
    x = np.linspace(-1, 1, 200)  # Sample data.

    plt.xlabel('x label')
    plt.ylabel('y label')
    plt.title("Approximation")
    plt.xlim(-1, 1)
    plt.ylim(-0.5, 1.5)

    # Plot original function
    plt.plot(x, abs(x), label='y = |x|')

    # Plot Newton polynomials
    for n in [11, 22, 44]:
        x1 = np.linspace(-1, 1, n)
        y1 = abs(x1)

        poly = NewtonPolynomial(x1, y1)
        plt.plot(x, poly(poly, x), label= f"NewtonPolynomial (n={n})")

        spline = CubicSpline(x1, y1)
        plt.plot(x, spline(spline, x), label= f"CubicSpline (n={n})")

    plt.legend()
    plt.show()
