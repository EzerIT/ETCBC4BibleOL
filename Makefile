# Copyright © 2022 Claus Tøndering.
# Released under an MIT License.

#
# The list of books available in the ETCBC BHS4 database.
#
ALL_BOOKS = Genesis \
            Exodus \
            Leviticus \
            Numeri \
            Deuteronomium \
            Josua \
            Judices \
            Samuel_I \
            Samuel_II \
            Reges_I \
            Reges_II \
            Jesaia \
            Jeremia \
            Ezechiel \
            Hosea \
            Joel \
            Amos \
            Obadia \
            Jona \
            Micha \
            Nahum \
            Habakuk \
            Zephania \
            Haggai \
            Sacharia \
            Maleachi \
            Psalmi \
            Iob \
            Proverbia \
            Ruth \
            Canticum \
            Ecclesiastes \
            Threni \
            Esther \
            Daniel \
            Esra \
            Nehemia \
            Chronica_I \
            Chronica_II


#
# BOOKS_TO_MAKE should either be empty, or contain a list of
# book names on which to run emdros_update. If it is empty,
# emdros_update will run on all books.
#
BOOKS_TO_MAKE = 




OBJFILES=emdros_iterators.o emdros_updater.o frequency_handler.o \
verb_stem_handler.o verb_class.o verb_class_handler.o strip_handler.o \
monad_feature_handler.o translit_handler.o suffix_handler.o hebrew_transliterator.o util.o

OBJFILES2=worddb.o emdros_iterators.o
OBJFILES3=hintsdb.o emdros_iterators.o util.o
OBJFILES4=make_sequence_csv.o emdros_iterators.o

DEPFILES=$(OBJFILES:.o=.d) $(OBJFILES2:.o=.d) $(OBJFILES3:.o=.d) $(OBJFILES4:.o=.d)

CXX=c++

CXXFLAGS=-std=c++20 -MMD -O3 -I /usr/local/include

EMDROS_LIBS = $(shell pkg-config --libs emdros)

LDLIBS= -L /usr/local/lib
LDFLAGS= -lpcrecpp $(EMDROS_LIBS) -lpthread -ldl

# Don't let make(1) delete update.mql if emdros_updater aborts /
# segfaults.
#.PRECIOUS: update.mql


all: ETCBC4 ETCBC4_words.db ETCBC4_hints.db

emdros_updater:	$(OBJFILES)
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $@ $+ $(LDFLAGS)

bhs4:
	curl -L 'https://www.dropbox.com/scl/fi/thy1qle2osb395mlxtcwv/v1.29_20170102_bhs4?rlkey=l49gtolsgeukk5mw2o93qc83h' > $@

update.mql:	emdros_updater bhs4
	./emdros_updater bhs4 $@  $(BOOKS_TO_MAKE) 


ETCBC4:	update.mql
	cp bhs4 $@
	mql -d $@ $+
	mqldump --batch-create-objects $@ | ./change_mql.sh > x.mql
	rm $@
	mql x.mql
	rm x.mql
	./fix_lower_dots.sh $@
	cp $@ ETCBC4-before-extra-tenses
	cd extra_tenses; ./make_update.sh; mv ETCBC4 ..

worddb: $(OBJFILES2)
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $@ $+ $(LDFLAGS)

worddb.sql: worddb ETCBC4
	./worddb ETCBC4 $@

ETCBC4_words.db: worddb.sql
	rm -f $@
	sqlite3 $@ < $+

hintsdb: $(OBJFILES3)
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $@ $+ $(LDFLAGS)

hintsdb.sql: hintsdb ETCBC4
	./hintsdb ETCBC4 $@

ETCBC4_hints.db: hintsdb.sql
	rm -f $@
	sqlite3 $@ < $+

make_sequence_csv: $(OBJFILES4)
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $@ $+ $(LDFLAGS)

sequence_map.csv: make_sequence_csv ETCBC4
	./make_sequence_csv ETCBC4 $@

testtranslit: hebrew_transliterator.cpp hebrew_transliterator.hpp util.hpp util.o
	$(CXX) $(CXXFLAGS) $(LDLIBS) -D TEST_TRANSLIT -o $@ $< util.o $(LDFLAGS)

build_aram_lex_file: build_aram_lex_file.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

build_heb_es_lex_file: build_heb_es_lex_file.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(OBJFILES) $(OBJFILES2) $(OBJFILES3) $(DEPFILES) emdros_updater update.mql ETCBC4 ETCBC4_words.db ETCBC4_hints.db hintsdb.sql hintsdb worddb.sql worddb sequence_map.csv

-include $(DEPFILES)
