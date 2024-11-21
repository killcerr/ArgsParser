#pragma once
#include <cassert>
#include <span>
#include <stdexcept>
#include <string_view>
#include <utility>
namespace ArgsParser {

namespace details {
inline void check_key(std::string_view key) {
  assert(key.starts_with('-') && "key is not starts with '-'.");
}
} // namespace details

struct OptionRange {
  int begin = -1, end = -1;
};

struct FlagOption {
  std::string_view key;
  bool val;
  OptionRange range;
  int parse(int argc, char **argv, int index) {
    assert(argv[index] == key && "key mismatch.");
    val = true;
    range = {index, index + 1};
    return index + 1;
  }
  bool is(int argc, char **argv, int index) const { return argv[index] == key; }
  FlagOption(std::string_view key, bool def = false) : key(key), val(def) {
    details::check_key(key);
  }
};

struct SimpleOption {
  std::string_view key;
  std::string_view val;
  OptionRange range;
  int parse(int argc, char **argv, int index) {
    assert(std::string_view{argv[index]}.starts_with(key) && "key mismatch.");
    if (argv[index] == key) {
      if (index + 1 < argc)
        val = argv[index + 1];
      else
        throw std::length_error("value is not set.");
      range = {index, index + 2};
      return index + 2;
    } else {
      val = argv[index];
      if (auto pos = val.find('='); pos + 1 < val.size()) {
        val = val.substr(pos + 1);
        range = {index, index + 1};
        return index + 1;
      } else {
        throw std::length_error("value is not set.");
      }
    }
  }
  bool is(int argc, char **argv, int index) const {
    return argv[index] == key ||
           strlen(argv[index]) >= key.size() &&
               std::string_view{argv[index], key.size()} == key &&
               argv[index][key.size()] == '=';
  }
  SimpleOption(std::string_view key, std::string_view def = "")
      : key(key), val(def) {
    details::check_key(key);
  }
};
struct ComplexOption {
  std::string_view key;
  std::span<FlagOption *> flags;
  std::span<SimpleOption *> simples;
  std::span<ComplexOption *> complexs;
  OptionRange range;
  int parse(int argc, char **argv, int index) {
    assert(std::string_view{argv[index]}.starts_with(key) && "key mismatch.");
    range.begin = index;
    index++;
    int current_flag_index = 0;
    int current_simple_index = 0;
    int current_complex_index = 0;
    while (index < argc) {
      bool continue_flag = false;
      for (auto i = current_complex_index; i < complexs.size(); i++) {
        if (complexs[i]->is(argc, argv, index)) {
          index = complexs[i]->parse(argc, argv, index);
          if (current_complex_index != i)
            std::swap(complexs[current_complex_index], complexs[i]);
          current_complex_index++;
          continue_flag = true;
          break;
        }
      }
      if (continue_flag)
        continue;
      for (auto i = current_simple_index; i < simples.size(); i++) {
        if (simples[i]->is(argc, argv, index)) {
          index = simples[i]->parse(argc, argv, index);
          if (current_simple_index != i)
            std::swap(simples[current_simple_index], simples[i]);
          current_simple_index++;
          continue_flag = true;
          break;
        }
      }
      if (continue_flag)
        continue;
      for (auto i = current_flag_index; i < flags.size(); i++) {
        if (flags[i]->is(argc, argv, index)) {
          index = flags[i]->parse(argc, argv, index);
          if (current_flag_index != i)
            std::swap(flags[current_flag_index], flags[i]);
          current_flag_index++;
          continue_flag = true;
          break;
        }
      }
      if (continue_flag)
        continue;
      break;
    }
    range.end = index;
    return index;
  }
  bool is(int argc, char **argv, int index) const { return argv[index] == key; }
  ComplexOption(std::string_view key, std::span<FlagOption *> flags = {},
                std::span<SimpleOption *> simples = {},
                std::span<ComplexOption *> complexs = {})
      : key(key), flags(flags), simples(simples), complexs(complexs) {
    details::check_key(key);
  }
};
struct NoKeyOption {
  std::string_view val;
  int parse(int argc, char **argv, int index) {
    val = argv[index];
    return index + 1;
  }
  bool is(int argc, char **argv, int index) const { return true; }
};
struct Parser {
  std::span<FlagOption *> flags;
  std::span<SimpleOption *> simples;
  std::span<ComplexOption *> complexs;
  int no_key_option_count = -1;
  int parse(int argc, char **argv) {
    int index = 1;
    int current_flag_index = 0;
    int current_simple_index = 0;
    int current_complex_index = 0;
    int no_key_option_count = 0;
    while (index < argc) {
      bool continue_flag = false;
      for (auto i = current_complex_index; i < complexs.size(); i++) {
        if (complexs[i]->is(argc, argv, index)) {
          index = complexs[i]->parse(argc, argv, index);
          if (current_complex_index != i)
            std::swap(complexs[current_complex_index], complexs[i]);
          current_complex_index++;
          continue_flag = true;
          break;
        }
      }
      if (continue_flag)
        continue;
      for (auto i = current_simple_index; i < simples.size(); i++) {
        if (simples[i]->is(argc, argv, index)) {
          index = simples[i]->parse(argc, argv, index);
          if (current_simple_index != i)
            std::swap(simples[current_simple_index], simples[i]);
          current_simple_index++;
          continue_flag = true;
          break;
        }
      }
      if (continue_flag)
        continue;
      for (auto i = current_flag_index; i < flags.size(); i++) {
        if (flags[i]->is(argc, argv, index)) {
          index = flags[i]->parse(argc, argv, index);
          if (current_flag_index != i)
            std::swap(flags[current_flag_index], flags[i]);
          current_flag_index++;
          continue_flag = true;
          break;
        }
      }
      if (continue_flag)
        continue;
      no_key_option_count++;
      index++;
    }
    this->no_key_option_count = no_key_option_count;
    return no_key_option_count;
  }
  struct NoKeyContext {
    int current_flag_index = 0;
    int current_simple_index = 0;
    int current_complex_index = 0;
  };
  bool is_no_key(int argc, char **argv, int index, NoKeyContext &context) {
    auto &[flag, simple, complex] = context;
    while (flag < flags.size() && flags[flag]->range.end <= index) {
      flag++;
    }
    while (simple < simples.size() && simples[simple]->range.end <= index) {
      simple++;
    }
    while (complex < complexs.size() && complexs[complex]->range.end <= index) {
      complex++;
    }
    if (flag < flags.size() && flags[flag]->range.begin <= index &&
        index < flags[flag]->range.end)
      return false;
    if (simple < simples.size() && simples[simple]->range.begin <= index &&
        index < simples[simple]->range.end)
      return false;
    if (complex < complexs.size() && complexs[complex]->range.begin <= index &&
        index < complexs[complex]->range.end)
      return false;
    return true;
  }
  void get_all_no_keys(int argc, char **argv,
                       std::span<NoKeyOption *> no_keys) {
    if (no_key_option_count == -1)
      throw std::runtime_error("please call parse first.");
    if (no_keys.size() < no_key_option_count)
      throw std::length_error("no_keys's length is too short.");
    int index = 1;
    int no_key_index = 0;
    NoKeyContext context;
    while (index < argc) {
      if (is_no_key(argc, argv, index, context)) {
        index = no_keys[no_key_index++]->parse(argc, argv, index);
      } else
        index++;
    }
  }
};
} // namespace ArgsParser