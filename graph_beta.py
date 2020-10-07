import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import matplotlib


df = pd.read_excel('result_tests.xlsx', 'Лист1', index_col=None, na_values=['NA'])

x = df['time'][1:]
y1 = df['count elements'][1:]
y2 = df['Unnamed: 2'][1:]

y1_max = max(y1)
y2_max = max(y2)
y_max = max(y1_max, y2_max)

y1_absolut_error = y1_max * 0.05
y2_absolut_error = y2_max * 0.05

# exit()

fig = plt.figure()
ax = fig.add_subplot(111)

ax.plot(x, y1, c='b', marker="o", alpha=0.5, label="1000 elems - test #1")
ax.plot(x, y2, c='r', marker="o", alpha=0.5, label="100000 elems - test #2")
y1_err = np.linspace(y1_absolut_error, y1_absolut_error, len(x))
y2_err = np.linspace(y2_absolut_error, y2_absolut_error, len(x))
plt.errorbar(x, y1, yerr=y1_err, color='b', label='confidence lim #1')
plt.errorbar(x, y2, yerr=y2_err, c='r', label='confidence lim #2')

ax.grid()
ax.set_title('Dependence of number threads\non execution time sorting array\nby merge method')
plt.ylim(-100, y_max * 1.25 + 100)
plt.xlabel("Counts")
plt.ylabel("Times (ms)")
plt.legend()

plt.savefig('test1.png')
plt.show()