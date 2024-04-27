import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.patches import Rectangle
from matplotlib.animation import PillowWriter
import os
# Definește numele fișierului video
video_filename = 'visualisation.mp4'

def read_bmd_info(filename):
    flt_bmd_info = []
    unique_keys = set()  # Folosim un set pentru a păstra cheile unice

    with open(filename, 'r') as file:
        lines = file.readlines()
        flt_data = {}
        index = 0
        for line in lines:
            parts = line.split(':')
            if len(parts) == 2:
                key = parts[0].strip()
                value = parts[1].strip()
                if key == 'Adresa FLT':
                    flt_addr = value
                    flt_data['Adresa FLT'] = flt_addr
                elif key == 'Adresa BMD':
                    bmd_addr = value
                    flt_data['Adresa BMD'] = bmd_addr
                elif key == 'Size':
                    flt_data['Size'] = value
                elif key == 'Num total':
                    flt_data['Num total'] = value
            elif line.strip() == 'Vectorul:':
                continue  # Sari peste linia 'Vecorul:'
            elif line.strip() == '':  # Verifică dacă linia este goală
                continue  # Sari peste linia goală
            else:
                vec = list(map(int, line.strip().split()))
                flt_bmd_key = (flt_addr, bmd_addr, flt_data['Size'], flt_data['Num total'])
                unique_keys.add(flt_bmd_key)  # Adăugăm cheia la setul de chei unice
                flt_bmd_info.append((index, vec, flt_bmd_key))
                index += 1
    unique_keys = sorted(unique_keys, key=lambda x: x[1], reverse=True)
    return flt_bmd_info, unique_keys


def create_subplot(ax, num_total, grid_width=10):
    grid_height = (num_total + grid_width - 1) // grid_width  # Calculăm înălțimea grid-ului
    squares = []
    for j in range(grid_height):
        for i in range(grid_width):
            index = j * grid_width + i  # Calculăm indexul elementului
            if index < num_total:  # Verificăm dacă suntem în interiorul numelui total de elemente
                square = Rectangle((i, j), 0.9, 0.9, edgecolor='black', facecolor='gray')
                ax.add_patch(square)
                squares.append(square)
    ax.set_xlim(0, grid_width)
    ax.set_ylim(0, grid_height)
    ax.invert_yaxis()  # Invertăm axa y pentru a începe de sus
    return squares


def update_subplot(squares, vec, prev_vec):
    if prev_vec is None:
        for square, val in zip(squares, vec):
            color = 'grey' if val == 0 else 'red' if val == 1 else 'green'
            square.set_facecolor(color)
    else:
        for square, val, prev_val in zip(squares, vec, prev_vec):
            if val != prev_val:  # Verifică dacă valoarea s-a modificat
                color = 'grey' if val == 0 else 'red' if val == 1 else 'green'
                square.set_facecolor(color)


def main():
    folder_path = './../cmake-build-debug/'
    filename = os.path.join(folder_path, 'bmd_info.txt')

    flt_info, unique_keys = read_bmd_info(filename)

    num_plots = len(unique_keys)
    fig, axs = plt.subplots(num_plots, figsize=(5,  2*num_plots))

    subplot_mapping = {}
    for i, key in enumerate(unique_keys):
        subplot_mapping[key] = axs[i]

    for i, key in enumerate(unique_keys):
        flt_addr, bmd_addr, size, num_total = key
        ax = axs[i]
        ax.set_title(f"FLT Addr: {flt_addr}, BMD Addr: {bmd_addr}, Size: {size}, Num Total: {num_total}")
        ax.set_xticks([])
        ax.set_yticks([])
        ax.squares = create_subplot(ax, int(num_total))
        subplot_mapping[key] = ax

    prev_vectors = {key: None for key in unique_keys}
    def update(frame):
        index, vector, key = flt_info[frame]
        ax = subplot_mapping[key]
        update_subplot(ax.squares, vector, prev_vectors[key])
        fig.canvas.draw()
        prev_vectors[key] = vector

    anim = FuncAnimation(fig, update, frames=len(flt_info), cache_frame_data=True)
    plt.show()
    # anim.save(video_filename, writer=PillowWriter(fps=2))


if __name__ == "__main__":
    main()
