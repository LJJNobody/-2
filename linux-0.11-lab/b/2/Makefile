FILES	=mmap munmap clone

INC	=-nostdinc -I./include

.PHONY: all test clean
all: echo
	for i in $(FILES); \
	do \
		(gcc $(INC) $$i.c -o $$i); \
	done

${FILES}: echo
	gcc $(INC) $@.c -o $@

echo: echo.c
	gcc $(INC) $@.c -o $@

test: echo
	@for i in $(FILES); \
	do \
		(echo "--------------testing " $$i":"); \
		(./$$i); \
	done

clean:
	-(rm ${FILES}; \
	rm *.o a.out *.log echo test_mmap.txt) > /dev/null 2>&1
