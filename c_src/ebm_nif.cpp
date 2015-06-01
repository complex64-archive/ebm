#include "erl_nif.h"
#include <algorithm>
#include <iostream>
#include <cstring>

namespace ebm {

/**
 * Linked list of Erlang integers.
 */
struct ErlIntList {
public:
  ErlIntList(ErlNifEnv* env)
      : env_(env), head_(enif_make_list(env, 0))
  {
  }

  void prepend(int num)
  {
    head_ = enif_make_list_cell(env_, enif_make_int(env_, num), head_);
  }

  ERL_NIF_TERM head()
  {
    return head_;
  }

private:
  ErlNifEnv* env_;
  ERL_NIF_TERM head_;
};

constexpr int alphabetSize()
{
  return 256;
}

void precomputeBadCharacterTable(unsigned const char pattern[],
                                 int patternLength,
                                 int badCharacterTable[])
{
  for (int i = 0; i < alphabetSize(); ++i)
    badCharacterTable[i] = patternLength;

  for (int i = 0; i < patternLength - 1; ++i)
    badCharacterTable[static_cast<int>(pattern[i])] = patternLength - i - 1;
}

void performTunedBoyerMoore(unsigned const char *pattern,
                            int patternLength,
                            unsigned char *string,
                            int stringLength,
                            unsigned int nMaxResults,
                            ErlIntList& resultList)
{
  // Preprocessing
  int badCharacterTable[alphabetSize()];
  precomputeBadCharacterTable(pattern, patternLength, badCharacterTable);

  int lastByte = pattern[patternLength - 1];
  int shift = badCharacterTable[lastByte];
  badCharacterTable[lastByte] = 0;

  memset(string + stringLength, lastByte, patternLength);

  // Searching
  int k = 0;
  int j = 0;
  while (j < stringLength) {
    k = badCharacterTable[string[j + patternLength - 1]];
    while (k != 0) {
      j += k;
      k = badCharacterTable[string[j + patternLength - 1]];
      j += k;
      k = badCharacterTable[string[j + patternLength - 1]];
      j += k;
      k = badCharacterTable[string[j + patternLength - 1]];
    }

    if (std::memcmp(pattern, string + j, patternLength - 1) == 0 && j
        < stringLength)
      resultList.prepend(j);
    j += shift;
  }
}

} // namespace ebm

#ifdef __cplusplus
extern "C" {
#endif

//static ErlNifResourceType *ebm_nif_table;

/**
 * NIF implementation of search/3.
 */
static ERL_NIF_TERM search(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  ErlNifBinary pat;
  if (!enif_inspect_binary(env, argv[0], &pat))
    return enif_make_atom(env, "bad_pattern");

  ErlNifBinary str;
  if (!enif_inspect_binary(env, argv[1], &str))
    return enif_make_atom(env, "bad_string");

  unsigned int nMaxResults;
  if (!enif_get_uint(env, argv[2], &nMaxResults))
    return enif_make_atom(env, "bad_num_max_results");

  ebm::ErlIntList resultList(env);
  if (str.size >= pat.size && pat.size > 0)
    ebm::performTunedBoyerMoore(pat.data, pat.size, str.data, str.size,
                                nMaxResults, resultList);
  return resultList.head();
}

static ErlNifFunc niftable[] = { { "search", 3, search } };
ERL_NIF_INIT(ebm, niftable, NULL, NULL, NULL, NULL)

#ifdef __cplusplus
}
#endif
