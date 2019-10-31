from main import read_file, execute_test, TMP_OUTPUT_DIR
import json

def generate_configs():
    grid = [0, 1, 2, 5, 10, 15, 25, 50, 100]
    seed = [1564, 0, 1000000, 1, 0.1, 1.5]
    rho = [0, 0.00001, 0.1, 0.25, 0.5, 0.99999, 1]
    max_clusters = [-1, 0, 1, 10, 100000]
    print("Generating a total of %d tests" % (len(grid) * len(seed) * len(rho) * len(max_clusters)))
    for g in grid:
        for s in seed:
            for r in rho:
                for m in max_clusters:
                    yield {
                        "description": "(g=%d, s=%d, r=%.4f, m=%d)" % (g, s, r, m),
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
    for i, configs in enumerate(generate_configs()):
        print("test %d" % i)
        dat, pgm = execute_test(configs["params"], silent=True)
        configs["dat"] = dat
        configs["pgm"] = pgm
        p = configs["params"]
        p["dat_file"] = p["dat_file"].split("/")[-1]
        p["perc_file"] = p["perc_file"].split("/")[-1]
        with open("automated/%02d.json" % i, "w") as out:
            out.write(json.dumps(configs, indent=4))

if __name__ == '__main__':
    generate_tests()
