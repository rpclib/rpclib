UTILS=../utils
DDIR=doxygen/xml

all: reference.md

$(DDIR)/index.xml: Doxyfile $(wildcard ../include/rpc/*.h)
	doxygen Doxyfile

all.xml: $(DDIR)/index.xml
	xsltproc $(DDIR)/combine.xslt $(DDIR)/index.xml >all.xml

reference.md: all.xml reference.md.mako
	python3 $(UTILS)/markygen.py all.xml reference.md.mako pages/reference.md
