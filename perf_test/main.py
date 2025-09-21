import subprocess
import sys
from pathlib import Path

sys.path.append(str(Path(__file__).parent.parent))

from perf_test.tests import *
from perf_test.utils import *

COUNT_TESTS = 30
LIMIT_TIME = 2

path_exec = "./build/src/main"
path_dir_in = "bmps/source/"
path_dir_out = "bmps/result/"
path_save_plot = "perf_test/plots/"

factor = "def"
bias = "def"


def exec(path_file_in, path_file_out, filter, type, mode="", count_ths=""):
    print(f"  Executing bmp-conv on {path_file_in}")
    data = []

    for _ in range(0, COUNT_TESTS):
        try:
            result = subprocess.run(
                [
                    path_exec,
                    path_file_in,
                    path_file_out,
                    filter,
                    factor,
                    bias,
                    type,
                    mode,
                    count_ths,
                ],
                capture_output=True,
                text=True,
                timeout=LIMIT_TIME,
            )

            # if result.stderr:
            # print("Output:\n", result.stderr, sep="")
            if not result.returncode:
                data.append(get_time(result.stderr))
        except subprocess.TimeoutExpired:
            print("Error: timeout expired")

    return analyze_data(data)


def run_test_case_single(arr_data, labels, test_case, files_parsed, dir_to_save):
    print(f"  Executing bmp-conv on {path_dir_in}")
    data = []

    for _ in range(0, COUNT_TESTS):
        time = 0.0
        for name_file in files_parsed:
            try:
                result = subprocess.run(
                    [
                        path_exec,
                        path_dir_in + name_file,
                        path_dir_out + name_file,
                        test_case[0],
                        factor,
                        bias,
                        test_case[1],
                        test_case[2],
                        test_case[3],
                    ],
                    capture_output=True,
                    text=True,
                    timeout=LIMIT_TIME,
                )

                time += get_time(result.stderr)
                # if result.stderr:
                # print("Output:\n", result.stderr, sep="")
            except subprocess.TimeoutExpired:
                print("Error: timeout expired")
                continue
        data.append(time)

    if not (len(data) == 0):
        output_test_case(arr_data, labels, test_case, data, dir_to_save)


def run_test_case_queue(arr_data, labels, test_case, dir_to_save):
    print(f"  Executing bmp-conv on {path_dir_in}")
    data = []

    for _ in range(0, COUNT_TESTS):
        try:
            result = subprocess.run(
                [
                    path_exec,
                    path_dir_in,
                    path_dir_out,
                    test_case[0],
                    factor,
                    bias,
                    test_case[1],
                    test_case[2],
                    test_case[3],
                    test_case[4],
                ],
                capture_output=True,
                text=True,
                timeout=LIMIT_TIME * COUNT_TESTS,
            )

            # if result.stderr:
            # print("Output:\n", result.stderr, sep="")
        except subprocess.TimeoutExpired:
            print("Error: timeout expired")
            continue
        data.append(get_time(result.stderr))

    if not (len(data) == 0):
        output_test_case(arr_data, labels, test_case, data, dir_to_save)


def main():
    files_parsed = parse_files(path_dir_in)

    index = 0
    tests = [test_bl__seq_par, test_par_row_bm__ths]
    for test in tests:
        dir_to_save_dir = path_save_plot + test[0] + "/"

        for name_file in files_parsed:
            dir_to_save_files = dir_to_save_dir + name_file[:-4] + "/"
            create_dir(dir_to_save_files)

            arr_data = []
            labels = []
            print(f"Running {index + 1} test")
            for test_case in test[1]:
                data = exec(
                    path_dir_in + name_file,
                    path_dir_out + name_file,
                    test_case[0],
                    test_case[1],
                    test_case[2],
                    test_case[3],
                )
                if len(data) == 0:
                    continue

                output_test_case(arr_data, labels, test_case, data, dir_to_save_files)

            if len(arr_data) == 0:
                continue

            output_test(
                arr_data,
                labels,
                name_file[:-4] + f" ({test_case[0]})",
                test_case[0],
                dir_to_save_files,
            )

            index += 1

    dir_to_save = "perf_test/plots/queue/"
    create_dir(dir_to_save)
    arr_data = []
    labels = []

    print(f"Running {index + 1} test")
    title = test_bm_6_ths__par_queue[0]
    test = test_bm_6_ths__par_queue[1]

    run_test_case_single(arr_data, labels, test[0], files_parsed, dir_to_save)
    for test_case in test[1:]:
        run_test_case_queue(arr_data, labels, test_case, dir_to_save)

    output_test(
        arr_data,
        labels,
        title + f" ({test[0][0]})",
        test[0][0],
        dir_to_save,
    )


if __name__ == "__main__":
    main()
