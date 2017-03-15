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

import viff.boost
viff.boost.install()
from viff.inlinecb import viffinlinecb, declareReturn, returnValue
from viff.local import localversion
from viff.runtime import Share

from viffvc.vcruntime import vc_input_online, VcShare, qap_wrapper, vcrun, vc_inlinecb, vc_assert_mult, vc_certificate, declareVcReturn

def eqz_check((rt, x), ret):
    m = VcShare.from_share(rt, rt.invert(x.sh+(1-ret.sh)))
    vc_assert_mult(x, m, ret)
    vc_assert_mult(x, VcShare.constant(rt, 1)-ret, VcShare.zero(rt))
    
@viffinlinecb
def eqz_local(rt, x, k=-1):
    yield declareReturn(rt, rt.Zp)
    xval = yield x
    returnValue(Share(rt, x.field, x.field(0 if xval==0 else 1)))
    
@vc_certificate(eqz_check)
@localversion(eqz_local)
def eqz(rt, x, k=-1):
    
    @viffinlinecb
    def eqz_int(rt, x, k=-1): # need int, otherwise vc_certificate and localversion also apply to subfunction
        yield declareReturn(rt, rt.Zp)
        
        if k==1: viff.inlinecb.returnValue(x)
        if k==2: viff.inlinecb.returnValue(1-(x-1)*(x-2)*(x-3)*(~x.field(-6)))
        if k==-1: k = rt.options.bit_length
        
        r_bits = [rt.prss_share_random(x.field, True) for _ in xrange(k)]
        r_divl = rt.prss_share_random_max(x.field, 2**rt.options.security_parameter)

        c = yield rt.open(x + sum([(2**i)*r_bits[i] for i in xrange(k)]) + (2**(k))*r_divl)
        c_bits = [x.field(c.bit(i)) for i in xrange(k)]
        
        d = sum([c_bits[i]+r_bits[i]-2*c_bits[i]*r_bits[i] for i in xrange(k)])
        
        rec = yield eqz_int(rt, d, k.bit_length())    
        returnValue(rec)
        
    return eqz_int(rt, x, k)

@qap_wrapper("subroutine")
def sub(runtime, v):
    return v*v

@qap_wrapper("pre")
def pre(runtime, a, i):
    vcs = vc_input_online(runtime, "in"+str(i), 1, 2, [10,11])
    sq = (vcs[0]+vcs[1])*(sub(runtime,vcs[0])+vcs[1])+a
    return sq

@qap_wrapper("main")
@vc_inlinecb
def run(runtime):
    yield declareVcReturn(runtime)
    
    val = VcShare.constant(runtime, 3)
    t = eqz(runtime, pre(runtime, val, 1))+1
    
    print "Result =", t
    
    returnValue(t)

vcrun(run, fp=False)
