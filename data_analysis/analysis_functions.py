import numpy as np
import matplotlib.pyplot as plt

def load_in_file(file_path, available_properties):
    """
    Creates a dictionary of data from a file path and array of available properties
    Will return the data dictionary as well as arrays containing the data order and all properties not available
    """
    with open(file_path) as f:
        data_order = f.readlines()[4].rstrip().split(', ')


    raw_data = np.loadtxt(file_path, dtype=float, delimiter=', ', skiprows=5, ndmin=2)

    data = {}
    not_available = []

    for i, property in enumerate(data_order):
        if property in available_properties:
            data[property] = raw_data[:,i]
        else:
            not_available.append(property)

    return data, data_order, not_available

def normalize_data(data, to_normalize):
    """
    Normalizes and returns a data dictionary when passed a data dict and an array of properties to normalize
    """
    normalized_data = data # Start with the original data to preserve non-normalized data
    
    for property in to_normalize:
        normalized_data[property] = (data[property] - np.mean(data[property])) / np.std(data[property])
    
    return normalized_data

def auto_crop(data_set, leading_indices = 0, following_indices = 0):
    """
    Automatically crops the data to the first and last change in the given data set, returning a tuple of the beginning and ending indexes
    """
    first_val = data_set[0]
    last_val = data_set[-1]

    for i, value in enumerate(data_set):
        if value != first_val and i >= leading_indices:
            first_index = i
            break

    for i, value in enumerate(data_set[::-1]):
        if value != last_val:
            last_index = len(data_set) - i
            try:
                data_set[last_index + following_indices]
                last_index += following_indices
            except IndexError:
                print("Following indeces are out of bounds")
            break
    
    return (first_index, last_index)

def create_graph(data, x_axis, y_axis, title = None, cropping = None, file_path = None):
    """
    Creates a graph of the given data, as well as however many y_axis are given
    If a file path is given, the graph will be saved to that file
    """
    f = plt.figure()
    print("Graphing " + title)
    x_data = data[x_axis]
    for y_axis_name in y_axis:
        y_data = data[y_axis_name]
        plt.plot(x_data[cropping[0]:cropping[1]], y_data[cropping[0]:cropping[1]], label=y_axis_name)
    
    plt.xlabel(x_axis)
    plt.ylabel(y_axis[0])
    plt.title(title)
    plt.legend()

    if file_path:
        f.savefig(file_path)

    plt.close(f)
    return 0

if __name__ == '__main__':
    print("This is a module, what are you doing")