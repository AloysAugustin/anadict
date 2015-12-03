# Anadict

Small code to efficiently find all the anagrams of a given list of words in a dictionary

### Usage

Compile with `gcc -o anadict -O3 anadict.c`

Run with `./anadict dico.txt tests.txt` for an example run (with french words).

The dictionary file must have one word per line, lines starting with a non alphbetic character are ignored.
The words file must have one or more words separated by a space. If no word file is specified, the words are read from stdin.

If a word has no anagram in the dictionary, it is not displayed in the output.

### Algorithm

This code computes a special hash that is identical for anagrams. The underlying idea is to associate a prime number to
each letter of the alphabet. The hash is simply the product of the values associated to each letter of the word considered.
This brings an efficient way to find anagrams.

In order to limit the risk of overflow in the hash computation, the most frequent letters are associated to the smallest primes.

### Limits

The hashing algorithm may overflow, so in case of match, even if the probability that two words get the sam hash modulo 2**64
is extremely low, a letter-by-letter verification should be added to guarantee correctness. In order to reduce the amount of cases
in which we fall back to a letter-by-letter comparison, the hashing function could also signal overflows. Then the comparison would
become necessary only if the computation overflowed for both words.


Also this code currently keeps all the dictionary in memory, so it will not work on extremely big dictionaries. It should be ok
for human languages though.


