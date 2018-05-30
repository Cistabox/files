// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
		(0, uint256("0x0000afe0b270088c11be8ab77e2c15b320ecc4eae7cc692ad75ed0ae606c01b9"))
		(3, uint256("0x0000d3a033b206ffbf80636e86e1253acd21c769a649d8ee788f13ae9303f66b"))
		(9, uint256("0x000030b10c0eeff7dde366665c49ed6baebc4cb893282d1172a3ce46c8a4b7c7"))
		(18, uint256("0x000027d3bf65ad54e5303ae0b7ee4c5632657a686494b7ab79de0ae097ccf841"))
		(38, uint256("0x0000020053bf9615158a1ed8cfe82474acb868e4bd2cbaef779e23d3c9926128"))
		(95, uint256("0x000001da148f4889e5810852dc5f03ebea6dd5511df0e8e8dd11585ba78864fb"))
		(186, uint256("0x0000005ce59f6db493dbab175d43e22c79d99fc54748fb7a5ad1550d8616d286"))
		(589, uint256("0x000000e1f94f2537f4dbd41b3c98d3bc6350bb554de8f82250ee30d33aed89e5"))
		(2789, uint256("0x708c29c8055cb98029eee8acebf9599a4c97a701db066e6a0e422ea2568ccbfb"))
		(3022, uint256("0x0000014b8bee3f907b219508d945599cca1aeb64f27f1e9453fcb5f6bc1434a9"))
		;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
