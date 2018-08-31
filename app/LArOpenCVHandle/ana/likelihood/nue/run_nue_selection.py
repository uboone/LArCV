import os, sys, gc

if len(sys.argv) != 5:
    print 
    print "COMB_CUT_DF = str(sys.argv[1])"
    print "CUT_CFG     = str(sys.argv[2])"
    print "NUM         = str(sys.argv[3])"
    print "OUTDIR      = str(sys.argv[4])"
    print 
    sys.exit(1)

import ROOT
import numpy as np
import pandas as pd

BASE_PATH = os.path.realpath(__file__)
BASE_PATH = os.path.dirname(BASE_PATH)
sys.path.insert(0,BASE_PATH)

COMB_CUT_DF = str(sys.argv[1])
CUT_CFG     = str(sys.argv[2])
NUM         = str(sys.argv[3])
OUTDIR      = str(sys.argv[4])

from lib.cuts import apply_cuts

cut_df = apply_cuts(COMB_CUT_DF,CUT_CFG)

out_file = os.path.join(OUTDIR,"nue_selected_df_%s.pkl" % NUM)
cut_df.to_pickle(out_file)

del cut_df
gc.collect()

sys.exit(0)
