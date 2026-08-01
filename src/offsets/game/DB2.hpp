// Data-table runtime storage offsets and definition-override targets.
// Copyright (C) 2026 WarcraftXL
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cstdint>

// INTERNAL to the core. Data-table runtime offsets. The map-data override rewrites the in-memory map
// storage built by the engine; the definition overrides target the stock storage object + accessor of
// each replaced table. Modules never include this; they use wxl::game / wxl::events.
namespace wxl::offsets::game::db2
{
    // Item DBC. Several hot consumers read this ID table inline instead of calling the generic accessor.
    namespace item
    {
        constexpr uintptr_t kStorageObject = 0x00AD3D4C;
        constexpr uintptr_t kMaxId         = 0x00AD3D58;
        constexpr uintptr_t kMinId         = 0x00AD3D5C;
        constexpr uintptr_t kIdTable       = 0x00AD3D6C;
        constexpr size_t    kRecordSize    = 32;
    }

    // -------------------------------------------------------------------------
    // Map storage. The engine compacts each on-disk map row into a fixed-size in-memory record and
    // indexes it by (id - minId). The override merges modern map rows into this storage in place.
    // -------------------------------------------------------------------------
    namespace map
    {
        constexpr uintptr_t kStorageLoadedFlag = 0x00AD4164; // u32, set to 1 once the storage is built
        constexpr uintptr_t kRecordCount       = 0x00AD4168; // u32
        constexpr uintptr_t kMaxId             = 0x00AD416C; // i32
        constexpr uintptr_t kMinId             = 0x00AD4170; // i32
        constexpr uintptr_t kRecordData        = 0x00AD417C; // record array base
        constexpr uintptr_t kIdTable           = 0x00AD4180; // record* table, indexed by (id - minId)
    }

    // -------------------------------------------------------------------------
    // Stock data-table definition override targets. A definition declares the storage object + accessor
    // its table replaces; the install reads these. Fields not needed for a table are left 0.
    // -------------------------------------------------------------------------
    namespace mapdef
    {
        constexpr uintptr_t kStorageObject  = 0x00AD4160; // storage instance
        constexpr uintptr_t kRecordCount    = 0x00AD4168; // recordCount field
        constexpr uintptr_t kRecordData     = 0x00AD417C; // recordData pointer field
        constexpr uintptr_t kLoader         = 0x00648510; // on-disk loader (no accessor funnel exists)
        constexpr uintptr_t kPostLoadBuild  = 0x0065E020; // post-load builder (rebuilds min/max + idTable)
        constexpr uintptr_t kIdTable        = 0x00AD4180; // idTable consumers read inline
    }

    namespace charsectionsdef
    {
        constexpr uintptr_t kStorageObject  = 0x00AD332C; // storage instance
        constexpr uintptr_t kRecordCount    = 0x00AD3334; // recordCount field
        constexpr uintptr_t kRecordData     = 0x00AD3348; // recordData pointer field
        constexpr uintptr_t kRecordLookup   = 0x004F3BA0; // consumer accessor (hook point)
        constexpr uintptr_t kCacheBuilder   = 0x004F3DD0; // cache builder
        constexpr uintptr_t kCacheRoot      = 0x00B6B864; // consumer cache root
    }

    // -------------------------------------------------------------------------
    // ChrRaces DBC. Compacted storage indexed by (id - minId). The ClientPrefix field at +0x18 is
    // a 4-char race code (e.g. "Hum", "Orc") used to build model paths.
    // -------------------------------------------------------------------------
    namespace chrraces
    {
        constexpr uintptr_t kMinId          = 0x00AD3438; // i32 minimum race id in the table
        constexpr uintptr_t kMaxId          = 0x00AD3434; // i32 maximum race id
        constexpr uintptr_t kIdTable        = 0x00AD3448; // record* table, indexed by (id - minId)
        constexpr size_t    kOffRecordPrefix= 0x18;       // char[4] race client prefix (e.g. "Hum")
    }

    // -------------------------------------------------------------------------
    // ItemDisplayInfo DBC. Looked up by display_id via the funnel accessor at kLookup. The record
    // contains model/texture names, the icon2 extension string, and the particle color id.
    // -------------------------------------------------------------------------
    namespace itemdisplayinfo
    {
        constexpr uintptr_t kStorageObject  = 0x00AD3DDC; // storage instance
        constexpr uintptr_t kMaxId          = 0x00AD3DE8;
        constexpr uintptr_t kMinId          = 0x00AD3DEC;
        constexpr uintptr_t kIdTable        = 0x00AD3DFC;
        // sub_4cfd90: thiscall(ecx=storageObj, displayId, outBuf); fills outBuf with the 256-byte
        // record copy (field pointers point into the live DBC string block); returns non-zero if found.
        constexpr uintptr_t kLookup         = 0x004CFD90;
        using LookupFn = uint32_t (__fastcall*)(void* storageObj, void* edx, uint32_t displayId, void* outBuf);

        // Field offsets within the resolved record pointer (byte offsets from the record base).
        constexpr size_t kOffModel1     = 0x04; // char* primary model filename (no path, no extension)
        constexpr size_t kOffModel2     = 0x08; // char* secondary model filename (left/right variant)
        constexpr size_t kOffTex1       = 0x0C; // char* primary texture name
        constexpr size_t kOffTex2       = 0x10; // char* secondary texture name
        constexpr size_t kOffIcon2      = 0x18; // char* icon2 string (consumer-defined convention; raw pointer only)
        constexpr size_t kOffParticleId = 0x60; // uint32 particle color id
        constexpr size_t kRecordSize    = 256;  // byte stride between records in the storage array
    }

    // -------------------------------------------------------------------------
    // Gender strings table. A flat array of char* pointers indexed by gender id (0 = Male, 1 = Female).
    // Used to build gender-specific model paths (e.g. "HumanMale" vs "HumanFemale").
    // -------------------------------------------------------------------------
    namespace genderstrings
    {
        constexpr uintptr_t kTable = 0x00AC46A0; // char*[2] (index 0 = "Male", index 1 = "Female")
    }

    // -------------------------------------------------------------------------
    // CreatureDisplayInfo DBC. Range-checked + array-indexed inline (no separate funnel function like
    // ItemDisplayInfo's kLookup -- this table's lookup is inlined directly into its one caller,
    // creaturemodeldata::kResolveFn below; see that namespace for the second, chained lookup the same
    // function does). RE'd via live read/write tracing while running `.creature transform`, confirmed
    // against a real sample row (displayId 33110 / 0x8156): ModelId read back as 7284 / 0x1C74 exactly,
    // and TextureVariation1/2/3 read back as real creature texture-variant names (e.g.
    // "manawyrm2_void"), confirming both the record layout and the field offsets below.
    // -------------------------------------------------------------------------
    namespace creaturedisplayinfo
    {
        constexpr uintptr_t kMinId   = 0x00AD34C8; // i32
        constexpr uintptr_t kMaxId   = 0x00AD34C4; // i32
        constexpr uintptr_t kIdTable = 0x00AD34D8; // record* table, indexed by (id - minId)

        constexpr size_t kOffModelId           = 0x04; // uint32 -- FK into CreatureModelData, NOT a string
        constexpr size_t kOffTextureVariation1 = 0x18; // char*
        constexpr size_t kOffTextureVariation2 = 0x1C; // char*
        constexpr size_t kOffTextureVariation3 = 0x20; // char*
    }

    // -------------------------------------------------------------------------
    // CreatureModelData DBC. Keyed by ModelId (creaturedisplayinfo::kOffModelId's value), NOT by
    // displayId -- several CreatureDisplayInfo rows (different texture variants of one creature)
    // commonly share a single ModelId, so a ModelId-keyed model-path override naturally covers every
    // variant with one registration. Same inline range-check + array-index lookup shape as
    // creaturedisplayinfo above, chained immediately after it inside kResolveFn. Confirmed via live
    // memory dump: row+0 read back as the row's own id (0x1C74 = 7284, matched exactly), row+8
    // pointed at the string "Creature\ManaWyrm2\ManaWyrm2.mdx" -- a real, well-formed archive path.
    // -------------------------------------------------------------------------
    namespace creaturemodeldata
    {
        constexpr uintptr_t kMinId   = 0x00AD3510; // i32
        constexpr uintptr_t kMaxId   = 0x00AD350C; // i32
        constexpr uintptr_t kIdTable = 0x00AD3520; // record* table, indexed by (id - minId)

        constexpr size_t kOffId        = 0x00; // uint32, row's own id (== ModelId)
        constexpr size_t kOffModelName = 0x08; // char* -- the field to substitute for a virtual path

        // kResolveMerge is NOT a function boundary -- it is the raw instruction address immediately
        // after the inline CreatureModelData lookup completes, inside kResolveFn. Both the
        // lookup-succeeded and lookup-failed branches jump to this exact address (it's a genuine merge
        // point in the control flow, not a mid-instruction landing), so nothing branches into the
        // middle of whatever bytes get overwritten there -- same safety justification as
        // unit::kUnitFieldSetWrite in Unit.hpp. EAX at this address holds the resolved
        // CreatureModelData row pointer, or 0 if ModelId was out of range -- check for null before
        // touching kOffModelName.
        constexpr uintptr_t kResolveMerge = 0x0072A531;
        // Raw instruction hook, not a real function boundary -- see kResolveMerge's doc comment
        // above. __cdecl() with no params/return: the naked hook stub reads eax manually rather
        // than through the calling convention, so this alias exists purely to type
        // g_origResolveMerge for the jmp thunk (same convention as unit::UnitFieldSetWrite2Fn).
        using ResolveMergeFn = void(__cdecl*)();
        // kResolveFn is this hook's containing function's real entry point (confirmed prologue:
        // push esi / push edi / mov edi,ecx / mov esi,[edi+9D4]). Confirmed via "find references to"
        // that this is the ONLY call site anywhere in the client that reaches this resolution path
        // (one caller, sub_73E410 at 0x0073E410) -- so a hook at kResolveMerge covers every way a
        // creature's model gets resolved, with no other path to miss. Recorded for reference/logging
        // only; kResolveFn itself is not a hook point the way kResolveMerge is.
        constexpr uintptr_t kResolveFn = 0x0072A480;
    }
}
