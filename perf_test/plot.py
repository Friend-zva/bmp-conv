import numpy
import matplotlib.pyplot as plt


def create_hist(data, title, path):
    plt.figure(figsize=(10, 6))
    plt.hist(data)
    plt.title(title)
    plt.savefig(path + "hist-" + title + ".png")
    plt.close()
    print(f"Histogram '{title}' created")


def create_boxplot(data, labels, title, name, path):
    plt.figure(figsize=(10, 6))
    plt.boxplot(data, tick_labels=labels, notch=False)
    plt.title(title)
    plt.grid(True)
    plt.savefig(path + "box-" + name + ".png")
    plt.close()
    print(f"Boxplot '{title}' created")


def create_bar(data, labels, title, name, path):
    means = [numpy.mean(test) for test in data]
    stds = [numpy.std(test) for test in data]

    plt.figure(figsize=(10, 6))
    plt.bar(labels, means, yerr=stds, capsize=5)
    plt.title(title)
    plt.grid(True)
    plt.savefig(path + "bar-" + name + ".png")
    plt.close()
    print(f"Bar '{title}' created")


# def create_scatter(data, labels, title, name, path):
#     plt.figure(figsize=(10, 6))
#     for i, test in enumerate(data):
#         x = numpy.random.normal(i + 1, 0.05, len(test))
#         plt.scatter(x, test, alpha=0.5, label=f"{i + 1}")
#
#     plt.xticks(range(1, len(names_test) + 1), names_test)
#     plt.title(title)
#     plt.grid(True)
#     plt.savefig(path + "scatter-" + name + ".png")
#     plt.close()
#     print(f"Scatter '{title}' created")
