// This file is automatically generated from ethereum_networks.h.mako
// DO NOT EDIT

#ifndef __ETHEREUM_NETWORKS_H__
#define __ETHEREUM_NETWORKS_H__

#define ASSIGN_ETHEREUM_SUFFIX(suffix, chain_id) \
			switch (chain_id) { \
				case          1: suffix = " ETH";   break;  /* Ethereum */ \
				case          2: suffix = " EXP";   break;  /* Expanse */ \
				case          3: suffix = " tROP";  break;  /* Ethereum Testnet Ropsten */ \
				case          4: suffix = " tRIN";  break;  /* Ethereum Testnet Rinkeby */ \
				case          5: suffix = " tGOR";  break;  /* Ethereum Testnet Goerli */ \
				case          8: suffix = " UBQ";   break;  /* Ubiq */ \
				case         28: suffix = " ETSC";  break;  /* Ethereum Social */ \
				case         30: suffix = " RBTC";  break;  /* RSK */ \
				case         31: suffix = " tRBTC"; break;  /* RSK Testnet */ \
				case         42: suffix = " tKOV";  break;  /* Ethereum Testnet Kovan */ \
				case         56: suffix = " BNB";   break;  /* Binance Smart Chain */ \
				case         60: suffix = " GO";    break;  /* GoChain */ \
				case         61: suffix = " ETC";   break;  /* Ethereum Classic */ \
				case         62: suffix = " tETC";  break;  /* Ethereum Classic Testnet */ \
				case         64: suffix = " ELLA";  break;  /* Ellaism */ \
				case         76: suffix = " MIX";   break;  /* Mix */ \
				case        237: suffix = " DXN";   break;  /* DEXON */ \
				case        820: suffix = " CLO";   break;  /* Callisto */ \
				case       1620: suffix = " ATH";   break;  /* Atheios */ \
				case       1987: suffix = " EGEM";  break;  /* EtherGem */ \
				case       2018: suffix = " EOSC";  break;  /* EOS Classic */ \
				case       2894: suffix = " REOSC"; break;  /* REOSC Ecosystem */ \
				case      31102: suffix = " ESN";   break;  /* Ethersocial Network */ \
				case     200625: suffix = " AKA";   break;  /* Akroma */ \
				case     246529: suffix = " ATS";   break;  /* ARTIS sigma1 */ \
				case     246785: suffix = " tATS";  break;  /* ARTIS tau1 */ \
				case    1313114: suffix = " ETHO";  break;  /* Ether-1 */ \
				case    7762959: suffix = " MUSIC"; break;  /* Musicoin */ \
				case 3125659152: suffix = " PIRL";  break;  /* Pirl */ \
				default: suffix = " UNKN"; break;  /* unknown chain */ \
			}

#endif
