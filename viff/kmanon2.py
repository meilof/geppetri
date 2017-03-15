# Copyright (c) 2016-2017 Koninklijke Philips N.V. All rights reserved. A
# copyright license for redistribution and use in source and binary forms,
# with or without modification, is hereby granted for non-commercial,
# experimental and research purposes, provided that the following conditions
# are met:
# - Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimers.
# - Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimers in the
#   documentation and/or other materials provided with the distribution. If
#   you wish to use this software commercially, kindly contact
#   info.licensing@philips.com to obtain a commercial license.
# 
# This license extends only to copyright and does not include or grant any
# patent license or other license whatsoever.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

from optparse import OptionParser

import viff.boost
viff.boost.install()
from viff.runtime import create_runtime, Runtime, make_runtime_class
from viff.comparison import Toft07Runtime
from viff.division import DivisionSH12Mixin
from viff.equality import ProbabilisticEqualityMixin
from viff.config import load_config

from viffvc.vcruntime import vcrun, vc_start, VcShare, vc_read_additive_shares, vc_import_additive_shares, vc_inlinecb, vc_declare_block, vc_output_open

from twisted.internet import reactor
    
def run(runtime):
    global config
    npoints = int(config.get("main", "npoints"))
    ninputters = int(config.get("main", "ninputters"))
    perblock = int(config.get("main", "perblock"))
    nblocks = (npoints+perblock-1)/perblock
    
    print "Welcome"
    
    shs = vc_read_additive_shares(runtime, runtime.Zp, "input", range(1,ninputters+1))
    
    summsize=25
    nsumm=npoints/summsize
    
    vc_start(runtime, "summ", "summ0")
    shcur,rndcur = vc_import_additive_shares(runtime, shs, 0, 4*npoints, 0, nblocks)
    vc_declare_block(runtime, shcur, rndcur, "input0")
    
    sumd1s=[]; sumn1s=[]; sumd2s=[]; sumn2s=[]
    
    for k in range(nsumm):
        sumd1s.append(sum(shcur[k*summsize:(k+1)*summsize:4]))
        sumn1s.append(shcur[k*summsize+1]);
        sumd2s.append(sum(shcur[k*summsize+2:(k+1)*summsize+2:4]))
        sumn2s.append(shcur[k*summsize+3]);
        
    vc_output_open(runtime, vc_declare_block(runtime, sumd1s+sumn1s+sumd2s+sumn2s, VcShare.random(runtime), "output"))
        
    print "Done."

        
# global KM config
import ConfigParser
config=ConfigParser.ConfigParser()
config.read("params.txt")
    
vcrun(run)