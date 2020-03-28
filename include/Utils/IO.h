#ifndef UTILS_IO_H_
#define UTILS_IO_H_

#include <string>   // std::string
#include <sstream>  // std::ostringstream
#include <fstream>  // std::ofstream, std::ifstream
#include <iterator> // std::ostream_iterator, std::istreambuf_iterator, std::back_inserter
#include <vector>   // std::vector

#ifdef _WIN32
	#include <windows.h>  // _mkdir
#else
	#include <sys/stat.h> // mkdir
#endif

#include "../Config.h"

namespace MiniDNN
{

namespace internal
{


///
/// Convert a number to a string in C++98
///
/// \tparam NumberType     Type of the number
/// \param  num            The number to be converted
/// \return                An std::string containing the number
///
template <class NumberType>
inline std::string to_string(const NumberType& num)
{
    std::ostringstream convert;
    convert << num;
    return convert.str();
}

///
/// Create a directory
///
/// \param dir     Name of the directory to be created
/// \return        \c true if the directory is successfully created
///
inline bool create_directory(const std::string& dir)
{
#ifdef _WIN32
	return 0 == _mkdir(dir.c_str());
#else
	return 0 == mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif
}

///
/// Write an std::vector<Scalar> vector to file
///
/// \param vec          The vector to be written to file
/// \param filename     The filename of the output
///
inline void write_vector_to_file(
	const std::vector<Scalar>& vec, const std::string& filename
)
{
    std::ofstream ofs(filename.c_str(), std::ios::out | std::ios::binary);
    std::ostream_iterator<char> osi(ofs);
    const char* begin_byte = reinterpret_cast<const char*>(&vec[0]);
    const char* end_byte = begin_byte + vec.size() * sizeof(Scalar);
    std::copy(begin_byte, end_byte, osi);
}

///
/// Write the parameters of an NN model to file
///
/// \param folder       The folder where the parameter files are stored
/// \param filename     The filename prefix of the parameter files
/// \param params       The parameters of the NN model
///
inline void write_parameters(
	const std::string& folder, const std::string& filename,
    const std::vector< std::vector< Scalar> >& params
)
{
    for (int i = 0; i < params.size(); i++)
    {
        write_vector_to_file(params[i], folder + "/" + filename + to_string(i));
    }
}

///
/// Read in an std::vector<Scalar> vector from file
///
/// \param filename     The filename of the input
/// \return             The vector that has been read
///
std::vector<Scalar> read_vector_from_file(const std::string& filename)
{
    std::vector<char> buffer;
    std::ifstream ifs(filename.c_str(), std::ios::in | std::ifstream::binary);
    std::istreambuf_iterator<char> iter(ifs);
    std::istreambuf_iterator<char> end;
    std::copy(iter, end, std::back_inserter(buffer));
    std::vector<Scalar> vec(buffer.size() / sizeof(Scalar));
    std::copy(&buffer[0], &buffer[0] + buffer.size(), reinterpret_cast<char*>(&vec[0]));
    return vec;
}

///
/// Read in parameters of an NN model from file
///
/// \param folder       The folder where the parameter files are stored
/// \param filename     The filename prefix of the parameter files
/// \param nlayer       Number of layers in the NN model
/// \return             A vector of vectors that contains the NN parameters
///
std::vector< std::vector< Scalar> > read_parameters(
	const std::string& folder, const std::string& filename, int nlayer
)
{
    std::vector< std::vector< Scalar> > params;
    params.reserve(nlayer);

    for (int i = 0; i < nlayer; i++)
    {
        params.push_back(read_vector_from_file(folder + "/" + filename + to_string(i)));
    }

    return params;
}


} // namespace internal

} // namespace MiniDNN


#endif /* UTILS_IO_H_ */
