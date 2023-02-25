# mstrie

## Description
The implementation of the `mstrie` program that is used for the Multiset-trie data structure management.

---

## Installation
To install `mstrie` use the following commands:
```bash
./configure
make
make install
make clean
```

The program has a manual that can be accessed by running:
```bash
man mstrie
```
---

## Running the program

The program relies on the configuration file to initialize its environment. 

The program can be run in 2 different modes:
1. CLI mode.
2. Benchmark mode.

### CLI mode
In this mode a command line interface is available for the user. One can load an existing or create a new Multiset-trie data structure which will pe parameterized via configuration file. The structure can be updated, queried and saved.

The config file for __CLI__ mode should look as the following example:

```config
run_mode = "cli"
default_mstrie_name = "mstrie"
mstrie:
	alphabet_length = "25"
	max_multiplicity = "10"
	mstrie_path = "/absolute/path/to/mstrie/file"
```

In this example, the config specifies that the execution mode for `mstrie` is CLI, the default name of the Multiset-trie object is __mstrie__, which will be persited at path __mstrie_path__ and have __alphabet_length__ of 25 and __max_multiplicity__ equal to 10.
The file at __mstrie_path__ does not have to exist (it can be created by the program), however, the user that runs a program must have appropriate permissions to create a file at the specified path.

It is possible to work with multiple Multiset-trie objects in the same session. To do so, one must first specify parameterization of other Multiset-trie objects in the configuration file as follows:

```config
run_mode = "cli"
default_mstrie_name = "mstrie"
mstrie:
	alphabet_length = "25"
	max_multiplicity = "10"
	mstrie_path = "/absolute/path/to/mstrie/file"
other_mstrie:
	alphabet_length = "25"
	max_multiplicity = "10"
	mstrie_path = "/absolute/path/to/other_mstrie/file"
```

When the above configuration file is loaded, CLI mode allows to switch between Multiset-trie objects __mstrie__ and __other_mstrie__.

### Benchmark mode
In this mode the program executes a benchmark according to its settings in the specified configuration file.

The config file for __benchmark__ mode should look as the following example:

```config
run_mode = "benchmark"
default_mstrie_name = "mstrie"
mstrie:
	alphabet_length = "25"
	max_multiplicity = "10"
	mstrie_path = "/absolute/path/to/mstrie/file"
benchmark:
	mstrie_name = "mstrie"
	run:
		type = "exact_search"
		test_file = "/absolute/path/to/test/file"
		result_file = "/absolute/path/to/result/file"
```

In this example, the config specifies that the execution mode for `mstrie` is benchmark. The benchmark is run against the Multiset-trie object that is configured via setting __mstrie_name__ that points to an exiting Multiset-trie object configuration in the file.

The __run__ section of benchmark configuration specifies a type of queries to run on the Multiset-trie. Allowed values are _exact_search_, _subset_search_ and _superset_search_.
The __test_file__ must contain a list of multisets that will be used for queries. The __result_file__ will be created by the program with results for each query performed on the Multiset-trie.

---

## Uninstallation
To uninstall `mstrie` use the following commands:
```bash
make uninstall
```
