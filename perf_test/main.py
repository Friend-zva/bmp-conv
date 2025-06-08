import subprocess
import sys
from time import perf_counter
from pathlib import Path

sys.path.append(str(Path(__file__).parent.parent))

from perf_test.test import *
from perf_test.utils import *

COUNT_TESTS = 30
LIMIT_TIME = 2

path_exec = "./build/src/main"
path_dir_in = "bmps/source/"
path_dir_out = "bmps/result/"
path_save_plot = "perf_test/plots/"

factor = "def"
bias = "def"


def exec_with_timer(path_file_in, path_file_out, filter, type, mode="", count_ths=""):
    print(f"  Executing bmp-conv on {path_file_in}")
    data = []

    for _ in range(0, COUNT_TESTS):
        try:
            start = perf_counter()
            result = subprocess.run(
                [path_exec, path_file_in, path_file_out, filter, factor, bias, type, mode, count_ths],
                capture_output=True,
                text=True,
                timeout=LIMIT_TIME,
            )
            end = perf_counter()

            if result.stderr:
                print("Error:", result.stderr)
            if not result.returncode:
                data.append(end - start)
        except subprocess.TimeoutExpired:
            print("Error: timeout expired")

    return analyze_data(data)


def run_test_case_single(arr_data, labels, test_case, files_parsed, dir_to_save):
    print(f"  Executing bmp-conv on {path_dir_in}")
    data = []

    for _ in range(0, COUNT_TESTS):
        start = perf_counter()
        for name_file in files_parsed:
            try:
                result = subprocess.run(
                    [path_exec, path_dir_in + name_file, path_dir_out + name_file, test_case[0],
                     factor, bias, test_case[1], test_case[2], test_case[3]],
                    capture_output=True,
                    text=True,
                    timeout=LIMIT_TIME,
                )

                if result.stderr:
                    print("Error:", result.stderr)
            except subprocess.TimeoutExpired:
                print("Error: timeout expired")
                continue
        end = perf_counter()
        data.append(end - start)
    if not (len(data) == 0):
        output_test_case(arr_data, labels, test_case, data, dir_to_save)


def run_test_case_queue(arr_data, labels, test_case, dir_to_save):
    print(f"  Executing bmp-conv on {path_dir_in}")
    data = []

    for _ in range(0, COUNT_TESTS):
        start = perf_counter()
        try:
            result = subprocess.run(
                [path_exec, path_dir_in, path_dir_out, test_case[0],
                 factor, bias, test_case[1], test_case[2], test_case[3], test_case[4]],
                capture_output=True,
                text=True,
                timeout=LIMIT_TIME * COUNT_TESTS,
            )

            if result.stderr:
                print("Error:", result.stderr)
        except subprocess.TimeoutExpired:
            print("Error: timeout expired")
            continue
        end = perf_counter()
        data.append(end - start)
    if not (len(data) == 0):
        output_test_case(arr_data, labels, test_case, data, dir_to_save)


def main():
    files_parsed = parse_files(path_dir_in)

    index = 0
    tests = [test_bl__seq_par, test_par_row_bm__ths]

    for name_file in files_parsed:
        dir_to_save = path_save_plot + name_file[:-4] + "/"
        create_dir(dir_to_save)

        test = tests[index]
        arr_data = []
        labels = []
        print(f"Running {index + 1} test")
        for test_case in test:
            data = exec_with_timer(
                path_dir_in + name_file,
                path_dir_out + name_file,
                test_case[0],
                test_case[1],
                test_case[2],
                test_case[3],
            )
            if len(data) == 0:
                continue

            output_test_case(arr_data, labels, test_case, data, dir_to_save)

        if len(arr_data) == 0:
            continue

        output_test(arr_data, labels, test, dir_to_save)

        index += 1
        if index == len(tests):
            break

    dir_to_save = "perf_test/plots/queue/"
    create_dir(dir_to_save)

    arr_data = []
    labels = []
    print(f"Running {index + 1} test")
    test = test_bm_6_ths__par_queue
    run_test_case_single(arr_data, labels, test[0], files_parsed, dir_to_save)
    for test_case in test[1:]:
        run_test_case_queue(arr_data, labels, test_case, dir_to_save)

    output_test(arr_data, labels, test_bm_6_ths__par_queue, dir_to_save)


if __name__ == "__main__":
    main()
