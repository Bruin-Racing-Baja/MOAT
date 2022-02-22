# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import sys
import numpy as np
import matplotlib.pyplot as plt


def print_graphs(path, data_selections=(), display_on_same_axes=False, manual_crop=False, crop_bounds=()):


    properties = ["status", "rpm", "act_vel", "enc_pos", "full_in", "full_out", "control_start", "control_stop",
                  "odrive_voltage", "odrive_current", "cooling_temp"]
    for selection in data_selections:
        if selection not in properties:
            print("Invalid Property to graph")
            sys.exit()
    properties_indices = {}
    j = 0
    for prop in properties:
        properties_indices[prop] = j
        j += 1

    file_data = open(path, "r")#open as a string, I'll do my own parsing
    string_data = file_data.read()
    file_data.close()

    runs_data = parse_txt(string_data)



    run_to_graph = runs_data[0]
    if len(runs_data) != 1:
        print("There are " + str(len(runs_data)) + " runs in this file. Which run would you like to graph? (int)")
        answer = input()
        if not answer.isdigit():
            print("pls type a number next time")
            sys.exit()
        run_to_graph = runs_data[int(answer)-1]

    if manual_crop:
        run_to_graph = run_to_graph[int(crop_bounds[0]): int(crop_bounds[1])]
    if display_on_same_axes:
        graph_same(data_selections, properties_indices, run_to_graph)
    else:
        graph_separate(data_selections, properties_indices, run_to_graph)

def graph_same(selections, indices, run):
    if len(selections) > 5:
        print("Can't Graph more than 5 plots")
        sys.exit()
    colors = ["tab:blue","tab:orange" , "tab:green", "gold","red","slategrey"]
    axes = {}
    fig, axes[0] = plt.subplots()
    fig.subplots_adjust(right=0.75)
    axes[0].set_xlabel("Time")
    for i in range(0, len(selections)):
        color = colors[i]
        if i != 0:
            axes[i] = axes[0].twinx()

        if i >1:
            axes[i].spines.right.set_position(('axes', 0.95+(0.1*i)))

        axes[i].plot(run[:, indices[selections[i]]], color=color, linewidth=0.5)
        axes[i].set_ylabel(selections[i], color=color)
        axes[i].ticklabel_format(axis="y", style="sci", scilimits=(0, 0))
        axes[i].tick_params(axis="y", labelcolor= color)
        # axes[i].set_title(data_selections[i] + " time series")

    plt.show()
def graph_separate(selections, indices, run):
    axes = {}
    for i in range(0, len(selections)):
        axes[i] = plt.subplot(len(selections), 1, i + 1)
        axes[i].plot(run[:, indices[selections[i]]])
        axes[i].set_xlabel("Time")
        axes[i].set_ylabel(selections[i])
        axes[i].ticklabel_format(axis="y", style="sci", scilimits=(0,0))
        # axes[i].set_title(data_selections[i] + " time series")

    plt.show()


def parse_txt(string_data):
    # parses the whole file, returns a dictionary of every run in the file
    runs = {}
    run_ctr = 0
    i = 0
    while i < len(string_data):
        if not string_data[i].isdigit():
            i = string_data.index('\n', i) + 1
        else:
            #run starts here:
            run = []
            while (i < len(string_data) and string_data[i].isdigit() == True):
                line = string_data[i:string_data.index('\n', i)]
                datapoint = line.split(", ")
                float_datapoint = [float(x) for x in datapoint]
                i = string_data.index('\n', i) + 1
                run.append(float_datapoint)
            runs[run_ctr] = run
            run_ctr += 1
    #return runs
    return auto_crop(runs)

def auto_crop(runs):
    new_runs = runs
    for key in runs:
        run = runs[key]
        run = np.array(run[1:])
        status = run[:,0]
        init_value = status[0]
        start_crop = 0
        for i in range(len(status)):
            if status[i] != init_value:
                start_crop = i
                break
        run = run[start_crop:]
        new_runs[key] = run
    return new_runs
# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    args = sys.argv
    data_series_list = []
    start_bound = 0
    end_bound = 1
    path = ""
    sp = False
    mc = False
    if len(args) < 2:
        print("Need to pass in txt file path as first argument. \n Options: \n\t- Pass in names of variables to be graphed (ex. \"print_graphs \'path\' status rpm\" to graph status and rpm wrt time)"
              "\n\t- Can use --sp to graph on same plot \n\t - Can use --mc start_crop end_crop to manually crop further")
        sys.exit()
    path = args[1]
    if len(args) == 2:
        print("Pass in properties to graph over time")
        sys.exit()
    if len(args) > 2:
        i = 2
        while i in range(2,len(args)):
            if args[i] == "--sp":
                sp = True
                i += 1
            elif args[i] == "--mc":
                mc = True
                if len(args) <= i + 2:
                    print("You gotta pass in 2 numbers as the bounds right after the --mc flag")
                    sys.exit()
                start_bound = int(args[i+1])
                end_bound = int(args[i+2])
                i += 3
            else:
                data_series_list.append(args[i])
                i += 1
    manual_crop_bounds = (start_bound, end_bound)
    data_series = tuple(data_series_list)

    print_graphs(path, data_series, display_on_same_axes=sp, manual_crop=mc, crop_bounds=manual_crop_bounds)

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
