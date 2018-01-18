import os, sys, gc

if len(sys.argv) != 9:
    print 
    print "SHR_ANA1  = str(sys.argv[1]) -- showerqualsingle"
    print "SHR_TRUTH = str(sys.argv[2]) -- shower_truth_match"
    print "TRK_ANA1  = str(sys.argv[3]) -- trackqualsingle"
    print "TRK_ANA2  = str(sys.argv[4]) -- tracker_anaout"
    print "TRK_TRUTH = str(sys.argv[5]) -- track_truth_match"
    print "TRK_PGRPH = str(sys.argv[6]) -- track_pgraph_match"
    print "PID_ANA   = str(sys.argv[7]) -- multipid_out"
    print "OUTDIR    = str(sys.argv[8])"
    print 
    sys.exit(1)

import ROOT
import numpy as np
import pandas as pd
BASE_PATH = os.path.realpath(__file__)
BASE_PATH = os.path.dirname(BASE_PATH)
sys.path.insert(0,BASE_PATH)

SHR_ANA1  = str(sys.argv[1])
SHR_TRUTH = str(sys.argv[2])
TRK_ANA1  = str(sys.argv[3])
TRK_ANA2  = str(sys.argv[4])
TRK_TRUTH = str(sys.argv[5])
TRK_PGRPH = str(sys.argv[6])
PID_ANA   = str(sys.argv[7])
OUTDIR    = str(sys.argv[8])

num = int(os.path.basename(SHR_ANA1).split(".")[0].split("_")[-1])

from util.fill_df import *

print "--> initialize_st(...)"

stp_df = initialize_stp(SHR_ANA1,
                        SHR_TRUTH,
                        TRK_ANA1,
                        TRK_ANA2,
                        TRK_TRUTH,
                        TRK_PGRPH,
                        PID_ANA)

stp_df.to_pickle(os.path.join(OUTDIR,"stp_comb_df_%d.pkl" % num))
del stp_df
gc.collect()

print "---> done"
sys.exit(0)