//
// Created: 03/25/17 by Matthew McCallum
//
// Header for custom conversion between pybind argument types and C++ argument types.
//



#include "pybind/pybind11.h"
#include "pybind/numpy.h"
#include <iostream>



namespace py = pybind11;



namespace cupcake
{

    
    
//
// Type mapping
//
    
//
// Specify conversion mapping from python arguments/return values to C++ arguments/return values for functions.
//
// The usual default case.
template< typename T >
struct cpp_argument_type{ typedef T type; };
// Map std::vector<float> to py::array_t<float>, and its references.
template<>
struct cpp_argument_type<py::array_t<float, 16>>{ typedef const std::vector<float>& type; };

//
// Specify mapping from C++ arguments to Python arguments for return values.
//
// The usual default case.
template< typename T >
struct python_return_type{ typedef T type; };
// Map std::vector<float> to py::array_t<float>.
template<>
struct python_return_type<std::vector<float>>{ typedef py::array_t<float> type; };


    
//
// The argument conversion functions
//
    
template< typename arg >
typename std::remove_reference<typename cpp_argument_type<arg>::type>::type convert_arg( arg&& x )
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

template<>
const std::vector<float> convert_arg<py::array_t<float>>( py::array_t<float>&& x )
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
    
    std::vector<float> ret( (float*)info_x.ptr, ( (float*)info_x.ptr ) + ( info_x.shape[0]-1 )  );
    
    // @note [matt.mccallum 09.30.17] Return value optimization as specified in the C++ standard - 12.8 (32), states
    //                                that an lvalue here is treated as an rvalue for overload resolution.
    //                                So we really do avoid any copying on return here which is great.
    return ret;
    
}
    
    

//
// The return type conversion functions
//
    
template< typename arg >
typename python_return_type<arg>::type convert_return( arg& x )
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
    std::cout << "convert return...\n";
    return x;
}

template<>
py::array_t<float> convert_return<std::vector<float>>( std::vector<float>& x )
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
    std::cout << "convert return v...\n";
    py::array_t<float> ret( x.size(), x.data() );
    return ret; // This is going to copy the vector back to the output, not so great...
}
    
    

//
// Function converters
//
    
template< typename obj, typename... args >
void py_wrapped_ctor( obj& instance, args... a )
///
/// Creates an object to be used in pybind as a python object. This is
/// distinct from wrapped C++ methods as it has to new the object, and
/// also it takes a reference to the soon-to-be memory of the object,
/// rather than a pointer to a pre-existing object (as with function
/// methods).
///
/// @param instance
///  The soon-to-be memory location of the new object.
///
/// @param a
///  The arguments passed into the new object upon construction.
///
{
    new (&instance) obj( convert_arg( std::forward<args>( a ) )... );
}

template< typename... args, typename obj, typename ret >
auto py_wrapped_func( ret (obj::*f)( typename cpp_argument_type<args>::type... ) )
///
/// A factory function for creating function pointers that wrap up class methods and automatically
/// convert all function arguments before calling the C++ function.
///
/// @param f
///  The function pointer to be wrapped up.
///
/// @return
///  The wrapped function pointer.
///
{
    return [f]( obj* o, args... a )
    {
        ret x = (o->*f)( convert_arg( std::forward<args>( a ) )... );
        typename python_return_type<ret>::type y = convert_return( x );
        return y;
    };
}

    
    
} // namespace cupcake

