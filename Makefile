# Copyright © 2017 Ezer IT Consulting.
# Released under an MIT License.

OBJFILES=emdros_iterators.o emdros_updater.o frequency_handler.o \
verb_stem_handler.o verb_class.o verb_class_handler.o strip_handler.o \
translit_handler.o hebrew_transliterator.o util.o

OBJFILES2=worddb.o emdros_iterators.o

DEPFILES=$(OBJFILES:.o=.d) $(OBJFILES2:.o=.d)

CXXFLAGS=-std=c++11 -MMD -O3 -I /usr/local/include
LDLIBS= -L /usr/local/lib 
LDFLAGS= -lpcrecpp -lmql -lemdf

BHS4=../bhs4/bhs4  # Location of source Emdros database

all: ETCBC4 ETCBC4_words.db

emdros_updater:	$(OBJFILES)
	g++ $(CXXFLAGS) $(LDLIBS) -o $@ $+ $(LDFLAGS)

update.mql:	emdros_updater $(BHS4)
	./emdros_updater $(BHS4) $@

ETCBC4:	update.mql
	cp $(BHS4) $@
	mql -d $@ $+
	mqldump --batch-create-objects $@ | ./change_mql.sh > x.mql
	rm $@
	mql x.mql
	rm x.mql
	./fix_lower_dots.sh $@
	cd extra_tenses; ./make_update.sh; mv ETCBC4 ..

worddb: $(OBJFILES2)
	g++ $(CXXFLAGS) $(LDLIBS) -o $@ $+ $(LDFLAGS)

worddb.sql: worddb ETCBC4
	./worddb ETCBC4 $@

ETCBC4_words.db: worddb.sql
	rm -f $@
	sqlite3 $@ < $+

clean:
	rm -f $(OBJFILES) $(OBJFILES2) $(DEPFILES) emdros_updater update.mql ETCBC4 ETCBC4_words.db worddb.sql worddb


-include $(DEPFILES)
