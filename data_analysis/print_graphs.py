'''
To Use in Command Line:

There are 2 ways to graph stuff.
    -Either pass the specific paths directly to the function, in which case you can plot quantities from one file
        - i.e print_graphs.py path_to_text_file.txt rpm act_vel --sp
    -Or, use the number method, where you can pass a number in to stand in for a path (i.e pass 3 to refer to log_3.txt)
        - With this, you need to be specific with the quantities to be graphed. To graph rpm from log_1.txt and act_vel from log_5.txt,
          run print_graphs 1 5 rpm#1 act_vel#5

    Flags:
        --sp : same plot
        --nac: no auto crop
        --mc : manual crop, pass in start and end crop values
            i.e --mc 100 2000
                * if the end crop is greater than the size of the array, it just goes to the edge


'''




import sys
import numpy as np
import matplotlib.pyplot as plt


def print_graphs(path=[], data_selections=(), display_on_same_axes=False, manual_crop=False, crop_bounds=(), plots_from_diff_runs = False, disable_auto_crop=False):


    # List of properties in order that appear in the log file. Change if the txt file output is changed.
    # These are also the names to pass in as arguments
    properties = ["status", "rpm", "act_vel", "enc_pos", "full_in", "full_out", "control_start", "control_stop",
                  "odrive_voltage", "odrive_current", "cooling_temp"]


    # Check if passed arguments for data selection match with actual ones
    # Data_selections will now be a tuple of lists. Each value will be a list where the first value is the selection, and the second is the log file #
    # i.e one selection will be ("rpm", 3) to graph rpm from log #3
    for selection in data_selections:
        if selection[0] not in properties:
            print("Invalid Property to graph")
            sys.exit()
    properties_indices = {}
    j = 0
    for prop in properties: # Dict as shorthand to get index of certain property
        properties_indices[prop] = j
        j += 1




    runs_data = parse_txt(path, different_files=plots_from_diff_runs, no_auto_crop=disable_auto_crop) #return dictionary with each value as a 2D array for a run


    if plots_from_diff_runs == False:
        #Select the run to use, prompts user input
        run_to_graph = runs_data[1][0]
        if len(runs_data) != 1:
            print("There are " + str(len(runs_data)) + " runs in this file. Which run would you like to graph?")
            for i in range(1, len(runs_data)+1):
                print("Run #" + str(i) + ": P (x1000) = " + runs_data[i][1][0]+ "  This is from run #" + str(runs_data[i][1][1]))
            print("Select which # run you want to graph (int from 1 - N)")
            answer = input()
            if not answer.isdigit():
                print("pls type a number next time")
                sys.exit()
            run_to_graph = runs_data[int(answer)][0]

        # Change bounds if manual cropping is enabled
        if manual_crop:
            start_crop = int(crop_bounds[0])
            end_crop=int(crop_bounds[1])
            if int(crop_bounds[0])<0:
                start_crop = 0
            if end_crop > len(run_to_graph):
                end_crop = len(run_to_graph) - 1
            run_to_graph = run_to_graph[start_crop: end_crop]


        if display_on_same_axes:
            graph_same(data_selections, properties_indices, specific_run=run_to_graph)
        else:
            graph_separate(data_selections, properties_indices, specific_run=run_to_graph)
    else:
        print("There are " + str(len(runs_data)) + " runs in this file.")
        for i in range(1,len(runs_data) + 1):
            print("Run #" + str(i) + ": P (x1000) = " + runs_data[i][1][0] + "  This is from file #" + str(runs_data[i][1][1]))
        for i in range(len(data_selections)):
            print("Plotting " + data_selections[i][0] + " from " + str(data_selections[i][1]))
        if display_on_same_axes:
            graph_same(data_selections, properties_indices, runs=runs_data)
        else:
            graph_separate(data_selections, properties_indices, runs=runs_data)


def graph_same(selections, indices, runs=None ,specific_run=None):
    # Method to graph multiple arrays on the same set of axes

    # Limit to 6, this is arbitrary
    # To increase this, just add more colors to the list and change the way that the twin axes are shifted rightward
    if len(selections) > 6:
        print("Can't Graph more than 6 plots")
        sys.exit()
    colors = ["tab:blue","tab:orange" , "tab:green", "gold","red","slategrey"]
    axes = {}
    fig, axes[0] = plt.subplots() #Create the plot within the matplotlib window
    fig.subplots_adjust(right=0.75) #make some space for multiple axes on the right side of the plot
    axes[0].set_xlabel("Time")

    for i in range(0, len(selections)):
        run = specific_run
        if runs is not None:
            for key in runs:
                if(runs[key][1][2] == selections[i][1]):
                    run = runs[key][0]
                    break


        color = colors[i]
        if i != 0:
            #create twin y-axis for each additional quantity to be plotted
            axes[i] = axes[0].twinx()

        if i >1:
            #offset additional twin axes for readability
            axes[i].spines.right.set_position(('axes', 0.95+(0.1*i)))

        #populate each set of axes with data, format them
        axes[i].plot(run[:, indices["control_start"]], run[:, indices[selections[i][0]]], color=color, linewidth=0.5)

        if selections[i][1] == -1:
            axes[i].set_ylabel(str(selections[i][0]), color=color)

        else:
            axes[i].set_ylabel(selections[i][0] + " from #" + str(selections[i][1]) ,color=color)
        axes[i].ticklabel_format(axis="y", style="sci", scilimits=(0, 0))
        axes[i].tick_params(axis="y", labelcolor= color)
        #axes[i].set_title(data_selections[i] + " time series")

    plt.show()
def graph_separate(selections, indices, runs=None ,specific_run=None):
    axes = {}
    for i in range(0, len(selections)):
        run = specific_run
        if runs is not None:
            for key in runs:
                if (runs[key][1][2] == selections[i][1]):
                    run = runs[key][0]
                    break

        #create a set of subplots, populate each one with data and format them
        axes[i] = plt.subplot(len(selections), 1, i + 1)
        axes[i].plot(run[:, indices[selections[i][0]]], linewidth=0.9)

        if selections[i][1] == -1:
            axes[i].set_ylabel(str(selections[i][0]))
        else:
            axes[i].set_ylabel(selections[i][0] + " from #" + str(selections[i][1]))
        axes[i].set_xlabel("Time")
        axes[i].ticklabel_format(axis="y", style="sci", scilimits=(0,0))
        # axes[i].set_title(data_selections[i] + " time series")

    plt.show()


def parse_txt(paths, different_files, no_auto_crop=False):
    runs = {}
    run_num = 0
    for path in paths:
        file_data = open(path, "r")  # open as a string, I'll do my own parsing
        string_data = file_data.read()
        file_data.close()
        # parses the whole file, returns a dictionary of every run in the file
        # the return object will be a dictionary, where the keys are integers starting from 0, and the values are tuples, with the first object being the actual run data and the second value being a list of strings to use as run metadata



        i = 0
        p = '0'
        while i < len(string_data):
            if string_data[i] == 'P':
                p = string_data[string_data.index(':', i) + 1:string_data.index('\n', i)]
                i += 1
            elif not string_data[i].isdigit():       # If the first character in the line is not a number, then we can assume that line is a header
                i = string_data.index('\n', i) + 1 # Skip to the next line
            else:
                # We must be at the start of a run
                run = []
                run_metadata = []
                run_metadata.append(p)
                run_metadata.append(path)

                if different_files:

                    run_metadata.append(int(path[4:path.index('.')]))

                run_metadata.append(run_num)
                run_num += 1
                while (i < len(string_data) and string_data[i].isdigit() == True):  # Each line is a datapoint, so add each one to the run
                    line = string_data[i:string_data.index('\n', i)]
                    datapoint = line.split(", ")
                    float_datapoint = [float(x) for x in datapoint]
                    i = string_data.index('\n', i) + 1
                    run.append(float_datapoint)
                    line = ""
                runs[run_num] = (run, run_metadata)
    new_runs = {}
    if disable_auto_crop:
        for key in runs:
            run = runs[key][0]
            run = np.array(run[1:])
            new_runs[key] = (run, runs[key][1])
        return new_runs
    else:
        return auto_crop(runs)

def auto_crop(runs):

    # Auto-crop the beginning of each run to the point where the status first changes
    new_runs = runs
    for key in runs:
        run = runs[key][0]
        run = np.array(run[1:])
        status = run[:,0]
        init_value = status[0]
        fin_value = status[len(status)-1]
        start_crop = 0
        end_crop = 1
        for i in range(len(status)):
            if status[i] != init_value:
                start_crop = i + 1
                break
        for i in reversed(range(len(status)-1)):
            if status[i] != fin_value:
                end_crop = i
                break
        run = run[start_crop:end_crop]
        new_runs[key] = (run, runs[key][1])
    return new_runs


if __name__ == '__main__':
    # MAIN ROUTINE

    # For IDE testing, just replace sys.argv with an array like this ['print_graphs', ...(all the other parameters in string form) ]
    args = sys.argv
    #args = ['print_graphs.py', 'C:/Users/srini/Desktop/Baja_2021-2022/Controls_Code/MOAT/data_analysis/Data/DriveNight3-2-22main1.txt','--nac' ,'rpm', 'act_vel', 'enc_pos']
    paths_list = []
    data_series_list = []
    start_bound = 0
    end_bound = 1
    sp = False
    mc = False
    diff_files = False
    contains_regular_path = False
    disable_auto_crop=False
    # Some arg validation and error messages
    if len(args) < 2:
        print("Need to pass in txt file path. \n Options: \n\t- Pass in names of variables to be graphed (ex. \"print_graphs \'path\' status rpm\" to graph status and rpm wrt time)"
              "\n\t- Can use --sp to graph on same plot \n\t - Can use --mc start_crop end_crop to manually crop further")
        sys.exit()

    # Parsing the argument list for the data selections, flags, and other inputs
    i = 1
    while i < len(args):
        arg = args[i]
        if arg[len(arg)-4:len(arg)] == ".txt":
            paths_list.append(arg)
            contains_regular_path = True
            i += 1
        elif arg.isdigit():
            paths_list.append("log_" + arg + ".txt")
            diff_files = True
            i += 1
        elif arg == "--sp":
            sp = True
            i += 1

        elif args[i] == "--mc":
            mc = True
            if len(args) <= i + 2:
                print("You gotta pass in 2 numbers as the bounds right after the --mc flag")
                sys.exit()
            start_bound = int(args[i + 1])
            end_bound = int(args[i + 2])
            i += 3
        elif args[i] == "--nac":
            disable_auto_crop = True
            i += 1
        else:
            data_series_list.append(args[i])
            i += 1

    manual_crop_bounds = (start_bound, end_bound)

    data_series = []
    for prop in data_series_list:
        selection_num = -1
        if(prop[len(prop)-2] == '#' or prop[len(prop)-3] == '#'):
            diff_files = True

            selection_num = int(prop[prop.index('#')+1:len(prop)])
            prop = prop[:prop.index('#')]
        data_series.append([prop, selection_num])
    data_tuple = tuple(data_series)

    if len(data_series) == 0:
        print("Pass in properties to graph over time")
        sys.exit()
    if len(paths_list) > 1 and mc == True:
        mc = False
        print("Manual Cropping Disabled, can't manual crop when dealing with multiple files")

    if contains_regular_path and diff_files:
        print("You can only use the regular path or the path by numbers method, not both")
        sys.exit()

    print_graphs(paths_list, data_tuple, display_on_same_axes=sp, manual_crop=mc, crop_bounds=manual_crop_bounds, plots_from_diff_runs=diff_files, disable_auto_crop=disable_auto_crop)


