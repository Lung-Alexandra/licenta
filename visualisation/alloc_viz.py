import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.patches import Rectangle
import os


def read_bmd_info(filename):
    flt_bmd_info = []
    unique_keys = set()

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
                continue
            elif line.strip() == '':
                continue
            else:
                vec = list(map(int, line.strip().split()))
                flt_bmd_key = (flt_addr, bmd_addr, flt_data['Size'], flt_data['Num total'])
                unique_keys.add(flt_bmd_key)
                flt_bmd_info.append((index, vec, flt_bmd_key))
                index += 1
    unique_keys = sorted(unique_keys, key=lambda x: x[1], reverse=True)

    last_positions = {}
    unique_modifications = []
    for i in unique_keys:
        last_positions[i] = [0] * (int(i[3]))

    for index, vec, flt_bmd_key in flt_bmd_info:
        last_position = last_positions[flt_bmd_key]
        if vec != last_position:
            index_of_difference = 0
            for i in range(len(vec)):
                if vec[i] != last_position[i]:
                    index_of_difference = i
                    break
            unique_modifications.append((index, index_of_difference, vec[index_of_difference], flt_bmd_key))
            last_positions[flt_bmd_key] = vec
    unique_modifications = sorted(unique_modifications, key=lambda x: x[0])
    mod = []
    j = 0
    for index, ind, val, flt_bmd_key in unique_modifications:
        mod.append((j, ind, val, flt_bmd_key))
        j += 1
    return unique_modifications, unique_keys


def create_subplot(ax, num_total, grid_width=10):
    grid_height = (num_total + grid_width - 1) // grid_width
    squares = []
    for j in range(grid_height):
        for i in range(grid_width):
            index = j * grid_width + i
            if index < num_total:
                square = Rectangle((i, j), 0.9, 0.9, edgecolor='black', facecolor='gray')
                ax.add_patch(square)
                square.set_facecolor("grey")
                squares.append(square)
    ax.set_xlim(0, grid_width)
    ax.set_ylim(0, grid_height)
    ax.invert_yaxis()
    return squares


def update_subplot(squares, index, val):
    color = 'red' if val == 1 else 'green'
    squares[index].set_facecolor(color)


def main():
    folder_path = './../cmake-build-debug/'
    filename = os.path.join(folder_path, 'bmd_info.txt')

    flt_info, unique_keys = read_bmd_info(filename)

    num_plots = len(unique_keys)
    fig, axs = plt.subplots(num_plots, figsize=(5, 2 * num_plots))

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

    def update(frame):
        index, index_of_update, val, key = flt_info[frame]
        ax = subplot_mapping[key]
        update_subplot(ax.squares, index_of_update, val)
        fig.canvas.draw()

    anim = FuncAnimation(fig, update, frames=len(flt_info), cache_frame_data=True)
    # plt.show()
    anim.save('viz.gif')


if __name__ == "__main__":
    main()
