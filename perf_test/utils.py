import os


def create_dir(path_dir):
    if not os.path.exists(path_dir):
        os.makedirs(path_dir)
        print(f"Directory '{path_dir}' created")
    else:
        print(f"Directory '{path_dir}' already exists")


def parse_files(path_dir):
    files_parsed = []

    for root, _, files in os.walk(path_dir):
        for file in files:
            if not file.endswith(".bmp"):
                continue
            path_abs = os.path.join(root, file)
            path_rel = os.path.relpath(path_abs, start=path_dir)
            files_parsed.append(path_rel)

    return files_parsed


def get_time(string):
    if string:
        line_time = string.strip().split("\n")[-1]
        time = line_time.split()[1]
        return float(time[0:-1])
    return 0.0
