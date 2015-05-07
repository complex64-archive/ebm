-module (ebm).
-export ([search/2, search/3, search_all/2]).
-on_load(load_shared_obj/0).


-spec search(binary(), binary()) -> integer().
-spec search(binary(), binary(), non_neg_integer()) -> integer().
-spec search_all(binary(), binary()) -> [integer()].


% Search for the first occurence of Needle in Haystack.
search(Needle, Haystack) ->
    search(Needle, Haystack, 0).

% Search for the first occurence of Needle in a section of Haystack.
% StartIndex is inclusive.
search(_Needle, _Haystack, _StartIndex) ->
    erlang:nif_error({nif_not_loaded, ?MODULE}).


% Search for all occurences of Needle in Haystack.
search_all(Needle, Haystack) ->
    do_search_all(Needle, Haystack, 0, size(Needle), []).

do_search_all(Needle, Haystack, StartIndex, NeedleLen, Acc) ->
    case search(Needle, Haystack, StartIndex) of
        % Next occurence found, advance index and continue.
        Int when Int >= 0 ->
            NewStart = StartIndex + NeedleLen,
            NewAcc = [Int|Acc],
            do_search_all(Needle, Haystack, NewStart, NeedleLen, NewAcc);

        % Done searching.
        -1 -> Acc
    end.


load_shared_obj() ->
    EbinDir = filename:dirname(code:which(?MODULE)),
    AppPath = filename:dirname(EbinDir),
    Path = filename:join([AppPath, "priv", "ebm"]),
    ok = erlang:load_nif(Path, 0).
