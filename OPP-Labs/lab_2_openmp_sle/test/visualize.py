import numpy as np
import matplotlib.pyplot as plt
import sys

N = 50

vecXFilename = sys.argv[1] if len(sys.argv) > 1 else "vecX.bin"

x = np.fromfile(vecXFilename, dtype=np.float32).reshape((N, N))
b = np.fromfile("vecB.bin", dtype=np.float32).reshape((N, N))

a = np.fromfile("matA.bin", dtype=np.float32).reshape((N*N, N*N))

fig, (ax1, ax2) = plt.subplots(ncols=2)

ax1.imshow(x)
ax1.set_title(f'Vector X ({vecXFilename})')

ax2.imshow(-b)
ax2.set_title('Vector B')

plt.show()