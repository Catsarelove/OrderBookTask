md_replay : subscriptions.cpp instrument.h instrument.cpp
	g++ subscriptions.cpp instrument.cpp -o md_replay -Wall -Wextra -O3
.PHONY : clean
clean :
	-rm edit $(objects)
