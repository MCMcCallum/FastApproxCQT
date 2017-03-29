//
// Created: 03/25/17 by Matthew McCallum
//
// Header for custom conversion between pybind argument types and C++ argument types.
//



#include "FastWavelet.h"
#include "PybindArgumentConversion.h"
#include <ThirdParty/pybind/pybind11/include/pybind11/pybind11.h>
#include <ThirdParty/pybind/pybind11/include/pybind11/numpy.h>



namespace py = pybind11;
using namespace cupcake;



PYBIND11_PLUGIN(FastWavelet) {
    py::module m("FastWavelet", "C++ implementation of the fast wavelet transform");
    
    py::class_<cupcake::FastWavelet>(m, "FastWavelet")
        .def("__init__", &cupcake::py_wrapped_ctor< FastWavelet, size_t, size_t, py::array_t<float>> )
        .def("PushSamples", make_wrapped_vec_fun< FastWavelet, float, py::array_t<float>>(&FastWavelet::PushSamples));

    return m.ptr();
};