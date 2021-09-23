#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

py::bytes fast_decrypt(std::string &file, std::string &key) {
  std::string result;
  result.resize(file.size());
  int key_length = key.size();
  for (int i = 0; i < file.size(); ++i)
    result[i] = file[i] ^ key[i % key_length];
  return py::bytes(result);
}

PYBIND11_MODULE(core, m) {
  m.doc() = "CORE C++ implementation";
  m.def("fast_decrypt", &fast_decrypt, "fast xor decryption");
}
