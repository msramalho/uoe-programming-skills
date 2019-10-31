from main import read_file, execute_test, TMP_OUTPUT_DIR, TESTS_PATTERN, create_folder, remove_folder
import json, sys



def generate_configs():
    grid = [None, 0, 1, 5, 10, 15, 25, 50, 100]
    seed = [None, 1564, 0, 1000000, 1, 0.1]
    rho = [None, 0, 0.00001, 0.1, 0.25, 0.99999, 1]
    max_clusters = [None, -1, 0, 1, 10, 100000]
    number_of_tests = len(grid) * len(seed) * len(rho) * len(max_clusters)
    print("Generating a total of %d tests" % number_of_tests)
    yield number_of_tests
    for g in grid:
        for s in seed:
            for r in rho:
                for m in max_clusters:
                    yield {
                        "description": "(g=%s, s=%s, r=%s, m=%s)" % (g, s, r, m),
                        "params": {
                            "grid": g,
                            "seed": s,
                            "rho": r,
                            "max_clusters": m,
                            "dat_file": TMP_OUTPUT_DIR + "map.dat",
                            "perc_file": TMP_OUTPUT_DIR + "map.pgm"
                        }
                    }


def clean_json_str(filename):
    v = read_file("1.tmp").strip()
    return v[1:-1]


def generate_tests():
    configs = generate_configs()
    number_of_tests = next(configs) # the first value is the number of configs
    for i, configs in enumerate(configs):
        sys.stdout.write('\r' + "generating test %d/%d (%.2f%%)" % (i + 1, number_of_tests, 100*(i/float(number_of_tests - 1))))
        sys.stdout.flush()
        dat, pgm = execute_test(configs["params"], silent=True)
        configs["dat"] = dat
        configs["pgm"] = pgm
        p = configs["params"]
        p["dat_file"] = p["dat_file"].split("/")[-1]
        p["perc_file"] = p["perc_file"].split("/")[-1]
        with open("automated/%02d.json" % i, "w") as out:
            out.write(json.dumps(configs, indent=4))
    print()


if __name__ == '__main__':
    # create folders
    create_folder()
    create_folder(TESTS_PATTERN.split("/")[0] + "/")
    # generate regression tests
    generate_tests()
    # remove tmp folder
    remove_folder()
