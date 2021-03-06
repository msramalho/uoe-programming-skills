import subprocess, json, os, shutil
from glob import glob
import unittest


TMP_OUTPUT_DIR = "tmp_output/"
TESTS_PATTERN = "automated/*.json"


# ----------------------- SETUP functions
def create_folder(folder=TMP_OUTPUT_DIR):
    if not os.path.exists(folder):
        os.makedirs(folder)

def remove_folder(folder=TMP_OUTPUT_DIR):
    shutil.rmtree(folder)

def make_project():
    custom_subprocess(["make", "-C", "../code/"], True, "Compilation successfull")


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


def custom_subprocess(params, silent=False, done_message=None):
    if silent:
        with open(os.devnull, 'w') as std_null:
            return_code = subprocess.call(params, stdout=std_null, stderr=subprocess.STDOUT)
    else:
        return_code = subprocess.call(params)

    if done_message and return_code == 0: print(done_message)
    return return_code


# ----------------------- Regression test functions
def execute(grid=20, seed=1564, rho=0.4, max_clusters=-1, dat_file="map.dat", perc_file="map.pgm", silent=False):
    # define the parameters to use when calling main.out
    params = ["../code/main.out"]
    if grid != None: params.append("-g %d" % grid)
    if seed != None: params.append("-s %d" % seed)
    if rho != None: params.append("-r %.10f" % rho)
    if max_clusters != None: params.append("-m %d" % max_clusters)
    if dat_file != None: params.append("-d%s" % dat_file)
    if perc_file != None: params.append("-p%s" % perc_file)
    return custom_subprocess(params, silent) == 0


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
        create_folder()
        make_project()

    def test_automated(self):
        # execute all the .json files in the expected folder
        for i, filename in enumerate(glob(TESTS_PATTERN)):
            configs = read_configs(filename)
            print("Regression test %2d <%s>..." % (i + 1, configs["description"])),
            # run and get results
            dat, pgm = execute_test(configs["params"], silent=True)
            # assert the correctness of the results
            self.assertEqual(dat, configs["dat"])
            self.assertEqual(pgm, configs["pgm"])
            print("Done")

    def tearDown(self):
        remove_folder()


# ----------------------- main driver code
if __name__ == '__main__':
    # call unit tests with custom level of verbosity
    suite = unittest.TestLoader().loadTestsFromTestCase(RegressionTests)
    unittest.TextTestRunner(verbosity=1).run(suite)
