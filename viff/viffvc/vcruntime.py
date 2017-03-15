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
import time
from twisted.internet import reactor, defer

from viff.config import load_config
from viff.equality import ProbabilisticEqualityMixin
from viff.field import GF, FieldElement
from viff.inlinecb import viffinlinecb
from viff.runtime import Share, gather_shares, create_runtime, Runtime, make_runtime_class
from viff.util import rand


# global data

vc_directory = None        # folder where VC files live 
my_suffix = None           # suffix for my shares of the files
vc_modulus = None          # modulus for VC computations
qap = None                 # qap equation file (only for key generation)
qapo = None                # qap output file
qapv = None                # qap wire value file


def vc_init(runtime = None, directory="data/", printeq=True):
    """ Initialize verifiable computation system, storing all intermediate
    files in the given directory.
    
    If no runtime is given, the VC system can only be used for secret-sharing
    input data.
    
    If printeq is True, print equations to a file (only needed for key generation). """
    
    global vc_directory, my_suffix, vc_modulus, qap, qapo, qapv
    
    print "*** Initializing VC (data dir=" + directory + ")"
    
    vc_directory = directory
    if vc_directory[-1]!="/": vc_directory = vc_directory + "/"
    
    my_suffix = "" if runtime==None or runtime.__class__.__name__=="LocalRuntime" else str(runtime.id)
    
    vc_modulus = 21888242871839275222246405745257275088548364400416034343698204186575808495617 
    
    if runtime:        
        if printeq: qap = open(vc_directory + "geppeq" + my_suffix, "w")
        print >>qap, "# geppetri qap"
        qapo = open(vc_directory + "geppout" + my_suffix, "w")
        print >>qapo, "# geppetri qap outputs"
        qapv = open(vc_directory + "geppval" + my_suffix, "w")
        print >>qapv, "# geppetri qap values"

        runtime.vc_counter = []
        runtime.Zp = GF(vc_modulus, runtime.res) if hasattr(runtime, "res") else GF(vc_modulus)
        return runtime
    
def vc_start(runtime, fname, call):
    """ Start a new call of the given function name. The call name should 
        be unique. """
    if qap!=None: print >>qap, "[function]", fname, call
    
    runtime.vc_counter=[call, 0]
    
    
    def printwire(val, nm):
        print >>qapv, nm+":", val.value
        qapv.flush()
    runtime.prss_share_random(runtime.Zp).addCallback(printwire, call+"/deltav") 
    runtime.prss_share_random(runtime.Zp).addCallback(printwire, call+"/deltaw")
    runtime.prss_share_random(runtime.Zp).addCallback(printwire, call+"/deltay")
    
    if qap!=None: print >>qap, "1",  call+"/one", "*", "1", call+"/one", "=", "1", call+"/one", "."
    print >>qapv, call+"/one:", "1"

def vc_declare_block(runtime, vcs, rnd1, bn=None):
    """ Declares a VC block. Return block information tuple. """
    global qap

    if bn==None: bn = ".".join(map(str,runtime.vc_counter[1:]))
    vcs = map(lambda x: x.ensure_single(runtime), vcs)
    rnd2 = VcShare.random(runtime)
    runtime.vc_counter[-1]+=1
    
    print >>qap, "[ioblock]", runtime.vc_counter[0], bn, " ".join(map(lambda x: x.sig[0][1], [rnd1,rnd2]+vcs))
    qap.flush()
    
    return (runtime.vc_counter[0],bn,vcs,rnd1,rnd2)
    
def vc_import(runtime, (qap1,bn1,vcs1,rnd11,rnd12)):
    """ Import a VC block into a new context. Return list of shares and block information. """
    (qap2,bn2,vcs2,_,_) = vc_declare_block(runtime, [VcShare.from_share(runtime, x.sh) for x in vcs1], VcShare.from_share(runtime, rnd11.sh))
    print >>qap, "[glue]", qap1, bn1, qap2, bn2
    qap.flush()
    return vcs2

def vc_read_values(infile):
    """ Return a list of all integer values from a file. """
    lns = [ln for ln in open(infile) if ln!="" and ln[0]!="#"]
    lines = [ln.split() for ln in lns]
    toks = [tok for toks in lines for tok in toks]
    ints = map(int, toks)
    return ints

def vc_share_offline(blockname, vals, nrnd=1):
    """ Pre-share block with given name consisting of given values. """
    out = open(vc_directory+"geppblk."+blockname, "w")
    out1 = open(vc_directory+"geppblk."+blockname+"1", "w")
    out2 = open(vc_directory+"geppblk."+blockname+"2", "w")
    out3 = open(vc_directory+"geppblk."+blockname+"3", "w")
    
    def writeshare(val,tosk=False):
        r = rand.randint(0, vc_modulus)
        print >>out, val
        print >>out1, (val+r)%vc_modulus
        print >>out2, (val+2*r)%vc_modulus
        print >>out3, (val+3*r)%vc_modulus
        
    map(writeshare, vals)
    for i in xrange(nrnd):
        writeshare(rand.randint(0, vc_modulus))
    
def vc_input_predist(runtime, blockname):
    """ Import inputs that were pre-distributed via vc_share_offline. Returns VcShares. """
    vals = vc_read_values(vc_directory+"geppblk."+blockname+my_suffix)
    importer = lambda x: VcShare.from_share(runtime, Share(runtime, runtime.Zp, runtime.Zp(x)))
    vcs = map(importer, vals[:-1])
    rnd1 = importer(vals[-1])
    (qp,bn,_,_,_) = vc_declare_block(runtime, vcs, rnd1)
    print >>qap, "[input]", qp, bn, blockname
    return vcs

def vc_read_additive_shares(runtime, Zp, blockname, inputters):
    allvals = map(vc_read_values, [vc_directory+"geppblk."+blockname+"."+str(curid)+my_suffix for curid in inputters])
    vals = map(lambda x: Share(runtime, Zp, Zp(sum(x)%vc_modulus)), zip(*allvals))
    #print "vals", vals
    return vals

def vc_import_additive_shares(runtime, shs, start, ln, blockix, nblocks):
    import_to_context = lambda x: VcShare.from_share(runtime, x)    
    return map(import_to_context, shs[start:start+ln]), import_to_context(shs[len(shs)-nblocks+blockix])
    
def vc_input_online(runtime, blockname, inputter, nvals, values=None):
    """ Share a given block provided by the given inputter between the parties.
        Return VcShares of values."""
    if runtime.id==inputter:
        # write share file
        allvals = values+[rand.randint(0, vc_modulus)]
        
        outf = open(vc_directory+"geppblk."+blockname, "w")
        for x in allvals: print >>outf, x
        outf.close()
        
        shs = [runtime.shamir_share([inputter], runtime.Zp, val) for val in allvals]
    else:
        shs = [runtime.shamir_share([inputter], runtime.Zp, None) for _ in xrange(nvals+1)]
        
    vcs = map(lambda x: VcShare.from_share(runtime, x), shs)
    (qp,bn,vcsp,_,_)=vc_declare_block(runtime, vcs[:-1], vcs[-1])
    print >>qap, "[input]", qp, bn, blockname 
    return vcsp

def vc_output_open(runtime, (qp,bn,vcs,rnd1,rnd2)):
    """ Opens a given block as computation output. """
    vals = [vc.open() for vc in vcs]
    rnd1.open()
    print >>qap, "[output]", qp, bn, " ".join(map(lambda x: x.sig[0][1], [rnd1]+vcs))
    qap.flush()
    return vals
    
def vc_assert_mult(v,w,y):
    """ Add QAP equation asserting that v*w=y. """
    if qap!=None:
        print >>qap, v.strsig(), "*", w.strsig(), "=", y.strsig(), "."
        qap.flush()
        
class VcShare:
    """ Encapsulates a share representing a linear combination of QAP witnesses. """
    
    def __init__(self, sh, sig):
        """ Constructor. """
        self.sh = sh
        self.sig = sig
        
    @classmethod
    def from_share(cls, rt, sh):
        """ Initialize from Share, assign new ID and make sure is printed. """
        
        rt.vc_counter[-1] += 1
        sid = "/".join(map(str,rt.vc_counter[:]))        
        def printwire(val):
            print >>qapv, sid+":", val.value
            qapv.flush()
            return val
        sh.addCallback(printwire)
        return cls(sh, [(1,sid)])

    @classmethod
    def deferred(cls, rt):
        """ Create a VcShare whose value will later be set by a callback. """
        return cls.from_share(rt, Share(rt, rt.Zp))
    
    def callback(self, val):
        """ Equate this VcShare to the VcShare val given as argument. """
        if not isinstance(val,VcShare): raise TypeError("VcShare.callback called with val of incorrect type '{}'".format(type(val)))

        if isinstance(val.sh, defer.Deferred):
            val.sh.chainDeferred(self.sh)
        else:
            self.sh.callback(val.sh)
        
        if qap!=None:
            print >>qap, "* = -1", self.sig[0][1], val.strsig()
            qap.flush()
            
        return val

    def errback(self, val):
        """ Error callback. """
        pass    

    def strsig(self):
        """ Return string representation of linear combination represented by this VcShare. """
        return " ".join(map(lambda (c,v): str(c)+" "+v, self.sig))

    def __repr__(self):
        """ Return string representation of this VcShare. """
        return "VcShare(" + self.strsig() + (":"+str(self.sh.result) if hasattr(self.sh, 'result') else "") + ")"
    
    def ensure_single(self, rt):
        """ Return a VcShare with the same value that is guaranteed to refer
            to one witness, by making a new VcShare and adding the required
            equation if necessary. """
        if len(self.sig)==1 and self.sig[0][0]==1: return self
        
        ret = VcShare.from_share(rt, self.sh)
        if qap!=None:
            print >>qap, "*", "=", self.strsig(), "-1", ret.sig[0][1]
            qap.flush()
            
        return ret
    
    @classmethod
    def zero(cls, rt):
        """ Return a VcShare representing the value zero. """
        return VcShare(Share(rt, rt.Zp, rt.Zp(0)), [])
    
    @classmethod
    def constname(self, rt):
        return rt.vc_counter[0] + "/one"
    
    @classmethod
    def constant(cls, rt, val):
        """ Return a VcShare representing the given constant value. """
        return VcShare(Share(rt, rt.Zp, rt.Zp(val)), [(val, cls.constname(rt))])

    @classmethod
    def random(cls, runtime):
        """ Return a VcShare representing a random value. """
        sh = runtime.prss_share_random(runtime.Zp)
        return cls.from_share(runtime, sh)

    def __neg__(self):
        """ Returns negated VcShare. """
        return VcShare(-self.sh, [(-c,v) for (c,v) in self.sig])
        
    def __add__(self, other):
        """ Add VcShare or constant to self. """
        if other==0: return self
        if isinstance(other,int): other=self.sh.field(other)
        if isinstance(other,FieldElement):
            return VcShare(self.sh+other, self.sig+[(other.value,self.constname(self.sh.runtime))])
        elif isinstance(other,VcShare):
            return VcShare(self.sh+other.sh, self.sig+other.sig)        
        else:
            raise TypeError("unsupported operand type(s) for VcShare.+: '{}' and '{}'".format(self.__class__, type(other)))
    __radd__ = __add__
            
    def __sub__(self, other):
        """ Subtract VcShare or constant from self. """
        if isinstance(other,FieldElement):
            return VcShare(self.sh-other, self.sig+[(-other.value,self.constname(self.sh.runtime))])
        elif isinstance(other,VcShare):
            return self+(-other)
        else:
            raise TypeError("unsupported operand type(s) for VcShare.-: '{}' and '{}'".format(self.__class__, type(other)))
            
    def __rsub__(self, other):
        return -self + other
    
    def __mul__(self, other):
        """ Multiply VcShare with other VcShare or constant. """
        global qap
        
        if isinstance(other,FieldElement):
            return VcShare(self.sh*other, [(c*other.value,v) for (c,v) in self.sig])
        elif isinstance(other,int):
            return VcShare(self.sh*other, [(c*other%vc_modulus,v) for (c,v) in self.sig])
        elif isinstance(other,VcShare):
            mr = self.sh.runtime.mul(self.sh, other.sh,False) if hasattr(self.sh, "fp") else self.sh.runtime.mul(self.sh, other.sh)
            res = VcShare.from_share(self.sh.runtime, mr)
            vc_assert_mult(self, other, res)
            return res
        else:
            raise TypeError("unsupported operand type(s) for VcShare.*: '{}' and '{}'".format(self.__class__, type(other)))        
    __rmul__ = __mul__
            
        
    def assert_zero(self):
        """ Assert that the present VcShare represents the value zero. """
        global qap
        if qap!=None:
            print >>qap, "* =", self.strsig(), "."
            qap.flush()
            
    def assert_positive(self, bl):
        """ Assert that the present VcShare represents a positive value, that
            is, a value in [0,2^bl] with bl the given bit length. """
        global qap
        
        rt = self.sh.runtime
        bits = rt.bit_decompose(self.sh, bl)
        
        # small optimization: instead of making vc shares for everything, we do things explicitly via the secret shares
        rt.vc_counter[-1] += 1
        sid = "/".join(map(str,rt.vc_counter[:])) 
        def printwires(vals):
            for (ix,val) in enumerate(vals):
                print >>qapv, sid+"b"+str(ix)+":", val.value
                qapv.flush()
        gather_shares(bits).addCallback(printwires)
            
        if qap!=None:
            for i in range(bl): print >>qap, "1", sid+"b"+str(i), "* 1", self.constname(self.sh.runtime), "-1", sid+"b"+str(i), "= ."                           # all values are bits        
            print >>qap, self.strsig(), " ".join([str(-(2**i))+" "+sid+"b"+str(i) for i in xrange(len(bits))]), "* 1", self.constname(self.sh.runtime), "= ."   # bit decompositon is correct
            qap.flush()

#     def tofp_noconv(self):
#         @viff.inlinecb.inlinecb2
#         def tofp(runtime, sh, fp=True):
#             yield Share(runtime, sh.field, None, False)
#             sh = yield sh
#             viff.inlinecb.returnValue(sh.field(sh.value))
#         return VcShare(tofp(self.sh.runtime, self.sh, False), self.sig)
            
    def tofp_conv(self):
        """ Returns a VcShare representing a fixed-point representation of the
            given value. """
        rt=self.sh.runtime
        selfp=self*(2**rt.options.res)
        return VcShare(rt.clone_share_fp(selfp.sh),selfp.sig)

    def nofp(self, rt):
        """ Returns a VcShare representing the same witnesses, where the
            share does not have the "fp" flag set. """
        return VcShare(rt.clone_share_nofp(self.sh), self.sig)

    def __div__(self, other):
        if not isinstance(other,VcShare):
            raise TypeError("unsupported operand type(s) for VcShare./: '{}' and '{}'".format(self.__class__, type(other)))
        
        rt = self.sh.runtime
        frc = VcShare.from_share(rt, rt.div(self.sh,other.sh))
        
        # prove correctness of divisions
        den=other.nofp(rt)
        df=(2**self.sh.runtime.options.res)*self.nofp(rt)-frc.nofp(rt)*den  # division error: should be in [-den,+den]      
        (2*den-df).assert_positive(40)                                      # bit decomposition proofs that value is positive
        (den+df).assert_positive(40)                                        # bit decomposition proofs that value is positive
        
        return frc
        
#     def printo(self):
#         self.sh.addCallback(printwireout, self.sig[0][1])
    
    def open(self):
        """ Opens the given VcShare. Return Deferred for result ."""
        rt = self.sh.runtime        
        sid = self.ensure_single(rt).sig[0][1]            
        op = rt.open(self.sh)
        def printwireout(val):
            global qapo
            print >>qapo, sid+":", val.value
            qapo.flush()
            return val
        op.addCallback(printwireout)
        return op
        
    def eqz(self):
        """ Returns VcShare equal to 1 if self is not zero, and 0 if self is zero. """
        rt = self.sh.runtime
        
        ret = VcShare.from_share(rt.eqz(self.sh))
        
        m = VcShare.from_share(rt, rt.invert(self.sh+(1-ret.sh)))
        vc_assert_mult(self, m, ret)
        vc_assert_mult(self, VcShare.constant(rt, 1)-ret, VcShare.zero(rt))
                
        return ret
    
        
def for_each_in(cls, f, struct):
    """ Recursively traversing all lists and tuples in struct, apply f to each
        element that is an instance of cls. Returns structure with f applied. """
    if isinstance(struct, list):
        return map(lambda x: for_each_in(cls, f, x), struct)
    elif isinstance(struct, tuple):
        return tuple(map(lambda x: for_each_in(cls, f, x), struct))
    else:
        if isinstance(struct, cls):
            return f(struct)
        else:
            return struct

def qap_wrapper(nm):
    """ Turns the given function into a subqap with the given name. The
        function should have the runtime as its first argument. Any other
        arguments and return values that are VcShares are imported to/exported
        from this function as glue. """
    def qap_wrapper_int(f):
        def wrapper(*args, **kwargs):
            runtime = args[0]
            prev_vc_counter = runtime.vc_counter
            
            # get all VcShare arguments
            argvcs = []
            for_each_in(VcShare, lambda x: argvcs.append(x), args)
            if argvcs!=[]: argexp = vc_declare_block(runtime, argvcs, VcShare.random(runtime))
            
            ctxnm = "_".join(map(str, runtime.vc_counter))+"_"+nm if runtime.vc_counter!=[] else nm
            vc_start(runtime, nm, ctxnm)
            
            # import VcShare arguments
            if argvcs!=[]:
                argimp = list(reversed(vc_import(runtime, argexp))) # reversed so we can pop
                args = for_each_in(VcShare, lambda x: argimp.pop(), args)
                
            # call
            ret = f(*args, **kwargs)
            
            # get all VcShare returns 
            retvcs = []
            for_each_in(VcShare, lambda x: retvcs.append(x), ret)            
            if retvcs!=[]: retexp = vc_declare_block(runtime, retvcs, VcShare.random(runtime))
            
            runtime.vc_counter = prev_vc_counter
            
            # import VcShare returns (if call from another function)
            if retvcs!=[]:
                if prev_vc_counter!=[]:
                    retimp = list(reversed(vc_import(runtime, retexp))) # reversed so we can pop
                    ret = for_each_in(VcShare, lambda x: retimp.pop(), ret)
                else:
                    # no previous context, assume this is an output block to be opened
                    retimp = list(reversed(vc_output_open(runtime, retexp))) # reversed so we can pop
                    ret = for_each_in(VcShare, lambda x: retimp.pop(), ret)
                
            return ret
            
        return wrapper
    return qap_wrapper_int            
            
def vcrun(f, fp=True, cb1=None, cb2=None):
    """ Verifiable computation entry point. Adding this to the end of the
        verifiable computation will load configurations, set up communication,
        and run the given function f. The qap_wrapper decorator should be
        applied to f, and f is assumed to give as public output the
        computation result. """
    
    parser = OptionParser()
    Runtime.add_options(parser)
    if cb1: cb1(parser) # callback to add custom options to parser 
    options, args = parser.parse_args()
    
    if len(args) < 1: parser.error("you must specify a config file")
    myid, players = load_config(args[0])
    
    if cb2: cb2(options, args[1:]) # handle parsing results
    
    if fp:
        from viff.comparison import Toft07Runtime
        from viff.division import DivisionSH12Mixin
        runtime_class = make_runtime_class(mixins=[DivisionSH12Mixin, ProbabilisticEqualityMixin, Toft07Runtime])
        pre_runtime = create_runtime(myid, players, options.threshold, options, runtime_class)    
        pre_runtime.addCallback(vc_init)
    else:
        pre_runtime = create_runtime(myid, players, options.threshold, options)
        pre_runtime.addCallback(vc_init)
        
    
    def callf(runtime):
        ret = f(runtime)
        
        retsh = []
        for_each_in(Share, lambda x: retsh.append(x), ret)
        
        def printAndExit(runtime, vals):
            valsrev = list(reversed(vals))
            retc = for_each_in(Share, lambda x: valsrev.pop(), ret)
            print "[ Verifiable computation result", retc, "]"
            
            if runtime.__class__.__name__!="LocalRuntime": time.sleep(1)
            runtime.shutdown()
            
            return runtime

        if retsh!=[]:
            # print all returned values and then shut down
            gather_shares(retsh).addCallback(lambda x: printAndExit(runtime, x))            
        else:
            if runtime.__class__.__name__!="LocalRuntime": time.sleep(1)
            runtime.shutdown()
        
            
        return runtime
    
    pre_runtime.addCallback(callf)
    reactor.run()


class VcEnvironment(object):
    """ Decorator inside which the VC counter is forked. """
    def __init__(self, f, runtime_getter=(lambda x: x[0])):
        self.my_runtime = None
        self.my_counter = None
        self.f = f
        self.runtime_getter = runtime_getter
        
    def __call__(self, *args, **kwargs):
        if self.my_runtime==None:
            # first time called, set up a new vc context for this call
            self.my_runtime = self.runtime_getter(args)
            
            if self.my_runtime.vc_counter!=[]: self.my_runtime.vc_counter[-1]+=1
                
            backup_counter = self.my_runtime.vc_counter[:]
            self.my_runtime.vc_counter.append(0)
        else:
            # next times, just restore backed-up contact
            backup_counter = self.my_runtime.vc_counter[:]
            self.my_runtime.vc_counter = self.my_counter    
                
        ret = self.f(*args, **kwargs)
        
        self.my_counter = self.my_runtime.vc_counter[:]
        self.my_runtime.vc_counter = backup_counter
        
        return ret

# TODO: what to do when return is fp???
def declareVcReturn(rt, *args):
    def make(ix):
        if ix>=len(args): return VcShare.deferred(rt)
        return [make(ix+1) for _ in xrange(args[ix])]
    return rt, make(0)
    
def vc_inlinecb(f):
    """ Inline callback variant for verifiable computation, taking care of the
        VC counter. """
    return viffinlinecb(VcEnvironment(f))


def vc_certificate(verf):
    """ Decorator for a computation on secret shares, whose correctness is
        verified with procedure verf on VcShares. """
    def vc_certificate_impl(compf):
        def runner(*argsvc, **kwargs):
            argssh = for_each_in(VcShare, lambda x: x.sh, argsvc)
            retsh = compf(*argssh, **kwargs)
            retvc = for_each_in(Share, lambda x: VcShare.from_share(argsvc[0], x), retsh)
            verf(argsvc, retvc)
            return retvc
            
        return runner
    
    return vc_certificate_impl

