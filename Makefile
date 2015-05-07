.PHONY: c compile test deps d dialyze

# List of application to include in the static analysis.
apps = erts kernel stdlib
deps = $(wildcard deps/*)

depnames = $(notdir $(deps))
appplts = $(foreach app,$(apps),ebin/$(app)_app.plt)
depplts = $(foreach dep,$(depnames),ebin/$(dep)_dep.plt)

all:
	./rebar compile

c: compile
compile:
	./rebar compile skip_deps=true

deps:
	./rebar get-deps && ./rebar compile

test: compile
	./rebar eunit skip_deps=true

shell: compile
	erl -pa ebin -pa deps/*/ebin

clean:
	rm -rf ebin/*.beam priv/*.so .dialyzer_plt

# Static Analysis:
d: dialyze
dialyze: compile .dialyzer_plt $(appplts) $(depplts)
	@dialyzer --no_native -Wno_unused -Wunmatched_returns -Werror_handling -Wno_return --plts $(local_plt) $(wildcard ebin/*.plt) -r ./ebin | grep -v "Checking whether "

# Update or build project-local PLT.
local_plt = .dialyzer_plt
ifeq ($(wildcard $(local_plt)),)
	local_plt_op = --build_plt --output_plt $(local_plt)
else
	local_plt_op = --check_plt --plt $(local_plt)
endif

$(local_plt):
	dialyzer -q $(local_plt_op) -r ebin

ebin/%_app.plt: pltname = $(basename $(notdir $@))
ebin/%_app.plt:
	dialyzer -q --build_plt --output_plt $@ --apps ${pltname:_app=}

# How to build a dependencies PLT.
ebin/%_dep.plt: pltname = $(basename $(notdir $@))
ebin/%_dep.plt:
	-dialyzer -q --build_plt --output_plt $@ -r deps/${pltname:_dep=}
