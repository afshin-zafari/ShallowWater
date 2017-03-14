
outdir= ./bin
libfile=./lib/libductteip.a
appfile=./bin/dtsw
SUPERGLUE_DIR=/pica/h1/afshin/sg/superglue/include
SUPERGLUE_FLAGS=-pthread -I$(SUPERGLUE_DIR)  #-pedantic -Wno-long-long -Wno-format
SOURCE_DIR=./src
HEADER_DIR=./include
DUCTTEIP_FLAGS=-I$(HEADER_DIR) -DWITH_MPI=1 -I$(SUPERGLUE_DIR)
CPP=mpiicpc
LINK=mpif90


ifeq ($(CXX),icpc)
#-----------------------Intel Compiler set---------------------
	LINK_FLAGS=-lm -lrt 
	COMP_FLAGS= $(SUPERGLUE_FLAGS) $(DUCTTEIP_FLAGS) -std=c++11 -qopt-prefetch -O3  -xCORE-AVX2
else
#-----------------------GCC Compiler set---------------------
	GCOV_FLAGS=-fprofile-arcs -ftest-coverage
	OPTIM_FLAGS=-mavx -march=bdver1 -mfma4 -Ofast  -Wwrite-strings
	SPECIAL_FLAGS=$(OPTIM_FLAGS)     
	LINK_FLAGS=-lm -lrt -lpthread
	COMP_FLAGS= $(SUPERGLUE_FLAGS) $(DUCTTEIP_FLAGS) -std=c++11 $(SPECIAL_FLAGS) 
endif
#########################################################
headers:=$(notdir $(shell ls -Sr $(SOURCE_DIR)/*.cpp))
objnames:=$(headers:%.cpp=%.o)
objects:=$(addprefix $(outdir)/,$(objnames))	
all: $(appfile)

$(appfile) : $(appfile)($(outdir)/*.o)
	$(info  )
	$(info ---------------------------------------------------------)
	$(info generate output exe '$(appfile)'.)
	$(info ---------------------------------------------------------)
	$(info  )
	

$(appfile)($(outdir)/*.o) : $(objects)
	$(info  )
	$(info ---------------------------------------------------------)
	$(info link objects to app. )
	$(info ---------------------------------------------------------)
	$(info  )
	@$(CPP) -o $(appfile) $?

$(objects):  $(outdir)/%.o:  $(SOURCE_DIR)/%.cpp 
	$(info compile $(notdir $<) )
	@$(CPP) -c -o $@ $< $(COMP_FLAGS)


clean: 
	rm $(outdir)/*.o $(appfile)
