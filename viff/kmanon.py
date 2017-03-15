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
    blockperk=summsize/perblock
    
    print "nsumm", npoints, summsize, nsumm
    
    for k in range(nsumm):
        vc_start(runtime, "summ", "summ" + str(k))
        
        sumd1=None; sumn1=None; sumd2=None; sumn2=None
        
        for i in range(blockperk):
            shcur,rndcur = vc_import_additive_shares(runtime, shs, 4*(k*blockperk+i)*perblock, 4*perblock, k*blockperk+i, nblocks)
            vc_declare_block(runtime, shcur, rndcur, "input" + str(i))
            
            if i==0: sumn1=shcur[1];
            if i==0: sumn2=shcur[3];
            
            for j in xrange(perblock):
                sumd1=sumd1+shcur[4*j+0] if sumd1!=None else shcur[4*j+0] 
                sumd2=sumd2+shcur[4*j+2] if sumd2!=None else shcur[4*j+2]
                
        vc_output_open(runtime, vc_declare_block(runtime, [sumd1,sumn1,sumd2,sumn2], VcShare.random(runtime), "output"))
        
    print "Done."

# global KM config
import ConfigParser
config=ConfigParser.ConfigParser()
config.read("params.txt")
    
vcrun(run)