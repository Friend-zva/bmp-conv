import numpy
from scipy import stats

from perf_test.plot import *

test_bl__seq_par = [
    ["bl", "seq", "", ""],
    ["bl", "par", "row", "4"],
    ["bl", "par", "column", "4"],
    ["bl", "par", "pixel", "4"],
]

test_par_row_bm__ths = [
    ["bm", "par", "row", "1"],
    ["bm", "par", "row", "2"],
    ["bm", "par", "row", "4"],
    ["bm", "par", "row", "8"],
    ["bm", "par", "row", "16"],
]

test_bm_6_ths__par_queue = [
    ["bm", "par", "row", "6"],
    ["bm", "queue", "1", "2", "3"],
    ["bm", "queue", "1", "3", "2"],
    ["bm", "queue", "2", "2", "2"],
    ["bm", "queue", "2", "3", "1"],
    ["bm", "queue", "2", "1", "3"],
    ["bm", "queue", "3", "1", "2"],
    ["bm", "queue", "3", "2", "1"],
]


def analyze_data(data):
    if len(data) == 0:
        return []

    _, p_1 = stats.normaltest(data)
    _, p_2 = stats.shapiro(data)
    if p_1 <= 0.05 and p_2 <= 0.05:
        print(f"  Data doesn't match the normal distribution: {p_1:.4f}, {p_2:.4f}")

    mean = numpy.mean(data)
    std = numpy.std(data, ddof=1)
    if std >= mean * 0.1:
        print(f"  The standard deviation is too large: {mean:.4f} ± {std:.4f}")

    return data


def output_test_case(arr_data, labels, test_case, data, dir_to_save):
    title = "-".join([s for s in test_case if s != ""])
    create_hist(data, title, dir_to_save)
    arr_data.append(data)
    label = "-".join([s for s in test_case[1:] if s != ""])
    labels.append(label)


def output_test(arr_data, labels, test, dir_to_save):
    title = test[0][0]
    create_boxplot(arr_data, labels, title, dir_to_save)
    create_bar(arr_data, labels, title, dir_to_save)
