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
from viff.inlinecb import returnValue
from viffvc.vcruntime import qap_wrapper, declareVcReturn, vc_input_predist, vcrun, vc_inlinecb

@qap_wrapper("calc1")
def calc1(runtime,di1,di2):
    di1p = 2*di1*(di1+1+di2)    
    dis1 = calc2(runtime,di1p)
    dis2 = calc2(runtime,di2)
    return dis1+dis2

@qap_wrapper("calc2")
def calc2(runtime,di):
    dip = 3*di*(2+di)
    return di*dip
    
@qap_wrapper("main")
@vc_inlinecb
def run(runtime):
    yield declareVcReturn(runtime)

    print "Welcome"
    
    vcs1 = vc_input_predist(runtime, "input11")
    sq1 = (vcs1[0]+vcs1[1])*(vcs1[0]+vcs1[1])
    
    vcs2 = vc_input_predist(runtime, "input12")
    sq2 = (vcs2[0]+vcs2[1])*(vcs2[0]+vcs2[1])
    
    ret = calc1(runtime,sq1,sq2)
    
    print "Done"
    
    returnValue(ret)

vcrun(run)
