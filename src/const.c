//
// Created by HaveYouTall
//
#include "const.h"


// ============================== BLS12-377 ShortW Parameters ============================

// bls12_377
// 258664426012969094010652733694893533536393512754914660539884262666720468348340822774968888139573360124440321458177
const uint64_t MODULUS[6] = {
    0x8508c00000000001,
    0x170b5d4430000000,
    0x1ef3622fba094800,
    0x1a22d9f300f5138f,
    0xc63b05c06ca1493b,
    0x1ae3a4617c510ea
}; 

const u48 _MODULUS_ = {
    0x8508c00000000001,
    0x170b5d4430000000,
    0x1ef3622fba094800,
    0x1a22d9f300f5138f,
    0xc63b05c06ca1493b,
    0x1ae3a4617c510ea
};


// ============================= base now is choosen as 8 ==========================
// mu = floor ( b^2k/modulus ) 

// k is 126
// b is base, which is 8 here.
// 0x98542343310183a5db0f28160bbd3dceeeb43799ddac681abcb52236169b40b43b5a1de2710a9647e7f56317936bff3
// 1465345440888336785010375189374315083602239024852731451287188631402381397748804180136956645283432552622682556252147 
const uint64_t mu[6] = {
    0x7e7f56317936bff3,
    0x43b5a1de2710a964,
    0xabcb52236169b40b,
    0xeeeb43799ddac681,
    0x5db0f28160bbd3dc,
    0x98542343310183a
};


// R_1 represents R^-1, where R * R^-1 = 1 + p * p'
// R_1 = 0x14212fc436b0736eb89746ca4c49351eeaaf41df7f2e657852e3c88a5582c936e20bbcde65839f5ef129093451269e8
const uint64_t R_1[6] = {
    0Xef129093451269e8,
    0X6e20bbcde65839f5,
    0X852e3c88a5582c93,
    0Xeeaaf41df7f2e657,
    0Xeb89746ca4c49351,
    0x14212fc436b0736
};

// P_1 = 0xbfa5205feec82e3d22f80141806a3cec5b245b86cced7a1335ed1347970debffd1e94577a00000008508bfffffffffff
const uint64_t p_1[6] = {
    0X8508bfffffffffff,
    0Xd1e94577a0000000,
    0X35ed1347970debff,
    0X5b245b86cced7a13,
    0X22f80141806a3cec,
    0xbfa5205feec82e3d
};

// R2 represents R^2, where R = 2^384
const uint64_t R2[6] = {
    0xb786686c9400cd22,
    0x0329fcaab00431b1,
    0x22a5f11162d6b46d,
    0xbfdf7d03827dc3ac,
    0x837e92f041790bf9,
    0x6dfccb1e914b88
};


const uint64_t inv_for_2[6] = {
    0x4284600000000001,
    0x0b85aea218000000,
    0x8f79b117dd04a400,
    0x8d116cf9807a89c7,
    0x631d82e03650a49d,
    0xd71d230be28875
};


const uint64_t montg_one[6] = {
    0x2cdffffffffff68, 
    0x51409f837fffffb1,
    0x9f7db3a98a7d3ff2,
    0x7b4e97b76e7c6305,
    0x4cf495bf803c84e8,
    0x8d6661e2fdf49a,
};

const u48 montg_one_u48 = {
    0x2cdffffffffff68, 
    0x51409f837fffffb1,
    0x9f7db3a98a7d3ff2,
    0x7b4e97b76e7c6305,
    0x4cf495bf803c84e8,
    0x8d6661e2fdf49a,
};



// ============================== BLS12-377 EdwardsParameters ============================

// const uint64_t COEFF_D = 3021; // Should be in BLS12-377 BaseField (MODULUS field).

// COEFF_A = -1 % modulus
const uint64_t COEFF_A[6] = {
    0x8508c00000000000,
    0x170b5d4430000000,
    0x1ef3622fba094800,
    0x1a22d9f300f5138f,
    0xc63b05c06ca1493b,
    0x1ae3a4617c510ea
};


const uint64_t montg_COEFF_A[6] = {
    0x823ac00000000099,
    0xc5cabdc0b000004f,
    0x7f75ae862f8c080d,
    0x9ed4423b9278b089,
    0x79467000ec64c452,
    0x120d3e434c71c50
};

const uint64_t montg_COEFF_D[6] = {
    0x7925bf4222538363, 
    0xf57011bac07d547d,
    0x7a6be793f79c7d2,
    0x2acb84bee2f95d93,
    0x45fb60ee86cad49f,
    0xbc23017dffb145
};
// ========================================================================================