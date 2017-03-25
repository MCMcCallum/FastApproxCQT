//
// Created: 03/25/17 by Matthew McCallum
//
// Header for custom conversion between pybind argument types and C++ argument types.
//



#include <ThirdParty/pybind/pybind11/include/pybind11/pybind11.h>
#include <ThirdParty/pybind/pybind11/include/pybind11/numpy.h>



namespace py = pybind11;



// Specify conversion mapping from python arguments to C++ arguments to functions.
// - This is distinct from the argument conversion types, because sometimes we want
//   references, rather than values.
template< typename T >
struct cpp_argument_type{ typedef T type; };
template<>
struct cpp_argument_type<py::array_t<double>>{ typedef const std::vector<double>& type; };



// Specify conversion between python types and c++ types. Each of these will require a custom
// conversion function.
// - A specialised type for the py::array_t type
template< typename T >
struct argument_conversion_return{ typedef T type; };
template< >
struct argument_conversion_return<py::array_t<double>>{ typedef std::vector<double> type; };


//
// The argument conversion functions
//
template< typename arg >
typename argument_conversion_return<arg>::type convert_to_vector( arg& x )
///
/// Simpy copies arguments to return value. This will in effect copy the argument
/// passed in, so there is some room for improvement in terms of efficiency here.
///
/// @param x
///  The argument to be converted to another type.
///
/// @return
///  The same argument to be copied as a return value.
///
{
    return x;
}

template< >
std::vector<double> convert_to_vector<py::array_t<double>>( py::array_t<double>& x )
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
    
    std::vector<double> ret( (double*)info_x.ptr, ( (double*)info_x.ptr ) + ( info_x.shape[0]-1 )  );
    
    return ret; // This is going to copy the vector back to the output, not so great...
    
}


//
// Return a function with the captured object and internal function as arguments...
// The object instance is passed by the pybind library as first argument...
//
template< typename obj, typename ret, typename... args >
auto wrapped_cpp_func(  obj* o, ret (obj::*f)( typename cpp_argument_type<args>::type... ), args... a )
///
/// A function that simple calls a class method on an instance, converting all arguments from python
/// types to C++ types.
///
/// @param o
///  Pointer to the class instance for method to be called on.
///
/// @param f
///  Reference to class method to be called.
///
/// @param a
///  Arguments of the method f as python types.
///
/// @return
///  The returned value of the class method.
///
{
    return (o->*f)( convert_to_vector(a)... );
}



template< typename obj, typename ret, typename... args >
auto make_wrapped_vec_fun( ret (obj::*f)( typename cpp_argument_type<args>::type... ) )
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
        return wrapped_vec_func( o, f, a...);
    };
}

