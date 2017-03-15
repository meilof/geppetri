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

#include <fstream>
#include <getopt.h>

#include <libqap/base.h>
#include <libqap/qap.h>
#include <libqap/qap2key.h>

#include <gmpxx.h>
#define BN_SUPPORT_SNARK
#define MIE_ATE_USE_GMP
#include <bn.h>

using namespace bn;

int main(int argc, char** argv) {
    libqap_init();

    int ninp = 1, logd = 10;

    while (1) {
        static struct option long_options[] = {
          {"inputters", required_argument, NULL, 'i' },
          {"size",      required_argument, NULL, 's' },
          {"help",      no_argument,       NULL, 'h' },
          {0, 0, 0, 0}
        };
        int c = getopt_long(argc, argv, "i:p:s:h", long_options, NULL);

        if (c==-1) break; // end of options

        switch (c) {
        case 0: // long option matched for which no variable is set
            printf("Opt 0\n");
            break;
        case 'i':
            ninp = atoi(optarg);
            printf("Number of inputters %d\n", ninp);
            break;
        case 's':
            logd = atoi(optarg);
            printf("Max proof size 2^%d\n", logd);
            break;
        case 'h': // long "help" or short "h" option
        case '?': // unknown option (getopt will print error)
            printf("Help!!!\n");
            return 2;
        default:
            cerr << "*** Unexpected getopt return: " << c << endl;
            return 2;
        }
    }

    // determine number of roots
    int nroots = 1;
    while (logd--) nroots <<= 1;

    modp s;

    ofstream mkeyfile("../viff/data/geppmasterkey");
    masterkey mk;
    generate_master_key(nroots, ninp, s, mk);
    mkeyfile << mk;
    mkeyfile.close();

    ofstream msk("../viff/data/geppmastersk");
    msk << s;
    msk.close();

    return 0;
}
