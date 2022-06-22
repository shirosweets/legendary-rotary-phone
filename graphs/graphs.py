# import pandas
from tokenize import group
import matplotlib.pyplot as plt

# from measurement import Measurement

filenames = [
    "../data/Clockwise_algorithm/CLOCKWISE_ALGORITHM_Case_I_vectors_intv_1_0.csv",
    "../data/Short_flood_algorithm/SHORT_FLOOD_ALGORITHM_Case_I_vectors_intv_1_0.csv",
    "../data/Chained_hello_algorithm/CHAINED_HELLO_ALGORITHM_Case_I_vectors_intv_1_0.csv",
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


def main():
    data_list = []
    for file in filenames:
        data_list.append(get_data_from_file(file))
    graph_average_buffer(data_list)


def graph_individual(data):
    fig, ax = plt.subplots()
    for node in range(8):
        for gate in range(2):
            x_axis = data[node][gate]["x_axis"]
            y_axis = data[node][gate]["y_axis"]
            if (len(x_axis) > 1):
                ax.plot(x_axis, y_axis, label=f"Nodo {node} gate {gate}")

    plt.style.use("ggplot")
    plt.legend()
    plt.title("Buffers individuales")
    plt.xlabel("Tiempo [s]")
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
    print(grouped_vector)
    for key in grouped_vector:
        for point in sorted_flat_vector:
            if(point[0] > key and point[0] < key+group_time):
                grouped_vector[key].append(point)
    # for group in grouped_vector:
    #     print(f"[{group}-{group+group_time}] :: {grouped_vector[group]}")
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


def graph_average_buffer(data_list):
    fig, ax = plt.subplots()
    group_time = 2
    for data in data_list:
        x_axis, y_axis = get_average_buffer_data(data, group_time)
        ax.plot(x_axis, y_axis, label=f"unu")

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

# def graph(
#     measurements,
#     x_tag,
#     y_tag,
#     x_label,
#     y_label,
#     graph_title,
#     xlambda=None,
#     ylambda=None
# ):
#     axis = get_axis_data(
#         [m.to_dictionary() for m in measurements],
#         x_tag,
#         y_tag
#     )
#     fig, ax = plt.subplots()
#     for case in axis:
#         print(f"y points for case {case}: {axis[case]['y_points']}")
#         x_points = axis[case]["x_points"]
#         y_points = axis[case]["y_points"]

#         if (xlambda is not None):
#             x_points = list(map(xlambda, x_points))

#         if (ylambda is not None):
#             y_points = list(map(ylambda, y_points))

#         ax.plot(x_points, y_points, label=case)
#     plt.style.use("ggplot")
#     plt.legend()
#     plt.title(graph_title)
#     plt.xlabel(x_label)
#     plt.ylabel(y_label)
#     plt.show()


if __name__ == "__main__":
    main()

# #######

# def get_axis_data(data: dict, x_label: str, y_label: str):
#     cases = set()
#     for m_dict in data:
#         cases.add(m_dict[Measurement.CASE_TAG])
#     axis = {}
#     case_tag = Measurement.CASE_TAG
#     for case in cases:
#         axis[case] = {}
#         x_points = [m[x_label] for m in data if m[case_tag] == case]
#         y_points = [m[y_label] for m in data if m[case_tag] == case]
#         axis[case]["x_points"] = x_points
#         axis[case]["y_points"] = y_points
#     return axis


# def graph(
#     measurements,
#     x_tag,
#     y_tag,
#     x_label,
#     y_label,
#     graph_title,
#     xlambda=None,
#     ylambda=None
# ):
#     axis = get_axis_data(
#         [m.to_dictionary() for m in measurements],
#         x_tag,
#         y_tag
#     )
#     fig, ax = plt.subplots()
#     for case in axis:
#         print(f"y points for case {case}: {axis[case]['y_points']}")
#         x_points = axis[case]["x_points"]
#         y_points = axis[case]["y_points"]

#         if (xlambda is not None):
#             x_points = list(map(xlambda, x_points))

#         if (ylambda is not None):
#             y_points = list(map(ylambda, y_points))

#         ax.plot(x_points, y_points, label=case)
#     plt.style.use("ggplot")
#     plt.legend()
#     plt.title(graph_title)
#     plt.xlabel(x_label)
#     plt.ylabel(y_label)
#     plt.show()


# def graph_measurements(measurements: list[Measurement]):
#     graph_useful_load_curve(measurements)
#     graph_delay_curve(measurements)
#     graph_network_overload(measurements)
#     graph_ack(measurements)
#     graph_ret(measurements)
#     graph_rtt(measurements)


# def graph_useful_load_curve(measurements: list[Measurement]):
#     x_tag = Measurement.GENRATE_TAG
#     y_tag = Measurement.RECRATE_TAG
#     x_label = "Carga ofrecida [pkt/seg]"
#     y_label = "Carga útil [pkt/seg]"
#     title = "Carga Útil vs Ofrecida"
#     graph(measurements, x_tag, y_tag, x_label, y_label, title)


# def graph_network_overload(measurements):
#     x_tag = Measurement.GENRATE_TAG
#     y_tag = Measurement.TOTALDROP_TAG
#     x_label = "Paquetes generados [pkt/seg]"
#     y_label = "Pérdida de paquetes"
#     title = "Índice de pérdida de paquetes"
#     graph(measurements, x_tag, y_tag, x_label, y_label, title)


# def graph_delay_curve(measurements: list[Measurement]):
#     x_tag = Measurement.GENRATE_TAG
#     y_tag = Measurement.AVDEL_TAG
#     x_label = "Carga ofrecida [pkt/seg]"
#     y_label = "Retraso promedio [s]"
#     title = "Retraso"
#     graph(measurements, x_tag, y_tag, x_label, y_label, title)


# def graph_ret(measurements: list[Measurement]):
#     x_tag = Measurement.GENRATE_TAG
#     y_tag = Measurement.RET_TAG
#     x_label = "Carga ofrecida [pkt/seg]"
#     y_label = "Cantidad de retransmisiones"
#     title = "Retransmisiones"
#     graph(measurements, x_tag, y_tag, x_label, y_label, title)


# def graph_rtt(measurements: list[Measurement]):
#     x_tag = Measurement.GENRATE_TAG
#     y_tag = Measurement.RTT_TAG
#     x_label = "Carga ofrecida [pkt/seg]"
#     y_label = "Tiempo RTT promedio"
#     title = "RTT"
#     graph(measurements, x_tag, y_tag, x_label, y_label, title)


# def graph_ack(measurements: list[Measurement]):
#     x_tag = Measurement.GENRATE_TAG
#     y_tag = Measurement.ACKT_TAG
#     x_label = "Carga ofrecida [pkt/seg]"
#     y_label = "Tiempo de ACK promedio"
#     title = "ACK Time"
#     graph(measurements, x_tag, y_tag, x_label, y_label, title)
