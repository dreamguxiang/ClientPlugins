#include "Core.h"
#include "HookAPI.h"
#include <iostream>

class ResourceLocation
{
public:
	__int32 mFileSystem;
	Core::PathBuffer<std::string> mPath;
	unsigned __int64 mPathHash;
	unsigned __int64 mFullHash;
	
	ResourceLocation(Core::Path path) {
		AddrCall("ResourceLocation::ResourceLocation", "40 53 48 ?? ?? ?? 33 C0 48 8B D9 89 01 48 ?? ?? ?? 48 89 ?? ?? ?? 48 83 ?? ?? ?? 4C 8B ?? ?? "
			"72 ?? 48 8B 12 48 89 01 48 89 ?? ?? 48 C7 ?? ?? ?? ?? ?? ?? 88 01 E8 ?? ?? ?? ?? 48 8B CB E8 ?? ?? ?? ?? 48 8B C3",
			ResourceLocation*, ResourceLocation*, Core::Path)(this, path);
	}
};

class ResourcePackManager {
private:
	//char filler[384];
public:
};

//
//AHook("ResourcePackManager::ResourcePackManager", ResourcePackManager*, "48 89 ?? ?? ?? 55 56 57 41 54 41 55 41 56 41 57 48 8D ?? ?? ?? 48 "
//	"?? ?? ?? ?? ?? ?? 45 0F B6 F1 49 8B F8 48 8B F2 48 8B D9 48 89 ?? ?? 48 89 ?? ?? 48 8D ?? ?? 48"
//	" 89 ?? ?? 45 33 ED", ResourcePackManager* a1, Core::PathBuffer<std::string> a2, void* a3, bool a4) {
//     //auto out = original(a1, a2, a3, a4);
//     //std::cout<< out<<std::endl;
//     //std::cout << out->isInStreamableLocation(ResourceLocation(Core::Path("test"))) << std::endl;
//     return original(a1, a2, a3, a4);
//}

////
//AClasslessInstanceHook("AppPlatform::readAssetFile", void*, "48 89 ?? ?? ?? 55 56 57 41 56 41 57 48 8D ?? ?? ?? 48 ?? ?? ?? ?? ?? ?? 48 ?? ?? ?? "
//	"?? ?? ?? 48 33 C4 48 89 ?? ?? 49 8B C0 48 8B FA 48 89 ?? ?? 45 33 F6 44 89 ?? ?? ?? 0F 57 C9", std::string* a2, Core::Path* a3) {
//	std::string filename = a3->mPath.mUtf8StdString;
//	if (filename.find("renderer/materials/") != std::string::npos && filename.find(".material.bin") != std::string::npos){
//		auto out = ResourceLocation(*a3);
//		std::cout << out.mPath.get() << std::endl;
//	}
//	return original(this, a2, a3);
//}
namespace persona {
    enum PieceType : unsigned int {
        Unknown = 0x0,
        Skeleton = 0x1,
        Body = 0x2,
        Skin = 0x3,
        Bottom = 0x4,
        Feet = 0x5,
        Dress = 0x6,
        Top = 0x7,
        High_Pants = 0x8,
        Hands = 0x9,
        Outerwear = 0xA,
        FacialHair = 0xB,
        Mouth = 0xC,
        Eyes = 0xD,
        Hair = 0xE,
        Hood = 0xF,
        Back = 0x10,
        FaceAccessory = 0x11,
        Head = 0x12,
        Legs = 0x13,
        LeftLeg = 0x14,
        RightLeg = 0x15,
        Arms = 0x16,
        LeftArm = 0x17,
        RightArm = 0x18,
        Capes = 0x19,
        ClassicSkin = 0x1A,
        Emote = 0x1B,
        Unsupported = 0x1C,
        Count = 0x1D,
    };
    enum PieceSide : unsigned int {
        Both = 0x0,
        Left = 0x1,
        Right = 0x2,
    };

    struct FreeItemState
    {
    public:
        bool mIsFreeItem;
        bool mIsAchievementItem;
        bool mIsRedeemable;
    };
}
namespace mce
{
    class UUID
    {
        uint64_t a, b;
    };
	
    class Color
    {
    public:
        float r;
        float g;
        float b;
        float a;
    };
}
#include <array>
struct TintMapColor
{
    std::array<mce::Color, 4> colors;
};

class PersonaPieceHandle {
    struct SerializedExcessData
    {
        persona::PieceSide mPieceSide;
        TintMapColor mTintingData;
        bool mUseTintColor;
    };
public:
    persona::PieceType mPieceType;
	mce::UUID mPackId;
	std::string mPieceId;
	std::string mPersonaCharacter;
	bool mDefaultPiece;
	std::string mProductId;
	PersonaPieceHandle::SerializedExcessData mSerializedExcessData;
	persona::FreeItemState mFreeItemState;
};

//PersonaRepository::_isPieceOwned
//AClasslessInstanceHook("sub_1408098D0", bool, "40 53 55 56 57 41 55 41 56 41 57 48 ?? ?? ?? 48 ?? ?? ?? ?? ?"
//	"? ?? 48 33 C4 48 89 ?? ?? ?? 49 8B F0 48 8B FA 4C 8B E9 4C 89 ?? ?? ?? 44 8B 32 41 ?? ?? ?? 75 ??", PersonaPieceHandle*a2,void*a3) {
//    std::cout << a2->mDefaultPiece << std::endl;
//	original(this, a2, a3);
//	return 0;
//}



SClasslessInstanceHook2("TrialManager::isTrial", bool,
    "40 53 48 ?? ?? ?? 48 ?? ?? ?? ?? ?? ?? 48 33 C4 48 89 ?? ?? ?? 48 8B D9 48 8B ?? ?? 48 8B 01 FF ?? ?? ?? ?? ?? "
    "41 ?? ?? ?? ?? ?? 48 8D ?? ?? ?? 48 8B C8 4C 8B 08 41 ?? ?? ?? 80 7C ?? ?? ?? 74 ?? 48 8B ?? ?? ?? 48 8B ?? ?? "
    "48 8B ?? ?? ?? ?? ?? 48 85 C9 74 ??") {
	
    return 0;
};