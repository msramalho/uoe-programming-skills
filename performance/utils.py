import subprocess, os, shutil, sys


def custom_subprocess(params, silent=False, done_message=None):
    if silent:
        return_code = subprocess.call(params, stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL)
    else:
        return_code = subprocess.call(params)

    if done_message and return_code == 0: print(done_message)
    return return_code


def make_project(silent=True):
    custom_subprocess(["make", "-C", "../code/"], silent, "Compilation successfull")