
outdir          = ./bin
appfile         = ./bin/dtsw
SUPERGLUE_DIR   = /pica/h1/afshin/sg/superglue/include
SUPERGLUE_FLAGS = -pthread -I$(SUPERGLUE_DIR)  #-pedantic -Wno-long-long -Wno-format
DUCTTEIP_DIR    = /pica/h1/afshin/Damavand/D4/DuctTeip/ResStudio/Prototype
DUCTTEIP_INC    = $(DUCTTEIP_DIR)/include
DUCTTEIP_LIBPATH= $(DUCTTEIP_DIR)/lib/
DUCTTEIP_LIB    = ductteip
SOURCE_DIR      = ./src
HEADER_DIR      = ./include
DUCTTEIP_FLAGS  = -I$(HEADER_DIR) -DWITH_MPI=1 -I$(SUPERGLUE_DIR) -I$(DUCTTEIP_INC)

ifeq ($(CXX),icpc)
#-----------------------Intel Compiler set---------------------
	CPP  = mpiicpc
	LINK_FLAGS=-lm -lrt -pthread -L$(DUCTTEIP_LIBPATH) -l$(DUCTTEIP_LIB) -Wl,--allow-multiple-definition 
	COMP_FLAGS= $(SUPERGLUE_FLAGS) $(DUCTTEIP_FLAGS) -std=c++11 -qopt-prefetch -O3  -xCORE-AVX2 -g
else
#-----------------------GCC Compiler set---------------------
	CPP  = mpicxx
	GCOV_FLAGS=-fprofile-arcs -ftest-coverage
	OPTIM_FLAGS=-mavx -march=bdver1 -mfma4 -Ofast  -Wwrite-strings
	SPECIAL_FLAGS=$(OPTIM_FLAGS)     
	LINK_FLAGS=-lm -lrt -lpthread -L$(DUCTTEIP_LIBPATH) -l$(DUCTTEIP_LIB) -Wl,--allow-multiple-definition -Wl,--whole-archive
	COMP_FLAGS= $(SUPERGLUE_FLAGS) $(DUCTTEIP_FLAGS) -std=c++11 $(SPECIAL_FLAGS) 
endif
#########################################################
source:=$(notdir $(shell ls -Sr $(SOURCE_DIR)/*.cpp))
objnames:=$(source:%.cpp=%.o)
objects:=$(addprefix $(outdir)/,$(objnames))
RED=\033[0;31m
GREEN=\033[1;32m
NC=\033[0m

all: $(appfile)

$(appfile) : $(appfile)($(outdir)/*.o)
	@echo -e "$(GREEN) generate output exe $(NC) $(RED)$(appfile)$(NC)."

$(appfile)($(outdir)/*.o) : $(objects)
	@echo -e "$(GREEN) *** link objects *** $(NC)"
	@$(CPP) -o $(appfile) $? $(LINK_FLAGS)

$(objects):  $(outdir)/%.o:  $(SOURCE_DIR)/%.cpp 
	@echo -e "$(GREEN) compile $(notdir $<) $(NC)"
	@$(CPP) -c -o $@ $< $(COMP_FLAGS)


clean: 
	rm $(outdir)/*.o $(appfile)
