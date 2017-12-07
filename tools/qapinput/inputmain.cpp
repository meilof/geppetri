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

    string keyfile = "geppmasterkey.ver";
    int inputter = 1;

    while (1) {
        static struct option long_options[] = {
          {"keyfile",  required_argument, NULL,   'k' },
          {"inputter", required_argument, NULL,   'i' },
          {"help",     no_argument,       NULL,   'h' },
          {0, 0, 0, 0}
        };
        int c = getopt_long(argc, argv, "k:d:s:h", long_options, NULL);

        if (c==-1) break; // end of options

        switch (c) {
        case 'k':
            keyfile = optarg;
            break;
        case 'i':
            inputter = atoi(optarg);
            break;
        case 'h': // long "help" or short "h" option
        case '?': // unknown option (getopt will print error)
            cerr << "Help!!!!" << endl;
            return 2;
        default:
            cerr << "*** Unexpected getopt return: " << c << endl;
            return 2;
        }
    }

    if (argc-optind != 1) {
        cerr << "*** Expected input file" << endl;
        return 2;
    }

    string datafile = argv[optind];

    cerr << "Building input: keyfile=" << keyfile << ", datafile=" << datafile << ", inputter=" << inputter << endl;

    masterkey mkey;
    ifstream fkeyfile(keyfile);
    fkeyfile >> mkey;
    fkeyfile.close();

    vector<modp> vals;
    ifstream fdatafile(datafile);
    string tok;
    while ((fdatafile>>tok) && tok!="") {
        vals.push_back(modp(tok));
    }
    fdatafile.close();

    modp rnd = modp(vals.back());
    vals.pop_back();

    datablock db = buildblock(mkey, inputter-1, vals, rnd);

    cout << db;

    return 0;
}
