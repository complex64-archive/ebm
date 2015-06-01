-module (ebm).
-export ([search/2, search/3, search_all/2]).
-on_load(load_shared_obj/0).


-spec search(binary(), binary()) -> integer().
-spec search(binary(), binary(), pos_integer()) -> integer().
-spec search_all(binary(), binary()) -> [integer()].


% Search for the first occurence of Pattern in String.
search(Pattern, String) ->
    search(Pattern, String, 1).

% Search for all occurences of Pattern in String.
search_all(Pattern, String) ->
    search(Pattern, String, -1).

% Search for the first occurence of Pattern in a section of String.
% StartIndex is inclusive.
search(_Pattern, _String, _NumSearches) ->
    erlang:nif_error({nif_not_loaded, ?MODULE}).


load_shared_obj() ->
    EbinDir = filename:dirname(code:which(?MODULE)),
    AppPath = filename:dirname(EbinDir),
    Path = filename:join([AppPath, "priv", "ebm"]),
    ok = erlang:load_nif(Path, 0).
