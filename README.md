# ArgsParser
c++23 header-only no heap allocation progream arguments parser
# Usage
ArgsParser.cpp
```cpp
#include "ArgsParser.hpp"
#include <iostream>
int main(int argc, char **argv) {
  using namespace ArgsParser;
  std::string_view flag_key[]{"--flag1", "--flag2"};
  FlagOption flag{flag_key};
  std::string_view simple_key[]{"--simple1", "--simple2"};
  SimpleOption simple{simple_key};
  FlagOption *flags[]{&flag};
  SimpleOption *simples[]{&simple};

  FlagOption cflag{flag_key};
  SimpleOption csimple{simple_key};
  FlagOption *cflags[]{&cflag};
  SimpleOption *csimples[]{&csimple};

  FlagOption ccflag{flag_key};
  SimpleOption ccsimple{simple_key};
  FlagOption *ccflags[]{&ccflag};
  SimpleOption *ccsimples[]{&ccsimple};
  std::string_view complex_key[]{"--complex1", "--complex2"};
  ComplexOption ccomplex{complex_key, ccflags, ccsimples};
  ComplexOption *ccomplexs[]{&ccomplex};

  ComplexOption complex{complex_key, cflags, csimples, ccomplexs};
  ComplexOption *complexs[]{&complex};

  Parser parser{flags, simples, complexs};
  parser.parse(argc, argv);
  std::cout << flag.val << " " << simple.val << " " << cflag.val << " "
            << csimple.val << " " << ccflag.val << " " << ccsimple.val << "\n";
  auto count = parser.no_key_option_count;
  std::cout << count << " ";
  // the count of NoKeyOption is cannot be decide compile time so that we need to use new.
  auto no_keys = new NoKeyOption *[count];
  for (int i = 0; i < count; i++)
    no_keys[i] = new NoKeyOption;
  parser.get_all_no_keys(argc, argv,
                         {no_keys, static_cast<std::size_t>(count)});
  for (int i = 0; i < count; i++)
    std::cout << no_keys[i]->val << " ";
  for (int i = 0; i < count; i++)
    delete no_keys[i];
  delete[] no_keys;
}
```
Command:
```
.\ArgsParser.exe --flag1 --simple1=55556 --complex1 --flag1 --complex1 --flag1 --simple1=0 --simple1=11 1 2 3 4 5 6 7 8 9 10 abcd
```
Output:
```
1 55556 1 11 1 0
11 1 2 3 4 5 6 7 8 9 10 abcd
```
Command:
```
.\ArgParser.exe --flag2 --simple2=55556 --complex1 --flag1 --complex2 --flag1 --simple1=0 --simple1=11 1 2 3 4 5 6 7 8 9 10 abcd
```
Output:
```
1 55556 1 11 1 0
11 1 2 3 4 5 6 7 8 9 10 abcd
```
