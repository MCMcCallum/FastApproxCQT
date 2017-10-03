//
// Created: 03/25/17 by Matthew McCallum
//
// Header for custom conversion between pybind argument types and C++ argument types.
//

// @todo [matt.mccallum 10.02.17] This wrapping of C++ argument types should probably be
//                                sitting in its own codebase as an extension to pybind11.

// In module includes.
// None.

// Third party includes.
#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"
#include "pybind11/complex.h"

// Std lib includes.
// None.

namespace py = pybind11;

namespace cupcake
{
    
//
// Type mapping
//
    
//
// Specify conversion mapping from python arguments to C++ arguments for functions.
//
template< typename T >
struct python_argument_type{ typedef T type; };
// Map std::vector<float> to py::array_t<float>, and its references.
template<>
struct python_argument_type<const std::vector<float>&>{ typedef py::array_t<float, 16> type; };


    
//
// The argument conversion functions
//
    
// The default...
template< typename arg >
typename std::remove_reference<arg>::type convert_arg( typename python_argument_type<arg>::type&& x )
///
/// Simply copies arguments to return value. This will in effect copy the argument
/// passed in, so there is some room for improvement in terms of efficiency here.
///
/// @param x
///  The argument to be converted to another type.
///
/// @return
///  The same argument to be copied as a return value.
///
{
    // @note [matt.mccallum 09.30.17] The following uses at most a move constructor, which is fine.
    //                                There is a risk of stealing xvalues due to the perfect forwarding
    //                                of an xvalue here, but that's not an issue in this wrapper.
    return std::forward<arg>( x );
}
    
// The python array to std::vector case.
template<>
const std::vector<float> convert_arg<const std::vector<float>&>( typename python_argument_type<const std::vector<float>&>::type&& x )
///
/// Converts a python array type to a C++ vector. At the moment this is only implemented
/// for 1D arrays.
///
/// @param x
///  A 1D python array to be converted to C++ type
///
/// @return
///  A vector containing the contents of the python array.
///
{
    py::buffer_info info_x = x.request();
    
    if (info_x.ndim != 1)
        throw std::runtime_error("Number of dimensions must be one");
    
    std::vector<float> ret( (float*)info_x.ptr, ( (float*)info_x.ptr ) + ( info_x.shape[0] )  );
    
    // @note [matt.mccallum 09.30.17] Return value optimization as specified in the C++ standard - 12.8 (32), states
    //                                that an lvalue here is treated as an rvalue for overload resolution.
    //                                So we really do avoid any copying on return here which is great.
    return ret;
    
}
    
    

//
// The return type conversion functions
//
// @todo [matt.mccallum 10.02.17] These conversion functions for one dimensional and multi-dimensional vector types
//                                could probably be abstracted a little further. At this stage, it's all very similar
//                                boiler plate stuff.
    
// The default...
template< typename arg >
arg convert_return( arg& x )
///
/// Function for converting C++ function return values to python types. This one simply copies
/// its arguments to return value. This will in effect copy the argument passed in, so there
/// is some room for improvement in terms of efficiency here.
///
/// @param x
///  The argument to be converted to another type.
///
/// @return
///  The same argument to be copied as a return value.
///
{
    return x; // @todo [matt.mccallum 10.01.17] This might copy the vector back to the output, not so great...
}
    
// The std::vector to py::array conversion.
py::array_t<float> convert_return( std::vector<float>& x )
///
/// Converts a vector (likely returned from a C++ function) to a python array, that may be used
/// back in python.
///
/// @param x
///  A C++ vector to be converted to an array python can understand.
///
/// @return
///  An array python can understand.
///
{
    py::array_t<float> ret( x.size(), x.data() );
    return ret; // This will not copy the object on return as specified in return value optimization as specified in the C++ standard - 12.8 (32)
}
    
// The std::vector<std::array<std::complex<float>,N>> to py::array conversion.
template< size_t ARRAY_SIZE >
py::array_t<std::complex<float>> convert_return( std::vector<std::array<std::complex<float>, ARRAY_SIZE>>& x )
///
/// Converts a two dimensional complex valued C++ data block into a two dimensional Python array.
///
/// @param x
///  The two dimensional C++ array to be converted into a python array.
///
/// @return
///  The resulting python C++ object that is interpretable by pybind11 and hence Python.
///
{
    std::vector<size_t> shape(2, 0);
    std::vector<size_t> strides(2, 0);
    shape[0] = x.size();
    shape[1] = x[0].size();
    strides[0] = (x[0].size())*sizeof( std::complex<float> );
    strides[1] = 1*sizeof( std::complex<float> );
    
    py::array_t<std::complex<float>> ret( shape,
                                         strides,
                                         x[0].data() );

    return ret; // This will not copy the object on return as specified in return value optimization as specified in the C++ standard - 12.8 (32)
}
    
// The std::vector<std::complex<float>> to py:array conversion
py::array_t<std::complex<float>> convert_return( std::vector<std::complex<float>>& x )
///
/// Converts a single dimensional complex valued C++ data block into a single dimensional Python array.
///
/// @param x
///  The single dimensional C++ array to be converted.
///
/// @return
///  The resulting python C++ object that is interpretable by pybind11 and hence Python.
///
{
    py::array_t<std::complex<float>> ret( x.size(), x.data() );
    return ret;
}
    

//
// Function converters
//
    
template< typename obj, typename... args >
void py_wrapped_ctor( obj& instance, typename python_argument_type<args>::type... a )
///
/// Creates an object to be used in pybind as a python object. This is
/// distinct from wrapped C++ methods as it has to new the object, and
/// also it takes a reference to the soon-to-be memory of the object,
/// rather than a pointer to a pre-existing object (as with function
/// methods).
/// It also must choose between the many possible constructors of a class
/// and so the `args` to the constructor should usually be provided explicitly
/// in the template.
///
/// @param instance
///  The soon-to-be memory location of the new object.
///
/// @param a
///  The arguments passed into the new object upon construction.
///
{
    new (&instance) obj( convert_arg<args>( std::forward<typename python_argument_type<args>::type>( a ) )... );
}

template< typename... args, typename obj, typename ret >
auto py_wrapped_func( ret (obj::*f)( args... ) )
///
/// A factory function for creating function pointers that wrap up class methods and automatically
/// convert all function arguments to/from python types, before and after calling the C++ function.
///
/// @param f
///  The function pointer to be wrapped up.
///
/// @return
///  The wrapped function pointer.
///
{
    return [f]( obj* o, typename python_argument_type<args>::type&... a )
    {
        auto x = (o->*f)( convert_arg<args>( std::forward<typename python_argument_type<args>::type>( a ) )... );
        auto y = convert_return( x );
        return y;
    };
}
    
} // namespace cupcake

