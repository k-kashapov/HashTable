basicFlags    = -I include -Wall -Wextra -O2

listTargets   = src/List.cpp

filesTargets  = src/files.cpp

objs          = obj/list.o obj/logs.o obj/files.o

tableTargets  = table.cpp src/HashTable.cpp $(objs)

#================== <EXE> =====================
all: table.exe

table.exe: 	$(tableTargets)
			g++ $(tableTargets) $(basicFlags) -o table.exe

fullDebug.exe:  $(tableTargets)
	  			g++ $(tableTargets) $(debugFlags) -o debug.exe -I include

#================== <OBJ> =====================
obj/list.o: $(listTargets)
			g++ $(listTargets) $(basicFlags) -o obj/list.o -c

obj/logs.o:	src/Logs.cpp
			g++ src/Logs.cpp $(basicFlags) -o obj/logs.o -c

obj/files.o: $(filesTargets)
			 g++ $(filesTargets) $(basicFlags) -o obj/files.o -c

#================== <MISC> ====================
cleanup: rm *.exe


debugFlags = -O0 -D MAX_INFO -D LIST_LOGS -D DEBUG -g -std=c++14 -fmax-errors=10 -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -lm -pie
