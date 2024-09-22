#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np

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
        print(x1)
        y1 = abs(x1)

        poly = NewtonPolynomial(x1, y1)

        plt.plot(x, poly(poly, x), label= f"NewtonPolynomial (n={n})")

    plt.legend()
    plt.show()
