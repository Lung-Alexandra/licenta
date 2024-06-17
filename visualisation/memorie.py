import matplotlib.pyplot as plt

# Listele pentru stocarea datelor din fișier
x_values = []
y_values = []

# Deschidem fișierul și citim datele
file_path = '../cmake-build-debug/memorie4.txt'
with open(file_path, 'r') as file:
    for line in file:
        parts = line.split()
        if len(parts) == 2:
            x_values.append(int(parts[0]))
            y_values.append(int(parts[1]))
ix = range(1, len(x_values) + 1)
iy = range(1, len(y_values) + 1)
print(max(x_values))
plt.plot(ix, x_values, label='x_values', color='red')

plt.plot(iy, y_values, label='y_values', color='blue')
plt.title('Memoria utilizată în timpul unui test având 329 obiecte mici')
plt.ylabel("Bytes")
plt.xlabel("Operații")
plt.legend(["dimensiunea obiectelor alocate (din aceași clasa și FLT)", "memoria utilizată de alocator"], loc="upper left")
plt.grid(True)
plt.show()
