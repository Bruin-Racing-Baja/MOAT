# The point of this script is to automatically intake files from a certain directory, and process them to create graphs and other data
# analysis tools very quickly. Once it starts running, any files placed in the target directory are processed and the results
# are saved in a different folder in the directory

from json import load
import os
import numpy as np
import matplotlib.pyplot as plt
from analysis_functions import *

test_status = [2, 4, 4, 4, 4, 4, 4, 4, 4, 5, 8, 4, 7, 9, 4, 4, 4, 4, 4, 4, 4]

# Outputs
graphs = [
    ("s_time", "rpm"), 
    ("s_time", "enc_pos"), 
    ("s_time", "status"),
    ("s_time", "o_curr"),
    ("s_time", "o_vol"),
    ("s_time", "couple"),
    ("s_time", "therm1"),
    ("s_time", "therm2"),
    ("s_time", "therm3"),
    ("s_time", "rpm", "enc_pos"), 
    ("s_time", "rpm", "act_vel"), 
    ("s_time", "o_curr", "act_vel"),
    ("s_time", "enc_pos", "in_trig", "out_trig"),
    ("s_time", "estop", "enc_pos"),
    ]

# Property Management
properties = [
"status", 
"rpm", 
"act_vel", 
"enc_pos", 
"in_trig", 
"out_trig", 
"s_time", 
"f_time", 
"o_vol", 
"o_curr",
"couple",
"therm1",
"therm2",
"therm3",
"estop",
"wheel_count",
"wheel_rpm",
]

properties_to_normalize = ["rpm", "act_vel", "enc_pos", "o_vol", "o_curr", "couple", "therm1", "therm2", "therm3"]

# Settings
target_directory = 'Data_Hopper'
run_continuously = True

leading_indices = 40
following_indices = 40

# Startup



# Main Loop
while run_continuously:
    # Get target file from target directory
    available_files = os.listdir(target_directory)
    available_files.remove('Output') # Ignore output file
    current_file = available_files[0] # Select first file in list, doesn't matter the order
    
    #Intake file
    print('Processing file: ' + current_file)
    data, data_order, not_recognized = load_in_file(target_directory + '/' + current_file, properties)
    print(data)
    if not_recognized:
        print('Not recognized: ' + str(not_recognized))

    normalized_data = normalize_data(data, properties_to_normalize)

    cropping = auto_crop(data['status'], leading_indices, following_indices)

    # Create output directory
    output_dir = target_directory + '/' + current_file + '_output'
    while os.path.exists(output_dir):
        output_dir += '_new'
    os.makedirs(output_dir)

    # Create and output all graphs
    for graph in graphs:
        create_graph(data = normalized_data, x_axis = graph[0], y_axis = graph[1:], title = graph[0] + ' vs ' + graph[1], cropping = cropping, file_path = output_dir + '/' + graph[0] + '_vs_' + graph[1] + '.png')

    # Save file as JSON in output and delete from target
    
    
    

