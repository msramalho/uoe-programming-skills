import subprocess, json, os, shutil
from glob import glob
import unittest


TMP_OUTPUT_DIR = "tmp_output/"
TESTS_PATTERN = "expected/*.json"


# ----------------------- SETUP functions
def create_output_folder():
    if not os.path.exists(TMP_OUTPUT_DIR):
        os.makedirs(TMP_OUTPUT_DIR)


def make_project():
    subprocess.call(["make", "-C", "../code/"])


# ----------------------- Util functions
def read_file(filename):
    with open(filename, "r") as in_file:
        return in_file.read()


def read_test(filename):
    return json.loads(read_file(filename))


def read_configs(filename):
    configs = read_test(filename)
    p = configs["params"]
    p["dat_file"] = TMP_OUTPUT_DIR + p["dat_file"]
    p["perc_file"] = TMP_OUTPUT_DIR + p["perc_file"]
    return configs


# ----------------------- Regression test functions
def execute(grid=20, seed=1564, rho=0.4, max_clusters=-1, dat_file="map.dat", perc_file="map.pgm", silent=False):
    # define the parameters to use when calling main.out
    params = ["../code/main.out", "-g %d" % grid, "-s %d" % seed, "-r %.10f" % rho, "-m %d" % max_clusters, "-d%s" % dat_file, "-p%s" % perc_file]

    # call main.out with the specified parameters
    if silent:
        with open(os.devnull, 'w') as std_null:
            return_code = subprocess.call(params, stdout=std_null, stderr=subprocess.STDOUT)
    else:
        return_code = subprocess.call(params)
    return return_code == 0  # True for proper execution


def execute_test(p, silent=False):
    # execute
    execute(p["grid"], p["seed"], p["rho"], p["max_clusters"], p["dat_file"], p["perc_file"], silent)
    # load output
    dat = read_file(p["dat_file"])
    pgm = read_file(p["perc_file"])
    return dat, pgm


# ----------------------- RegressionTests class
class RegressionTests(unittest.TestCase):
    def setUp(self):
        create_output_folder()
        make_project()

    def test_regression(self):
        # execute all the .json files in the expected folder
        for i, filename in enumerate(glob(TESTS_PATTERN)):
            configs = read_configs(filename)
            print("Regression test %2d (%s)..." % (i + 1, configs["description"])),
            # run and get results
            dat, pgm = execute_test(configs["params"], silent=True)
            # assert the correctness of the results
            self.assertEqual(dat, configs["dat"])
            self.assertEqual(pgm, configs["pgm"])
            print("Done")

    def tearDown(self):
        # shutil.rmtree(TMP_OUTPUT_DIR)
        pass


# ----------------------- main driver code
if __name__ == '__main__':
    # call unit tests with custom level of verbosity
    suite = unittest.TestLoader().loadTestsFromTestCase(RegressionTests)
    unittest.TextTestRunner(verbosity=1).run(suite)
