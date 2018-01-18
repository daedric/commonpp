/*!
@mainpage

These pages contain the API documentation of JSON for Modern C++, a C++11
header-only JSON class.

Class @ref nlohmann::basic_json is a good entry point for the documentation.

@copyright The code is licensed under the [MIT
           License](http://opensource.org/licenses/MIT):
           <br>
           Copyright &copy; 2013-2015 Niels Lohmann.
           <br>
           Permission is hereby granted, free of charge, to any person
           obtaining a copy of this software and associated documentation files
           (the "Software"), to deal in the Software without restriction,
           including without limitation the rights to use, copy, modify, merge,
           publish, distribute, sublicense, and/or sell copies of the Software,
           and to permit persons to whom the Software is furnished to do so,
           subject to the following conditions:
           <br>
           The above copyright notice and this permission notice shall be
           included in all copies or substantial portions of the Software.
           <br>
           THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
           EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
           MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
           NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
           BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
           ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
           CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
           SOFTWARE.

@author [Niels Lohmann](http://nlohmann.me)
@see https://github.com/nlohmann/json to download the source code
*/

#include <string>

namespace commonpp
{
namespace string
{

std::string escape_json_string(const std::string& s) noexcept;

} // namespace string
} // namespace commonpp
