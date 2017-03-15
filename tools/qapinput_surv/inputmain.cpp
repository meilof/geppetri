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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <getopt.h>
#include <time.h>

#include <libqap/base.h>
#include <libqap/key.h>
#include <libqap/proof.h>
#include <libqap/prove.h>
#include <libqap/qap2key.h>

using namespace std;



int main(int argc, char** argv) {
    libqap_init();

    string keyfile = "../viff/data/geppmasterkey";
    string datafile = argv[1];
    int perb = atoi(argv[2]);
    int nbl = atoi(argv[3]);
    int inputter = atoi(argv[4]);

    masterkey mkey;
    ifstream fkeyfile(keyfile);
    fkeyfile >> mkey;
    fkeyfile.close();

    ifstream fdatafile(datafile);

    cerr << "Building inputs for party " << inputter << endl;
    clock_t clbeg = clock();

    vector<modp> allvals;
    string tok;

    for (int i = 0; i < (perb+1)*nbl; i++) {
        fdatafile >> tok;
        allvals.push_back(modp(tok));
    }

    cerr << "Read " << (perb+1)*nbl << " entries" << endl;

    for (int i = 0; i < nbl; i++) {
        vector<modp> vals;
        modp rnd;

        for (int j = 0; j < perb; j++) vals.push_back(allvals[i*perb+j]);
        rnd = allvals[nbl*perb+i];

        stringstream outnm; outnm << "../viff/data/geppblk.input." << inputter << ".comm" << "." << (i+1);
        cerr << outnm.str() << endl;
        ofstream curout(outnm.str());

        curout << buildblock(mkey, inputter-1, vals, rnd);

        curout.close();
    }

    clock_t clend = clock();
    fprintf (stderr, " ... took %ld clicks (%f seconds)\n",clend-clbeg,((float)(clend-clbeg))/CLOCKS_PER_SEC);



    return 0;
}
