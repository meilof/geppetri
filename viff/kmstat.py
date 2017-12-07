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

import sys, time

import viff.boost
viff.boost.install()
from viff.runtime import create_runtime, Runtime, make_runtime_class
from viff.comparison import Toft07Runtime
from viff.division import DivisionSH12Mixin
from viff.equality import ProbabilisticEqualityMixin
from viff.config import load_config

from viffvc.vcruntime import viffinlinecb, vcrun, vc_start, VcShare, vc_read_additive_shares, vc_import_additive_shares, vc_inlinecb, vc_declare_block, vc_output_open
from viff.inlinecb import declareReturnNop

from twisted.internet import reactor

from scipy.stats import chi2

def inner(runtime,di1,ni1,di2,ni2):
    frc     = (di1+di2).tofp_conv()/(ni1+ni2).tofp_conv()
    ecur=frc*ni1
    v1n=(ni1*ni2*(di1+di2)*(ni1+ni2-di1-di2)).tofp_conv()
    v1d=((ni1+ni2)*(ni1+ni2)*(ni1+ni2-runtime.Zp(1))).tofp_conv()
    v1=v1n/v1d
    return (di1,ecur,v1)

def fin(runtime,dtot,etot,vtot):
    dtot=dtot.tofp_conv()
    chi0=(dtot-etot)
    chi=(chi0/vtot)*chi0
    return chi
    

@viffinlinecb
def run(runtime):
    yield declareReturnNop(runtime, runtime.Zp)
    
    global config
    npoints = int(config.get("main", "npoints"))
    ninputters = int(config.get("main", "ninputters"))
    perblock = int(config.get("main", "perblock"))
    nblocks = (npoints+perblock-1)/perblock
    
    print "Welcome"
    start = time.time()
    
    shs = vc_read_additive_shares(runtime, runtime.Zp, "input", range(1,ninputters+1))
    
    ds=[]
    es=[]
    vs=[]
    rs=[]
    
    for i in xrange(nblocks):
        print "Block", i
        vc_start(runtime, "block", "block" + str(i))
        shcur,rndcur = vc_import_additive_shares(runtime, shs, 4*i*perblock, 4*perblock, i, nblocks)
        vc_declare_block(runtime, shcur, rndcur, "input")
        dsm=None; esm=None; vsm=None
        for j in xrange(perblock):
            #print "Point", j
            d,e,v=inner(runtime, shcur[4*j+0], shcur[4*j+1], shcur[4*j+2], shcur[4*j+3])
            if j==0: # TODO: clean this up
                dsm=d; esm=e; vsm=v
            else:
                dsm=dsm+d; esm=esm+e; vsm=vsm+v
        
        rndi=VcShare.random(runtime)
        vc_declare_block(runtime, [dsm,esm,vsm], rndi, "output")
        
        ds.append(dsm)
        es.append(esm)
        vs.append(vsm)
        rs.append(rndi)
        
    d=sum([di.sh for di in ds])
    e=sum([ei.sh for ei in es])
    v=sum([vi.sh for vi in vs])
    r=sum([ri.sh for ri in rs]) # TODO: we have to rely on the specific shape of the randomness here...
    
    vc_start(runtime, "fin", "fin")
    dv,ev,vv = map(lambda x: VcShare.from_share(runtime, x), [d,e,v])
    rndcur = VcShare.from_share(runtime, r)
    vc_declare_block(runtime, [dv,ev,vv], rndcur, "input")
    chi = fin(runtime, dv, ev, vv)
    
    print "Now here", time.time()-start
    sys.stdout.flush()
    
    chis = chi.ensure_single(runtime)
    chio = yield chis.open()
    chio = runtime.get_value(chio)
    print "Final result: chi statistic=", chio, ", p-value=", 1-chi2.cdf(chio,1)
    
    vc_output_open(runtime, vc_declare_block(runtime, [chis], VcShare.random(runtime), "output"))
    
    print "Done.", time.time()-start
    sys.stdout.flush()

        
# global KM config
import ConfigParser
config=ConfigParser.ConfigParser()
config.read("params.txt")
    
vcrun(run)