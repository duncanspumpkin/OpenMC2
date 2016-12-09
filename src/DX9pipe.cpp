/**********************************************************************
* OpenMC2 - An Open Source Re-Implementation of Midnight Club 2
* Copyright (C) 2016  LRFLEW
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************************************************/

#include "Config.hpp"
#include "DX9pipe.hpp"
#include "Game.hpp"
#include <d3d9.h>

struct resolution {
    uint32_t width;
    uint32_t height;
    uint32_t bits;
    uint32_t unkC;
};

struct unk_85A99C {
    uint8_t pad000[0x57C];
    uint32_t num_resoluitions;
    resolution *resolutions; //0x580
};

#define glo_D3D (MC2_GLOBAL<IDirect3D9 *>(0x00858368))
#define glo_85A1D0 (MC2_GLOBAL<uint32_t>(0x0085A1D0))
// Some sort of struct size 0x584
#define glo_85A99C (MC2_GLOBAL<unk_85A99C*>(0x0085A99C))
// Compatible resolution?
#define glo_858714 (MC2_GLOBAL<uint32_t>(0x00858714))

#define loc_8586D0 (MC2_POINTER<uint32_t>(0x008586D0))
// Likely some sort of struct
#define loc_858728 (MC2_POINTER<uint32_t>(0x00858728))
// Array end
#define loc_859B28 (MC2_POINTER<uint32_t>(0x00859B28))

#define sub_5F1140 (MC2_PROC_PTR<void>(0x005F1140))
// Register window and create
#define sub_5ED560 (MC2_PROC_PTR<void, char *>(0x005ED560))

// mc2: 0x005F02B0
uint32_t sub_5F02B0(uint32_t unk1) {
    return glo_85A99C[unk1].num_resoluitions;
}

// mc2: 0x005F02E0
void sub_5F02E0(uint32_t unk1, uint32_t unk2, uint32_t *width, uint32_t *height, uint32_t *bits) {
    unk_85A99C *unk = &glo_85A99C[unk1];

    *width = unk->resolutions[unk2].width;
    *height = unk->resolutions[unk2].height;
    *bits = unk->resolutions[unk2].bits;
}

bool init_directx() {
    IDirect3D9 *pD3D = Direct3DCreate9(D3D9b_SDK_VERSION);
    glo_D3D = pD3D;
    if (pD3D == nullptr) {
        return false;
    }

    sub_5F1140();

    for (uint32_t *unk1 = loc_858728;
        unk1 < loc_859B28;
        unk1 += 10) {
        *unk1 = 0;
    }
    
    glo_85A1D0 = 0;
    sub_5ED560("D3D9 Pipeline");
    
    if (!glo_windowed_mode) {
        ShowCursor(FALSE);
    }
    else {
        D3DDISPLAYMODE mode;
        if (glo_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode) == D3D_OK) {
            if (mode.Format == D3DFMT_X8R8G8B8 ||
                mode.Format == D3DFMT_A8R8G8B8) {
                glo_674FB4 = 0x20;
            }
            else {
                glo_674FB4 = 0x10;
            }
        }
    }

    uint32_t num_resolutions = sub_5F02B0(0);
    if (num_resolutions > 0) {
        for (int i = 0; i < num_resolutions; ++i) {
            uint32_t width, height, bits;
            sub_5F02E0(0, i, &width, &height, &bits);

            if (width != glo_674FAC)
                continue;

            if (height != glo_674FB0)
                continue;

            if (bits != glo_674FB4)
                continue;

            glo_858714 = i;
        }
    }

    memset(loc_8586D0, 0, 14 * 4);
    //5f13ff
}
