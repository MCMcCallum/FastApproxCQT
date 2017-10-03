FastApproxCQT
=============

A C++ implementation of a transform that is kind of like the CQT except super fast.

Written by Matt C. McCallum.

Dependencies
------------

This code depends on:
 * [GYP](https://gyp.gsrc.io/)
 * [IPP](https://software.intel.com/en-us/intel-ipp/details)
 * [VecLib](https://github.com/MCMcCallum/VecLib)
 * [PyBind11](https://github.com/MCMcCallum/pybind11)

Help with installing IPP can be found [here](https://software.intel.com/en-us/intel-ipp/details).

Install
-------

First you'll need to install IPP.

If you're on OSX the rest of the procedure is simple:
 * `git clone https://github.com/MCMcCallum/FastApproxCQT`
 * `git submodule init`
 * `run ./pull_thirdparty_osx.sh`

Unfortunately no scripts have been written for other operating systems yet. Yet to come...

If you wish to build the C++ project and run tests or build the library via XCode or something:
 * `gyp --depth ./FastApproxCQT.gyp`

If you want to just build and install the Python module (only tested on system installed python 2.7 on OSX)
 * `python setup.py build`
 * `python setup.py install`

Demo
----

There is a simple demo for which you can view the results of the C++ module and an equivalent Python implementation.

Simply run `python Demo.py`.