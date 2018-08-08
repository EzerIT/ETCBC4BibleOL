#!/bin/bash

# Copyright © 2017 Ezer IT Consulting.
# Released under an MIT License.

#This script adds "FROM SET" and "WITH INDEX" to a number of features.
#It takes an MQL file as input and produces a new one as output

sed \
-e 's/^  g_vbe : string DEFAULT/  g_vbe : string FROM SET DEFAULT/' \
-e 's/^  g_word : string DEFAULT/  g_word : string FROM SET DEFAULT/' \
-e 's/^  g_word_utf8 : string DEFAULT/  g_word_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_cons_utf8 : string DEFAULT/  g_cons_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_cons : string DEFAULT/  g_cons : string FROM SET DEFAULT/' \
-e 's/^  g_pfm : string DEFAULT/  g_pfm : string FROM SET DEFAULT/' \
-e 's/^  g_pfm_utf8 : string DEFAULT/  g_pfm_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_vbs : string DEFAULT/  g_vbs : string FROM SET DEFAULT/' \
-e 's/^  g_vbs_utf8 : string DEFAULT/  g_vbs_utf8 : string FROM SET DEFAULT/' \
-e 's/^  lex : string DEFAULT/  lex : string FROM SET WITH INDEX DEFAULT/' \
-e 's/^  lex_utf8 : string DEFAULT/  lex_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_prs_utf8 : string DEFAULT/  g_prs_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_prs : string DEFAULT/  g_prs : string FROM SET DEFAULT/' \
-e 's/^  g_lex : string DEFAULT/  g_lex : string FROM SET DEFAULT/' \
-e 's/^  g_uvf_utf8 : string DEFAULT/  g_uvf_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_uvf : string DEFAULT/  g_uvf : string FROM SET DEFAULT/' \
-e 's/^  g_lex_utf8 : string DEFAULT/  g_lex_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_nme_utf8 : string DEFAULT/  g_nme_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_nme : string DEFAULT/  g_nme : string FROM SET DEFAULT/' \
-e 's/^  g_vbe_utf8 : string DEFAULT/  g_vbe_utf8 : string FROM SET DEFAULT/' \
-e 's/^  nme : string DEFAULT/  nme : string FROM SET DEFAULT/' \
-e 's/^  frequency_rank : integer DEFAULT/  frequency_rank : integer WITH INDEX DEFAULT/' \
-e 's/^  english : string DEFAULT/  english : string FROM SET DEFAULT/' \
-e 's/^  german : string DEFAULT/  german : string FROM SET DEFAULT/' \
-e 's/^  g_voc_lex : string DEFAULT/  g_voc_lex : string FROM SET WITH INDEX DEFAULT/' \
-e 's/^  g_voc_lex_utf8 : string DEFAULT/  g_voc_lex_utf8 : string FROM SET WITH INDEX DEFAULT/' \
-e 's/^  qere : string DEFAULT/  qere : string FROM SET DEFAULT/' \
-e 's/^  qere_utf8 : string DEFAULT/  qere_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_lex_cons_utf8 : string DEFAULT/  g_lex_cons_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_nme_cons_utf8 : string DEFAULT/  g_nme_cons_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_pfm_cons_utf8 : string DEFAULT/  g_pfm_cons_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_prs_cons_utf8 : string DEFAULT/  g_prs_cons_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_vbs_cons_utf8 : string DEFAULT/  g_vbs_cons_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_vbe_cons_utf8 : string DEFAULT/  g_vbe_cons_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_word_cons_utf8 : string DEFAULT/  g_word_cons_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_uvf_cons_utf8 : string DEFAULT/  g_uvf_cons_utf8 : string FROM SET DEFAULT/' \
-e 's/^  lex_cons_utf8 : string DEFAULT/  lex_cons_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_voc_lex_cons_utf8 : string DEFAULT/  g_voc_lex_cons_utf8 : string FROM SET WITH INDEX DEFAULT/' \
-e 's/^  g_word_nocant : string DEFAULT/  g_word_nocant : string FROM SET WITH INDEX DEFAULT/' \
-e 's/^  g_word_nostress : string DEFAULT/  g_word_nostress : string FROM SET WITH INDEX DEFAULT/' \
-e 's/^  g_word_nocant_utf8 : string DEFAULT/  g_word_nocant_utf8 : string FROM SET WITH INDEX DEFAULT/' \
-e 's/^  g_word_nostress_utf8 : string DEFAULT/  g_word_nostress_utf8 : string FROM SET WITH INDEX DEFAULT/' \
-e 's/^  g_suffix : string DEFAULT/  g_suffix : string FROM SET DEFAULT/' \
-e 's/^  g_suffix_utf8 : string DEFAULT/  g_suffix_utf8 : string FROM SET DEFAULT/' \
-e 's/^  g_word_translit : string DEFAULT/  g_word_translit : string FROM SET DEFAULT/' \
-e 's/^  g_word_nopunct_translit : string DEFAULT/  g_word_nopunct_translit : string FROM SET DEFAULT/' \
-e 's/^  g_suffix_translit : string DEFAULT/  g_suffix_translit : string FROM SET DEFAULT/' \
-e 's/^  g_prs_translit : string DEFAULT/  g_prs_translit : string FROM SET DEFAULT/' \
-e 's/^  g_vbs_translit : string DEFAULT/  g_vbs_translit : string FROM SET DEFAULT/' \
-e 's/^  g_pfm_translit : string DEFAULT/  g_pfm_translit : string FROM SET DEFAULT/' \
-e 's/^  g_vbe_translit : string DEFAULT/  g_vbe_translit : string FROM SET DEFAULT/' \
-e 's/^  g_nme_translit : string DEFAULT/  g_nme_translit : string FROM SET DEFAULT/' \
-e 's/^  g_uvf_translit : string DEFAULT/  g_uvf_translit : string FROM SET DEFAULT/' \
-e 's/^  g_voc_lex_translit : string DEFAULT/  g_voc_lex_translit : string FROM SET WITH INDEX DEFAULT/' \
-e 's/^  qere_translit : string DEFAULT/  qere_translit : string FROM SET DEFAULT/' \
-e 's/^  pfm : string DEFAULT/  pfm : string FROM SET DEFAULT/' \
-e 's/^  prs : string DEFAULT/  prs : string FROM SET DEFAULT/' \
-e 's/^  txt : string DEFAULT/  txt : string FROM SET DEFAULT/' \
-e 's/^  uvf : string DEFAULT/  uvf : string FROM SET DEFAULT/' \
-e 's/^  vbe : string DEFAULT/  vbe : string FROM SET DEFAULT/' \
-e 's/^  vbs : string DEFAULT/  vbs : string FROM SET DEFAULT/'
