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

# test if all equations hold

p=21888242871839275222246405745257275088548364400416034343698204186575808495617

vals = {}
vals["one"] = 1 # constant

for ln in open("geppout1", "r"):
  ln = ln.strip()
  if ln=="" or ln[0]=="#" or ln=="[data]": continue
  (var,val) = ln.split(" ")
  var = var[:-1]
  vals[var] = long(val)
#
#for ln in open("qapi2", "r"):
#  (var,val) = ln.strip().split(" ")
#  vals[var] = long(val)

for ln in open("geppval1", "r"):
  ln = ln.strip()
  if ln=="" or ln[0]=="#": continue
  (var,val) = ln.split(" ")
  var = var[:-1]
  vals[var] = 2*long(val)
  
for ln in open("geppval2", "r"):
  ln = ln.strip()
  if ln=="" or ln[0]=="#": continue
  (var,val) = ln.split(" ")
  var = var[:-1]
  vals[var] = (vals[var] - long(val) + p) % p
  #print var, vals[var]
  
def val(str):
    global vals
    lst = str.strip().split(" ")
    return sum([long(c)*vals[v] for (c,v) in zip(lst[0::2],lst[1::2])])

for (ix,ln) in enumerate(open("geppeq1")):
    ln = ln.strip()
    if ln[0]=="[" or ln[0]=="#": continue
    lhs,rhs = ln.split("=")
    t1,t2 = lhs.strip().split("*")
    eval = ((val(t1)%p)*(val(t2)%p)-val(rhs))%p
    if eval!=0:
        print "*** line", ix, "gave non-zero value:", eval
        print ln,
        
print "Tested all", ix+1, "lines"