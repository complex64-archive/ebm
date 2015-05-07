#include "erl_nif.h"

static int boyer_moore(unsigned char *needle_data, int needle_size,
		       unsigned char *haystack_data, int haystack_size,
		       unsigned int start)
{
	/* TODO: Implement the Boyer-Moore algorithm. */
	return 0;
}

static ERL_NIF_TERM search(ErlNifEnv * env, int argc,
			   const ERL_NIF_TERM argv[])
{
	ErlNifBinary needle;
	ErlNifBinary haystack;
	unsigned int start;

	if (!enif_inspect_binary(env, argv[0], &needle)
	    || !enif_inspect_binary(env, argv[1], &haystack)
	    || !enif_get_uint(env, argv[2], &start))
		return enif_make_badarg(env);

	return enif_make_int(env,
			     boyer_moore(needle.data, needle.size,
					 haystack.data,
					 haystack.size, start));
}

static ErlNifFunc niftable[] = { {"search", 3, search} };

ERL_NIF_INIT(ebm, niftable, NULL, NULL, NULL, NULL);
