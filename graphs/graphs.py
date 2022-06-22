# import pandas
from tokenize import group
import matplotlib.pyplot as plt

# from measurement import Measurement

filenames = [
    # "../data/Clockwise_algorithm/CLOCKWISE_ALGORITHM_Case_I_vectors_intv_1_0.csv",
    # "../data/Short_flood_algorithm/SHORT_FLOOD_ALGORITHM_Case_I_vectors_intv_1_0.csv",
    # "../data/Chained_hello_algorithm/CHAINED_HELLO_ALGORITHM_Case_I_vectors_intv_1_0.csv",
    #
    # "../data/Clockwise_algorithm/CLOCKWISE_ALGORITHM_Case_I_vectors_intv_2_0.csv",
    # "../data/Short_flood_algorithm/SHORT_FLOOD_ALGORITHM_Case_I_vectors_intv_2_0.csv",
    # "../data/Chained_hello_algorithm/CHAINED_HELLO_ALGORITHM_Case_I_vectors_intv_2_0.csv",
    ###############
    # "../data/Clockwise_algorithm/CLOCKWISE_ALGORITHM_Case_II_vectors_intv_1_0.csv",
    # "../data/Short_flood_algorithm/SHORT_FLOOD_ALGORITHM_Case_II_vectors_intv_1_0.csv",
    # "../data/Chained_hello_algorithm/CHAINED_HELLO_ALGORITHM_Case_II_vectors_intv_1_0.csv",
    #
    # "../data/Clockwise_algorithm/CLOCKWISE_ALGORITHM_Case_II_vectors_intv_2_0.csv",
    # "../data/Short_flood_algorithm/SHORT_FLOOD_ALGORITHM_Case_II_vectors_intv_2_0.csv",
    # "../data/Chained_hello_algorithm/CHAINED_HELLO_ALGORITHM_Case_II_vectors_intv_2_0.csv",
    ###############
    "../data/Clockwise_algorithm/CLOCKWISE_ALGORITHM_Case_III_vectors_intv_1_0.csv",
    "../data/Short_flood_algorithm/SHORT_FLOOD_ALGORITHM_Case_III_vectors_intv_1_0.csv",
    "../data/Chained_hello_algorithm/CHAINED_HELLO_ALGORITHM_Case_III_vectors_intv_1_0.csv",
    #
    # "../data/Clockwise_algorithm/CLOCKWISE_ALGORITHM_Case_III_vectors_intv_2_0.csv",
    # "../data/Short_flood_algorithm/SHORT_FLOOD_ALGORITHM_Case_III_vectors_intv_2_0.csv",
    # "../data/Chained_hello_algorithm/CHAINED_HELLO_ALGORITHM_Case_III_vectors_intv_2_0.csv",
]


def get_data_from_file(filename):
    with open(filename, 'r') as f:
        dic = {}
        for i in range(8):
            dic[i] = {}
            for j in range(2):
                dic[i][j] = {}
        for line in f:
            if "Network.node[" in line and "lnk" in line:
                measurements = line.split(",")[-2:]
                node, link = get_gate_from_name(line.split(",")[2])
                measurements = [m.strip().strip('"') for m in measurements]
                x_axis = [float(m) for m in measurements[0].split(" ")]
                y_axis = [float(m) for m in measurements[1].split(" ")]
                dic[node][link]["x_axis"] = x_axis
                dic[node][link]["y_axis"] = y_axis
        return dic


def get_gate_from_name(name):
    node = int(name.strip("Network.node[")[0])
    link = int(name[-2])
    return node, link


def graph_individual(graph_name, data):
    fig, ax = plt.subplots()
    for node in range(8):
        for gate in range(2):
            x_axis = data[node][gate]["x_axis"]
            y_axis = data[node][gate]["y_axis"]
            if (len(x_axis) > 1):
                ax.plot(x_axis, y_axis, label=f"Nodo {node} gate {gate}")

    plt.style.use("ggplot")
    plt.legend()
    plt.title(f"Buffers individuales de {formatted_label(graph_name)}")
    plt.xlabel("Tiempo [s]")
    plt.yscale("log")
    plt.ylabel("Tamaño de buffer")
    plt.show()


def graph_individual_stacked(graph_name, data):
    fig, ax = plt.subplots(4)
    for node in range(4):
        for gate in range(2):
            x_axis = data[node][gate]["x_axis"]
            y_axis = data[node][gate]["y_axis"]
            if (len(x_axis) > 1):
                ax[node].plot(x_axis, y_axis, label=f"Nodo {node} gate {gate}")
                ax[node].legend()

    plt.style.use("ggplot")
    plt.legend()
    plt.suptitle(f"Buffers individuales de {formatted_label(graph_name)}")
    plt.xlabel("Tiempo [s]")
    # plt.yscale("log")
    plt.ylabel("Tamaño de buffer")
    plt.show()

    fig, ax = plt.subplots(4)
    for node in range(4, 8):
        for gate in range(2):
            x_axis = data[node][gate]["x_axis"]
            y_axis = data[node][gate]["y_axis"]
            if (len(x_axis) > 1):
                ax[node-4].plot(x_axis, y_axis, label=f"Nodo {node} gate {gate}")
                ax[node-4].legend()

    plt.style.use("ggplot")
    plt.legend()
    plt.suptitle(f"Buffers individuales de {formatted_label(graph_name)}")
    plt.xlabel("Tiempo [s]")
    # plt.yscale("log")
    plt.ylabel("Tamaño de buffer")
    plt.show()

def get_average_buffer_data(data, group_time=2):
    UPPER_LIMIT = 200 # simulation time
    flat_vector = flatten_points(data)
    sorted_flat_vector = sorted(flat_vector)
    grouped_vector = {}
    slice = 0
    while slice < UPPER_LIMIT:
        grouped_vector[slice] = []
        slice += group_time
    for key in grouped_vector:
        for point in sorted_flat_vector:
            if(point[0] > key and point[0] < key+group_time):
                grouped_vector[key].append(point)
    x_axis = []
    y_axis = []
    for group in grouped_vector:
        x = (2*group + group_time) / 2
        aux_list = [tup[1] for tup in grouped_vector[group]]
        if (len(aux_list) > 0):
            y = sum(aux_list) / len(aux_list)
            x_axis.append(x)
            y_axis.append(y)
    return (x_axis, y_axis)


def graph_average_buffer(data_dict):
    fig, ax = plt.subplots()
    group_time = 2
    for key in data_dict:
        x_axis, y_axis = get_average_buffer_data(data_dict[key], group_time)
        ax.plot(x_axis, y_axis, label=formatted_label(key))

    plt.style.use("ggplot")
    plt.legend()
    plt.title("Tamaño de buffer promedio")
    plt.xlabel("Tiempo [s]")
    plt.ylabel("Tamaño de buffer")
    plt.show()

def flatten_points(data):
    flat_vector = []
    for node in range(8):
        for gate in range(2):
            x_axis = data[node][gate]["x_axis"]
            y_axis = data[node][gate]["y_axis"]
            if len(x_axis) < 5:
                continue
            for i in range(len(x_axis)):
                flat_vector.append((x_axis[i], y_axis[i]))
    return flat_vector


def formatted_label(label):
    aux = label.replace("vectors_", "").split("_")
    units = aux[-2]
    dec = aux[-1]
    aux = aux[:-2]
    aux.append(f"{units}.{dec}")
    return " ".join(aux).lower().title()


def main():
    data_dict = {}
    for file in filenames:
        data_dict[file.split("/")[-1].split(".")[-2]] = get_data_from_file(file)
    graph_average_buffer(data_dict)
    for data in data_dict:
        graph_individual_stacked(data, data_dict[data])


if __name__ == "__main__":
    main()
