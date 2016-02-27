#Copyright (c) 2016 Simon Schmidt
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.


include generic.mk

tools += bin/ls bin/sleep bin/kill bin/grep bin/chmod
tools += bin/yes bin/touch bin/tee bin/chgrp bin/chown
tools += bin/cmp bin/col bin/comm bin/cp bin/date bin/dd
tools += bin/fgrep bin/ln bin/look bin/mkdir bin/mv bin/nice
tools += bin/od bin/rev bin/rm bin/rmdir bin/sort bin/who
tools += bin/wc

allofit: $(tools)
	echo done

#clean:
#	rm src/*.o

src/%/a.out: src/%
	$(MAKE) -C $< a.out

src/%.o: src/%.c
	$(C) -c $? -o $@

#bin/login: src/login.o
#	$(C) $? -lcrypt -o $@

bin/%: src/%.o
	$(C) $? -o $@

bin/%: src/%/a.out
	mv $< $@



