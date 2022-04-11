# The point of this script is to automatically intake files from a certain directory, and process them to create graphs and other data
# analysis tools very quickly. Once it starts running, any files placed in the target directory are processed and the results
# are saved in a different folder in the directory

import os

# Settings
target_directory = 'Data_Hopper'
run_continuously = True

# Main Loop
while run_continuously:
    available_files = os.listdir(target_directory)
    available_files.remove('Output') # Ignore output file
    current_file = available_files[0] # Select first file in list, doesn't matter the order
    print('Processing file: ' + current_file)
    
