# ETCBC4BibleOL

## Adapt ETCBC4 for Bible Online Learner


The ETCBC4 (or BHS4) Emdros database provided by the Eep Talstra Centre for Bible and Computing
needs some modification before it can be used with Bible OL.

This package contains C++ programs and Shell scripts that provide the necessary modifications.

The modifications made to the Emdros database are primarily:


*  The following features are added:
    - *lexeme_occurrences* (counts number of occurrences of a lexeme)
    - *frequency_rank* (frequency rank of lexeme, a separate value for Hebrew and Aramaic)
    - *g_word_nocant* (g_word with cantillation marks removed)
    - *g_word_nocant_utf8* (g_word_utf8 with cantillation marks removed)
    - *g_lex_cons_utf8, g_nme_cons_utf8, g_pfm_cons_utf8, g_prs_cons_utf8, g_vbs_cons_utf8, g_vbe_cons_utf8, g_uvf_cons_utf8, lex_cons_utf8, g_voc_lex_cons_utf8, g_word_cons_utf8* (the value of the corresponding XXX_utf8 features but with vowels and cantillation marks removed)
    - *g_word_translit, g_word_nopunct_translit, g_suffix_translit, g_prs_translit, g_vbs_translit, g_pfm_translit, g_vbe_translit, g_nme_translit, g_uvf_translit, g_voc_lex_translit, g_lex_translit, qere_translit* (the transliterated version of the corresponding XXX_utf8 features)
    - verb_class (the verb class)
* Erroneous non-final consonants in *g_nme_utf8, g_prs_utf8,* and *g_vbe_utf8* have been fixed.
* Jussive, cohortative, and emphatic imperative verb tenses have be added.
* The unused verbal stems *esht* and *etta* have been removed from the *verbal_stem_t* enumeration type.
* The lower dots (Puncta extraordinaria) under the first word of Ps 27:13 have been fixed by replacing Unicode 0323 with 05C5.



## Prerequisites

The code is intended to run on a Linux system. The following software must be available:

* Gnu C++ compiler with C++17 support
* libprcecpp
* Emdros


## The Makefile

A Makefile is supplied which can generate the following targets: ETCBC4 and ETCBC4_words.db. The
command "make all" generates both targets.

### Generating ETCBC4

The command "make ETCBC4" modifies an existing ETCBC4 database so that it can be used with Bible OL.

The source database is taken from the file specified in the variable BHS4 in the Makefile.

The modification consists of the following steps:

* Building the *emdros_updater* program from the C++ source code.
* Running the *emdros_updater* program with input from the source database. This generates an MQL
  file with the necessary modifications.
* Running *mql* to apply the modifications.
* Piping the database through the *change_mql.sh* script. This adds "FROM SET" to many Emdros
  feature declations. (This cannot be done by *emdros_updater* because of what is prsumably a
  bug in Emdros.)
* Generating a database from the output from *change_mql.sh*.
* Running *fix_lower_dots.sh* on the database. This fixes the lower dots under the first word of
  Ps 27:13.
* Running *extra_tenses/make_update.sh* on the database. This adds the verb tenses *cohortative,
  jussive* and *emphatic imperative* to the database and applies the new tenses where necessary.


### Generating ETCBC4_words.db

The file ETCBC4_words.db is an SQLite3 database containing information used to generate multiple
choice questions for a few features.

The generation of the database consists of the following steps:

* Building the *worddb* program from the C++ source code.
* Running *worddb* on the modified ETCBC4 database. This will generate an SQL text file.
* Running *sqlite3* on the SQL text file to generate ETCBC4_words.db.

### Generating ETCBC4_hints.db

The file ETCBC4_hints.db is an SQLite3 database containing information used to generate hints
for exercises about verbs with ambiguous forms.

The generation of the database consists of the following steps:

* Building the *hintsdb* program from the C++ source code.
* Running *hintsdb* on the modified ETCBC4 database. This will generate an SQL text file.
* Running *sqlite3* on the SQL text file to generate ETCBC4_hints.db.


## Installing in Bible Online Learner

The files ETCBC4, ETCBC4_words.db, and ETCBC4_hints.db must be copied to the *db* folder of Bible OL. Additionally,
the Bible OL JSON files describing the database may need to be modified.
