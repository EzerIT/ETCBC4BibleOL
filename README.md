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
    - *g_word_translit, g_word_nopunct_translit, g_suffix_translit, g_prs_translit, g_vbs_translit, g_pfm_translit, g_vbe_translit, g_nme_translit, g_uvf_translit, g_voc_lex_translit, qere_translit* (the transliterated version of the corresponding XXX_utf8 features)
    - verb_class (the verb class)
* Erroneous non-final consonants in *g_nme_utf8, g_prs_utf8,* and *g_vbe_utf8* have been fixed.
* Jussive, cohortative, and emphatic imperative verb tenses have be added.
* The unused verbal stems *esht* and *etta* have been removed from the *verbal_stem_t* enumeration type.
* The lower dots (Puncta extraordinaria) under the first word of Ps 27:13 have been fixed by replacing Unicode 0323 with 05C5.
