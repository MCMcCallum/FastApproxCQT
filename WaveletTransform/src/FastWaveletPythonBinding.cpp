//
// Created: 03/25/17 by Matthew McCallum
//
// Header for custom conversion between pybind argument types and C++ argument types.
//



#include "FastWavelet.h"
#include "PybindArgumentConversion.h"
#include "pybind/pybind11.h"
#include "pybind/numpy.h"



namespace py = pybind11;
using namespace cupcake;



PYBIND11_PLUGIN(FastWavelet) {
    py::module m("FastWavelet", "C++ implementation of the fast wavelet transform");
    
    py::class_<cupcake::FastWavelet>(m, "FastWavelet")
        .def( "__init__", &py_wrapped_ctor< FastWavelet, float, const std::vector<float>& > )
        .def( "PushSamples", py_wrapped_func( &FastWavelet::PushSamples ) )
        .def( "GetWindow", py_wrapped_func( &FastWavelet::GetWindow ) );

    return m.ptr();
};