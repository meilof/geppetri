/**
 *
 * Copyright (c) 2016-2017 Koninklijke Philips N.V. All rights reserved. A
 * copyright license for redistribution and use in source and binary forms,
 * with or without modification, is hereby granted for non-commercial,
 * experimental and research purposes, provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimers.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimers in the
 *   documentation and/or other materials provided with the distribution. If
 *   you wish to use this software commercially, kindly contact
 *   info.licensing@philips.com to obtain a commercial license.
 *
 * This license extends only to copyright and does not include or grant any
 * patent license or other license whatsoever.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "base.h"
#include "key.h"
#include "qap.h"

using namespace std;

void generate_master_key(unsigned int nroots, unsigned int ninp, modp& s, masterkey& mk) {
    Ec1 tmp1;
    Ec2 tmp2;
    modp curfac("1");

    s = modp::rand();
    modp rc = modp::rand();
    vector<modp> als(ninp);
    for (unsigned int j = 0; j < ninp; j++) als[j] = modp::rand();

    mk.g_als = vector<Ec2>(ninp);
    for (unsigned int j = 0; j < ninp; j++) mk.g_als[j] = g2^als[j];

    mk.g_s = vector<Ec1>(nroots+1);
    mk.g2_s = vector<Ec2>(nroots+1);
    mk.g_rcs = vector<Ec1>(nroots+1);
    mk.g_rcalcs = vector<vector<Ec2>>(ninp);
    for (unsigned int j = 0; j < ninp; j++) mk.g_rcalcs[j] = vector<Ec2>(nroots+1);

    for (unsigned int i = 0; i < nroots+1; i++) {
        mk.g_s[i] = g1^curfac;
        mk.g2_s[i] = g2^curfac;
        mk.g_rcs[i] = g1^(curfac*rc);
        for (unsigned int j = 0; j < ninp; j++) mk.g_rcalcs[j][i] = g2^(curfac*rc*als[j]);
        curfac = curfac*s;
    }

    //modp tar = curfac-1; // value of the target polynomial in s
    //mk.g_t = g1^tar;
    //mk.g2_t = g2^tar;
}

vector<modp> compute_lagcofs(unsigned int nroots, unsigned int ncofs, const modp& s) {
    modp gen    = modp(5)^(modp(-1)/nroots);
    modp lambda = ((s^nroots)-1)*modp(nroots).inv();
    modp root   = 1;

    vector<modp> ret = vector<modp>();

    for (unsigned int cur = 0; cur < ncofs; cur++) {
        ret.push_back((s-root).inv()*lambda);
        root        = root*gen;
        lambda      = lambda*gen;
    }

    return ret;
}

void interpolate(const modp& lagcof,
                 const qapeq& eq,
                 map<string,modp>& vis,
                 map<string,modp>& wis,
                 map<string,modp>& yis) {
    for (auto const& vcur: eq.v) {
        vis[vcur.te] = vis[vcur.te]+lagcof*vcur.co;
    }
    for (auto const& wcur: eq.w) {
        wis[wcur.te] = wis[wcur.te]+lagcof*wcur.co;
    }
    for (auto const& ycur: eq.y) {
        yis[ycur.te] = yis[ycur.te]+lagcof*ycur.co;
    }
}


void qap2key(const qap& theqap, const masterkey& mk, modp& s, qapek& ret1, qapvk& ret2) {
    unsigned int neq = theqap.eqs.size();

    unsigned int qapdeg = 1; while (qapdeg < neq) qapdeg <<= 1;
    modp tar("1");
    for (unsigned int i = 0; i < qapdeg; i++) tar = tar*s;
    tar = tar-1;

    cerr << "Using QAP degree=" << qapdeg << endl;

    modp alv = modp::rand(),  alw = modp::rand(), aly = modp::rand(),
         rv=1, rw=1, ry=1, //rv = modp::rand(),   rw = modp::rand(),  ry = rv*rw,
         rvav = rv*alv,       rwaw = rw*alw,      ryay = ry*aly,
         beta = modp::rand(),
         rvb = rv*beta,       rwb = rw*beta,      ryb = ry*beta;

    ret1.g_rvt   = g1^(tar*rv);
    ret1.g_rvavt = g2^(tar*rvav);
    ret1.g2_rwt  = g2^(tar*rw);
    ret1.g_rwawt = g1^(tar*rwaw);
    ret1.g_ryt   = g1^(tar*ry);
    ret1.g_ryayt = g2^(tar*ryay);
    ret1.g_beta  = g1^beta;
    ret1.g_rvbt  = g1^(tar*rvb);
    ret1.g_rwbt  = g1^(tar*rwb);
    ret1.g_rybt  = g1^(tar*ryb);

    ret2.g2alv   = g2^alv;
    ret2.g1alw   = g1^alw;
    ret2.g2aly   = g2^aly;
    ret2.g2ryt   = g2^(tar*ry);
    ret2.g1bet   = g1^beta;
    ret2.g2bet   = g2^beta;


    map<string,modp> vis = map<string,modp>();
    map<string,modp> wis = map<string,modp>();
    map<string,modp> yis = map<string,modp>();

    vector<modp> lagcofs = compute_lagcofs(qapdeg, neq, s);

    int cur = 0;
    for (auto const& eq: theqap.eqs) interpolate(lagcofs[cur++], eq, vis, wis, yis);

    // generate evaluation keys for all values
    unordered_set<string> done = unordered_set<string>();
    for (map<string,modp>::iterator iter = vis.begin(); iter != yis.end(); ) {
        if (iter == vis.end()) { iter = wis.begin(); continue; }
        if (iter == wis.end()) { iter = yis.begin(); continue; }

        if (done.find(iter->first) == done.end()) {
            done.insert(iter->first);

            modp& vval = vis[iter->first], wval = wis[iter->first], yval = yis[iter->first];

            if (iter->first == "one" || (iter->first.c_str()[0] == 'o' && iter->first.c_str()[1] == '_')) {
                wirevk wvk;
                wvk.g_rvvk = g1^(rv*vval);
                wvk.g_rwwk = g2^(rw*wval);
                wvk.g_ryyk = g1^(ry*yval);
                ret2.pubinputs[iter->first] = wvk;
            } else {
                wireek ek;
                ek.g_rvvk   = g1^(rv*vval);
                ek.g_rwwk   = g2^(rw*wval);
                ek.g_ryyk   = g1^(ry*yval);
                ek.g_rvavvk = g2^(rvav*vval);
                ek.g_rwawwk = g1^(rwaw*wval);
                ek.g_ryayyk = g2^(ryay*yval);
                ek.g_rvvkrwwkryyk = g1^((rv*vval+rw*wval+ry*yval)*beta);
                ret1.wires[iter->first] = ek;
            }
        }

        iter++;
    }

    // generate keys for all block types

    int totix = 1; // current index in overall beta block

    for (auto const& blk: theqap.blocks) {
//        cerr << "Generating block " << blk.first << ", start=" << totix << endl;
        unsigned int sz = blk.second.wires.size();
        modp cural = modp::rand(), curbet = modp::rand();

        blockek ek;
        ek.gstart = totix;
        ek.g2als = vector<Ec2>(sz);
        ek.g1betas = vector<Ec1>(sz);
        ek.g2al = g2^cural;
        ek.g1betar1 = mk.g_rcs[0]^curbet;
        ek.g1betar2 = g1^curbet;
        for (unsigned int i = 0; i < sz; i++) {
            ek.g2als[i] = mk.g2_s[totix]^cural;
            ek.g1betas[i] = (mk.g_s[totix]+mk.g_rcs[i+1])^curbet;
            ret1.wires[blk.second.wires[i]].g_rvvkrwwkryyk += mk.g_s[totix]^beta;
            totix++;
        }
        ret1.blocks[blk.first] = ek;

        blockvk vk;
        vk.g2al = g2^cural;
        vk.g2beta = g2^curbet;
        ret2.blocks[blk.first] = vk;
    }
}
