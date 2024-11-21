# ArgsParser
c++20 header-only no heap allocation progream arguments parser
# Usage
ArgsParser.cpp
```cpp
#include "ArgsParser.hpp"
#include <iostream>
int main(int argc, char **argv) {
  using namespace ArgsParser;
  FlagOption flag{"--flag1"};
  SimpleOption simple{"--simple1"};
  FlagOption *flags[]{&flag};
  SimpleOption *simples[]{&simple};

  FlagOption cflag{"--flag1"};
  SimpleOption csimple{"--simple1"};
  FlagOption *cflags[]{&cflag};
  SimpleOption *csimples[]{&csimple};

  FlagOption ccflag{"--flag1"};
  SimpleOption ccsimple{"--simple1"};
  FlagOption *ccflags[]{&ccflag};
  SimpleOption *ccsimples[]{&ccsimple};
  ComplexOption ccomplex{"--complex1", ccflags, ccsimples};
  ComplexOption *ccomplexs[]{&ccomplex};

  ComplexOption complex{"--complex1", cflags, csimples, ccomplexs};
  ComplexOption *complexs[]{&complex};

  Parser parser{flags, simples, complexs};
  parser.parse(argc, argv);
  std::cout << flag.val << " " << simple.val << " " << cflag.val << " "
            << csimple.val << " " << ccflag.val << " " << ccsimple.val << "\n";
  auto count = parser.no_key_option_count;
  std::cout << count << " ";
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
Output
```
1 55556 1 11 1 0
11 1 2 3 4 5 6 7 8 9 10 abcd
```
